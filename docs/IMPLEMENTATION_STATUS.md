# Cypher Implementation Status

## Test Suite

| Metric | Value |
|--------|-------|
| **Total tests** | 200 |
| **Passing** | 200 (100%) |
| **Build warnings** | 0 |
| **Valgrind** | Clean (0 leaks, 0 errors) |
| **Binary size** | ~319KB |

Run with: `make test` or `bash run_200_final.sh`

## Feature Status

### Fully Implemented (61 features)

**Query Structure**: Single/multi-part queries, statement lists, multi-clause

**Reading Clauses**: MATCH (single + comma-separated patterns), OPTIONAL MATCH (parsed, treated as required), UNWIND

**Updating Clauses**: CREATE (node + edge with target properties), SET (property = value + label assignment with overwrite), DELETE + DETACH DELETE, MERGE (create-or-match with properties and edges), REMOVE (property removal)

**Patterns**: Node patterns with variables/labels/properties, multi-label `:L1:L2`, anonymous `()`, directed edges `-[:TYPE]->` and `<-[:TYPE]-`, undirected `--`, type union `-[:T1|T2]->` (first type matched), variable-length `[*1..3]` and `[*]` (BFS), multi-hop up to 8, comma-separated patterns (cross-product)

**Expressions**: Property lookup `var.key`, list indexing `list[0]`, AND/OR/XOR/NOT, comparisons `= <> < > <= >=`, IS NULL/NOT NULL, IN list, CONTAINS/STARTS WITH/ENDS WITH, arithmetic `+ - * / %`, power `^`, unary `+ -`, parenthesized `(expr)`, boolean literals `true`/`false`, CASE WHEN expressions

**Literals**: Strings (single/double quoted), integers (decimal/hex), floats, negative numbers, booleans, NULL, maps, lists

**RETURN/WITH**: Column projection, AS aliasing, `RETURN *`, DISTINCT dedup, ORDER BY (qsort, multi-key, ASC/DESC), SKIP, LIMIT (configurable via MAX_ROWS=200)

**Aggregation**: COUNT(*), SUM(expr), AVG(expr), MIN(expr), MAX(expr) — multi-aggregate in single query

**Other**: Single/multi-line comments, semicolons, backtick identifiers, error recovery (sync-on-keyword), `.help`/`.schema`/`.stats`/`.hist` dot commands

### Partial (4 features)

| Feature | Status |
|---------|--------|
| WITH pipelining | Parsed; column aliasing forwarded. Full result piping not yet implemented |
| OPTIONAL MATCH | Parsed; treated as required MATCH (no outer-join NULL handling) |
| Type union `[:T1|T2]` | Parsed; executor matches first type only |
| Relationship properties | Parsed but not executed |

### Not Implemented (19 features)

| Feature | Priority |
|---------|----------|
| UNION / UNION ALL | High |
| Variable SET (`SET x = value`) | Medium |
| SET `+=` list append | Low |
| MERGE ON MATCH/CREATE SET | Medium |
| Pattern variable assignment `p = (a)-->(b)` | Low |
| List slicing `[a..b]` | Low |
| Parameters `$param` | Low |
| EXISTS pattern | Medium |
| ALL/ANY/NONE/SINGLE predicates | Low |
| List comprehension | Low |
| Pattern comprehension | Low |
| CALL/YIELD procedures | Low |
| COALESCE/toInteger/toString | Low |
| Other aggregation (collect, stdev, etc.) | Low |
| Unary NOT keyword in WHERE | Medium |
| Relationship properties in patterns | Medium |
| Full parser error messages | Medium |
| Transaction support | Low |
| Multi-statement queries | Low |

## Running Tests

```bash
# Run 200-test suite
make test

# Valgrind memory check
make valgrind

# Benchmark (requires sidecar.json)
make bench

# Debug build
make debug
```

## Performance Benchmarks

Run `make bench` with a sidecar.json file to measure:
- Label index lookup
- WHERE property equality
- Edge expansion
- Full scan with WHERE numeric
- ORDER BY with LIMIT
