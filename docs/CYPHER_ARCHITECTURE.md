# Cypher Graph Engine Architecture

## Overview

A high-performance in-memory graph engine with a custom Cypher query language,
supporting 50K+ nodes and 23K+ edges with sub-millisecond label-indexed queries.
200-test suite validates 61 grammar features.

Two query modes:
- `--sidecar` — full graph store with indexes, edges, mutations (all Cypher)
- `--scan` — mmap'd direct JSON scan via Ragel scanner, zero import cost (~70ms for 34MB)

```
Cypher Query (string)
     ↓
cypher_lexer.rl       → tokens          ← Ragel lexer using priority scanner with keyword DFA
     ↓
cypher_parser.c       → AST             ← Recursive descent (+ error recovery, sync-on-keyword)
     ↓
cypher_graph.c        → dispatch        ← Mutation apply + execution routing
     ↓
graph_exec.c          → FSM executor    ← Goto-based state machine with BFS for var-len paths
     ↓                → JIT Ragel       ← AST→.rl→ragel→gcc→dlopen (disabled by default)
     ↓
graph_scan.rl         → direct mmap     ← Ragel single-pass scanner (--scan mode)
graph_text.rl         → trigrams        ← Ragel trigram extractor (Phase 5)
     ↓
graph_store.c         → result rows     ← Storage + 4 indexes + klib (Phases 1,2)
```

All hot-path scanning uses Ragel scanners compiled with `-G2` (goto-driven, expanded actions).

## File Inventory (5,497 total lines)

### Core Pipeline

| File | Lines | Purpose | Generated |
|------|-------|---------|-----------|
| `cypher_lexer.rl` | 380 | Ragel tokenizer: 40 keyword patterns via DFA, 12 punctuation, 6 literal types | → cypher_lexer.c |
| `cypher_parser.c` | 953 | Recursive-descent AST builder with prioritized expression tree (unary→power→term→arith→comparison→NOT→AND→XOR→OR) | — |
| `cypher_parser.h` | 122 | Token types (71), AST types (28 including AST_REMOVE), graph API, result API, MAX_ROWS/MAX_STR constants | — |
| `cypher_graph.c` | 607 | Thin wrapper: mutation execution (CREATE, SET, DELETE, MERGE, REMOVE, UNWIND), sidecar JSON loader, result API | — |
| `cypher_repl.c` | 448 | CLI: readline REPL, pipe mode, batch file mode, dot-commands (.help, .schema, .stats, .hist), Ctrl-R history browser | — |

### Storage Engine

| File | Lines | Purpose |
|------|-------|---------|
| `graph_store.c` | 709 | Struct-of-arrays: CSR edges, columnar props (backward-scan for SET overwrite), arena allocator, 4 klib hash indexes, trigram text index |
| `graph_store.h` | 176 | Node/edge/prop structs (32B/12B/8B), label table (132B×64), index types, public API |

### Execution Engine

| File | Lines | Purpose |
|------|-------|---------|
| `graph_exec.c` | 884 | FSM executor: goto-based state machine, multi-hop (up to 8), BFS variable-length paths, arithmetic evaluation including power/unary, aggregation (COUNT/SUM/AVG/MIN/MAX), ORDER BY (qsort), DISTINCT dedup, LIMIT/SKIP, CASE evaluation |
| `graph_exec.h` | 36 | FSM context struct, public API |
| `graph_jit.c` | 211 | Phase 3 JIT: AST→Ragel .rl generator, ragel+gcc runner, dlopen cache (disabled: `g_jit_enabled=0`) |
| `graph_jit.h` | 26 | JIT API, jit_query_func typedef |

### Scan & Text

| File | Lines | Purpose | Generated |
|------|-------|---------|-----------|
| `graph_scan.rl` | 93 | Ragel single-pass newline-delimited JSON scanner (--scan mode) | → graph_scan.c |
| `graph_text.rl` | 54 | Ragel 3-byte window trigram extractor | → graph_text.c |

### Testing & Benchmarking

| File | Lines | Purpose |
|------|-------|---------|
| `run_200_final.sh` | 298 | 200-test suite: 18 test groups covering CREATE, MATCH/WHERE, special ops, RETURN, ORDER BY, edges, aggregation, MERGE, DELETE, SET, UNWIND, error recovery, multi-line, edge cases, REMOVE, compound ops, expressions |
| `smoke_tests.sh` | 334 | Legacy 36-test smoke suite (superseded by 200-test suite) |
| `bench_exec.c` | 93 | FSM executor benchmark runner (7 query types, 100 rounds each) |
| `Makefile` | 73 | Build: ragel→c generation, gcc compile, debug/asan/valgrind targets |

### Vendored Libraries

| File | Lines | Purpose |
|------|-------|---------|
| `vendor/klib/khashl.h` | — | Linear-probing hash table (Fibonacci hashing, 75% load factor) — MIT licensed |
| `vendor/klib/kvec.h` | — | Dynamic array (C++ vector-style) — MIT licensed |

### Legacy / Test Grammar

| File | Purpose |
|------|---------|
| `test_grammar/` | Alternative lexer/parser implementation (development reference, not built) |

## Architecture Details

### Lexer (`cypher_lexer.rl`)

- 40 case-insensitive keyword patterns defined as Ragel named patterns (`KW_MATCH`, `KW_RETURN`, etc.)
- Priority scanner mode (`|*`) dispatches keywords via inline `=>` actions
- 12 punctuation tokens: `;,.():[]{}` plus `..` (TOK_DOTDOT) for variable-length paths
- 6 literal types: identifiers, single/double-quoted strings, backtick identifiers, integers (decimal/hex), floats
- Line comments `//` and block comments `/* */` silently consumed
- Newlines consumed as whitespace (not semicolons — multi-line queries supported)

### Parser (`cypher_parser.c`)

Recursive descent with precedence climbing:

```
parse_expression → parse_xor (OR)*
parse_xor → parse_and (XOR)*
parse_and → parse_not (AND)*
parse_not → (NOT)* parse_comparison
parse_comparison → parse_arithmetic (cmp parse_arithmetic)?
parse_arithmetic → parse_term (('+'|'-') parse_term)*
parse_term → parse_power (('*'|'/'|'%') parse_power)*
parse_power → parse_unary ('^' parse_unary)*
parse_unary → (('+'|'-'))* parse_atom parse_postfix
parse_atom → funccall | parens | ident | string | int | float | bool | null | list | CASE
parse_postfix → ('.' property | '[' index)*
```

Error recovery: syncs to next clause-starting keyword (MATCH, CREATE, RETURN, etc.) instead of skipping to semicolon.

### Storage Engine (`graph_store.c`)

Key design decisions:
- **Struct-of-arrays**: filter reads only relevant columns
- **CSR adjacency**: O(degree) edge traversal, no global scan
- **Bitmap labels**: 64-bit mask per node for O(1) label membership
- **Columnar properties**: properties searched backwards (last SET wins)
- **Flat arena**: single `realloc`-based buffer for property values
- **Filepath interning**: `strdup`'d strings via khashl, saves ~12MB
- **No MVCC, locks, or transactions**: single-writer, batch-load model
- **klib** (`khashl.h`, `kvec.h`): MIT-licensed, single-header

Four hash indexes: label, property key, edge type, trigram text.

### Execution Engine (`graph_exec.c`)

Goto-based state machine: `state_scan → state_filter → state_expand → state_expand_loop → state_emit_multi`

- **Multi-hop**: up to 8 hops via stack-based traversal (`hop_nids[]`, `hop_ej[]`, `hop_ec[]`)
- **Variable-length paths**: BFS from source node up to 2048 reachable nodes, collects results within [vmin, vmax] range
- **Arithmetic**: `+`, `-`, `*`, `/`, `%`, `^` (power), unary `-`, `+`
- **Comparisons**: `=`, `<>`, `<`, `>`, `<=`, `>=`, `IS NULL`, `IS NOT NULL`, `IN`, `CONTAINS`, `STARTS WITH`, `ENDS WITH`
- **Boolean**: `AND`, `OR`, `XOR`, `NOT`, `true`/`false` literals
- **Aggregation**: `COUNT(*)`, `SUM(expr)`, `AVG(expr)`, `MIN(expr)`, `MAX(expr)` — multi-aggregate in single query
- **CASE**: `CASE WHEN cond THEN val ELSE default END` evaluated in expressions
- **Post-processing**: ORDER BY (qsort, multi-key), DISTINCT (dedup), LIMIT/SKIP (with proper free)

## Performance

| Query pattern | Time |
|---------------|------|
| Label index lookup | 10-70 µs |
| Label + WHERE str eq | <10 µs |
| Edge traversal (label + expand) | ~200 µs (fixed 1-hop) |
| Full scan (50K nodes, no index) | ~20-50 ms |
| --scan Ragel (34MB mmap) | ~70 ms total |
| Variable-length BFS | Depends on path depth and fanout |

## Build System

```
make              # optimized build (-O2)
make debug        # debug build (-O0 -g)
make asan         # AddressSanitizer build
make test         # run legacy smoke tests (36 queries)
make valgrind     # valgrind memcheck on 4 query types
make bench        # build and run benchmark
make tools        # build data manipulation tools
make clean        # remove artifacts
```

Requires: `gcc`, `ragel` (7.x), `libreadline-dev`, `valgrind` (optional), `fzf` (optional).

## Key Bug Fixes History

| Bug | Fix | Impact |
|-----|-----|--------|
| O(n²) property scans | Added `prop_count` per node | 50K nodes: 2.9GB RSS → ~75MB |
| Filepath duplication | `fp_table` interning via khashl + `strdup` | 50K copies → 37 unique, saves ~12MB |
| Arena realloc UAF in fp_table | Separate heap allocation for filepath strings | Eliminated crash |
| Robin Hood hash corruption | Replaced with klib linear probing | Correct label lookups |
| SET property append-only | Property reader scans backwards (last-set wins) | SET overwrites correctly |
| Compound CREATE missed target node props | Added label/property loop for target node | Target nodes created properly |
| DISTINCT flag corrupted by union overlap | Moved DISTINCT to `rel.dir` (offset 24) | DISTINCT deduplication works |
| Function call parsing broken by paren expr | Reordered checks in parse_atom | SUM/AVG/MIN/MAX, CASE work |
| Multi-aggregate freed rows early | Two-pass: compute all then free once | COUNT+SUM+AVG+MIN+MAX in one query |
| Property parser infinite loop on malformed input | Added break on unrecognized token types | No hangs on bad braces |
| Newlines treated as semicolons | Changed to whitespace consumption | Multi-line queries work |
| 34-strcmp keyword dispatch | Replaced with Ragel keyword patterns in scanner | O(1) DFA dispatch |
| Bubble sort O(n²) for ORDER BY | Replaced with qsort() | O(n log n) | 
| AST_UNWIND/AST_CASE memory leak | Added cases to cypher_ast_free() | Valgrind clean |

## Grammar Coverage

61 features fully supported, 4 partial, 19 missing (see `CYPHER_GAP_ANALYSIS.md`).

**Test coverage**: 200 tests, all passing, valgrind clean, 0 build warnings.
