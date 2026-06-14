# Cypher Graph Engine Architecture

## Overview

A high-performance in-memory graph engine with a custom Cypher query language,
supporting 50K+ nodes and 23K+ edges with sub-millisecond label-indexed queries.

Two query modes:
- `--sidecar` — full graph store with indexes, edges, mutations (all Cypher)
- `--scan` — mmap'd direct JSON scan via Ragel scanner, zero import cost (~70ms for 34MB)

```
Cypher Query (string)
     ↓
cypher_lexer.rl       → tokens          ← Ragel lexer (scanner mode)
     ↓
cypher_parser.c       → AST             ← Recursive descent (+ error recovery)
     ↓
graph_exec.c          → FSM executor    ← Goto-based state machine (Phase 4)
     ↓                → JIT Ragel       ← AST→.rl→ragel→gcc→dlopen (Phase 3)
     ↓
graph_scan.rl         → direct mmap     ← Ragel single-pass scanner (--scan mode)
graph_text.rl         → trigrams        ← Ragel trigram extractor (Phase 5)
     ↓
graph_store.c         → result rows     ← Storage + 4 indexes + klib (Phases 1,2)
```

All hot-path scanning uses Ragel scanners compiled with `-G2` (goto-driven, expanded actions).

## Phases

### Phase 1: Storage Engine (`graph_store.h/c`)

Struct-of-arrays layout. Nodes auto-grow from 64K initial capacity.

| Structure | Size | Purpose |
|-----------|------|---------|
| `gs_node_t` | 32B | CSR adjacency head, edge_count, label bitmap, props_off, prop_count |
| `gs_edge_t` | 12B | CSR singly-linked list: dst, type hash, next pointer |
| `gs_prop_t` | 8B | Columnar: key_hash + value offset into arena |
| Label table | 132B × 64 | Name hash + string, 64-bit bitmap per node |
| Value arena | growable | Contiguous buffer, property values as [type:1B][data] |
| Filepath table | khashl `fpt_t` | `filepath_id → strdup'd string` (interning, saves ~12MB) |

Key design decisions:
- **No MVCC, no locks, no transactions** — single-writer, batch-load model
- **CSR adjacency** — O(degree) edge traversal, no global scan
- **Bitmap labels** — 64-bit mask per node for O(1) label membership test
- **Columnar properties** — `prop_count` per node for O(prop_count) scans (was O(n²) before fix)
- **Flat arena** — single `realloc`-based buffer
- **Filepath interning** — 37 unique `strdup`'d strings instead of 50K arena copies
- **klib** (`khashl.h`, `kvec.h`) — MIT-licensed, single-header, battle-tested

### Phase 2: Index Layer (`graph_store.h/c`)

Four hash indexes built after batch loading:

| Index | Key | Value | Purpose |
|-------|-----|-------|---------|
| `label_idx` | `hash("LabelName")` | `{offset, count}` → node list | `MATCH (n:Label)` |
| `prop_idx` | `hash("propertyKey")` | `{offset, count}` → node list | `WHERE n.key = val` candidate narrowing |
| `edge_idx` | (reserved) | — | Edge type lookups |
| `text_idx` | `3-byte trigram` | `{offset, count}` → node list | `CONTAINS/STARTS/ENDS` (Phase 5) |

All indexes use `klib/khashl` (Fibonacci hashing, 75% load factor) and `klib/kvec` dynamic arrays.

### Phase 3: JIT Compiler (`graph_jit.h/c`)

**Ragel edition:** Generates a `.rl` file with label names and CONTAINS needles inlined
as Ragel scanner literals. Pipeline:

```
AST → graph_jit.c (generate .rl)
        ↓
   /tmp/cypher_jit/q_HASH.rl     ← Ragel scanner with inlined "type":"LABEL" pattern
        ↓  ragel -G2
   /tmp/cypher_jit/q_HASH.c      ← Deterministic goto-driven state machine
        ↓  gcc -O2 -shared -fPIC
   /tmp/cypher_jit/q_HASH.so     ← dlopen'd, cached by query hash
```

Eliminates `strstr`/`strcmp` on the hot path — the Ragel FSM directly matches bytes.
Falls back to pre-compiled FSM interpreter if ragel/gcc unavailable at runtime.

### Phase 4: Execution Engine (`graph_exec.c`)

A goto-based state machine with:
- Multi-hop support via stack-based traversal (`hop_nids[]`, `hop_ej[]`, `hop_ec[]`)
- IS NULL / IS NOT NULL evaluation
- Arithmetic expression evaluation (`+`, `-`, `*`, `/`, `%`)
- CONTAINS / STARTS WITH / ENDS WITH via `strstr`/`strncmp`
- IN list operator
- Post-execution: ORDER BY (bubble sort), DISTINCT (dedup), LIMIT/SKIP (proper free), COUNT(*) aggregation
- OPTIONAL MATCH stub (`cypher_fsm_exec_optional`)

### Phase 5: Trigram Text Index (`graph_store.h/c` + `graph_text.rl`)

**Ragel extractor:** `graph_text.rl` scans property strings with a 3-byte window pattern:

```ragel
main := |*
  any{3} => store_tg;   // every 3-byte window → packed uint32
  any;                    // trailing bytes
*|;
```

Called via `gs_extract_trigrams(str, len, out, max)`. Replaces the hand-written
sliding window loop. Built into `gs_build_text_index(gs)` when invoked manually.

### --scan Mode (`graph_scan.rl`)

Single-pass Ragel scanner over mmap'd sidecar JSON:

```ragel
main := |*
  '{'  => start_rec;
  '"type":"' [^"]+ '"' => found_type;   // label matching
  '"text":"' ([^"\\] | '\\' any)* '"' => found_text;  // text extraction
  any;
*|;
```

Eliminates per-line `strstr` calls and 16KB stack buffer copies. 34MB file scanned
in ~70ms (including mmap+process startup). The JIT path inlines the label name
into the Ragel pattern (e.g., `'"type":"math"'`) for zero-comparison-overhead matching.

### Math Token AST (`scanner.rl` + `latex.rl`)

The scanner emits 8 typed math tokens inside display/inline math.
EMIT_BLOCK recursively scans inner content for structural constructs:

| Token | Examples | Children via EMIT_BLOCK |
|-------|----------|------------------------|
| `math_op` | `+`, `-`, `\times`, `\cdot`, `\pm`, `\div` | — |
| `math_rel` | `=`, `\lt`, `\gt`, `\leq`, `\equiv`, `\approx` | — |
| `math_fn` | `\sin`, `\cos`, `\log`, `\ln`, `\exp` | — |
| `math_greek` | `\alpha`–`\omega` (24 letters) | — |
| `math_num` | digit sequences | — |
| `math_var` | single letters | — |
| `math_sub` | `_{i=1}`, `_{xyz}` | — |
| `math_sup` | `^{n}`, `^{\infty}` | — |
| `frac` | `\frac{a}{b}` | braces ×2 (num, den) |
| `sum` | `\sum_{i=1}^{n}` | math_sub, math_sup |
| `prod` | `\prod_{i=1}^{n}` | math_sub, math_sup |
| `lim` | `\lim_{x\to 0}` | math_sub |
| `int` | `\int_{0}^{\infty}` | math_sub, math_sup |

## Build System

```
make              # optimized build (-O2)
make debug        # debug build (-O0 -g)
make asan         # AddressSanitizer build
make test         # run smoke tests (26 queries)
make valgrind     # valgrind memcheck on 4 query types
make bench        # build and run benchmark
make tools        # build data manipulation tools
make clean        # remove artifacts
```

Requires: `gcc`, `ragel` (7.x), `libreadline-dev`, `valgrind` (optional), `fzf` (optional).

Ragel files regenerated on change: `cypher_lexer.rl`, `graph_scan.rl`, `graph_text.rl`.

## Data Manipulation Tools (`tools/`)

OpenRefine-inspired sidecar modification system with clustering, rule chains,
and undo logs.

| Tool | Purpose |
|------|---------|
| `sidecar_mod` | Rule engine — mmap stream, regex/strip/trim transforms, undo log |
| `cluster_find` | Clustering engine — fingerprint + Levenshtein distance |
| `cluster_apply.sh` | Interactive fzf cluster merge workflow |
| `sidecar_apply.sh` | Interactive fzf rule application workflow |
| `strip_latex.rules` | 10 example rules for LaTeX delimiter removal |

### Rule Format (JSON lines)
```json
{"step":1,"name":"strip author prefix","match_type":"author",
 "prop":"text","transform":"regex",
 "pattern":"\\\\author\\{(.*)\\}","replace":"$1"}
```

Transform types: `regex`, `strip_prefix`, `strip_suffix`, `trim`.

### Clustering
```bash
# Fingerprint clustering (normalize → hash → group)
./cluster_find sidecar.json label

# Levenshtein clustering (edit distance threshold)
./cluster_find sidecar.json label --levenshtein 3
```

### Interactive Merge Workflow
```
./cluster_apply.sh sidecar.json label
  → lists clusters with fzf preview
  → Enter selects canonical value
  → generates merge rules + undo log
  → applies via sidecar_mod
```

## REPL Features

| Feature | Binding / Command |
|---------|-------------------|
| Up/Down arrows | Navigate command history (readline) |
| Left/Right, Ctrl-A/E/W/U | Line editing (readline) |
| Ctrl-R | Open fzf history browser (same as `.hist`) |
| `.help` | Show available commands and syntax |
| `.schema` | Show all labels with node counts |
| `.stats` | Show node/edge/property counts |
| `.history` | Browse command history with fzf |

History auto-loaded from `~/.cypher_history` on startup, saved on exit (1000-line cap).
Parser error recovery: skips to next `;` on unrecognized clause instead of aborting.

## Performance (50K nodes, -O2)

| Query pattern | Time |
|---------------|------|
| Label index lookup | 10-70 µs |
| Label + WHERE str eq | <10 µs |
| Edge traversal (label + expand) | ~200 µs |
| Full scan (50K nodes, no index) | ~20-50 ms |
| --scan Ragel (34MB mmap) | ~70 ms total |

## File Inventory

| File | Purpose | Generated |
|------|---------|-----------|
| `cypher_lexer.rl` | Ragel tokenizer (28 keywords) | → cypher_lexer.c |
| `cypher_parser.c` | Recursive-descent AST builder + error recovery | — |
| `cypher_parser.h` | Token/AST types, graph API, result API | — |
| `cypher_graph.c` | Mutation handler, sidecar importer (mmap'd), execution dispatch | — |
| `cypher_repl.c` | CLI: readline, piped, batch, dot-commands, Ctrl-R | — |
| `graph_store.c` | Storage engine, 4 indexes, prop_count fix, fp interning | — |
| `graph_store.h` | Node/edge/prop structs, index types, public API | — |
| `graph_exec.c` | FSM executor, multi-hop, arithmetic, IS NULL, IN | — |
| `graph_exec.h` | FSM API, optional match stub | — |
| `graph_jit.c` | AST→Ragel .rl generator, ragel+gcc runner, dlopen | — |
| `graph_jit.h` | JIT API | — |
| `graph_scan.rl` | Ragel single-pass JSON scanner (--scan mode) | → graph_scan.c |
| `graph_text.rl` | Ragel trigram extractor (Phase 5) | → graph_text.c |
| `vendor/klib/khashl.h` | Vendored hash table (MIT) | — |
| `vendor/klib/kvec.h` | Vendored dynamic array (MIT) | — |
| `tools/sidecar_mod.c` | Rule engine for sidecar transforms | — |
| `tools/cluster_find.c` | Clustering engine (fingerprint + Levenshtein) | — |
| `tools/cluster_apply.sh` | Interactive fzf cluster merge workflow | — |
| `tools/sidecar_apply.sh` | Interactive fzf rule application workflow | — |
| `tools/strip_latex.rules` | Example rules (10 LaTeX transforms) | — |

## Key Bug Fixes

| Bug | Fix | Impact |
|-----|-----|--------|
| O(n²) property scans | Added `prop_count` per node | 50K nodes: 2.9GB RSS → ~75MB |
| Filepath duplication | `fp_table` interning via khashl + `strdup` | 50K copies → 37 unique, saves ~12MB |
| Arena realloc UAF in fp_table | Separate heap allocation for filepath strings | Eliminated crash |
| Truncated pointer in fp_table | `fpt_t` with pointer-sized value instead of uint32 | Fixed on 64-bit |
| parse_set memory leak | `parse_expression()` consumed `=` → `parse_atom()+parse_postfix()` | 5 AST nodes (1520 bytes) |
| result_limit_skip leak | Rows beyond limit/skip freed before truncation | Variable |
| Robin Hood hash corruption | Replaced with klib linear probing | Correct label lookups |
