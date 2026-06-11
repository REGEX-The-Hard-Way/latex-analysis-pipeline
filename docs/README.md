# LaTeX Analysis Pipeline Documentation

Welcome to the documentation for the LaTeX Analysis Pipeline.

## Quick Links

- **[Project README](../README.md)** - Overview, installation, and usage
- **[Architecture Guide](ARCHITECTURE.md)** - Detailed architecture documentation
- **[Token Types Reference](TOKEN_TYPES.md)** - Complete list of supported token types
- **[Issues Analysis](ISSUES_ANALYSIS.md)** - GitHub issues analysis and TODO list

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

## Components

| Component | Purpose |
|-----------|---------|
| `scanner.rl` | Main LaTeX tokenizer |
| `sent_split.rl` | Sentence segmentation |
| `multi_analyzer/` | Chainable text processing |
| `regex_util` | Common regex utilities |

## Project Structure

```
├── docs/
│   ├── README.md           - Documentation index
│   ├── ARCHITECTURE.md     - Architecture guide
│   ├── TOKEN_TYPES.md      - Token types reference
│   └── ISSUES_ANALYSIS.md  - Issues analysis
├── tests/
│   ├── README.md           - Test documentation
│   └── test_issues.py      - Test cases
├── scanner/                - Main implementation
├── include/                - Shared libraries
└── ...
```

## Output Formats

- **Token format**: Structured records with byte offsets and types
- **Sentence format**: One sentence per line

For detailed architecture information, see [ARCHITECTURE.md](ARCHITECTURE.md).