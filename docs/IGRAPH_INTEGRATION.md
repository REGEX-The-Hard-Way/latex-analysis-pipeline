# igraph Integration Plan

## Motivation

The LaTeX Analysis Pipeline currently produces flat `.tok` output: a list of tokens with
`parent_id` chains forming a tree per file. These per-file trees are disconnected —
there is no cross-document graph that captures how papers relate through shared equations,
citations, symbols, or definitions.

[igraph](https://igraph.org/) is a high-performance graph library (C core with Python/R
bindings) that can represent and query graphs with millions of nodes and edges. Integrating
igraph enables:

1. **Citation graphs** — papers as nodes, `\cite` relationships as edges
2. **Equation overlap graphs** — papers sharing identical or structurally similar equations
3. **Symbol usage graphs** — papers connected by shared symbols/variables
4. **Derivation dependency graphs** — chains of equations within a single paper
5. **Cross-document reference validation** — resolve `\ref{...}` across included files
6. **Subgraph isomorphism** — find papers that use the same mathematical structure

The script `scripts/install_deps.sh` already clones and builds igraph from source.

---

## Graph Models

### Model A: Document Relationship Graph

**Purpose:** Find papers that are related via citations, shared equations, or shared symbols.

```
Nodes: Paper (one per .tex file)
Edges:
  - CITES: paper A cites paper B  (weight = citation count)
  - SHARES_EQUATION: papers A and B have ≥1 identical equation (weight = count)
  - SHARES_SYMBOL: papers A and B use the same non-trivial symbol (weight = count)
  - SHARES_BIBENTRY: papers A and B cite the same reference key
```

**Output:** A weighted, undirected (mostly) multi-graph.

### Model B: Token Tree → Property Graph

**Purpose:** Query the internal structure of papers and their relationships.

```
Node types: Paper, Token (equation, citation, label, symbol, definition)
Edge types:
  Paper ──CONTAINS──> Token
  Token ──PARENT_OF──> Token (parent_id chain)
  Token ──CITES──> Token (citation key → bibliography entry)
  Token ──EQUIVALENT_TO──> Token (structural hash match across papers)
  Token ──DEFINES──> Token (variable definition → math token)
```

**Output:** A directed, typed multi-graph suitable for Neo4j import.

### Model C: Equation Flow Graph

**Purpose:** Capture derivation structure within a single paper.

```
Nodes: Equation tokens (ordered by position in paper)
Edges:
  Equation_i ──USES_SYMBOL_FROM──> Equation_j  (j < i, symbol first defined in j)
  Equation_i ──TRANSFORMS_TO──> Equation_{i+1}  (sequential in same derivation block)
```

**Output:** A directed acyclic graph (DAG) per paper.

---

## Integration Architecture

### Phase 1: C Library Integration (Month 1-2)

Create `include/graph_builder.h` and `include/graph_builder.c`:

```c
#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include <igraph.h>
#include <stdint.h>

typedef struct {
    igraph_t          graph;
    igraph_vector_t   vertex_types;    // 0=Paper, 1=Equation, 2=Citation, etc.
    igraph_strvector_t vertex_labels;  // "paper://hep-th/0301022", "eq://0xABCDEF", etc.
    igraph_vector_t   edge_types;      // 0=CITES, 1=SHARES_EQUATION, etc.
    igraph_vector_t   edge_weights;    // Weight for weighted queries
} latex_graph_t;

// Build a per-paper token tree graph from .tok data
latex_graph_t *graph_from_tok(const char *tok_data, size_t len);

// Build a document relationship graph from multiple .tok files
latex_graph_t *graph_from_corpus(const char *const *tok_files, size_t n_files);

// Query: find all papers that share a specific equation hash
igraph_vector_int_t *graph_find_equation_cluster(latex_graph_t *g, uint32_t eq_hash);

// Export to GraphML for visualization
int graph_export_graphml(latex_graph_t *g, const char *path);

// Export to CSV edge list for Neo4j import
int graph_export_edgelist(latex_graph_t *g, const char *path);

// Cleanup
void graph_destroy(latex_graph_t *g);

#endif
```

### Phase 1.1: Linking igraph

Update `scanner/Makefile` to include igraph:

```makefile
IGRAPH_CFLAGS = $(shell pkg-config --cflags igraph)
IGRAPH_LIBS   = $(shell pkg-config --libs igraph)

scanner: scanner.rl
	ragel -m -G2 scanner.rl
	$(CC) -O2 scanner.c main.c murmur3.c json_escape.c \
	      ../include/regex_util.c ../include/graph_builder.c \
	      -o scanner.out -lm $(IGRAPH_CFLAGS) $(IGRAPH_LIBS)
```

### Phase 1.2: Token Tree → igraph

The token tree in `.tok` output maps naturally to an igraph:

```c
// For each .tok file:
// 1. Parse tokens into an array
// 2. Create an igraph with n_tokens + 1 vertices (extra vertex = "root")
// 3. For each token, add edge: parent_id → token_id
// 4. Annotate vertices with attributes: type, offset, length, text_hash

igraph_t g;
igraph_vector_int_t edges;
igraph_vector_int_init(&edges, 0);

// Add root node
igraph_add_vertices(&g, 1, 0);

// token_id and parent_id are Murmur3 hashes — use as vertex names
// Build a map: hash → igraph vertex index
igraph_vector_t token_hashes;
// For each token:
//   igraph_add_vertices(&g, 1, 0)
//   VECTOR(edges)[e++] = parent_index;
//   VECTOR(edges)[e++] = token_index;

igraph_add_edges(&g, &edges, 0);
```

### Phase 2: Python Bindings via igraph (Month 2-3)

```python
# python/latex_analysis_pipeline/graph.py
import igraph as ig

class LaTeXGraph:
    """Python wrapper around the C graph builder."""

    def __init__(self):
        self.graph = ig.Graph(directed=True)

    def add_tok_file(self, path: str):
        """Load a .tok file into the graph."""
        ...

    def add_corpus(self, paths: list[str]):
        """Load multiple .tok files with cross-document edges."""
        ...

    def find_equation_clusters(self, min_share: int = 2):
        """Find equations shared across multiple papers."""
        ...

    def find_citation_communities(self):
        """Community detection on citation graph."""
        ...

    def export_neo4j(self, path: str):
        """Export as CSV files for Neo4j import."""
        ...

    def export_graphml(self, path: str):
        """Export as GraphML for Gephi/VisiData."""
        ...
```

### Phase 3: Cross-Document Analysis Pipeline (Month 3-4)

Build a standalone tool `graph_analyzer` that:

1. **Input:** Directory of `.tok` files (or raw `.tex` files + scanner)
2. **Process:**
   - Scan all files → `.tok`
   - Parse `.tok` → igraph
   - Compute cross-document edges:
     - Equation deduplication via structural hashing
     - Citation key matching
     - Shared symbol detection
   - Run graph algorithms
3. **Output:**
   - GraphML/CSV export
   - Summary statistics (connectivity, clusters, hubs)
   - Report of cross-document relationships

---

## Graph Algorithms

### 1. Equation Deduplication via Structural Hashing

**Input:** All equation tokens across all documents.

**Algorithm:**
```python
def structural_hash(eq_text: str) -> str:
    """
    Normalize an equation and produce a structural hash:
    1. Strip whitespace, line breaks
    2. Replace all identifiers (\\alpha, x, y, etc.) with generic placeholders
    3. Hash the resulting pattern
    """
    # Example: "\\frac{x}{2} = \\alpha(y)" → "\\frac{VAR}{NUM} = VAR(VAR)"
    #                                       → sha256(...)
    pass
```

**Edge creation:** Connect all equations (across documents) that share the same
structural hash. Weight = count of shared equations.

### 2. Citation Graph Construction

**Input:** All `\cite{key}` tokens across all documents.

**Algorithm:**
```python
# Build a bipartite graph: Paper → CitationKey
# Then project to: Paper → Paper (via shared citations)

papers = {}   # filepath_id → Paper node
cites = {}    # citation key → Citation node

for token in all_tokens:
    if token.type == 'cite':
        for key in extract_citation_keys(token.text):
            papers[token.filepath_id].add_edge(key)  # Paper ——> Citation

# Projection: Paper A —— Paper B if they share ≥N citations
```

### 3. Community Detection

Using igraph's community detection algorithms:
- **Leading eigenvector** for large citation networks
- **Louvain** for modularity-based clustering on equation overlap graphs
- **Walktrap** for hierarchical structure discovery

### 4. Centrality Measures

Identify influential papers:
- **Degree centrality:** Most-cited papers
- **Betweenness centrality:** Papers bridging different research communities
- **PageRank:** Influence propagation through citation network

---

## Graph Export Formats

### GraphML (for Gephi)

```xml
<?xml version="1.0" encoding="UTF-8"?>
<graphml xmlns="http://graphml.graphdrawing.org/xmlns">
  <key id="type" for="node" attr.name="type" attr.type="string"/>
  <key id="label" for="node" attr.name="label" attr.type="string"/>
  <graph edgedefault="directed">
    <node id="n0">
      <data key="type">paper</data>
      <data key="label">hep-th/0301022</data>
    </node>
    <node id="n1">
      <data key="type">equation</data>
      <data key="label">\frac{x}{2}=\alpha(y)</data>
    </node>
    <edge source="n0" target="n1">
      <data key="type">contains</data>
    </edge>
  </graph>
</graphml>
```

### Neo4j CSV (for import)

```csv
# nodes_paper.csv
id:ID,label:string,type:string
"paper://hep-th/0301022","hep-th/0301022","paper"
"paper://hep-th/0301023","hep-th/0301023","paper"

# nodes_equation.csv
id:ID,text:string,hash:string,type:string
"eq://a1b2c3","\frac{x}{2}=\alpha(y)","struct_sha256_abc","equation"

# edges_contains.csv
:START_ID,:END_ID,:TYPE
"paper://hep-th/0301022","eq://a1b2c3","CONTAINS"
```

### Cypher Queries

```cypher
// Find all papers that share structurally identical equations
MATCH (p1:Paper)-[:CONTAINS]->(eq:Equation)<-[:CONTAINS]-(p2:Paper)
WHERE p1 <> p2 AND eq.structural_hash IS NOT NULL
RETURN p1.label, p2.label, eq.structural_hash, count(*) AS shared
ORDER BY shared DESC

// Find the most central papers in the citation graph
MATCH (p:Paper)-[r:CITES]->(q:Paper)
RETURN p.label, count(r) AS out_degree
ORDER BY out_degree DESC
LIMIT 10

// Find communities (Louvain via igraph, then label propagation)
CALL igraph.louvain()  // hypothetical procedure
```

---

## Performance Considerations

| Operation | Expected Scale | Time |
|-----------|----------------|------|
| Build per-paper token tree | 10K tokens/paper | <1ms |
| Build corpus citation graph | 100K papers, 5M edges | ~10s |
| Equation deduplication | 500K equations | ~30s |
| Community detection (Louvain) | 100K nodes | ~5s |
| GraphML export | 100K nodes, 5M edges | ~20s |

**Memory:** igraph uses flat arrays internally. A graph with 100K nodes and 5M edges
requires approximately:
- Vertex attributes: 100K × (4 + 8 + 8) ≈ 2 MB
- Edge list: 5M × 2 × 4 bytes ≈ 40 MB
- Edge weights/attributes: 5M × 8 bytes ≈ 40 MB
- **Total: ~100 MB** — acceptable for a single-machine corpus analysis.

---

## Build Dependencies

igraph is already built by `scripts/install_deps.sh`:

```bash
# In scripts/install_deps.sh:
git clone https://github.com/igraph/igraph
cd igraph && mkdir build && cd build
cmake .. && make && sudo make install
```

Post-install check:
```bash
pkg-config --cflags --libs igraph
# Expected: -I/usr/local/include/igraph -L/usr/local/lib -ligraph
```

---

## Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| igraph C API changes | Low | Medium | Pin igraph version, use stable API subset |
| Large graphs exceed memory | Medium | Medium | Add streaming/batched processing mode |
| Equation structural hashing is too aggressive | Medium | Medium | Use adjustable normalization level |
| Citation key matching is fragile | High | Medium | Normalize keys (lowercase, strip spaces) |
| `.tok` format changes | Low | Medium | Define a stable graph-building API separate from parser |

---

## Success Criteria

1. **Single-paper graph:** `graph_from_tok("output.tok")` produces a valid igraph
   matching the parent_id tree — all tokens connected, no orphaned nodes.

2. **Corpus graph:** `graph_from_corpus(directory)` produces a multi-document graph
   with correct CITES, SHARES_EQUATION, and SHARES_SYMBOL edges.

3. **Query:** `graph_find_equation_cluster(eq_hash)` returns ≥2 papers sharing the same
   equation within the HEP-TH 2003 corpus.

4. **Performance:** Building the full HEP-TH 2003 corpus graph (20K files) completes
   in under 60 seconds on a single machine.

5. **Export:** Neo4j CSV import files load without errors, and the cypher queries
   in this document produce non-empty results.
