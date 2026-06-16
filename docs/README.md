# LaTeX Analysis Pipeline Documentation

## Quick Links

| Document | Description |
|----------|-------------|
| **[Project README](../README.md)** | Overview, installation, and usage |
| **[Architecture Guide](ARCHITECTURE.md)** | Detailed architecture documentation |
| **[Token Types Reference](TOKEN_TYPES.md)** | Complete list of supported token types |

### Cypher Query Engine

| Document | Description |
|----------|-------------|
| **[Cypher Architecture](CYPHER_ARCHITECTURE.md)** | Engine architecture, file inventory, expression tree, bug history |
| **[Cypher Syntax Reference](CYPHER_SYNTAX.md)** | Full Cypher syntax with examples |
| **[Cypher Gap Analysis](CYPHER_GAP_ANALYSIS.md)** | Grammar feature coverage (61 supported, 4 partial, 19 missing) |
| **[Cypher Implementation Guide](Cypher_Implementation_Guide.md)** | Developer guide: how to add features, known limitations |
| **[Implementation Status](IMPLEMENTATION_STATUS.md)** | Current status dashboard, 200 tests, performance benchmarks |
| **[Cypher Demo History](CYPHER_DEMO_HISTORY.cypher)** | Example Cypher queries for the REPL |

### Macro & Graph

| Document | Description |
|----------|-------------|
| **[Macro Expander](macro_expander.md)** | Macro expansion documentation |
| **[Macro Expansion Plan](MACRO_EXPANSION_PLAN.md)** | Bug-fix and feature roadmap for macro expander |
| **[igraph Integration](IGRAPH_INTEGRATION.md)** | Plan for graph-based cross-document analysis |
| **[Graph Store Architecture](GRAPH_STORE_ARCHITECTURE.md)** | High-performance storage engine design |
| **[Memgraph Analysis](Memgraph_Analysis.md)** | Competitive analysis vs Memgraph |
| **[Cypher Enterprise Analysis](Cypher_Enterprise_Analysis.md)** | Enterprise positioning |
| **[Zoekt Library Analysis](ZOEKT_LIBRARY_ANALYSIS.md)** | Text search integration |

### Planning & Analysis

| Document | Description |
|----------|-------------|
| **[Gold Standard Roadmap](GOLD_STANDARD_ROADMAP.md)** | Path to production-grade processing |
| **[Data Model Roadmap](DATA_MOD_ROADMAP.md)** | Data model evolution plan |
| **[Lean4 Proof Roadmap](LEAN4_PROOF_ROADMAP.md)** | Formal verification integration plan |
| **[Dead Code Report](DEAD_CODE_REPORT.md)** | Audit of unused and dead code |
| **[Debug Notes](DEBUG_NOTES.md)** | Development debugging notes |
| **[Issues Analysis](ISSUES_ANALYSIS.md)** | Upstream issue tracking and analysis |
| **[ANALYZING_MACRO_EXPANDER.md](ANALYZING_MACRO_EXPANDER.md)** | Macro expander analysis |
| **[Analyzing Macro Expander](ANALYZING_MACRO_EXPANDER.md)** | In-depth macro analysis |

### Reference

| Document | Description |
|----------|-------------|
| **[Cypher.g4](Cypher.g4)** | openCypher ANTLR4 reference grammar (793 lines) |
| **[Ragel Guide](RAGEL_GUIDE.md)** | Ragel for C programmers |

## Getting Started

```bash
# Build
cd scanner
make scanner

# Run
./scanner.out input.tex tex > output.tok

# Query with Cypher
cd cypher
make
./cypher_repl.out --sidecar ../sidecar.json
```

## Key Concepts

1. **Hierarchical Tokenization**: Tokens form a tree via `parent_id` chains
2. **Ragel State Machines**: Generated scanners for efficient parsing (lexer + scan + text)
3. **Sidecar Output**: `.tok` and `.sent` files for modular downstream processing
4. **Cypher Query Engine**: Embedded graph database queries the token tree directly
5. **Graph Analysis**: `.tok` token trees convert to igraph graphs for cross-document queries

## Components

| Component | Purpose | Lines |
|-----------|---------|-------|
| `scanner.rl` | Main LaTeX tokenizer | — |
| `sent_split.rl` | Sentence segmentation | — |
| `cypher_lexer.rl` | Cypher keyword DFA lexer | 380 |
| `cypher_parser.c` | Recursive descent AST builder | 953 |
| `graph_store.c` | Struct-of-arrays storage engine | 709 |
| `graph_exec.c` | Goto-based FSM executor | 884 |
| `cypher_repl.c` | Interactive REPL | 448 |
| `multi_analyzer/` | Chainable text processing | — |
| `macro_expander.c/h` | Token-at-a-time macro expansion engine | — |
| `regex_util` | Common regex utilities | — |
| `graph_builder` | igraph-based cross-document graph construction (planned) | — |

## Project Structure

```
├── docs/
│   ├── README.md                   - Documentation index (this file)
│   ├── ARCHITECTURE.md             - Architecture guide
│   ├── TOKEN_TYPES.md              - Token types reference
│   ├── CYPHER_ARCHITECTURE.md      - Cypher engine architecture
│   ├── CYPHER_SYNTAX.md            - Cypher syntax reference
│   ├── CYPHER_GAP_ANALYSIS.md      - Grammar coverage analysis
│   ├── Cypher_Implementation_Guide.md - Developer guide
│   ├── IMPLEMENTATION_STATUS.md    - Feature status dashboard
│   ├── Cypher.g4                   - openCypher reference grammar
│   ├── RAGEL_GUIDE.md              - Ragel for C programmers
│   ├── MACRO_EXPANSION_PLAN.md     - Macro expander roadmap
│   ├── IGRAPH_INTEGRATION.md       - Graph analysis plan
│   ├── DEAD_CODE_REPORT.md         - Dead code audit
│   ├── GOLD_STANDARD_ROADMAP.md    - Production-grade roadmap
│   └── ...
├── tests/
│   ├── README.md                   - Test documentation
│   ├── test_issues.py              - Integration tests
│   ├── test_macro_expander.py      - Macro expander unit tests
│   ├── validation_tests.py         - Cross-reference validation
│   └── benchmark_runner.py         - Performance benchmarks
├── scanner/
│   ├── scanner.rl                  - Main tokenizer Ragel source
│   ├── latex.rl                    - 150+ LaTeX environment definitions
│   ├── sent_split.rl               - Sentence segmentation
│   ├── main.c                      - File traversal and orchestration
│   ├── Makefile                    - Build system
│   ├── cypher/                     - Cypher query engine
│   │   ├── cypher_lexer.rl         - Keyword DFA lexer
│   │   ├── cypher_parser.c         - Recursive descent parser
│   │   ├── graph_store.c           - Storage engine
│   │   ├── graph_exec.c            - FSM executor
│   │   ├── Makefile                - Build + test + valgrind
│   │   └── run_200_final.sh        - 200-test suite
│   └── multi_analyzer/             - Chainable text processing pipeline
├── include/                        - Shared libraries
├── python/                         - Python bindings
├── scripts/                        - Utility scripts
├── benchmarks/                     - Performance results
├── examples/                       - Example inputs/outputs
└── sandbox/                        - Experimental features
```

## Output Formats

- **Token format (`.tok`)**: Structured records with byte offsets and types
- **Sentence format (`.sent`)**: One sentence per line
- **Sidecar format (`.json`)**: JSON lines, one per token, with parent_id chains
- **Cypher graph**: Loaded from sidecar.json into in-memory graph store
- **Graph format**: GraphML, Neo4j CSV (planned, via igraph integration)

For detailed architecture information, see [ARCHITECTURE.md](ARCHITECTURE.md).
