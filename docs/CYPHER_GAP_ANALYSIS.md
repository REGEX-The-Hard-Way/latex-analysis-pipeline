# Cypher Grammar Gap Analysis

Comparison of our implementation against the openCypher grammar (`docs/Cypher.g4`, 793 lines).

## Scope

We implement an openCypher subset targeting analytical queries over a LaTeX token graph (140K+ nodes). The grammar is parsed by a recursive-descent parser (`cypher_parser.c`) consuming tokens from a Ragel lexer (`cypher_lexer.rl`).

## Legend

| Symbol | Meaning |
|--------|---------|
| ✅ | Fully supported (parse + execute) |
| ⚠️ | Parsed but not executed / partial execution |
| 🔶 | Lexer token exists, parser doesn't use it |
| ❌ | Not implemented |

---

## 1. Query Structure

| openCypher Construct | Status | Notes |
|---------------------|--------|-------|
| `oC_Cypher` (statement list) | ✅ | Single query per invocation |
| `oC_Union` | ❌ | `UNION`/`UNION ALL` not parsed |
| `oC_MultiPartQuery` (WITH) | ⚠️ | Parsed, basic pipelining. Does not actually pipe intermediate results |
| `oC_SinglePartQuery` | ✅ | MATCH + RETURN, or mutation + RETURN |
| `oC_StandaloneCall` (CALL) | ❌ | Procedure calls not supported |

## 2. Reading Clauses

| Construct | Status | Notes |
|-----------|--------|-------|
| `MATCH` | ✅ | Single pattern per query |
| `OPTIONAL MATCH` | ⚠️ | Parsed, `OPTIONAL` keyword consumed. Always treated as required MATCH |
| `MATCH` with multiple patterns (comma-separated) | ❌ | Only first pattern processed |
| `UNWIND` | ❌ | Token exists (`TOK_UNION`) but no parse/exec |

## 3. Updating Clauses

| Construct | Status | Notes |
|-----------|--------|-------|
| `CREATE` (node) | ✅ | Labels, properties, variable binding |
| `CREATE` (relationship) | ✅ | Single edge: `(a)-[:TYPE]->(b)` |
| `SET` (property = value) | ✅ | Parsed + executed. Appends; first-match-wins on read |
| `SET` (variable = value) | ❌ | Not supported |
| `SET` (+= for list append) | ❌ | Not supported |
| `SET` (variable :Label) | ❌ | Label assignment via SET |
| `DELETE` | ✅ | Clears labels + properties on node |
| `DETACH DELETE` | ✅ | Also removes incident edges |
| `MERGE` | ⚠️ | Parsed (AST_MERGE). Not executed |
| `MERGE ... ON MATCH SET` | ❌ | Not parsed |
| `MERGE ... ON CREATE SET` | ❌ | Not parsed |
| `REMOVE` | ❌ | Token exists (`TOK_REMOVE`), not parsed |

## 4. Patterns

| Construct | Status | Notes |
|-----------|--------|-------|
| `(var:Label)` node pattern | ✅ | Variable, labels, properties |
| `(var:Label1:Label2)` multi-label | ✅ | AND semantics |
| `(var)` anonymous node | ✅ | All nodes (full scan) |
| Relationship: `-[:TYPE]->` | ✅ | Right-directed |
| Relationship: `<-[:TYPE]-` | ✅ | Left-directed |
| Relationship: `--` (any type) | ✅ | |
| Relationship: `-[:T1\|T2]->` (type union) | ❌ | Pipe-separated types |
| `[*1..3]` variable-length paths | ❌ | Range literals not supported |
| `[*]` any-length paths | ❌ | |
| Pattern comma-separated | ❌ | `MATCH (a), (b)` |
| `(a)-[:R]->(b)-[:S]->(c)` multi-hop | ❌ | Only single-hop relationships. `oC_PatternElementChain` not iterated |
| Pattern with variable assignment: `p = (a)-->(b)` | ❌ | |

## 5. Expressions

| Construct | Status | Notes |
|-----------|--------|-------|
| Property lookup: `var.key` | ✅ | |
| `AND` / `OR` | ✅ | |
| `NOT` | ✅ | |
| `XOR` | ❌ | Token exists, not parsed |
| `=` (equality) | ✅ | |
| `<>` (not equal) | ✅ | |
| `<`, `>`, `<=`, `>=` | ✅ | Numeric only |
| `IS NULL` / `IS NOT NULL` | ❌ | Token exists (`TOK_IS`, `TOK_NULL`), partial parse |
| `IN` (list membership) | ❌ | Token exists, not parsed |
| `STARTS WITH` | ❌ | Token exists (`TOK_STARTS`), not parsed |
| `ENDS WITH` | ❌ | Token exists (`TOK_ENDS`), not parsed |
| `CONTAINS` | ❌ | Token exists (`TOK_CONTAINS`), not parsed |
| `+`, `-`, `*`, `/`, `%`, `^` (arithmetic) | ❌ | Tokens exist, not in expression grammar |
| `[]` list indexing | ❌ | |
| `[a..b]` list slicing | ❌ | |

## 6. Literals

| Construct | Status | Notes |
|-----------|--------|-------|
| String (single/double quotes) | ✅ | |
| Integer (decimal) | ✅ | via TOK_INTEGER |
| Integer (hex: `0x...`) | ✅ | Lexer handles |
| Float | ✅ | |
| Boolean (`true`, `false`) | ⚠️ | Tokenized, not usable in expressions |
| `NULL` | ⚠️ | Tokenized, `IS NULL` partially parsed |
| Map literal: `{key: val}` | ✅ | In CREATE/SET context |
| List literal: `[1, 2, 3]` | ❌ | |
| Parameters: `$param` | ❌ | Token exists, not parsed |

## 7. RETURN / WITH Projections

| Construct | Status | Notes |
|-----------|--------|-------|
| `RETURN expr` | ✅ | |
| `RETURN expr AS alias` | ✅ | |
| `RETURN *` | ✅ | Returns node placeholder: `(:Label)` |
| `RETURN DISTINCT expr` | ✅ | Dedup executed |
| `ORDER BY expr [ASC/DESC]` | ✅ | Bubble sort |
| `SKIP n` | ✅ | |
| `LIMIT n` | ✅ | |
| `WITH` | ⚠️ | Parsed. Does not actually pipe intermediate results to next clause |
| Aggregation: `COUNT(*)` | ❌ | Token exists |
| Aggregation: `COUNT(expr)`, SUM, AVG, MIN, MAX | ❌ | Not implemented |

## 8. Functions

| Construct | Status | Notes |
|-----------|--------|-------|
| User-defined functions | ❌ | Not supported |
| `EXISTS { pattern }` | ❌ | Token exists, not parsed |
| `COALESCE`, `toInteger`, `toString` etc. | ❌ | Scalar functions |
| `ALL`, `ANY`, `NONE`, `SINGLE` (predicates) | ❌ | Not implemented |

## 9. Other Constructs

| Construct | Status | Notes |
|-----------|--------|-------|
| `CASE WHEN ... THEN ... ELSE ... END` | ❌ | Tokens exist, not parsed |
| List comprehension: `[x IN list WHERE ...]` | ❌ | |
| Pattern comprehension | ❌ | |
| `CALL` procedure | ❌ | |
| `YIELD` | ❌ | |
| Comments: `// line`, `/* block */` | ✅ | Lexer skips |
| Semicolon query separator | ✅ | Consumed in parser |

---

## Summary Statistics

| Category | Supported | Partial | Missing |
|----------|-----------|---------|---------|
| Query structure | 1 | 1 | 2 |
| Reading clauses | 1 | 1 | 2 |
| Updating clauses | 4 | 1 | 6 |
| Patterns | 5 | 0 | 5 |
| Expressions | 5 | 0 | 11 |
| Literals | 3 | 2 | 3 |
| Projections | 7 | 1 | 2 |
| Functions | 0 | 0 | 6 |
| Other | 2 | 0 | 8 |
| **Total** | **28** | **6** | **45** |

## Priority Recommendations

### High Impact, Low Effort (do next)

1. **Multi-hop paths** — `MATCH (a)-->(b)-->(c)`. The parser already handles `oC_PatternElementChain`. Just need to iterate over `pat->list.items[]` pairs.
2. **IN operator** — `WHERE n.type IN ['math', 'cite']`. Lexer token exists, simple to add to `eval_bool`.
3. **CONTAINS / STARTS WITH / ENDS WITH** — String matching. Lexer tokens exist.
4. **COUNT(\*)** — Aggregate function. Simple row counter.

### High Impact, Medium Effort

5. **OPTIONAL MATCH** — Left-outer-join semantics. Needs NULL handling in emit.
6. **Multiple patterns** — `MATCH (a), (b)` — comma-separated patterns in one MATCH.
7. **WITH pipelining** — Actually pipe results from WITH to subsequent MATCH. Needs variable binding across clause boundaries.
8. **SET label** — `SET n:NewLabel` — add labels to existing nodes.

### High Impact, High Effort

9. **Variable-length paths** — `[*1..3]`, `[*]`. Requires BFS/DFS traversal.
10. **UNION** — Merge result sets from multiple queries.
11. **Arithmetic in expressions** — `WHERE n.a + n.b > 10`.
12. **MERGE execution** — Match-or-create semantics.
13. **List literals** — `WHERE n.type IN ['a', 'b']` needs list parsing.

### Low Priority (for analytical use case)

14. CASE WHEN expressions
15. Pattern comprehensions
16. CALL / YIELD procedures
17. REMOVE clause
18. XOR operator
19. List indexing/slicing
20. Parameters ($param)
