# Cypher Graph Engine Architecture

## Overview

A high-performance in-memory graph engine with a custom Cypher query language,
targeting 140K+ nodes and 60K+ edges with sub-millisecond label-indexed queries.

```
Cypher Query (string)
     ↓
cypher_lexer.rl       → tokens          ← Ragel lexer
     ↓
cypher_parser.c       → AST             ← Recursive descent
     ↓
graph_exec.c          → FSM executor    ← Goto-based state machine (Phase 4)
     ↓                → JIT compiler    ← AST→C→gcc→dlopen (Phase 3)
     ↓
graph_store.c         → result rows     ← Storage + 4 indexes (Phases 1,2,5)
```

## Phases

### Phase 1: Storage Engine (`graph_store.h/c`)

Struct-of-arrays layout targeting 2M+ nodes, 10M+ edges.

| Structure | Size | Purpose |
|-----------|------|---------|
| `gs_node_t` | 32B | CSR adjacency head, label bitmap, property offset |
| `gs_edge_t` | 12B | CSR singly-linked list: dst, type hash, next pointer |
| `gs_prop_t` | 8B | Columnar: key_hash + value offset into arena |
| Label table | 132B × 64 | Name hash + string, 64-bit bitmap per node |
| Value arena | growable | Contiguous buffer, property values as [type:1B][data] |

Key design decisions:
- **No MVCC, no locks, no transactions** — single-writer, batch-load model
- **CSR adjacency** — O(degree) edge traversal, no global scan
- **Bitmap labels** — 64-bit mask per node for O(1) label membership test
- **Columnar properties** — filter reads only the `key_hash` column
- **Flat arena** — single `realloc`-based buffer avoids chunk offset bugs
- **klib** (`khashl.h`, `kvec.h`) — MIT-licensed, single-header, battle-tested

### Phase 2: Index Layer (`graph_store.h/c`)

Four hash indexes built after batch loading:

| Index | Key | Value | Purpose |
|-------|-----|-------|---------|
| `label_idx` | `hash("LabelName")` | `{offset, count}` → node list | `MATCH (n:Label)` |
| `prop_idx` | `hash("propertyKey")` | `{offset, count}` → node list | `WHERE n.key = val` candidate narrowing |
| `edge_idx` | (reserved) | — | Edge type lookups |
| `text_idx` | `3-byte trigram` | `{offset, count}` → node list | `CONTAINS/STARTS/ENDS` candidate narrowing (Phase 5) |

All indexes use `klib/khashl` (Fibonacci hashing, 75% load factor) and `klib/kvec` dynamic arrays.

### Phase 3: JIT Compiler (`graph_jit.h/c`)

AST → C source → gcc -O2 -shared -fPIC → dlopen. Each query gets a compiled `.so`
with all label names, property keys, and comparison values inlined as C literals.
Eliminates AST traversal, string comparison for filter keys, and virtual dispatch.

```
Cypher Query → AST → graph_jit.c (generate C code)
                         ↓
                    /tmp/cypher_jit/q_HASH.c
                         ↓  gcc -O2 -shared -fPIC
                    /tmp/cypher_jit/q_HASH.so
                         ↓  dlopen + dlsym("query_exec")
                    compiled function pointer
```

Cached on disk by query hash. Falls back to interpreter on compilation failure.

### Phase 4: Execution Engine (`graph_exec.c`)

A goto-based state machine (no function calls on hot path, no for/while nesting):

```
state_scan ──(no more)──→ state_done
     │
     ▼
state_filter ──(reject)──→ state_scan
     │
     ├──(no edges)──→ state_emit ──→ state_scan
     │
     └──(has edges)──→ state_expand_loop ──(emit multi-hop)──→ state_expand_loop
                                                                    │
                                                    └──(no more edges)──→ state_scan
```

Multi-hop support: `MATCH (a)-->(b)-->(c)` uses stack-based traversal
(`hop_nids[]`, `hop_ej[]`, `hop_ec[]`) with recursive goto dispatch.

Post-execution: ORDER BY (bubble sort), DISTINCT (dedup), LIMIT/SKIP (with
proper memory freeing), COUNT(*) aggregation.

### Phase 5: Trigram Text Index (`graph_store.h/c` + `graph_exec.c`)

**Design (inspired by zoekt):**

3-byte sliding window over property text → packed `uint32` trigrams →
`khashl` index: `trigram → sorted node list`.

| Aspect | zoekt | Our implementation |
|--------|-------|-------------------|
| Trigram encoding | 3 runes → uint64 (UTF-8) | 3 bytes → uint32 (ASCII) |
| Index structure | B+ tree on disk | `khashl` in-memory |
| Postings storage | Delta-encoded varints | Sorted `uint32_t` arrays via `kvec` |
| Per-node dedup | Implicit | `seen[64]` array during build |
| Query selectivity | Pick 2 lowest-freq, distance AND | Pick rarest trigram, `strstr` verify |

**Two-phase query:**
1. Extract trigrams from search string, find rarest trigram's posting list
2. For each candidate node, verify with `strstr`/`strncmp`

Build via `gs_build_text_index(gs)` (manually invoked after sidecar import,
kept separate from `gs_build_indexes` due to memory/compute cost on large
datasets).

## Build System

```
make              # optimized build (-O2)
make debug        # debug build (-O0 -g)
make asan         # AddressSanitizer build
make test         # run 15 smoke tests
make valgrind     # valgrind memcheck on 4 query types
make bench        # build and run benchmark
make clean        # remove artifacts
```

Requires: `gcc`, `ragel`, `valgrind` (optional), `fzf` (optional, for REPL history).

## Performance (10K nodes, -O2, FSM executor)

| Query pattern | µs/query |
|---------------|----------|
| Label index lookup (38 rows) | 12.6 µs |
| Label index lookup (200 rows) | 63-69 µs |
| Label + WHERE str eq (6 rows) | 3.7 µs |
| Edge traversal (label + expand) | 195 µs |
| ORDER BY + LIMIT (50 rows) | 507 µs |
| CONTAINS with text index | O(rarest trigram freq) |
| Full scan (10K nodes) | 20,777 µs |

## File Inventory

| File | Purpose | Lines |
|------|---------|-------|
| `cypher_lexer.rl` | Ragel tokenizer (28 keywords, strings, numbers) | 352 |
| `cypher_parser.c` | Recursive-descent AST builder | 650 |
| `cypher_parser.h` | Token/AST types, graph API, result API | 160 |
| `cypher_graph.c` | Mutation handler, sidecar importer, execution dispatch | 673 |
| `cypher_repl.c` | CLI: interactive/piped/batch modes, dot-commands, history | 329 |
| `graph_store.c` | Storage engine, 4 indexes, queries, stats, trigram search | 645 |
| `graph_store.h` | Node/edge/prop structs, index types, public API | 160 |
| `graph_exec.c` | FSM executor, multi-hop, property/trigram index lookup | 589 |
| `graph_exec.h` | FSM API, JIT placeholder | 34 |
| `graph_jit.c` | AST→C code generator, gcc runner, dlopen loader | 395 |
| `graph_jit.h` | JIT API | 28 |
| `vendor/klib/khashl.h` | Vendored hash table library (MIT) | 482 |
| `vendor/klib/kvec.h` | Vendored dynamic array library (MIT) | 90 |

## Dependencies

- `gcc` — compilation, JIT shared library compilation
- `ragel` (7.x) — lexer state machine generation
- `klib` (`khashl.h`, `kvec.h`) — vendored in `vendor/klib/`, MIT license
- `valgrind` — optional, for memory checking (`make valgrind`)
- `fzf` — optional, for `.history` REPL command
