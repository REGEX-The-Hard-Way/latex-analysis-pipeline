# Zoekt Integration & Library Evaluation

## Zoekt Latency Secrets (Applied to Our Graph Store)

Zoekt achieves sub-millisecond latency on 100G+ code indexes. Here are the
techniques directly applicable to the latex-analysis-pipeline graph store.

### 1. Positional Trigram Index (TEXT SEARCH)

Zoekt picks the **2 least-frequent trigrams** from a query string and checks
they occur at the correct distance, rather than intersecting all trigrams.

**Applicable to us:** Replace linear `strstr` scanning in the property text index.
Build a trigram bitmap per property value at insert time. At query time for
`CONTAINS` / `STARTS` / `ENDS`:

```
1. Decompose query string into trigrams
2. Pick the trigram with the fewest matching nodes (from index)
3. Verify candidates with distance check or full strstr
```

**Expected gain:** 10-100× faster CONTAINS queries (currently O(N × strstr)).

### 2. Graduated Cost Evaluation

Zoekt evaluates matches at increasing cost levels: `const` → `memory` → `content` → `regex`.
Cheap rejections never pay for expensive work.

**Applicable to us:**

| Level | Our equivalent | Cost |
|-------|---------------|------|
| const | Label bitmap (single `uint64_t & mask`) | ~1ns |
| memory | Property key hash match (O(1) table lookup) | ~10ns |
| content | String value comparison (`strcmp`) | ~100ns |
| text | Full-text trigram + `strstr` verify | ~1us |
| regex | Hybrid regex engine (if needed) | ~10us |

```c
// Current: always does strcmp for every property
if (!strcmp(val, expected)) match = 1;

// Zoekt-style: reject at cheapest level first
if (!(node.label_mask & paper_mask)) continue;           // cost: const
if (node.prop_hash[title_idx] != title_hash) continue;   // cost: memory
// Only THEN do string comparison
if (strcmp(prop_val, "quantum")) continue;                // cost: content
```

**Expected gain:** 3-5× faster property filtering (most nodes rejected at label/const level).

### 3. Delta-Compressed Posting Lists

Zoekt stores posting lists as varint delta-encoded: `[first, Δ1, Δ2, ...]`.
Posting lists average 10 bytes, 78% under 64 bytes.

**Applicable to us:** Our label index stores raw `uint32_t` arrays
(4 bytes per node ID). With sorted node IDs, delta encoding reduces to
1-3 bytes per entry for dense sets.

```
Current:   [0, 1, 2, 5, 7, 12, ...] × 4B = 4B per entry
Zoekt-style: [0, 1, 1, 3, 2, 5, ...] × varint = ~1.5B per entry average
```

**Expected gain:** 60% smaller indexes, better cache utilization for sequential scans.
Trade-off: slightly slower decode (but sequential — prefetch-friendly).

### 4. Query Simplification (Pre-Filtering)

Zoekt converts Repo/RepoId queries to `Const(true)` or `Const(false)` per shard
before any search work, skipping irrelevant shards entirely.

**Applicable to us:** When building a graph across many papers, pre-filter:

```c
// Before searching, check if any node in this "section" has the label
uint64_t section_mask = gs->section_label_mask[section_idx];
if (!(section_mask & paper_mask)) {
    // No Paper nodes in this section — skip entirely
    return 0;
}
```

We already have this partially with the label bitmap on each node.
Could extend to "macro-regions" (groups of 64 nodes sharing a summary mask).

### 5. mmap for Persistence

Zoekt mmaps the entire index file; the OS page cache handles I/O transparently.

**Applicable to us:** The graph store currently lives entirely in heap memory.
For persistence across sessions, allocate the node/edge/prop arrays via mmap
with a backing file, using `MAP_SHARED`:

```c
int fd = open("graph.dat", O_RDWR | O_CREAT, 0644);
ftruncate(fd, total_size);
gs->nodes = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
```

This gives us:
- Instant "load" (just mmap, no copying)
- Crash resilience (OS flushes dirty pages)
- Zero serialization overhead

### 6. Co-operative Yielding

Zoekt demotes long-running queries from interactive to batch queue after 5 seconds.

**Applicable to us:** Less relevant for single-user REPL, but useful if the
query engine is exposed as a service. A simple timeout + early termination
would suffice.

---

## Library Evaluation Summary

| Library | Benefit | Action |
|---------|---------|--------|
| **mimalloc** | **MEDIUM** | Test with `LD_PRELOAD` on corpus scan — zero code changes needed |
| **klib** | **LOW** | kvec reduces ~50 lines of dynamic-array boilerplate; khash is already equivalent |
| **GMP** | **NONE** | No arbitrary-precision math; igraph bundles mini-gmp |
| **OpenMPI** | **NONE** | Single-machine scale sufficient; `xargs -P` handles parallelism |
| **MKL** | **NONE** | No BLAS/LAPACK in project C code |
| **SIMD** | **NONE/LOW** | Ragel DFA is byte-at-a-time, hashes on short inputs, bitmap ops single-word |

### mimalloc Test Plan

```bash
# Build scanner as-is
cd scanner && make scanner

# Baseline
time ./scanner.out 2003 '.*\.tex$' > /dev/null

# With mimalloc
time LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libmimalloc.so ./scanner.out 2003 '.*\.tex$' > /dev/null

# For the graph store benchmark
cd cypher
gcc -O2 -I. -o bench_store graph_store.c bench_store_main.c -lmimalloc
```

Expected: 5-15% speedup on allocation-heavy workloads (graph store creation,
corpus scanning). The arena allocator in graph_store.c already handles
small-string fragmentation, so mimalloc's main impact would be on large
structural array allocations.

### Zoekt Techniques to Implement Now

| Priority | Technique | File | Effort |
|----------|-----------|------|--------|
| 1 | Graduated cost (const→memory→content) | `graph_store.c` | 30 min |
| 2 | Delta-compressed posting lists | `graph_store.c` | 1 hour |
| 3 | Trigram bitmap text index | `graph_text.rl` (new) | 2 hours |
| 4 | mmap-based persistence | `graph_store.c` | 1 hour |
| 5 | Query simplification/section masks | `graph_store.c` | 30 min |
