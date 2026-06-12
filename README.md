# latex-analysis-pipeline

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/version-0.2.0-blue)]()

A high-performance, hierarchical LaTeX tokenizer, macro expander, and cross-reference
validator. Handles context-sensitive grammars with arbitrary nesting depth. Processes
100+ levels of nested environments in **4 milliseconds**.

Forked from [allofphysicsgraph/latex-in-arxiv](https://github.com/allofphysicsgraph/latex-in-arxiv).

## Quick Start

```bash
# Install dependencies (Ragel, GCC)
bash scripts/install_deps.sh

# Build the scanner and sentence splitter
cd scanner
make scanner
make sent_split

# Build the macro expander
cd /path/to/latex-analysis-pipeline
gcc -O2 macro_expander.c -o macro_expander.out

# Build the multi-analyzer pipeline (optional)
cd scanner/multi_analyzer
make all

# Tokenize a LaTeX file
cd scanner
./scanner.out sound1.tex tex > sidecar.tok

# Expand macros
cat input.tex | ../macro_expander.out > expanded.tex

# Split into sentences
./sent_split.out < sidecar.tok > clean.sent
```

## What This Does

Most LaTeX tools strip commands and lose structure. This pipeline preserves
everything: every `\cite`, every `\ref`, every `\begin{equation}`, every
nested `(...)`, with exact byte offsets and hierarchical relationships.

It turns LaTeX from opaque text into a **queryable token tree**.

## Why This Matters

- **Pharma companies** submit regulatory documents with thousands of
  cross-references. One broken `\ref` can cause a rejection costing time and money.
- **Academic publishers** process millions of manuscripts. Undefined
  references and broken citations waste reviewer time and damage
  credibility.
- **Patent offices** and **government agencies** manage document
  corpuses where internal consistency is a legal requirement.

Existing tools either can't handle nested LaTeX structures or produce
flat output that loses containment relationships. This pipeline solves
both problems.

## Performance

| Test Case | Nesting | Time |
|-----------|---------|------|

All benchmarks use memory-mapped I/O. See `benchmarks/README.md` for details.

## Architecture

```
Raw .tex file
    ↓
[Memory-mapped file I/O]              ← Zero-copy file access
    ↓
[Pass 1: Structural Tokenizer]        ← Finds blocks, math, citations, refs
    │   EMIT_BLOCK: strip delimiters, rescan interior recursively
    │   EMIT_TOKEN: leaf tokens with type, offset, length, parent_id
    ↓
[Hierarchical Token Tree]             ← parent_id chains preserve containment
    ↓
[Pass 2: Macro Expansion]             ← \newcommand, \def, \let resolution
    ↓
[Pass 3: Sentence Segmentation]       ← English text ready for NLP
    ↓
[Pass 4: Cross-Reference Validation]  ← \ref → \label resolution
    ↓
[Sidecar output: .tok / .sent]        ← Queryable, joinable, modular
```

### Core Mechanism: EMIT_BLOCK / EMIT_TOKEN

| Type | Meaning | Example |
|------|---------|---------|
| `EMIT_TOKEN` | Leaf token. No further decomposition. | `\cite{handbook}`, `\ref{elemental}` |
| `EMIT_BLOCK` | Structural container. Interior re-scanned. | `\begin{equation}...\end{equation}`, `(...)` |

**EMIT_BLOCK processing:**
1. Detect balanced delimiters (via Ragel state machine with stack counters)
2. Emit block token with full span
3. Strip prefix and suffix
4. Re-scan interior with `parent_id` = block's `token_id`
5. All interior tokens inherit the block as their structural parent

This is parsing-as-tokenization. No separate AST construction needed.

### Stack Counter (Ragel)

```ragel
abstract_begin = '\\begin{abstract}' @{n++;};
abstract_end   = '\\end{abstract}'   @{n--;};
abstract_body  = any+ - (abstract_begin|abstract_end);
abstract       = '\\begin{abstract}' @{n=1;}
                 (abstract_begin|abstract_end|abstract_body)*
                 abstract_end :> any when{!n};
```

Handles arbitrary nesting of identical environments without recursion
in the C call stack.

## Installation

### Prerequisites

- GCC or Clang
- [Ragel](https://github.com/adrian-thurston/colm-suite) (for building from `.rl` sources)
- Make
- Python 3 (for validation scripts and tests)

### Build from Source

```bash
# Install Ragel and dependencies
bash scripts/install_deps.sh

# Build all core components
cd scanner
make scanner        # → scanner.out (main tokenizer)
make sent_split     # → sent_split.out (sentence segmenter)

# Build macro expander (standalone C, no Ragel dependency)
cd ..
gcc -O2 macro_expander.c -o macro_expander.out

# Build multi-analyzer pipeline (optional)
cd scanner/multi_analyzer
make all            # → multi_analyzer.out
```

## Usage

### Basic Tokenization

```bash
# Single file
./scanner/scanner.out input.tex tex > sidecar.tok

# Directory with regex pattern matching
./scanner/scanner.out /path/to/docs '.*\.tex$' > sidecar.tok
```

### Macro Expansion

```bash
# Pipe mode (stdin → stdout)
cat input.tex | ./macro_expander.out > expanded.tex

# Report mode: list all macros found in files
./macro_expander.out --report file1.tex file2.tex
```

Supported macros: `\newcommand`, `\renewcommand`, `\def`, `\edef`, `\xdef`,
`\let`, `\expandafter`, `\aftergroup`, `\noexpand`.

### Sentence Segmentation

```bash
cat sidecar.tok | ./scanner/sent_split.out > clean.sent
```

### Cross-Reference Validation

```bash
python tests/validation_tests.py sidecar.tok
```

Exit codes: `0` = clean, `1` = warnings/errors found.

### Multi-Analyzer Pipeline

The `multi_analyzer` provides a chainable text processing pipeline:

```bash
# Tokenize
echo "Hello world" | ./scanner/multi_analyzer/multi_analyzer.out --tokenize

# Tokenize + lowercase
echo "Hello World" | ./scanner/multi_analyzer/multi_analyzer.out --tokenize --lower

# Full pipeline: tokenize + normalize + stem
echo "running jumping" | ./scanner/multi_analyzer/multi_analyzer.out --tokenize --lower --stem
# Output: run jump
```

## Output Format

Each token is a structured record:

```
{filepath:/path/to/file.tex, filepath_id:1425541370, token_id:3328404677,
 parent_id:1425541370, offset:1, length:71, type:equation,
 tok:\begin{equation}\n \label{test}\n  \frac{x}{2}=\\alpha(y)\n\end{equation}}

{filepath:/path/to/file.tex, filepath_id:1425541370, token_id:1585187646,
 parent_id:3328404677, offset:18, length:12, type:label,
 tok:\label{test}}
```

The `\label`, `\frac`, and `(y)` are all children of the `equation` block
(`parent_id: 3328404677`). The `(y)` token is itself a block whose interior
could be further decomposed.

### Fields

| Field | Type | Description |
|-------|------|-------------|
| `filepath` | string | Original file path |
| `filepath_id` | uint32 | Murmur3 hash of filepath |
| `token_id` | uint32 | Murmur3 hash of token text |
| `parent_id` | uint32 | Hash of containing block (same as filepath_id at root) |
| `offset` | int | Byte offset in original file |
| `length` | int | Span length in bytes |
| `type` | string | Semantic token type |
| `tok` | string | Raw matched text |

## Supported Token Types

| Type | Example |
|------|---------|
| `abstract` | `\begin{abstract}...\end{abstract}` |
| `equation` | `\begin{equation}...\end{equation}` (including starred and 150+ variant environments) |
| `cite` | `\cite{handbook,handbook1}` |
| `ref` | `\ref{elemental}` |
| `label` | `\label{test}` |
| `frac` | `\frac{x}{2}` |
| `parens` | `(y)` or nested `(((...)))` |
| `math` | `$A=1$` |
| `braces` | `{document}` |
| `display_math` | `\[...\]` |
| `display_2_math` | `$$...$$` |

Full list of 150+ environment types in `docs/TOKEN_TYPES.md` and defined in
`scanner/latex.rl`.

## Project Structure

```
latex-analysis-pipeline/
├── macro_expander.c               # Standalone LaTeX macro expander
├── expand_macros.sh           # Shell wrapper for macro expansion
├── include/                   # Shared libraries
│   ├── regex_util.c/h         # Regex utilities (sub, split, find, macro_expand)
│   └── old/                   # Original upstream macro_expander.c (archived)
├── scanner/                   # Main scanner implementation
│   ├── scanner.rl             # Ragel state machine for LaTeX tokenization
│   ├── latex.rl               # 150+ LaTeX environment definitions
│   ├── sent_split.rl          # Sentence segmentation state machine
│   ├── main.c                 # File traversal and scanner orchestration
│   ├── globals.h              # Shared definitions and prototypes
│   ├── file_mmap.c/h          # Memory-mapped file I/O utilities
│   ├── murmur3.c              # MurmurHash3 (token/filename hashing)
│   ├── Makefile               # Build instructions
│   ├── strip_non_ascii.c      # Dataset preprocessing utility
│   ├── pre-process-dataset.sh # arXiv corpus preparation script
│   ├── sound1.tex             # Test fixture
│   └── multi_analyzer/        # Chainable text processing pipeline
│       └── src/
│           ├── multi_analyzer.rl   # Ragel analyzer framework
│           ├── multi_analyzer.h    # Public API
│           ├── main.c              # CLI entry point
│           └── porter2.c/h         # Porter2 stemmer
├── sandbox/                   # Experimental/research features
│   ├── qa/
│   │   ├── benepar_qa.py      # NLP variable-definition extractor
│   │   ├── output.json        # Sample QA output
│   │   └── requirements.txt   # Python dependencies
│   ├── statechart.rl          # (Ragel tutorial demo — dead code)
│   └── macro_statechart.rl    # (Incomplete macro expander — dead code)
├── tests/                     # Test suite
│   ├── test_issues.py         # Scanner + macro expander integration tests
│   ├── validation_tests.py    # Cross-reference validation tests
│   └── benchmark_runner.py    # Timing and accuracy benchmarks
├── benchmarks/                # Performance benchmark results and docs
├── docs/                      # Documentation
│   ├── ARCHITECTURE.md        # Architecture guide
│   ├── TOKEN_TYPES.md         # Complete token type reference
│   ├── macro_expander.md      # Macro expander documentation
│   ├── DEBUG_NOTES.md         # Development/debugging notes
│   └── ISSUES_ANALYSIS.md     # Upstream issue tracking analysis
├── scripts/
│   └── install_deps.sh        # Dependency installer
├── examples/
│   └── README.md              # Example usage walkthrough
├── DEAD_CODE_REPORT.md        # Audit of unused/dead code
├── GOLD_STANDARD_ROADMAP.md   # Path to gold-standard LaTeX processing
└── README.md                  # This file
```

## Design Principles

**Hierarchical, not flat.** Tokens form a tree via `parent_id` chains.
Containment is explicit. No separate AST construction required.

**Error-tolerant by design.** Malformed input doesn't crash the tokenizer.
Well-formed regions parse correctly regardless of errors elsewhere.

**Modular output.** `.tok` and `.sent` files are independent sidecars.
Regenerate either from source. Join on offset ranges. No monolithic format.

**Incremental by construction.** Every token carries precise `offset` and
`length`. Changed regions can be invalidated and rescanned without full
reprocessing.

**Performance-first.** Memory-mapped I/O, single-pass scanning, minimal
allocation on the hot path. The tokenizer operates directly on mapped
file contents.

## Roadmap

| Milestone | Status |
|-----------|--------|
| Core tokenizer (150+ LaTeX environments) | ✅ Stable |
| Macro expansion (`\newcommand`, `\def`, `\let`, etc.) | ✅ Stable |
| Sentence segmentation | ✅ Stable |
| Multi-analyzer pipeline (tokenize → normalize → stem) | ✅ Stable |
| Cross-reference validation (`\ref` → `\label`) | ✅ Functional |
| JSON output format | 📋 Planned |
| Docker image | 📋 Planned |
| Incremental mode (`--watch`) | 📋 Planned |
| Compliance rule engine | 📋 Planned |
| Corpus-wide analytics dashboard | 📋 Planned |
| NLP variable-definition extraction (benepar) | 🔬 Research |

See `GOLD_STANDARD_ROADMAP.md` for the detailed path to production-grade LaTeX processing.

## Running Tests

```bash
# Build the scanner first
cd scanner && make scanner && make sent_split && cd ..

# Build the macro expander
gcc -O2 macro_expander.c -o macro_expander.out

# Run the test suite
python -m pytest tests/
```

## Contributing

Issues and pull requests welcome. Before opening a PR:

1. Run the test suite: `python -m pytest tests/`
2. Add tests for new functionality
3. Review `DEAD_CODE_REPORT.md` to avoid re-adding dead code

## Related Documents

- [DEAD_CODE_REPORT.md](DEAD_CODE_REPORT.md) — Audit of unused and dead code
- [GOLD_STANDARD_ROADMAP.md](GOLD_STANDARD_ROADMAP.md) — Path to production-grade LaTeX processing
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) — Detailed architecture
- [docs/TOKEN_TYPES.md](docs/TOKEN_TYPES.md) — Complete token type reference
- [docs/DEBUG_NOTES.md](docs/DEBUG_NOTES.md) — Development debugging notes

## License

MIT License — see the [LICENSE](https://opensource.org/licenses/MIT) file for details.
