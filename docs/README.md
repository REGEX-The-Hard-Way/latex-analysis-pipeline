# Documentation Index

## Core Architecture

| Document | Description |
|----------|-------------|
| [ARCHITECTURE.md](ARCHITECTURE.md) | Pipeline architecture: tokenizer, macro expander, sentence splitter |
| [TOKEN_TYPES.md](TOKEN_TYPES.md) | Complete list of 150+ supported LaTeX token types |
| [GRAPH_STORE_ARCHITECTURE.md](GRAPH_STORE_ARCHITECTURE.md) | High-performance in-memory storage engine design (CSR, bitmap labels, arena) |
| [RAGEL_GUIDE.md](RAGEL_GUIDE.md) | Ragel for C programmers: patterns, EMIT_BLOCK, scanners, context tracking |
| [Cypher.g4](Cypher.g4) | openCypher ANTLR4 reference grammar (793 lines) |

## Cypher Query Engine

| Document | Description |
|----------|-------------|
| [CYPHER_ARCHITECTURE.md](CYPHER_ARCHITECTURE.md) | Engine architecture: lexer, parser, storage, executor, file inventory |
| [CYPHER_SYNTAX.md](CYPHER_SYNTAX.md) | Full Cypher syntax reference with query examples |
| [CYPHER_GAP_ANALYSIS.md](CYPHER_GAP_ANALYSIS.md) | Grammar coverage: 62 supported, 3 partial, 19 missing |
| [Cypher_Implementation_Guide.md](Cypher_Implementation_Guide.md) | Developer guide: how to add features, known limitations, test structure |
| [IMPLEMENTATION_STATUS.md](IMPLEMENTATION_STATUS.md) | Current status dashboard with 200-test metrics |
| [CYPHER_CRITICAL_ANALYSIS.md](CYPHER_CRITICAL_ANALYSIS.md) | Critical code review and cleanup plan |
| [CYPHER_DEMO_HISTORY.cypher](CYPHER_DEMO_HISTORY.cypher) | Runnable Cypher query examples for the REPL |
| [Cypher_Quick_Start.md](Cypher_Quick_Start.md) | Quick-start guide for the Cypher REPL |

## Macro Expansion

| Document | Description |
|----------|-------------|
| [macro_expander.md](macro_expander.md) | Macro expander usage and supported macros |
| [MACRO_EXPANSION_PLAN.md](MACRO_EXPANSION_PLAN.md) | Bug-fix and feature roadmap for macro expansion |
| [ANALYZING_MACRO_EXPANDER.md](ANALYZING_MACRO_EXPANDER.md) | In-depth macro expander analysis |

## Planning & Roadmaps

| Document | Description |
|----------|-------------|
| [GOLD_STANDARD_ROADMAP.md](GOLD_STANDARD_ROADMAP.md) | Path to production-grade LaTeX processing |
| [DATA_MOD_ROADMAP.md](DATA_MOD_ROADMAP.md) | Data model evolution plan |
| [IGRAPH_INTEGRATION.md](IGRAPH_INTEGRATION.md) | Graph-based cross-document analysis plan (igraph) |
| [LEAN4_PROOF_ROADMAP.md](LEAN4_PROOF_ROADMAP.md) | Formal verification integration plan (Lean4) |

## Analysis & Audit

| Document | Description |
|----------|-------------|
| [DEAD_CODE_REPORT.md](DEAD_CODE_REPORT.md) | Audit of unused and dead code |
| [ISSUES_ANALYSIS.md](ISSUES_ANALYSIS.md) | Upstream issue tracking and analysis |
| [DEBUG_NOTES.md](DEBUG_NOTES.md) | Development debugging notes |
| [ZOEKT_LIBRARY_ANALYSIS.md](ZOEKT_LIBRARY_ANALYSIS.md) | Text search library analysis (Zoekt) |

## Competitive Analysis

| Document | Description |
|----------|-------------|
| [Memgraph_Analysis.md](Memgraph_Analysis.md) | Competitive analysis: Memgraph vs our engine |
| [Cypher_Enterprise_Analysis.md](Cypher_Enterprise_Analysis.md) | Enterprise positioning and market analysis |

## Getting Started

```bash
# Install dependencies
bash scripts/install_deps.sh

# Build the scanner
cd scanner
make scanner
./scanner.out sound1.tex tex > output.tok

# Build and run Cypher engine
cd cypher
make
./cypher_repl.out --sidecar ../sidecar.json
```

## Key Concepts

1. **Hierarchical Tokenization** — Tokens form a tree via `parent_id` chains, no separate AST needed
2. **Ragel State Machines** — All hot-path scanning uses compiled DFAs (lexer, scanner, text extractor)
3. **Sidecar Output** — `.tok`, `.sent`, and `.json` files are independent and recomputable from source
4. **Cypher Query Engine** — In-memory graph database queries the token tree directly
5. **EMIT_BLOCK / EMIT_TOKEN** — Structural blocks recursively scan interiors; leaf tokens carry type+offset+length
