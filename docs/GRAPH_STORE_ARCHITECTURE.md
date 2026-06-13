# Graph Store Architecture: 2M-Node Ragel-Powered Engine

## Target

2 million papers (nodes), 10 million edges, in-memory, sub-millisecond queries.
**50% faster than memgraph** on equivalent queries.

## Current Bottlenecks (65K node benchmark)

| Operation | Ops/sec | Issue |
|-----------|---------|-------|
| Node creation | 183K/s | `calloc` on monolithic 300MB struct |
| Label filter | 30M/s | Good — `strcmp` on contiguous `char[256]` |
| Property filter | 10M/s | Inner loop over up to 8 props per node |
| Rel traversal | O(N×R) | No adjacency index — linear scan of ALL relationships |

## Architecture

```
Cypher Query (string)
     ↓
cypher_lexer.rl  → tokens            ← Ragel lexer (exists)
     ↓
cypher_parser.c  → AST               ← Recursive descent (exists)
     ↓
graph_compile.rl → Ragel FSM         ← NEW: query compiled to state machine
     ↓
graph_exec.c     → result rows       ← NEW: FSM executed over data
         ↑
Storage Layer (graph_store.c)
  nodes: struct-of-arrays (32B/node)
  edges: CSR adjacency (12B/edge)
  labels: hash → sorted uint32[]
  props: columnar pool + dict compression
  text: trigram bitmap index
```

### Key Innovation: Ragel Query Compiler

Instead of an interpreted query engine (memgraph uses pipeline operators with virtual dispatch), each query is **compiled into a monolithic Ragel state machine at query time**, then executed in a single pass over the data.

Eliminates: operator dispatch overhead, virtual calls, intermediate materialization, query plan interpretation.

## Phase 1: Storage Engine (`graph_store.h/c`)

### Node Layout (32 bytes)

```
typedef struct {
    uint32_t first_edge;   // CSR start index
    uint16_t edge_count;   // out-degree
    uint16_t label_count;
    uint32_t labels_off;   // offset into label-set pool
    uint32_t props_off;    // offset into property row
} gs_node_t;
```

- 2M nodes × 32B = 64MB
- Flat array, mmap'd, zero-copy access
- Labels stored as a flat pool of uint32 hashes (no string indirection per filter)

### Edge Layout — CSR (12 bytes)

```
typedef struct {
    uint32_t dst;     // target node index
    uint32_t type;    // edge type hash
    uint32_t next;    // next edge from same source (singly-linked list in array)
} gs_edge_t;
```

- 10M edges × 12B = 120MB
- O(degree) traversal: walk the linked list starting at `first_edge`
- No per-edge lock, no MVCC, no Delta

### Property Store — Columnar

```
key_hash[N]   uint32    // hash of property key (e.g., "title")
val_offset[N] uint32    // offset into value heap
value_heap    char[]    // arena-allocated: "Hello World\02024\0..."
```

- Filter by property reads ONLY the `key_hash` column first
- Value comparison only on matched key_hash entries
- Value heap uses dictionary/delta compression for repeated values

### Label Index — Hash → Sorted Node List

```
label_hash → uint32_t* [n]   — sorted array of node indices
```

- O(1) lookup by label, O(k) for result set of size k
- Multiple labels combined via sorted-merge

## Phase 2: Index Layer (`graph_index.h/c`)

Three open-addressing hash maps (Robin Hood style):

| Index | Key | Value |
|-------|-----|-------|
| Label index | `hash(label_name)` | sorted `uint32_t*` node list |
| Property index | `hash(label_hash, key_hash, value_hash)` | sorted `uint32_t*` node list |
| Edge-type index | `hash(type_name)` | sorted `uint32_t*` edge list |

Set operations on node lists use SIMD-capable `uint64_t[]` bitmaps for AND/OR.

## Phase 3: Ragel Query Compiler (`graph_compile.rl`)

Compiles an AST into a `.rl` file, then invokes Ragel to produce a C file, which is compiled and `dlopen`'d.

### Example Compilation

```
Input:  MATCH (a:Paper)-[r:CITES]->(b:Paper)
        WHERE a.year > 2020 AND b.title CONTAINS 'quantum'
        RETURN a.title, b.title

Generated Ragel FSM:
  main := (
    'LABEL_SCAN' @{ start_scan("Paper"); }
    'FILTER_YEAR' @{ if (prop_lt_or("year",2020)) fgoto reject; }
    'EXPAND_CITES' @{ target = expand_rel("CITES"); }
    'FILTER_TITLE' @{ if (!prop_contains("title","quantum")) fgoto reject; }
    'EMIT' @{ emit(prop_get("title"), prop_get_other("title")); fgoto next; }
    'REJECT' @{ fgoto next; }
  );
```

The state machine is a single C function — one `goto` table lookup per transition, no virtual calls, no allocations on the hot path.

## Phase 4: Execution Engine (`graph_exec.c`)

Three strategies, auto-selected:

| Strategy | When | Mechanism |
|----------|------|-----------|
| Index scan | Label + property indexes exist | Load sorted list from index, FSM filters inline |
| Expand | Relationship traversal | Start from index scan, walk CSR adjacency |
| Full scan | No usable index | Scan all nodes, FSM runs every row |

**Bitmap acceleration:** AND/OR between multiple index result sets computes bitwise ops on `uint64_t[]` bitmaps (64 nodes per word). 2M nodes = 31,250 words = 250KB bitmap.

## Phase 5: Trigram Text Index (`graph_text.rl`)

For CONTAINS/STARTS/ENDS queries:

- At insert time: extract trigrams from property value
- Store as a `uint64_t[]` bitmap (one bit per trigram of 3 chars)
- At query time: decompose search string into trigrams, AND the bitmaps
- Candidate nodes get verified with `strstr`

**The trigram extractor IS a Ragel state machine** — scans the property string and emits trigram bits.

## Memory Budget (2M nodes, 10M edges)

| Component | Size |
|-----------|------|
| Node headers (32B × 2M) | 64 MB |
| Edge CSR (12B × 10M) | 120 MB |
| Property key hash column (4B × 6M) | 24 MB |
| Property offset column (4B × 6M) | 24 MB |
| Value heap | 200 MB |
| Label index | 50 MB |
| Property index | 100 MB |
| Bitmaps (scratch) | 20 MB |
| **Total** | **~600 MB** |

Memgraph for same data: ~2GB. **3.3× less memory.**

## Performance Targets vs Memgraph

| Metric | Memgraph | Target | Speedup |
|--------|----------|--------|---------|
| Memory/node | 204B | 32B | 6.4× |
| Memory/edge | 154B | 12B | 12.8× |
| Total memory | ~2GB | ~600MB | 3.3× |
| Node scan (labels) | 5M/s | 50M/s | 10× |
| Property filter | 2M/s | 20M/s | 10× |
| Rel expansion | 3M/s | 100M/s | 33× |
| Query latency | ~5ms | <1ms | 5× |
| Import speed | 200K/s | 2M/s | 10× |

## Implementation Order

| Step | Files | Lines | Depends on |
|------|-------|-------|------------|
| 1. Storage engine | `graph_store.h/c` | 400 | — |
| 2. Index layer | `graph_index.h/c` | 250 | Step 1 |
| 3. Query compiler | `graph_compile.rl` | 350 | Steps 1,2 |
| 4. Execution engine | `graph_exec.c` | 200 | Steps 1-3 |
| 5. Text index | `graph_text.rl` | 150 | Steps 1,2 |
| 6. Wire into REPL | Replace `cypher_graph.c` | 100 | Steps 1-5 |
