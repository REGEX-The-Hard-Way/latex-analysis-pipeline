# Cypher Grammar Gap Analysis (Updated)

Comparison against the openCypher grammar (`docs/Cypher.g4`, 793 lines).

## Legend

| Symbol | Meaning |
|--------|---------|
| ✅ | Fully supported (parse + execute) |
| ⚠️ | Parsed but not executed / partial execution |
| ❌ | Not implemented |

---

## 1. Query Structure

| openCypher Construct | Status | Notes |
|---------------------|--------|-------|
| `oC_Cypher` (statement list) | ✅ | Single query per invocation |
| `oC_Union` | ❌ | `UNION`/`UNION ALL` not parsed |
| `oC_MultiPartQuery` (WITH) | ⚠️ | Parsed; column aliasing forwarded. Full pipelining pending |
| `oC_SinglePartQuery` | ✅ | MATCH + RETURN, or mutation + RETURN |
| `oC_StandaloneCall` (CALL) | ❌ | Procedure calls not supported |

## 2. Reading Clauses

| Construct | Status | Notes |
|-----------|--------|-------|
| `MATCH` | ✅ | Single or multiple comma-separated patterns |
| `OPTIONAL MATCH` | ⚠️ | Parsed; treated as required MATCH |
| `MATCH` with multiple patterns (comma-separated) | ✅ | Cross-product execution |
| `UNWIND` | ✅ | List expansion into rows |

## 3. Updating Clauses

| Construct | Status | Notes |
|-----------|--------|-------|
| `CREATE` (node) | ✅ | Labels, properties, variable binding |
| `CREATE` (relationship) | ✅ | Single edge with target node properties |
| `SET` (property = value) | ✅ | Parsed + executed; overwrites via backwards scan |
| `SET` (variable = value) | ❌ | Not supported |
| `SET` (+= for list append) | ❌ | Not supported |
| `SET` (variable :Label) | ✅ | `SET n:NewLabel` adds labels |
| `DELETE` | ✅ | Clears labels + properties on node |
| `DETACH DELETE` | ✅ | Also removes incident edges |
| `MERGE` | ✅ | Match-or-create with properties and edges |
| `MERGE ... ON MATCH SET` | ❌ | Not parsed |
| `MERGE ... ON CREATE SET` | ❌ | Not parsed |
| `REMOVE` | ✅ | Removes properties from nodes |

## 4. Patterns

| Construct | Status | Notes |
|-----------|--------|-------|
| `(var:Label)` node pattern | ✅ | |
| `(var:Label1:Label2)` multi-label | ✅ | AND semantics |
| `(var)` anonymous node | ✅ | Full scan |
| Relationship: `-[:TYPE]->` | ✅ | |
| Relationship: `<-[:TYPE]-` | ✅ | |
| Relationship: `--` (any type) | ✅ | |
| Relationship: `-[:T1\|T2]->` (type union) | ⚠️ | Parsed; first type matched |
| `[*1..3]` variable-length paths | ✅ | BFS traversal |
| `[*]` any-length paths | ✅ | Unbounded BFS |
| Pattern comma-separated | ✅ | Cross-product |
| `(a)-[:R]->(b)-[:S]->(c)` multi-hop | ✅ | Up to 8 hops |
| Pattern with variable assignment | ❌ | |

## 5. Expressions

| Construct | Status | Notes |
|-----------|--------|-------|
| Property lookup: `var.key` | ✅ | |
| `AND` / `OR` | ✅ | |
| `NOT` | ✅ | |
| `XOR` | ✅ | Full expression support |
| `=` (equality) | ✅ | |
| `<>` (not equal) | ✅ | |
| `<`, `>`, `<=`, `>=` | ✅ | |
| `IS NULL` / `IS NOT NULL` | ✅ | |
| `IN` (list membership) | ✅ | |
| `STARTS WITH` | ✅ | |
| `ENDS WITH` | ✅ | |
| `CONTAINS` | ✅ | |
| `+`, `-`, `*`, `/` (arithmetic) | ✅ | |
| `%` (modulo) | ✅ | |
| `^` (power) | ✅ | Right-associative |
| Unary `-`, `+` | ✅ | |
| Parenthesized `(expr)` | ✅ | |
| `[]` list indexing | ✅ | Parse support |
| `[a..b]` list slicing | ❌ | |
| CASE WHEN expression | ✅ | In RETURN and WHERE |

## 6. Literals

| Construct | Status | Notes |
|-----------|--------|-------|
| String (single/double quotes) | ✅ | |
| Integer (decimal) | ✅ | |
| Integer (hex: `0x...`) | ✅ | |
| Float | ✅ | |
| Negative numbers (-5) | ✅ | |
| Boolean (`true`, `false`) | ✅ | Usable in expressions |
| `NULL` | ✅ | `IS NULL`, equality |
| Map literal: `{key: val}` | ✅ | In CREATE/SET context |
| List literal: `[1, 2, 3]` | ✅ | IN, UNWIND |
| Parameters: `$param` | ❌ | |

## 7. RETURN / WITH Projections

| Construct | Status | Notes |
|-----------|--------|-------|
| `RETURN expr` | ✅ | Including arithmetic/power/unary |
| `RETURN expr AS alias` | ✅ | |
| `RETURN *` | ✅ | |
| `RETURN DISTINCT expr` | ✅ | |
| `ORDER BY expr [ASC/DESC]` | ✅ | qsort-based |
| `SKIP n` | ✅ | |
| `LIMIT n` | ✅ | |
| `WITH` | ⚠️ | Parsed; column forwarding only |
| `COUNT(*)` | ✅ | |
| `SUM(expr)`, `AVG(expr)`, `MIN(expr)`, `MAX(expr)` | ✅ | Multi-aggregate support |
| Other aggregation | ❌ | |

## 8. Functions

| Construct | Status | Notes |
|-----------|--------|-------|
| User-defined functions | ❌ | |
| `EXISTS { pattern }` | ❌ | |
| `COALESCE`, `toInteger`, `toString` | ❌ | |
| `ALL`, `ANY`, `NONE`, `SINGLE` | ❌ | |

## 9. Other Constructs

| Construct | Status | Notes |
|-----------|--------|-------|
| Comments: `// line`, `/* block */` | ✅ | |
| Semicolon query separator | ✅ | |
| Multi-line queries | ✅ | Newlines as whitespace |
| Backtick identifiers | ✅ | |
| List comprehension | ❌ | |
| Pattern comprehension | ❌ | |

## Summary

| Category | Supported | Partial | Missing |
|----------|-----------|---------|---------|
| Query structure | 2 | 1 | 1 |
| Reading clauses | 3 | 1 | 0 |
| Updating clauses | 7 | 0 | 4 |
| Patterns | 9 | 1 | 1 |
| Expressions | 18 | 0 | 1 |
| Literals | 8 | 0 | 1 |
| Projections | 10 | 1 | 1 |
| Functions | 1 | 0 | 6 |
| Other | 3 | 0 | 4 |
| **Total** | **61** | **4** | **19** |

**Test coverage**: 200 tests, all passing, valgrind clean, 0 build warnings.
