# Graph Store vs SQLite — Critical Review

## What the Graph Store Is

A custom **in-memory graph query engine** (~6,500 lines of C + Ragel) implementing a Cypher-like query language. It loads the scanner's sidecar.json, builds indexes, and answers structural queries like "find all equations in this paper" or "what tokens are children of this math block".

---

## Architecture Summary

| Layer | Lines | Role |
|-------|-------|------|
| `cypher_lexer.rl` | 401 → 6,717 (generated) | Tokenizes Cypher queries via Ragel DFA |
| `cypher_parser.c` | 720 | Recursive-descent parser, 28 AST node types |
| `cypher_graph.c` | 607 | Mutation dispatch, variable binding, sidecar import |
| `graph_store.c` | 709 | Struct-of-arrays storage, hash indexes, text search |
| `graph_exec.c` | 884 | Goto-based FSM query interpreter, BFS, aggregates |
| `graph_jit.c` | 211 | JIT compiler (disabled, limited) |

**Data model**: Nodes (32B), CSR singly-linked edges (12B), columnar properties (8B each), value arena. Four `khashl` hash indexes (label, property key, edge type, trigram text). One of the four (`edge_idx`) is allocated but **never populated** — dead code.

---

## What It Does Well (Pros)

### 1. Extremely Fast for Structural Queries
- Label index: O(1) hash lookup → O(k) node list
- Trigram text search: O(rarest trigram) pruning + strstr verification
- Label filtering: 64-bit mask per node, single bitwise AND, no string comparison
- `--scan` mode: mmap'd JSON, 70ms for 34MB — zero import cost

### 2. Proper Graph Traversal
- CSR adjacency: edges form linked lists in global array, not per-node malloc
- Multi-hop (up to 8): chained via static hop state array
- Variable-length BFS: collects nodes within depth [min, max] via static queue
- Node expansion: `edge_heads[node] → csr_walk` traverses outgoing edges

### 3. Cypher Query Language
- 62 of 81 openCypher features supported
- MATCH, RETURN, WHERE, ORDER BY, LIMIT, SKIP, DISTINCT
- CREATE, DELETE, SET, MERGE, REMOVE
- Aggregation: COUNT, SUM, AVG, MIN, MAX
- 200-test suite (positive-path)

### 4. Tight Integration with Scanner Pipeline
- `cypher_graph_load_sidecar()` directly parses `sidecar.json`
- Parent-child relationships loaded as `PARENT_OF` edges
- Filepath interning via hash table (deduplicates 49,500 copies to ~1,000)
- Token type becomes a graph label — queryable like `MATCH (n:equation) RETURN n`

### 5. Zero External Dependencies for Core
- libc + Ragel (build-time only) for lexer/parser/storage/execution
- klib vendored (2 header files, MIT)
- No database server, no driver, no ORM

---

## What It Doesn't Do Well (Cons vs SQLite)

### 1. Entirely In-Memory — No Persistence
The graph store has **no disk-backed storage**. After loading sidecar.json, all data lives in malloc'd arrays. If the process exits, everything is lost. There's no checkpoint, no WAL, no snapshot, no serialization.

**SQLite**: WAL mode gives atomic durability. Database survives process restart. `VACUUM INTO` for backups. This is table-stakes for any data pipeline that processes 1M+ papers incrementally.

### 2. No Disk-Aware Indexing
All four indexes (label, property key, edge type, text) are in-memory hash tables pointing to in-memory arrays. There's no B-tree, no page cache, no partial I/O. For 50K nodes (~2MB data), this is fine. For 50M nodes (~2GB), the entire dataset must fit in RAM.

**SQLite**: B-tree indexes spill to disk. A 100GB database with a 1GB page cache is fast and efficient. The working set, not the full dataset, determines memory usage.

### 3. No Concurrency
Single-threaded, no locks, static global buffers. The FSM executor uses global variables (`gs_sort_rows`, `gs_sort_ncols`). The BFS queue is a static `uint32_t[2048]`. Two queries cannot run concurrently.

**SQLite**: Multi-reader, single-writer. WAL mode allows concurrent reads during writes. Connection pooling for multi-threaded access.

### 4. No Composite / Value-Level Indexes
The property index maps `key_hash → [node_ids]` — it only indexes property KEYS, not values. A query like `WHERE offset > 1000` does a full scan of all nodes with that property key, performing O(N) filter checks.

**SQLite**: `CREATE INDEX idx_off ON tokens(offset)` gives O(log N) B-tree lookup on exact values and range scans.

### 5. Missing Operation Coverage
- **DISTINCT is O(N²)** — nested loops comparing every row pair. For 10,000 result rows, that's 50M comparisons.
- **Edge traversal by position is O(degree)** — random access into an adjacency list requires walking a singly-linked chain. No array-based random access.
- **No DELETE cascade** — deleting a node doesn't remove incoming edges. `DETACH DELETE` does O(E) global scan.
- **No edge type index** — edge type filtering happens at traversal time, not at plan time.

**SQLite**: DISTINCT via hash table or sort-uniq is O(N log N). Random access via rowid is O(1). Foreign key cascades are declarative.

### 6. Parser and Execution Model Limitations
- **Interpreted, not compiled**: the FSM walks the AST at runtime. Every WHERE evaluation recurses through the expression tree per candidate node.
- **No query optimization**: no cost-based join ordering. No predicate pushdown. No index selection.
- **JIT is disabled and broken**: `graph_jit.c` is dead code. It generates Ragel FSMs that operate on raw JSON, not the graph store. It uses `system()` to call ragel and gcc at runtime — a security and portability disaster.
- **No prepared statements**: every query is parsed fresh.

**SQLite**: Bytecode VM with query planner. Prepared statements avoid re-parsing. `EXPLAIN QUERY PLAN` for debugging.

### 7. Hard Coded Limits
| Limit | Value | Consequence |
|-------|-------|-------------|
| `GS_MAX_LABELS` | 64 | Silently ignored when exceeded |
| `MAX_HOPS` | 8 | Multi-hop paths truncated |
| `MAX_TOKENS` | 1,024 | Queries exceeding this silently fail |
| `MAX_STR` | 256 | Identifiers, strings truncated |
| `MAX_ROWS` | 10,000 | Result sets truncated |
| `vresults[1024]` | 1,024 | Variable-length path results truncated |
| `vbuf[2048]` | 2,048 | BFS queue silently overflows |

**SQLite**: Default limits are in millions (SQLITE_MAX_VARIABLE_NUMBER=999, SQLITE_MAX_COLUMN=2000). Tunable at compile time.

### 8. Maintenance Burden
- **6,500 lines** of hand-rolled C across 11 files
- Custom lexer (Ragel), custom parser (recursive descent), custom storage engine (SoA), custom executor (goto FSM)
- No standard tooling: no EXPLAIN, no query profiler, no backup command, no migration support
- Two test suites (200 positive-path tests, 7 benchmarks) — no negative assertions, no regression coverage for edge cases
- The `CYPHER_CRITICAL_ANALYSIS.md` documents 12 issues that were fixed; the codebase has accumulated technical debt from rapid iteration

**SQLite**: 150K+ lines, but maintained by a dedicated team since 2000. `EXPLAIN`, `.dump`, `.schema`, `sqlite3_analyzer`. The test suite (the TCL-based one, not the public one) has orders of magnitude more coverage.

---

## When the Graph Store Makes Sense

1. **You need Cypher syntax specifically** — SQL can express the same queries but the syntax is more verbose for path patterns like `(a)-[:PARENT_OF*1..3]->(b)`.
2. **Your entire dataset fits in RAM** — up to ~500K nodes, ~2M edges, ~2GB total.
3. **Queries are read-heavy** — you load once from sidecar.json, then run many queries.
4. **You're doing graph traversal** (variable-length paths, multi-hop) and want a dedicated API.
5. **You need zero-dependency deployment** — no SQLite shared library to link.

## When SQLite Makes More Sense

1. **You need persistence** — data survives process restart, incremental processing possible.
2. **You're processing 1M+ papers** — cannot fit the entire corpus in RAM.
3. **You need value-level or composite indexes** — `WHERE offset BETWEEN 100 AND 200` should use an index.
4. **You need concurrent access** — multiple processes reading sidecar data simultaneously.
5. **You need standard tooling** — `sqlite3` CLI for ad-hoc queries, `.dump` for backup, `EXPLAIN` for debugging.
6. **You need ACID** — atomic batch inserts with rollback on error.

---

## Recommendation: Keep Both

The graph store has a clear niche: **fast in-memory graph queries over a single paper's token tree.** For interactive REPL use (`cypher_repl.out`) and single-paper analysis, it's excellent. The Cypher syntax is genuinely more ergonomic for graph patterns than SQL.

For the **million-document pipeline**, SQLite is the right choice:
- Sidecar data is already imported into SQLite by `load_tokens.py`
- `build_graph.py` and `phase_c_chains.py` already query SQLite
- Multi-process concurrency (scanner + analyzer running in parallel)
- Incremental processing (process 10K papers, checkpoint, continue)
- Value-level indexes on `offset`, `parent_id`, `type` enable fast lookups

The two can coexist: the graph store for interactive exploration and the Cypher REPL; SQLite for the batch pipeline. The `cypher_graph_load_sidecar()` function could be extended to also populate a SQLite database, giving both worlds.

---

## Scorecard

| Criterion | Graph Store | SQLite |
|-----------|-------------|--------|
| Persistence | ✗ | ✓ |
| RAM-only operation | ✓ Fast | ✓ With `:memory:` |
| Disk-backed | ✗ | ✓ |
| Concurrency | ✗ | ✓ (WAL) |
| Index types | 3 (label, prop key, text) | B-tree, arbitrary columns |
| Value-level index | ✗ | ✓ |
| Composite index | ✗ | ✓ |
| Query language | Cypher (62/81) | SQL (full) |
| Graph traversal | ✓ Native | ~ Requires recursive CTE |
| Query planning | ✗ | ✓ Cost-based |
| Prepared statements | ✗ | ✓ |
| EXPLAIN | ✗ | ✓ |
| ACID | ✗ | ✓ |
| Test coverage | 200 pos-path | SQLite TCL suite |
| Code size | ~6,500 lines | ~150K lines |
| Dependencies | libc only | libsqlite3 (~1MB) |
| Maintainer | Us | SQLite team (25 years) |
