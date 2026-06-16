# latex-analysis-pipeline

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A high-performance LaTeX tokenizer, macro expander, sentence splitter, and embedded
Cypher graph query engine. Written in C with Ragel-generated state machines.

| Component | Purpose | Tech |
|-----------|---------|------|
| **Structural Tokenizer** | Parses LaTeX into a hierarchical token tree | Ragel, C |
| **Macro Expander** | Resolves `\newcommand`, `\def`, `\let` | C |
| **Sentence Splitter** | Segments token streams into sentences | Ragel, C |
| **Cypher Query Engine** | Queries the token tree with openCypher | Ragel, C, klib |
| **Multi-Analyzer** | Chainable text pipeline (tokenize→stem) | Ragel, C |

Documentation: [docs/](docs/README.md)

## Quick Start

```bash
# Install dependencies
bash scripts/install_deps.sh

# Build the scanner and sentence splitter
cd scanner
make scanner
make sent_split

# Build the Cypher query engine
cd cypher
make

# Tokenize a LaTeX file
cd scanner
./scanner.out sound1.tex tex > sidecar.tok

# Query the token tree
cd cypher
./cypher_repl.out --sidecar ../sidecar.json
```

## Cypher Query Engine

Embedded in-memory graph database with openCypher query support.
**200 tests**, **62 grammar features**, **0 build warnings**, **valgrind clean**.

```
cd scanner/cypher
make                    # optimized build (-O2)
make test               # 200-test suite
make valgrind           # memory safety check

# Interactive REPL
./cypher_repl.out

# Load token sidecar and query
./cypher_repl.out --sidecar ../sidecar.json

# Direct mmap scan (70ms for 34MB)
./cypher_repl.out --scan ../sidecar.json
```

Example queries:
```cypher
MATCH (eq:equation) RETURN eq LIMIT 5;
MATCH (eq:equation)-[:PARENT_OF]->(sub:math_sub) RETURN sub.text LIMIT 10;
MATCH (n:Token) RETURN COUNT(*), SUM(n.length), AVG(n.length);
MATCH (s:section)-[:PARENT_OF*1..3]->(t:Token) RETURN t.text LIMIT 5;
```

## Project Structure

```
latex-analysis-pipeline/
├── scanner/
│   ├── scanner.rl          Main LaTeX tokenizer (Ragel)
│   ├── latex.rl            150+ LaTeX environment definitions
│   ├── sent_split.rl       Sentence segmentation (Ragel)
│   ├── main.c              File traversal and orchestration
│   ├── murmur3.c           MurmurHash3
│   ├── json_escape.c       JSON output escaping
│   ├── file_mmap.c         Memory-mapped file I/O
│   ├── Makefile            Build: scanner, sent_split, macro_expander, cypher
│   ├── sound1.tex          Test fixture
│   ├── 2003/               1019 arXiv hep-th papers
│   ├── multi_analyzer/     Chainable text processing pipeline
│   └── cypher/             Embedded Cypher graph engine
│       ├── cypher_lexer.rl    Ragel keyword DFA lexer (40 keywords)
│       ├── cypher_parser.c    Recursive descent parser
│       ├── cypher_graph.c     Mutation execution + sidecar loader
│       ├── cypher_repl.c      Interactive REPL
│       ├── graph_store.c/h    Struct-of-arrays storage engine
│       ├── graph_exec.c/h     Goto-based FSM executor
│       ├── graph_jit.c/h      JIT compiler (disabled)
│       ├── graph_scan.rl      Ragel mmap JSON scanner
│       ├── graph_text.rl      Ragel trigram extractor
│       ├── run_200_final.sh   200-test suite
│       ├── Makefile           Build + test + valgrind targets
│       └── vendor/klib/       klib (khashl.h, kvec.h)
├── include/
│   ├── macro_expander.c/h  Macro expansion engine
│   └── regex_util.c/h      Regex utilities
├── docs/                    Documentation
│   ├── README.md            Documentation index
│   ├── ARCHITECTURE.md      Pipeline architecture
│   ├── CYPHER_*.md          Cypher engine documentation
│   └── Cypher.g4            openCypher reference grammar
├── tests/                   Python test suite
├── scripts/                 install_deps.sh, author_replace_children.sql
├── benchmarks/              Performance results
├── examples/                Example inputs/outputs
├── sandbox/                 Experimental features (benepar QA)
└── tools/                   Sidecar data manipulation tools
```

## Architecture

```
Raw .tex file
    ↓
[Memory-mapped file I/O]
    ↓
[Pass 1: Structural Tokenizer]
    EMIT_BLOCK: strip delimiters, rescan interior recursively
    EMIT_TOKEN: leaf tokens with type, offset, length, parent_id
    ↓
[Hierarchical Token Tree]         parent_id chains preserve containment
    ↓
[Pass 2: Cypher Graph Import]     sidecar.json → in-memory graph store
    ↓
[Pass 3: Cypher Query Engine]     openCypher queries over token tree
    ↓
[Pass 4: Macro Expansion]         \newcommand, \def, \let resolution
    ↓
[Pass 5: Sentence Segmentation]   English text extraction
```

## Output Format

Each token is a structured JSON record:

```json
{"filepath":"doc.tex","filepath_id":1425541370,"token_id":3328404677,
 "parent_id":1425541370,"offset":1,"length":71,"type":"equation",
 "text":"\\begin{equation}\n \\label{test}\n  \\frac{x}{2}=\\alpha(y)\n\\end{equation}"}
```

| Field | Type | Description |
|-------|------|-------------|
| `filepath` | string | Original file path |
| `filepath_id` | uint32 | Murmur3 hash of filepath |
| `token_id` | uint32 | Murmur3 hash of token text |
| `parent_id` | uint32 | Hash of containing block |
| `offset` | int | Byte offset in original file |
| `length` | int | Span length in bytes |
| `type` | string | Semantic token type |
| `text` | string | Raw matched text |

## Supported Token Types

150+ LaTeX environments plus math tokens. See `docs/TOKEN_TYPES.md` for the full list.
Key types: `equation`, `abstract`, `cite`, `ref`, `label`, `frac`, `parens`,
`math`, `display_math`, `display_2_math`, `braces`, `sum`, `prod`, `lim`, `int`,
`math_sub`, `math_sup`, `math_op`, `math_rel`, `math_fn`, `math_greek`,
`math_var`, `math_num`, `author`, `title`, `section`.

## Design Principles

**Hierarchical, not flat** — Tokens form a tree via `parent_id`.
**Error-tolerant** — Malformed regions don't crash parsing of well-formed regions.
**Modular output** — `.tok` and `.sent` sidecar files are independent.
**Incremental** — Every token carries offset and length for partial reprocessing.
**Performance-first** — Memory-mapped I/O, single-pass Ragel scanners.

## Related Documents

| Document | Description |
|----------|-------------|
| [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) | Pipeline architecture |
| [docs/TOKEN_TYPES.md](docs/TOKEN_TYPES.md) | Token type reference |
| [docs/CYPHER_ARCHITECTURE.md](docs/CYPHER_ARCHITECTURE.md) | Cypher engine architecture |
| [docs/CYPHER_SYNTAX.md](docs/CYPHER_SYNTAX.md) | Cypher syntax reference |
| [docs/CYPHER_GAP_ANALYSIS.md](docs/CYPHER_GAP_ANALYSIS.md) | Grammar coverage |
| [docs/IMPLEMENTATION_STATUS.md](docs/IMPLEMENTATION_STATUS.md) | Feature status |
| [docs/GOLD_STANDARD_ROADMAP.md](docs/GOLD_STANDARD_ROADMAP.md) | Production roadmap |
| [docs/MACRO_EXPANSION_PLAN.md](docs/MACRO_EXPANSION_PLAN.md) | Macro expander plan |

## License

MIT License
