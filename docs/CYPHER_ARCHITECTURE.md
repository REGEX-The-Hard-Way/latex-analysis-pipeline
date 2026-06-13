# Cypher Graph Engine Architecture

## Overview

A high-performance in-memory graph engine with a custom Cypher query language,
supporting 50K+ nodes and 23K+ edges with sub-millisecond label-indexed queries.

Two query modes:
- `--sidecar` — full graph store with indexes, edges, mutations (all Cypher)
- `--scan` — mmap'd direct JSON scan, zero import cost (~60ms for 34MB)

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
graph_scan.c          → direct mmap     ← strstr on mapped JSON (--scan mode)
     ↓
graph_store.c         → result rows     ← Storage + 4 indexes (Phases 1,2,5)
```

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
| Filepath table | khashl | `filepath_id → strdup'd string` (interning, saves ~12MB) |

Key design decisions:
- **No MVCC, no locks, no transactions** — single-writer, batch-load model
- **CSR adjacency** — O(degree) edge traversal, no global scan
- **Bitmap labels** — 64-bit mask per node for O(1) label membership test
- **Columnar properties** — `prop_count` per node for O(prop_count) scans (was O(n²) before fix)
- **Flat arena** — single `realloc`-based buffer
- **Filepath interning** — 37 unique strings instead of 50K arena copies
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
Cached on disk by query hash. Falls back to interpreter on compilation failure.

### Phase 4: Execution Engine (`graph_exec.c`)

A goto-based state machine with multi-hop support via stack-based traversal.
Post-execution: ORDER BY (bubble sort), DISTINCT (dedup), LIMIT/SKIP (with
proper memory freeing), COUNT(*) aggregation.

### Phase 5: Trigram Text Index (`graph_store.h/c` + `graph_exec.c` + `graph_scan.c`)

3-byte sliding window over property text → packed `uint32` trigrams →
`khashl` index: `trigram → sorted node list`. Two-phase query: rarest trigram
narrows candidates, `strstr` verifies. Built via `gs_build_text_index(gs)`.

In `--scan` mode, `strstr_len` on mmap'd JSON performs the same function
without building the index.

## Build System

```
make              # optimized build (-O2)
make debug        # debug build (-O0 -g)
make asan         # AddressSanitizer build
make test         # run smoke tests (15+ queries)
make valgrind     # valgrind memcheck on 4 query types
make bench        # build and run benchmark
make clean        # remove artifacts
```

Requires: `gcc`, `ragel`, `libreadline-dev`, `valgrind` (optional), `fzf` (optional).

## REPL Features

| Feature | Binding / Command |
|---------|-------------------|
| Up/Down arrows | Navigate command history |
| Left/Right arrows | Move cursor within line |
| Ctrl-A / Ctrl-E | Jump to start/end of line |
| Ctrl-R | Open fzf history browser (same as `.hist`) |
| `.help` | Show available commands and syntax |
| `.schema` | Show all labels with node counts |
| `.stats` | Show node/edge/property counts |
| `.history` | Browse command history with fzf |

History auto-loaded from `~/.cypher_history` on startup, saved on exit (capped at 1000 entries).

## Performance (50K nodes, -O2, FSM executor)

| Query pattern | Estimate |
|---------------|----------|
| Label index lookup | 10-70 µs |
| Label + WHERE str eq | <10 µs |
| Edge traversal (label + expand) | ~200 µs |
| Full scan (50K nodes, no index) | ~20-50 ms |
| --scan mode (34MB mmap) | ~60 ms total, ~2 ms actual scan |

## File Inventory

| File | Purpose | Lines |
|------|---------|-------|
| `cypher_lexer.rl` | Ragel tokenizer (28 keywords) | 352 |
| `cypher_parser.c` | Recursive-descent AST builder | 650 |
| `cypher_parser.h` | Token/AST types, graph API, result API | 180 |
| `cypher_graph.c` | Mutation handler, sidecar importer (mmap'd), execution dispatch | 703 |
| `cypher_repl.c` | CLI: readline interactive, piped, batch, dot-commands, Ctrl-R | 441 |
| `graph_store.c` | Storage engine, 4 indexes, prop_count fix, fp interning, trigram | 700 |
| `graph_store.h` | Node/edge/prop structs, index types, public API | 180 |
| `graph_exec.c` | FSM executor, multi-hop, property/trigram index lookup | 592 |
| `graph_exec.h` | FSM API, JIT placeholder | 34 |
| `graph_jit.c` | AST→C code generator, gcc runner, dlopen loader | 395 |
| `graph_jit.h` | JIT API | 28 |
| `graph_scan.c` | mmap'd sidecar direct query (--scan mode) | 95 |
| `graph_scan.h` | Scan API | 11 |
| `vendor/klib/khashl.h` | Vendored hash table library (MIT) | 482 |
| `vendor/klib/kvec.h` | Vendored dynamic array library (MIT) | 90 |

## Key Bug Fixes

| Bug | Fix | Impact |
|-----|-----|--------|
| O(n²) property scans | Added `prop_count` per node, stop after own properties | 50K nodes: 2.9GB RSS → ~75MB |
| Filepath duplication | `fp_table` interning via khashl + `strdup` | 50K copies → 37 unique, saves ~12MB |
| Arena realloc UAF in fp_table | Separate heap allocation for filepath strings | Eliminated crash |
| Truncated pointer in fp_table | Changed from `gs_node_range_t.off` (uint32) to `fpt_t` (pointer-sized) | Fixed on 64-bit |
| parse_set memory leak | `parse_expression()` consumed `=` → replaced with `parse_atom()+parse_postfix()` | 5 AST nodes (1520 bytes) |
| result_limit_skip leak | Rows beyond limit/skip now freed before truncation | Variable |
| Robin Hood hash corruption | Replaced with klib linear probing | Correct label lookups |
