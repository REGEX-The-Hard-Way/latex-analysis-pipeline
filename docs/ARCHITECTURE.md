# LaTeX Analysis Pipeline Documentation

## Overview

The LaTeX Analysis Pipeline is a high-performance, hierarchical LaTeX tokenizer and cross-reference validator. It handles context-sensitive grammars with arbitrary nesting depth, processing 500+ levels of nested environments in milliseconds.

## Architecture

### Directory Structure

```
latex-analysis-pipeline/
├── include/                    # Shared libraries
│   ├── macro_expander.c/h     # Token-at-a-time macro expansion engine
│   ├── regex_util.c/h         # Regex utilities (sub, split, find)
│   └── old/                   # Archived original upstream macro_expander.c
├── expand_macros.sh           # Shell wrapper for macro expansion
├── scanner/                   # Main scanner implementation
│   ├── scanner.rl             # Ragel state machine for LaTeX tokenization
│   ├── latex.rl               # 150+ LaTeX environment definitions
│   ├── sent_split.rl          # Sentence segmentation state machine
│   ├── main.c                 # File traversal and scanner orchestration
│   ├── globals.h              # Shared global definitions
│   ├── file_mmap.c/h          # Memory-mapped file I/O utilities
│   ├── murmur3.c              # MurmurHash3 for token/filename hashing
│   ├── json_escape.c/h        # JSON escaping utility
│   ├── Makefile               # Build instructions
│   ├── sound1.tex             # Test fixture
│   └── multi_analyzer/        # Chainable text processing pipeline
│       └── src/
│           ├── multi_analyzer.rl  # Ragel chainable analyzer
│           ├── main.c             # CLI entry point
│           ├── multi_analyzer.h   # Public API
│           └── porter2.c/h        # Porter2 stemmer
├── python/                    # Python bindings
│   └── latex_analysis_pipeline/
│       └── __init__.py        # MacroExpander, LaTeXScanner, SentenceSplitter
├── sandbox/                   # Experimental/research features
│   └── qa/
│       ├── benepar_qa.py      # NLP variable-definition extractor
│       ├── output.json        # Sample output
│       └── requirements.txt   # Python dependencies
├── tests/                     # Test suite
│   ├── test_issues.py         # Integration tests
│   ├── test_macro_expander.py # Macro expander unit tests
│   ├── validation_tests.py    # Cross-reference validation
│   └── benchmark_runner.py    # Performance benchmarks
├── benchmarks/                # Benchmark results
├── docs/                      # Documentation
│   ├── README.md              # Documentation index
│   ├── ARCHITECTURE.md        # This file
│   ├── TOKEN_TYPES.md         # Supported token types
│   ├── MACRO_EXPANSION_PLAN.md # Macro expander bug-fix roadmap
│   ├── IGRAPH_INTEGRATION.md  # Graph analysis integration plan
│   ├── GOLD_STANDARD_ROADMAP.md # Production-grade roadmap
│   ├── DEAD_CODE_REPORT.md    # Audit of unused/dead code
│   ├── macro_expander.md      # Macro expansion docs
│   ├── DEBUG_NOTES.md         # Development notes
│   └── ISSUES_ANALYSIS.md     # Upstream issue tracking analysis
├── scripts/
│   └── install_deps.sh        # Dependency installation (Ragel + igraph)
├── examples/                  # Example inputs/outputs
│   ├── sidecar.tok            # Sample token output
│   └── clean.sent             # Sample sentence output
└── README.md                  # Project overview
```

### Core Components

#### 1. Structural Tokenizer (`scanner.rl`)

The main tokenizer uses Ragel to generate a state machine that:

- Identifies LaTeX blocks (environments, math, citations, references)
- Emits hierarchical tokens with exact byte offsets
- Handles arbitrary nesting via stack counters

**Key Macros:**
- `EMIT_TOKEN`: Emit a leaf token
- `EMIT_BLOCK`: Emit a container block and recursively scan interior

#### 2. Sentence Segmentation

Two sentence splitters serve different input formats:

**`sent_split.rl`** (Ragel, `scanner/` directory)
- Input: raw scanner stdout (hash IDs separated by newlines, one token per line)
- Output: `clean.sent` (one sentence per line, tokens preserved)
- Use case: post-processing scanner output for NLP pipelines

**`tools/sent_split.py`** (Python)
- Input: preprocessed TeX body text with inline hash IDs
- Output: one sentence per line, ready for constituency parsing
- Use case: feeding sentences to `meta/profile` parser for definition extraction
- Usage: `python3 tools/sent_split.py file.tex sidecar.tok > sentences.txt`

#### 3. Cross-Reference Validation (`tests/validation_tests.py`)

Parses `.tok` output to validate:
- `\ref{...}` → `\label{...}` resolution
- `\cite{...}` → bibliography entries
- Nested structure containment

#### 4. Macro Expander (`macro_expander.c`)

A standalone C program (no Ragel dependency) that handles:
- `\newcommand`, `\renewcommand` with optional argument counts
- `\def`, `\edef`, `\xdef` with inline parameter markers
- `\let` (copy definitions)
- `\expandafter`, `\aftergroup`, `\noexpand`
- Reports undefined/redefined macros in `--report` mode

#### 5. Shared Utilities

| File | Purpose |
|------|---------|
| `include/regex_util.c/h` | Regex substitution, splitting, finding, and macro expansion |
| `scanner/file_mmap.c/h` | Memory-mapped file creation and append (zero-copy I/O) |
| `scanner/murmur3.c` | MurmurHash3 for reproducible token_id and filepath_id generation |

## Token Types

| Type | Pattern | Example |
|------|---------|---------|
| `abstract` | `\begin{abstract}...\end{abstract}` | Abstract environments |
| `equation` | Display math environments | `equation`, `align`, etc. |
| `cite` | `\cite{key1,key2}` | Citations |
| `ref` | `\ref{label}` | References |
| `label` | `\label{name}` | Labels |
| `frac` | `\frac{x}{2}` | Fractions |
| `parens` | `(nested)` | Parentheses |
| `math` | `$...$` | Inline math |
| `braces` | `{content}` | Brace groups |

See `TOKEN_TYPES.md` for complete list.

## Output Format

Each token is a structured record:

```
{filepath:/path/to/file.tex,
 filepath_id:3725270426,
 token_id:4107091439,
 parent_id:2511937068,
 offset:1,
 length:83,
 type:equation,
 tok:\begin{equation}...\end{equation}}
```

### Fields

| Field | Type | Description |
|-------|------|-------------|
| `filepath` | string | Original file path |
| `filepath_id` | uint32 | Murmur3 hash of filepath |
| `token_id` | uint32 | Murmur3 hash of token text |
| `parent_id` | uint32 | Hash of containing block |
| `offset` | int | Byte offset in original file |
| `length` | int | Span length in bytes |
| `type` | string | Semantic token type |
| `tok` | string | Raw matched text |

## Building

### Prerequisites

- GCC or Clang
- Ragel (https://github.com/adrian-thurston/colm-suite)
- Make

### Quick Build

```bash
# Install dependencies
bash scripts/install_deps.sh

# Build scanner
cd scanner
make scanner
make sent_split
# Build macro expander
gcc -O2 -Iinclude include/macro_expander.c include/regex_util.c -o macro_expander.out -lm

# Build multi_analyzer
cd multi_analyzer
make all
```

## Usage

### Basic Tokenization

```bash
# Single file
./scanner.out sound1.tex tex > sidecar.tok

# Directory with pattern
./scanner.out /path/to/texfiles '.*\.tex$' > sidecar.tok
```

### Sentence Segmentation

```bash
cat sidecar.tok | ./sent_split.out > clean.sent
```

### Cross-Reference Validation

```bash
python resolve_ref_labels.py
```

## Design Principles

1. **Hierarchical, not flat**: Tokens form a tree via `parent_id` chains
2. **Error-tolerant**: Malformed input doesn't crash the tokenizer
3. **Modular output**: `.tok` and `.sent` files are independent sidecars
4. **Incremental by construction**: Precise offsets enable targeted rescans
5. **Performance-first**: Memory-mapped I/O, single-pass scanning

## API Reference

### multi_analyzer Library

The `multi_analyzer` component provides a chainable text processing pipeline:

```c
#include "multi_analyzer.h"

// Create a chain
analyzer_t *chain = analyzer_create_chain(1, 1, 0);  // tokenize + normalize

// Run on text
analyzer_run(chain, text, strlen(text), emit_callback, userdata);

// Clean up
analyzer_free_chain(chain);
```

**Analyzers:**
- `tokenizer`: Split on whitespace, emit words
- `normalizer`: Lowercase, strip punctuation
- `stemmer`: Apply Porter2 stemming

## Extending

### Adding New Token Types

Edit `latex.rl` to add environment definitions:

```ragel
newenv_begin = '\\begin{newenv}' @{n++; };
newenv_end  = '\\end{newenv}' @{n--; };
newenv_body = any+ - (newenv_begin|newenv_end);
newenv = '\\begin{newenv}' @{n=1;}
         (newenv_begin|newenv_end|newenv_body)*
         newenv_end :> any when{!n};
```

Then add the emission in `scanner.rl`:

```ragel
newenv => { EMIT_BLOCK("newenv",1,0); };
```