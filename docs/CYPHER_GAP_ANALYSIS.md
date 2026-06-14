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
| `oC_MultiPartQuery` (WITH) | ⚠️ | Parsed, executor detects WITH clause. Full pipelining needs variable binding across clauses |
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
| `SET` (variable :Label) | ✅ | `SET n:NewLabel` adds labels to existing node |
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
| `(a)-[:R]->(b)-[:S]->(c)` multi-hop | ✅ | Stack-based traversal in FSM |
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
| `IS NULL` / `IS NOT NULL` | ✅ | `WHERE n.key IS NULL` |
| `IN` (list membership) | ✅ | `WHERE n.key IN ['a', 'b']` |
| `STARTS WITH` | ✅ | `WHERE n.key STARTS WITH 'prefix'` (trigram index when built) |
| `ENDS WITH` | ✅ | `WHERE n.key ENDS WITH 'suffix'` (trigram index when built) |
| `CONTAINS` | ✅ | `WHERE n.key CONTAINS 'substr'` (trigram or --scan strstr) |
| `+`, `-`, `*`, `/` (arithmetic) | ✅ | `WHERE n.val + 5 > 10` |
| `%` (modulo) | ✅ | Parsed, not heavily tested |
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
| List literal: `[1, 2, 3]` | ✅ | Parsed + executed (IN operator) |
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
| `COUNT(*)` | ✅ | Single aggregate row |
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

## 10. Math AST (Scanner)

| Construct | Status | Notes |
|-----------|--------|-------|
| `math_op` (binary operators) | ✅ | `+`, `-`, `\times`, `\cdot`, `\pm`, `\div` |
| `math_rel` (relations) | ✅ | `=`, `\lt`, `\gt`, `\leq`, `\equiv`, `\approx` |
| `math_fn` (functions) | ✅ | `\sin`, `\cos`, `\log`, `\ln`, `\exp` |
| `math_greek` (Greek) | ✅ | `\alpha`–`\omega`, 24 letters |
| `math_num` (numbers) | ✅ | Digit sequences |
| `math_var` (variables) | ✅ | Single letters |
| `frac` → children (EMIT_BLOCK) | ✅ | Numerator/denominator as `braces` children |
| `sum`/`prod`/`lim`/`int` → children | ✅ | Limits as `math_sub`/`math_sup` children |
| Expression tree (CAS-style) | ❌ | Flat tokens. Operator precedence needs post-processing |
| Full expression grammar | ❌ | No `Expr → Term + Term` grammar |

---

## Summary Statistics

| Category | Supported | Partial | Missing |
|----------|-----------|---------|---------|
| Query structure | 2 | 1 | 1 |
| Reading clauses | 1 | 1 | 2 |
| Updating clauses | 5 | 1 | 5 |
| Patterns | 6 | 0 | 4 |
| Expressions | 10 | 0 | 3 |
| Literals | 4 | 2 | 2 |
| Projections | 8 | 1 | 1 |
| Functions | 0 | 0 | 6 |
| Other | 3 | 0 | 7 |
| Math AST (scanner) | 8 | 0 | 2 |
| **Core Cypher** | **39** | **6** | **31** |
| Data manipulation (tools/) | 2 | 0 | 5 |
| **Total** | **49** | **6** | **38** |

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
