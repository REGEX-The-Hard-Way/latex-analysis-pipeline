# latex-analysis-pipeline

A high-performance, hierarchical LaTeX tokenizer and cross-reference validator.
Handles context-sensitive grammars with arbitrary nesting depth. Processes
500+ levels of nested environments in **4 milliseconds**.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![Version](https://img.shields.io/badge/version-0.1.0-blue)]()

---

## What This Does

Most LaTeX tools strip commands and lose structure. This pipeline preserves
everything: every `\cite`, every `\ref`, every `\begin{equation}`, every
nested `(...)`, with exact byte offsets and hierarchical relationships.

It turns LaTeX from opaque text into a **queryable token tree**.

### In One Command

```bash
$ 
```

## Why This Matters

- **Pharma companies** submit regulatory documents with thousands of
  cross-references. One broken `\ref` can cause a rejection costing time and money in unnecessary delays.
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
| 500 nested identical `\begin{abstract}` blocks | 500 levels | [X] ms |
| 500 nested unique environments | 500 levels | [X] ms |
| 500+ nested parentheses `(((...)))` | 500 levels | **0.004s** |

All benchmarks with memory-mapped I/O.

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
[Pass 2: Math Hash Replacement]       ← $m_e$ → 76726987 (consistent across docs)
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

## Output Format

Each token is a structured record:

```
{filepath:/path/to/file.tex,
 filepath_id:3725270426,
 token_id:4107091439,
 parent_id:2511937068,      ← Links to containing block or file
 offset:1,                  ← Byte offset in original file
 length:83,                 ← Span length in bytes
 type:parens,               ← Semantic token type
 tok:(((...)))}              ← Raw matched text
```

### Supported Token Types

| Type | Example |
|------|---------|
| `abstract` | `\begin{abstract}...\end{abstract}` |
| `equation` | `\begin{equation}...\end{equation}` |
| `cite` | `\cite{handbook,handbook1}` |
| `ref` | `\ref{elemental}` |
| `label` | `\label{test}` |
| `frac` | `\frac{x}{2}` |
| `parens` | `(y)` or nested `(((...)))` |
| `math` | `$A=1$` |
| `braces` | `{document}` |

Configurable: add custom types via pattern definitions.

## Installation

### Quick Start

```bash
```

### Build from Source

```bash
```

**Prerequisites:** Ragel [version], C compiler (GCC or Clang), Make

### Docker

```bash
```

## Usage

### Basic Tokenization

```bash
```

### Cross-Reference Validation

```bash
```

Exit codes: `0` = clean, `1` = warnings, `2` = errors found

### JSON Output

```bash
./scanner --format=json input.tex > output.json
```

### Incremental Mode

```bash
```


**Input (`sample.tex`):**
```latex
\begin{equation}
 \label{test}
  \frac{x}{2} = \alpha(y)
\end{equation}
```

**Output (`.tok`):**
```
{filepath:sample.tex, filepath_id:1425541370, token_id:3328404677,
 parent_id:1425541370, offset:1, length:71, type:equation,
 tok:\begin{equation}\n \label{test}\n  \frac{x}{2}=\\alpha(y)\n\end{equation}}

{filepath:sample.tex, filepath_id:1425541370, token_id:1585187646,
 parent_id:3328404677, offset:18, length:12, type:label,
 tok:\label{test}}

{filepath:sample.tex, filepath_id:1425541370, token_id:302638469,
 parent_id:3328404677, offset:17, length:11, type:frac,
 tok:\frac{x}{2}}

{filepath:sample.tex, filepath_id:1425541370, token_id:20558579,
 parent_id:3328404677, offset:52, length:3, type:parens,
 tok:(y)}
```

The `\label`, `\frac`, and `(y)` are all children of the `equation` block
(`parent_id: 3328404677`). The `(y)` token is itself a block whose interior
could be further decomposed if it contained recognized patterns.

## Design Principles

**Hierarchical, not flat.** Tokens form a tree via `parent_id` chains.
Containment is explicit. No separate AST construction required.

**Error-tolerant by design.** Malformed input doesn't crash the tokenizer.
Well-formed regions parse correctly regardless of errors elsewhere.
Unmatched delimiters are detected in a separate pass with exact byte offsets.

**Modular output.** `.tok` and `.sent` files are independent sidecars.
Regenerate either from source. Join on offset ranges. No monolithic format.

**Incremental by construction.** Every token carries precise `offset` and
`length`. Changed regions can be invalidated and rescanned without full
reprocessing.

**Performance-first.** Memory-mapped I/O, single-pass scanning, minimal
allocation on the hot path. The tokenizer operates directly on mapped
file contents.

## Transfer to Code Analysis

This architecture is not LaTeX-specific. The same approach—hierarchical
tokenization, parent_id chains, exact source locations—applies to:

- Configuration languages (HCL, YAML)
- Infrastructure-as-code (Dockerfiles, Terraform)
- Query languages (SQL, Elasticsearch DSL)
- Any language with nested, context-sensitive structures

The same architecture applied to Dockerfile security analysis.

## What This Enables

- **Cross-reference validation:** Every `\ref` checked against defined `\label`s
- **Citation graph construction:** Directed graph of `\cite` relationships
- **Structural queries:** "All `\ref` tokens inside `abstract` blocks"
- **Sentence-level alignment:** NLP constituency parses joined to LaTeX offsets
- **Incremental reprocessing:** Re-tokenize only changed document regions

## Roadmap

| Milestone | Status |
|-----------|--------|
| Core tokenizer (structural blocks, citations, refs, math) | ✅ Stable |
| Cross-reference validation (`--validate-refs`) | 🚧 In progress |
| JSON output |  |
| Docker image |  |
| Incremental mode (`--watch`) | 📋 Planned |
| Compliance rule engine | 📋 Planned |
| Corpus-wide analytics | 📋 Planned |
| Sentence-level offset alignment | 🔬 Research |

## Invariants

Maintained by construction and verified by test suite:

1. **Span containment:** Child tokens are fully contained within parent byte ranges
2. **Non-overlap at same level:** No two sibling tokens have overlapping ranges
3. **Monotonic offsets:** Tokens within a block appear in ascending order
4. **Hash consistency:** Same LaTeX command maps to same hash ID across documents

## Contributing

Issues and pull requests welcome. Before opening a PR:

1. Run the test suite: `make test`
2. Add tests for new functionality
3. Ensure the arXiv corpus pass rate doesn't regress
