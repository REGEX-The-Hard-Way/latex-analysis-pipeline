# Cypher Implementation: Critical Analysis & Implementation Plan

## Architecture Assessment

The codebase implements a custom Cypher-like graph query engine:

```
cypher_lexer.rl → cypher_parser.c → [graph_jit / graph_exec + graph_store] → results
```

### What Works Well
- `graph_store.c` storage engine is solid: struct-of-arrays, CSR edges, bitmap labels, arena allocator, klib indexes
- Ragel scanner for `--scan` mode (`graph_scan.rl`) — ~70ms for 34MB mmap'd JSON
- Basic CREATE/MATCH/RETURN/WHERE/SET/DELETE/MERGE queries execute correctly (36 smoke tests pass)
- Recursive descent parser (`cypher_parser.c`) handles the supported grammar subset cleanly

### Critical Issues Found

| # | Issue | Severity | Effort |
|---|-------|----------|--------|
| 1 | ~50% dead code: `exec_match_return()` + helpers never called | High | Low |
| 2 | 9 pairs of duplicated functions between cypher_graph.c and graph_exec.c | High | Medium |
| 3 | JIT compiler (`graph_jit.c`) is a fragile runtime hack on the hot path | High | Low |
| 4 | Newline→semicolon breaks multi-line queries | High | Low |
| 5 | 34-way strcmp chain for keyword dispatch instead of Ragel patterns | Medium | Medium |
| 6 | UNWIND execution is broken | Medium | Medium |
| 7 | Bug: `'-' '--'` lexer pattern matches `---` instead of being dead code | Low | Low |
| 8 | Documentation references `cypher_parser.rl` which doesn't exist | Low | Low |
| 9 | Multiple strcpy() buffer overflow risks | Medium | Low |
| 10 | Only positive-path tests (grep -qF) | Medium | Medium |
| 11 | ORDER BY is O(n²) bubble sort | Low | Low |
| 12 | 200-row hard limit spread across 3 files | Low | Low |

### Duplicated Functions (9 pairs)
- `match_label2` ↔ `fsm_match_label`
- `match_props2` ↔ `fsm_match_props`
- `eval_bool` ↔ `fsm_eval_where`
- `extract_labels` ↔ `fsm_extract_labels`
- `extract_props` ↔ `fsm_extract_props`
- `emit_cell` ↔ `fsm_emit_cell`
- `result_sort` ↔ inline bubble sort
- `result_distinct` ↔ inline dedup
- `result_limit_skip` ↔ inline SKIP/LIMIT

## Implementation Plan

### Phase 0: Cleanup (Week 1)

**Step 0.1** — Remove dead code: Delete `exec_match_return()` and helpers from `cypher_graph.c`
**Step 0.2** — Fix newline handling: Consume newlines as whitespace, not semicolons
**Step 0.3** — Disable JIT by default: Set `g_jit_enabled = 0`
**Step 0.4** — Remove dead `'-' '--'` lexer pattern

### Phase 1: Consolidation (Week 2)

**Step 1.1** — Replace 34-strcmp chain with Ragel keyword patterns in lexer
**Step 1.2** — Unify execution path: Single executor instead of two
**Step 1.3** — Fix UNWIND execution

### Phase 2: Hardening (Weeks 3-4)

**Step 2.1** — Buffer safety audit (strcpy → snprintf where missing)
**Step 2.2** — Replace bubble sort with qsort()
**Step 2.3** — Make row limit configurable
**Step 2.4** — Add error-path tests
**Step 2.5** — Proper error recovery in parser

### Phase 3: Production (Weeks 5-6, optional)

**Step 3.1** — Convert parser to Ragel (`cypher_parser.rl`)
**Step 3.2** — Full negative test suite

### Non-Goals (Deferred)
- UNION/Multi-part queries
- Pattern comprehensions
- CALL/YIELD procedures
- Enterprise features (ACLs, replication)
- Python/Rust bindings
