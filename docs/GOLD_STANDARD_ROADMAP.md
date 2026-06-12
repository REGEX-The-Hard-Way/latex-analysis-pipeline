# Gold Standard Roadmap

## Making This the Definitive LaTeX Processing Pipeline

This document defines the concrete path from the current state to a
**gold-standard LaTeX analysis pipeline** — one that is complete, correct,
fast, and well-integrated enough that it becomes the default choice for
any organization processing LaTeX at scale.

---

## What "Gold Standard" Means

A gold-standard LaTeX processor must satisfy five criteria:

| Criterion | Definition | Current Grade |
|-----------|-----------|---------------|
| **Completeness** | Handles all LaTeX constructs, not a subset | B (150+ envs, but gaps remain) |
| **Correctness** | Respects TeX semantics: expansion order, catcodes, grouping | C (macro expander is line-at-a-time, not token-at-a-time) |
| **Performance** | Processes a million-document corpus in minutes, not hours | A (4ms for 500-level nesting, memory-mapped I/O) |
| **Robustness** | Graceful degradation on malformed input, never crashes | B+ (error-tolerant, but no formal fuzzing) |
| **Interoperability** | Standard output formats, Docker, library API, CI/CD | D (no JSON output, no Docker, no library API, no CI) |

Current overall grade: **C+ → B-**. The path to A+ is defined below.

---

## Phase 1: Foundation Hardening (0–3 months)

*Goal: Production-grade correctness and test coverage.*

### 1.1 Eliminate Dead Code

Remove all files identified in `DEAD_CODE_REPORT.md`:
- Delete `sandbox/statechart.rl`, `sandbox/macro_statechart.rl`, `scanner/latex_macro.rl`
- Delete `scanner/expand_macros.sh`, `scanner/build_multi_analyzer.sh`
- Delete `scanner/resolve_ref_labels.py`, `scanner/read_tok.py`
- Delete `docs/issues/` directory (22 upstream mirror files)
- Remove `include/old/` or move to a git tag

**Success criterion:** `grep -r "dead\|unused\|TODO\|FIXME" --include="*.c" --include="*.rl"` returns zero hits.

### 1.2 Rename and Consolidate

| Current | Target | Reason |
|---------|--------|--------|
| `macro_expander.c` / `macro_expander.out` | `macro_expander.c` / `macro_expander.out` | All docs and scripts already reference this name |
| `scanner.out` | `latex_scanner` or keep `scanner.out` | Consistency |
| `sent_split.out` | `sent_split` or keep `sent_split.out` | Consistency |
| `multi_analyzer.out` | `multi_analyzer` or keep `multi_analyzer.out` | Consistency |

**Success criterion:** All `make` targets, scripts, and docs use consistent naming. Zero references to the old names.

### 1.3 Comprehensive Test Suite

Current test coverage is ~7 integration tests. Gold standard requires:

```python
# tests/test_macro_expander.py — 50+ unit tests
test_newcommand_simple       # \newcommand{\foo}{bar} \foo → bar
test_newcommand_with_args    # \newcommand{\add}[2]{#1+#2} \add{x}{y} → x+y
test_renewcommand            # \renewcommand overwrites
test_def_with_params         # \def\foo#1#2{...}
test_edef_expansion          # \edef expands at definition time
test_let_copy                # \let\a\b copies definition
test_expandafter_order       # \expandafter\tok1\tok2
test_noexpand_suppress       # \noexpand suppresses one expansion
test_aftergroup              # \aftergroup queues token
test_nested_braces           # \def\foo{\bar{baz}}
test_double_hash             # ## → # in body
test_at_in_name              # \def\@foo{...}
test_comment_handling        # % comments inside definitions
test_nested_macro_call       # \foo{\bar{x}}
test_recursive_limit         # \def\a{\a} doesn't infinite loop
test_unicode_body            # UTF-8 in macro bodies

# tests/test_scanner.py — 30+ unit tests
test_nested_same_env         # 500 \begin{abstract}...\end{abstract}
test_nested_parens           # 500 levels of ((...))
test_nested_mixed            # equation inside abstract inside document
test_malformed_begin         # \begin without \end
test_malformed_end           # \end without \begin
test_overlapping_envs        # \begin{A}\begin{B}\end{A}\end{B}
test_math_in_verbatim        # $...$ inside verbatim should be literal
test_empty_env               # \begin{equation}\end{equation}
test_comment_in_env          # % comments inside environment names
test_inline_math_nested      # $a + $b$ + c$
test_display_math_nested     # $$...$$ inside equation
test_offset_correctness      # Verify byte offsets against original file
test_parent_id_chain         # Verify parent_id → token_id linkage
test_filepath_id_consistent  # Same filepath → same filepath_id

# tests/test_fuzzing.py — Property-based tests
test_random_tex_valid        # Random LaTeX never crashes scanner
test_random_tex_macro        # Random LaTeX never crashes macro expander
test_roundtrip_no_macros     # File without macros → tokenize → identical content
test_token_tree_consistent   # Every parent_id references a valid token_id
```

**Success criterion:** 80%+ line coverage on `scanner.rl` (generated C), 90%+ on `macro_expander.c`.

### 1.4 CI/CD Pipeline

```yaml
# .github/workflows/ci.yml
name: CI
on: [push, pull_request]
jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: bash scripts/install_deps.sh
      - run: cd scanner && make scanner && make sent_split
      - run: gcc -O2 macro_expander.c -o macro_expander.out
      - run: cd scanner/multi_analyzer && make all
      - run: python -m pytest tests/ -v --cov
      - run: python tests/benchmark_runner.py
```

**Success criterion:** Every PR runs tests and benchmarks. Regression in accuracy or >10% performance regression blocks merge.

---

## Phase 2: Output and Interoperability (3–6 months)

*Goal: Make the pipeline a drop-in component in any LaTeX processing stack.*

### 2.1 JSON Output Format

Current output is a custom key-value format. Add a `--json` flag:

```json
{
  "version": "1.0",
  "source": {"filepath": "/path/to/file.tex", "filepath_id": 1425541370},
  "tokens": [
    {
      "token_id": 3328404677,
      "parent_id": 1425541370,
      "offset": 1,
      "length": 71,
      "type": "equation",
      "text": "\\begin{equation}\n \\label{test}\n  \\frac{x}{2}=\\alpha(y)\n\\end{equation}",
      "children": [1585187646, 302638469, 20558579]
    },
    {
      "token_id": 1585187646,
      "parent_id": 3328404677,
      "offset": 18,
      "length": 12,
      "type": "label",
      "text": "\\label{test}",
      "children": []
    }
  ]
}
```

Key additions over current format:
- `children` array (denormalized for query convenience)
- Version field for forward compatibility
- Proper JSON escaping (current format breaks on `"` and `\n` in token text)

**Success criterion:** `python -c "import json; json.load(open('output.json'))"` succeeds on real arXiv corpus output.

### 2.2 Streaming Output

For large corpora, buffering all tokens in memory is unacceptable. Add streaming:

```c
// Current: writes to file after each token (already append-mode)
file_create_or_append("sidecar.tok", emit_buf, emit_len);

// Gold standard: also supports callback API
typedef void (*token_callback)(const Token *tok, void *userdata);
void scanner_with_callback(const char *in, int length, ...,
                           token_callback cb, void *userdata);
```

**Success criterion:** Process a 1 GB `.tex` file with constant memory (<100 MB RSS).

### 2.3 C Library API

Expose the scanner and macro expander as linkable libraries:

```c
// latex_scanner.h — Public API
typedef struct { ... } LaTeXScanner;
LaTeXScanner *latex_scanner_new(void);
void latex_scanner_free(LaTeXScanner *s);
int latex_scanner_scan(LaTeXScanner *s, const char *tex, size_t len,
                       void (*on_token)(const LaTeXToken *, void *), void *ctx);

// macro_expander.h — Public API
typedef struct { ... } MacroExpander;
MacroExpander *macro_expander_new(void);
void macro_expander_free(MacroExpander *e);
char *macro_expander_expand(MacroExpander *e, const char *tex, size_t len);
```

**Success criterion:** Third-party C/Python/Rust programs can `#include "latex_scanner.h"` and tokenize LaTeX.

### 2.4 Python Bindings

```bash
pip install latex-analysis-pipeline
```

```python
from latex_analysis import LaTeXScanner, MacroExpander

scanner = LaTeXScanner()
for token in scanner.scan_file("paper.tex"):
    print(f"{token.type}: {token.text[:50]}...")

expander = MacroExpander()
expanded = expander.expand(r"\newcommand{\foo}{bar}\foo")
assert expanded == "bar"
```

**Success criterion:** `pip install` works on Linux, macOS, and Windows (via WSL or native).

### 2.5 Docker Image

```dockerfile
FROM debian:bookworm-slim
RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc libc6-dev ragel make python3
COPY . /opt/latex-analysis-pipeline
RUN cd /opt/latex-analysis-pipeline/scanner && make
ENTRYPOINT ["/opt/latex-analysis-pipeline/scanner/scanner.out"]
```

```bash
docker run --rm -v $(pwd):/data latex-analyzer /data/input.tex tex
```

**Success criterion:** `docker pull ghcr.io/.../latex-analysis-pipeline:latest` and one-command processing.

---

## Phase 3: Semantic Completeness (6–12 months)

*Goal: Handle the full TeX language, not just common constructions.*

### 3.1 Catcode-Aware Parsing

Current scanner treats all characters literally. Real TeX has dynamic category codes.

```
Task: Implement a catcode table (256 entries × 16 catcodes) and make the
scanner catcode-aware. This enables:
- Verbatim detection (catcode change for \, {, }, etc.)
- Proper comment handling (% only when catcode 14)
- \makeatletter / \makeatother (@ as letter)
- \verb|...| detection
```

**Success criterion:** Scanner correctly handles files using `\makeatletter`, `\verb`, and custom catcode changes.

### 3.2 Full Macro Expansion with TeX Semantics

Current `macro_expander.c` does line-at-a-time scanning. Real TeX is token-at-a-time.

```
Task: Rewrite macro_expander as a proper TeX token processor:
1. Tokenize input into a token stream (not characters)
2. Process tokens one at a time
3. Expand macros when encountered (not in a separate pass)
4. Handle \expandafter correctly (swap next two tokens)
5. Handle \aftergroup (queue token for after group end)
6. Handle \noexpand (suppress expansion of next token)
7. Track grouping with {} properly
8. Support \begingroup / \endgroup
9. Support \csname...\endcsname (construct command names dynamically)
10. Support \if, \ifx, \ifnum, etc. (conditionals)
```

**Success criterion:** Macro expander passes the TeXbook's macro expansion test cases.

### 3.3 Math Mode Awareness

Current scanner treats `$...$` as a flat token. Gold standard requires:

```
- Distinguish inline math ($...$) from display math ($$...$$, \[...\])
- Track math mode nesting ($a + $b$ + c$ is legal)
- Detect subscripts (_) and superscripts (^) within math
- Recognize \left...\right pairs
- Handle \text{}, \mathrm{}, \mathbf{} within math
```

**Success criterion:** Scanner correctly identifies all math sub-structures in 1000 randomly sampled arXiv papers.

### 3.4 Bibliography and Citation Completeness

Current scanner detects `\cite`, `\ref`, `\label` but misses:

```
- \citep, \citet, \citeauthor, \citeyear (natbib)
- \autocite, \textcite, \parencite (biblatex)
- \nocite
- \bibitem with optional argument: \bibitem[Author(year)]{key}
- Multiple bibliographies: \begin{thebibliography} per chapter
- .bib file parsing (extract entries the same way)
```

**Success criterion:** Citation extraction passes on 100% of the 2003 arXiv HEP-TH corpus.

---

## Phase 4: Validation and Compliance (6–12 months, parallel with Phase 3)

*Goal: Make the pipeline a truth-enforcement tool, not just an extractor.*

### 4.1 Cross-Reference Validator

Current state: `tests/validation_tests.py` does basic `\ref` → `\label` matching.

Gold standard:

```
- \ref → \label resolution with line/column error reporting
- \cite → bibliography key resolution
- \pageref, \eqref, \autoref variant support
- Forward references (label defined after ref)
- Duplicate label detection
- Orphaned label detection (defined but never referenced)
- Cross-file references (e.g., \externaldocument)
- Machine-readable error output: --format=json, --format=sarif
```

**Success criterion:** `./validate_refs corpus/` exits 0 on a clean 1000-paper corpus, exits non-zero with structured error list on a corpus with known broken refs.

### 4.2 Style Compliance Engine

Add a rule engine for institutional style guides:

```yaml
# .latex-compliance.yaml
rules:
  - name: no-unexpanded-macros
    description: All user macros must be expanded before submission
    severity: error

  - name: require-amsthm
    description: Theorem environments must use amsthm package
    pattern: "\\\\usepackage{amsthm}"
    severity: warning

  - name: no-eqnarray
    description: Use align instead of eqnarray
    pattern: "\\\\begin{eqnarray}"
    severity: error

  - name: max-nesting-depth
    description: Maximum environment nesting depth
    value: 10
    severity: warning
```

**Success criterion:** A publisher can define a `.latex-compliance.yaml` and CI fails when authors violate it.

### 4.3 Equation Deduplication

Across a corpus, detect duplicate and near-duplicate equations:

```
- Hash-based exact match (murmur3 of math body, already computed)
- Normalization pass: strip whitespace, normalize variable names
- Structural hash: replace identifiers with placeholders, hash the structure
- Cluster by structural hash → report near-duplicates
```

**Success criterion:** Run on the HEP-TH corpus and identify papers that share identical equations.

---

## Phase 5: Analytics and Scale (12–18 months)

*Goal: Corpus-wide analytics at industrial scale.*

### 5.1 Incremental Processing

Current: reprocess entire file on any change. Gold standard:

```
- Store token tree with file hash
- On file change, mmap new version and diff against old
- Re-scan only changed byte ranges
- Preserve token_ids for unchanged regions
- Re-resolve cross-references across changed regions
```

**Success criterion:** Re-processing a 1000-page document after a 1-line edit takes <100ms.

### 5.2 Property Graph Output

Output a property graph (Neo4j-compatible) connecting papers by shared constructs:

```
Nodes: Paper, Equation, Citation, Author, Symbol, Environment
Edges: CONTAINS, CITES, DEFINES, USES, EQUIVALENT_TO
```

```cypher
// Find all papers that use a specific symbol
MATCH (p:Paper)-[:CONTAINS]->(s:Symbol {name: "\\alpha"})
RETURN p.title, s.context

// Find papers connected by shared equations
MATCH (p1:Paper)-[:CONTAINS]->(eq:Equation)<-[:CONTAINS]-(p2:Paper)
WHERE p1 <> p2
RETURN p1, p2, eq, count(*) AS shared_eqs
```

**Success criterion:** Load the HEP-TH corpus into Neo4j and run the above queries.

### 5.3 NLP Integration

Merge `sandbox/qa/benepar_qa.py` into the main pipeline:

```
- Variable definition extraction as a first-class token type: "definition"
- Symbol → text-description linking (parent_id from definition to math token)
- Derivation step detection: "proof" environments with sequential equations
- Build a queryable symbol-definition database per paper / per corpus
```

**Success criterion:** For a given symbol `$m_e$`, return "electron mass" with the defining sentence and byte offset.

### 5.4 REST API / gRPC Service

```
POST /v1/scan
Body: {"tex": "\\begin{equation}...\\end{equation}", "options": {...}}
Response: {"tokens": [...], "errors": [...], "stats": {...}}

POST /v1/validate
Body: {"tex": "..."}
Response: {"valid": false, "errors": [{"type": "unresolved_ref", ...}]}
```

**Success criterion:** A web application can submit LaTeX and get structured tokens in <50ms.

---

## Phase 6: Ecosystem and Community (18–24 months)

*Goal: Become the standard tool that other projects depend on.*

### 6.1 Comparison and Benchmarking Framework

Create a reproducible benchmark comparing against every other LaTeX processing tool:

| Tool | Approach | Token Accuracy | Speed (1k docs) | Handles Nesting | Handles Macros |
|------|----------|----------------|-----------------|-----------------|----------------|
| latex-analysis-pipeline | Ragel FSM + C | 99%+ | 5.7s | ✅ | ✅ |
| [LaTeXML](https://github.com/brucemiller/LaTeXML) | Perl → XML | TBD | TBD | TBD | TBD |
| [pandoc](https://pandoc.org/) | Haskell → AST | TBD | TBD | TBD | TBD |
| [tex2py](https://github.com/alvinwan/tex2py) | Python → AST | TBD | TBD | TBD | TBD |
| [plasTeX](https://github.com/plastex/plastex) | Python → DOM | TBD | TBD | TBD | TBD |
| [latexml](https://github.com/brucemiller/LaTeXML) | Perl → XML | TBD | TBD | TBD | TBD |

**Success criterion:** Publish benchmark results. Be fastest by ≥2× on all metrics.

### 6.2 Standard Output Schema

Propose a community standard for LaTeX token representation (coordinate with LaTeXML, pandoc, etc.):

```
latex-token-schema — version 1.0
A JSON Schema for representing tokenized LaTeX documents.
Adopted by: latex-analysis-pipeline, ${other_tools}
```

**Success criterion:** At least one other tool adopts the schema.

### 6.3 Plugin Architecture

Allow community contributions for domain-specific rules:

```
plugins/
├── ieee_style.py       # IEEE-specific compliance rules
├── acm_style.py        # ACM-specific rules
├── arxiv_compliance.py # arXiv submission requirements
└── pharma_gxp.py       # Pharma GxP document rules
```

**Success criterion:** External contributors submit plugins without modifying core code.

### 6.4 Published Benchmarks and Academic Reference

- Publish a paper or tech report describing the approach
- Submit to JOSS (Journal of Open Source Software) or similar
- Register on Zenodo with DOI

---

## Success Metrics Summary

| Phase | Metric | Target |
|-------|--------|--------|
| 1 | Test coverage | >80% line, >90% branch |
| 1 | Dead code | 0 lines |
| 1 | CI/CD | Green on every commit |
| 2 | JSON output | Validated against JSON Schema |
| 2 | Python package | `pip install` on all platforms |
| 2 | Docker image | <100 MB, one-command usage |
| 3 | arXiv corpus pass rate | >99.5% of HEP-TH 2003 |
| 3 | Macro correctness | Pass TeXbook test suite |
| 4 | Cross-reference accuracy | 100% on clean corpus |
| 4 | Compliance engine | Pluggable rules, user-extensible |
| 5 | Processing speed | 1M documents / hour (single machine) |
| 5 | Memory | Constant memory, <100 MB for any file size |
| 6 | Adoption | Used by ≥1 publisher, ≥1 pharma company |
| 6 | Citation | ≥1 academic reference |

---

## Immediate Next Steps (This Week)

1. **Delete dead code** per `DEAD_CODE_REPORT.md` §Recommended Cleanup Order "Immediate"
2. **Rename** `macro_expander.c` → `macro_expander.c` and fix all references
3. **Add** 10 more unit tests for the macro expander (start with the list in §1.3)
4. **Set up** a GitHub Actions CI that builds all components and runs tests
5. **Tag** v0.2.0 with cleaned codebase

---

*This roadmap is a living document. As each phase is completed, update it with learnings, revised estimates, and new opportunities discovered along the way.*
