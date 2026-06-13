# LaTeX Analysis Pipeline Documentation

Welcome to the documentation for the LaTeX Analysis Pipeline.

## Quick Links

| Document | Description |
|----------|-------------|
| **[Project README](../README.md)** | Overview, installation, and usage |
| **[Architecture Guide](ARCHITECTURE.md)** | Detailed architecture documentation |
| **[Token Types Reference](TOKEN_TYPES.md)** | Complete list of supported token types |
| **[Macro Expander](macro_expander.md)** | Macro expansion documentation |
| **[Macro Expansion Plan](MACRO_EXPANSION_PLAN.md)** | Bug-fix and feature roadmap for macro expander |
| **[igraph Integration](IGRAPH_INTEGRATION.md)** | Plan for graph-based cross-document analysis |
| **[Debug Notes](DEBUG_NOTES.md)** | Development debugging notes |
| **[Issues Analysis](ISSUES_ANALYSIS.md)** | Upstream issue tracking and analysis |
| **[Dead Code Report](DEAD_CODE_REPORT.md)** | Audit of unused and dead code |
| **[Gold Standard Roadmap](GOLD_STANDARD_ROADMAP.md)** | Path to production-grade processing |

## Getting Started

```bash
# Build
cd scanner
make scanner

# Run
./scanner.out input.tex tex > output.tok
```

## Key Concepts

1. **Hierarchical Tokenization**: Tokens form a tree via `parent_id` chains
2. **Ragel State Machines**: Generated scanners for efficient parsing
3. **Sidecar Output**: `.tok` and `.sent` files for modular downstream processing
4. **Graph Analysis**: `.tok` token trees convert to igraph graphs for cross-document queries

## Components

| Component | Purpose |
|-----------|---------|
| `scanner.rl` | Main LaTeX tokenizer |
| `sent_split.rl` | Sentence segmentation |
| `multi_analyzer/` | Chainable text processing |
| `macro_expander.c/h` | Token-at-a-time macro expansion engine |
| `regex_util` | Common regex utilities |
| `graph_builder` | igraph-based cross-document graph construction (planned) |

## Project Structure

```
├── docs/
│   ├── README.md              - Documentation index
│   ├── ARCHITECTURE.md        - Architecture guide
│   ├── TOKEN_TYPES.md         - Token types reference
│   ├── MACRO_EXPANSION_PLAN.md - Macro expander bug-fix roadmap
│   ├── IGRAPH_INTEGRATION.md  - Graph analysis integration plan
│   ├── DEAD_CODE_REPORT.md    - Dead code audit
│   ├── GOLD_STANDARD_ROADMAP.md - Production-grade roadmap
│   ├── macro_expander.md      - Macro expander usage
│   ├── DEBUG_NOTES.md         - Development debugging notes
│   └── ISSUES_ANALYSIS.md     - Issues analysis
├── tests/
│   ├── README.md              - Test documentation
│   ├── test_issues.py         - Integration tests
│   ├── test_macro_expander.py - Macro expander unit tests
│   ├── validation_tests.py    - Cross-reference validation
│   └── benchmark_runner.py    - Performance benchmarks
├── scanner/                   - Main implementation
│   ├── scanner.rl             - Main tokenizer Ragel source
│   ├── latex.rl               - 150+ LaTeX environment definitions
│   ├── sent_split.rl          - Sentence segmentation
│   ├── main.c                 - File traversal and orchestration
│   ├── Makefile               - Build system
│   └── multi_analyzer/        - Chainable text processing pipeline
├── include/                   - Shared libraries
│   ├── macro_expander.c/h     - Macro expansion engine
│   ├── regex_util.c/h         - Regex utilities
│   └── old/                   - Archived upstream code
├── python/
│   └── latex_analysis_pipeline/
│       └── __init__.py        - Python bindings
├── scripts/
│   └── install_deps.sh        - Dependency installer (Ragel, igraph)
├── benchmarks/                - Performance benchmark results
├── examples/                  - Example inputs/outputs
├── sandbox/                   - Experimental/research features
└── tests/                     - Test suite
```

## Output Formats

- **Token format**: Structured records with byte offsets and types (`.tok`)
- **Sentence format**: One sentence per line (`.sent`)
- **Graph format**: GraphML, Neo4j CSV (planned, via igraph integration)
- **JSON format**: Structured JSON with `children` arrays (planned)

For detailed architecture information, see [ARCHITECTURE.md](ARCHITECTURE.md).