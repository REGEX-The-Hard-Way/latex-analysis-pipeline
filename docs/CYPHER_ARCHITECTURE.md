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
graph_exec.c          → FSM executor    ← Goto-based state machine
     ↓
graph_store.c         → result rows     ← Storage + indexes
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

### Phase 2: Index Layer (`graph_store.h/c`)

Three hash indexes built once after batch loading, using `klib/khashl.h`.

| Index | Key | Value | Lookup |
|-------|-----|-------|--------|
| `label_idx` | `hash("LabelName")` | `{offset, count}` into contiguous node list | `gs_label_nodes()` — O(1) + O(k) emit |
| `prop_idx` | `hash("propertyKey")` | `{offset, count}` into contiguous node list | `gs_prop_key_nodes()` — narrows candidates for WHERE clauses |
| `edge_idx` | (reserved) | — | — |

Both indexes use `klib/khashl` (MIT-licensed, macro-based open-addressing hash with
Fibonacci hashing, 75% load factor). Node lists are stored in `klib/kvec` dynamic arrays
for automatic resize.

**Index selection logic** (in FSM executor):
1. If label is specified: use label index
2. If WHERE references a property: probe property index
3. Pick the index with fewer candidates
4. If no usable index: full scan

### Phase 3: Query Compiler (`graph_exec.h`)

The API provides a JIT-compilation path (`cypher_fsm_compile()`) that would:
1. Serialize an AST to a Ragel `.rl` file with query-specific FSM states
2. Run `ragel -G2` → produce C code
3. Run `gcc -shared -fPIC -O2` → produce `.so`
4. `dlopen` the `.so` and invoke the compiled query function

This eliminates all AST traversal, virtual dispatch, and interpreter overhead on
the query hot path. Currently a placeholder; the fallback is Phase 4 below.

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
     └──(has edges)──→ state_expand_loop ──(emit)──→ state_expand_loop
                                                     │
                                                     └──(no more edges)──→ state_scan
```

Each state does exactly one thing. Post-execution: ORDER BY (bubble sort),
DISTINCT (dedup), LIMIT/SKIP (truncate with proper memory freeing).

**Performance** (10K nodes, 200-round average, -O2):

| Query pattern | µs/query |
|---------------|----------|
| Label index lookup (38 rows) | 12.6 µs |
| Label index lookup (200 rows) | 63-69 µs |
| Edge traversal (label + expand) | 195 µs |
| ORDER BY + LIMIT (50 rows) | 507 µs |
| Full scan (10K nodes) | 20,777 µs |

Full scans are 500-5000× slower than indexed queries. Production queries
should always use label or property index filters.

## Build System

```
make          # optimized build (-O2)
make debug    # debug build (-O0 -g)
make asan     # AddressSanitizer build
make test     # run 15 smoke tests
make valgrind # valgrind memcheck on 4 query types
make bench    # build and run benchmark
make clean    # remove artifacts
```

Requires: `gcc`, `ragel`, `valgrind` (optional, for `make valgrind`).

## File Inventory

| File | Purpose | Lines |
|------|---------|-------|
| `cypher_lexer.rl` | Ragel tokenizer (28 keywords, strings, numbers) | 344 |
| `cypher_parser.c` | Recursive-descent AST builder | 596 |
| `cypher_parser.h` | Token/AST types, graph API, result API | 117 |
| `cypher_graph.c` | Mutation handler, sidecar importer, old interpreter | 679 |
| `cypher_repl.c` | CLI: interactive/piped/batch modes, dot-commands, history | 328 |
| `graph_store.c` | Storage engine, indexes, queries, stats | 520 |
| `graph_store.h` | Node/edge/prop structs, index types, public API | 153 |
| `graph_exec.c` | FSM executor, property index lookup, sort/distinct/limit | 402 |
| `graph_exec.h` | FSM API, JIT placeholder | 30 |
| `vendor/klib/khashl.h` | Vendored hash table library (MIT) | 482 |
| `vendor/klib/kvec.h` | Vendored dynamic array library (MIT) | 90 |

## Dependencies

- `gcc` — compilation
- `ragel` (7.x) — lexer state machine generation
- `klib` (`khashl.h`, `kvec.h`) — vendored in `vendor/klib/`, MIT license
- `valgrind` — optional, for memory checking
- `fzf` — optional, for `.history` REPL command
