# Cypher Implementation Guide

## Current State

| Metric | Value |
|--------|-------|
| Grammar features supported | 61 |
| Grammar features partial | 4 |
| Grammar features missing | 19 |
| Test coverage | 200 tests, 100% pass |
| Memory safety | Valgrind clean |
| Build warnings | 0 |
| Binary size | ~319KB |

## Architecture

```
cypher_lexer.rl      Ragel DFA lexer (40 keyword patterns, 12 punctuation, 6 literal types)
cypher_parser.c      Recursive descent parser (precedence climbing, expression tree)
cypher_graph.c       Mutation apply + execution dispatch + sidecar loader
graph_store.c        Struct-of-arrays storage + 4 klib hash indexes + arena allocator
graph_exec.c         Goto-based FSM executor (multi-hop, BFS var-len, aggregation, CASE)
graph_jit.c          JIT compiler (disabled: g_jit_enabled=0)
graph_scan.rl        Ragel mmap JSON scanner (--scan mode)
graph_text.rl        Ragel trigram extractor
```

See `CYPHER_ARCHITECTURE.md` for full details.

## Running

```bash
# Build
cd scanner/cypher && make

# Run 200-test suite
make test

# Valgrind checks
make valgrind

# Benchmark (needs sidecar.json)
make bench

# Debug build
make debug
```

## Where to Add New Features

### Adding a new keyword
1. `cypher_lexer.rl`: Add `KW_XXX = [Xx]...;` pattern and `=> { type = TOK_XXX; }` action before `id =>`
2. `cypher_parser.h`: Add `TOK_XXX` to `cypher_tok_type_t` enum

### Adding a new clause
1. `cypher_parser.c`: Add `parse_xxx()` function
2. Wire into `parse_clause()` switch
3. `cypher_parser.h`: Add `AST_XXX` to `cypher_ast_type_t` enum
4. `cypher_graph.c`: Add `apply_xxx()` or handler in `cypher_execute()`
5. `cypher_parser.c`: Add case to `cypher_ast_free()`

### Adding a new expression operator
1. `cypher_parser.c`: Add to `parse_atom()`, `parse_unary()`, `parse_power()`, etc.
2. `graph_exec.c`: Add evaluation to `fsm_eval_value()` and/or `fsm_eval_where()`
3. Add emit handling to `fsm_emit_cell()` if used in RETURN

### Adding a new aggregation function
1. `graph_exec.c`: Add to aggregation detection in `state_done`
2. The argument is emitted by `fsm_emit_cell()` for AST_FUNCALL (recursively)

## Key Design Decisions

- **Lexer uses Ragel keyword patterns**, not C string comparisons. 40 case-insensitive keyword patterns defined as Ragel named patterns with inline token-type actions.
- **Parser is recursive descent** with precedence climbing. Expression tree: unary → power → term → arithmetic → comparison → NOT → AND → XOR → OR.
- **Properties use backwards scan** — `gs_prop_str/num/int` search from last to first, so SET overwrites work correctly.
- **DISTINCT flag** stored in `a->rel.dir` (AST union offset 24) to avoid overlapping with `list.n` (offset 8) and `bin.op` (offset 0).
- **Function calls** checked BEFORE plain identifiers in `parse_atom()` to avoid `SUM(1)` being parsed as `SUM` (ident) + `(1)` (paren expr).
- **Aggregation is two-pass**: compute all aggregate values first, then free rows and emit a single aggregate row.
- **Variable-length paths** use BFS from source node, collecting reachable nodes within [vmin, vmax] range.
- **Newlines are whitespace**, not semicolons. Multi-line queries supported.
- **JIT disabled** (`g_jit_enabled=0`). Runtime ragel+gcc+dlopen is fragile and unnecessary for current performance targets.

## Known Limitations

1. **Union struct overlap**: AST nodes use a C union, so `list.n` overlaps with `bin.op`, `ival`, etc. New fields must be placed at non-conflicting offsets (see `rel.dir` for DISTINCT).
2. **No true outer join**: OPTIONAL MATCH parsed but treated as required.
3. **Type union**: executor matches only the first type in `-[:T1|T2]->`.
4. **WITH pipelining**: column aliases forwarded, but intermediate results not piped.
5. **200-row limit**: hardcoded in MAX_ROWS, applies to all queries.
6. **Bubble sort replaced**: ORDER BY now uses qsort (O(n log n)).
7. **COUNT/EXISTS are keyword tokens**: their `str` field is empty; parser hardcodes the name.
8. **CASE in RETURN emits as numeric**: currently outputs `%g` format, not string values.

## Test Suite Structure

`run_200_final.sh` has 18 test groups:
1. CREATE + RETURN (tests 1-10)
2. MATCH + WHERE comparisons (11-30)
3. WHERE special operators (31-45)
4. RETURN variants (46-58)
5. ORDER BY (59-72)
6. LIMIT/SKIP (73-78)
7. Edge traversal (79-95)
8. Aggregation (96-100)
9. MERGE (101-108)
10. DELETE (109-114)
11. SET (115-122)
12. UNWIND (123-132)
13. Error recovery (133-140)
14. Multi-line (141-145)
15. Edge cases (146-165)
16. REMOVE (166-173)
17. Compound operations (174-190)
18. Expression edges (191-200)
