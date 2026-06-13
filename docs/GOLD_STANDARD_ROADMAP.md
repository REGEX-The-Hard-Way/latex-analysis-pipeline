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
| **Correctness** | Respects TeX semantics: expansion order, catcodes, grouping | C (macro expander has ~8% real-world failure rate) |
| **Performance** | Processes a million-document corpus in minutes, not hours | A (4ms for 500-level nesting, memory-mapped I/O) |
| **Robustness** | Graceful degradation on malformed input, never crashes | B+ (error-tolerant, but no formal fuzzing) |
| **Interoperability** | Standard output formats, Docker, library API, CI/CD, graph DB | D (no Docker, no graph output, no CI, binary naming broken) |

Current overall grade: **C+ → B-**. The path to A+ is defined below.

---

## Phase 0: Documentation and Housekeeping (Week 1)

*Goal: Accurate documentation, dead code eliminated, builds working.*

### 0.1 Fix Broken Documentation and References

| Fix | File(s) |
|-----|---------|
| Rename all `macro_expander` → `macro_expander` throughout | README.md, docs/*.md, Makefile, shell scripts |
| Fix file paths: `scanner/macro_expander.c` → `include/macro_expander.c` | `docs/macro_expander.md`, `docs/ARCHITECTURE.md` |
| Fix `docs/ARCHITECTURE.md` `.cpp` reference → `.c` | `docs/ARCHITECTURE.md` |
| Fix `docs/macro_expander.md` `make expand_macros` → correct `make macro_expander` or build command | `docs/macro_expander.md` |
| Remove references to nonexistent `issues_list.json`, `sample.tex`, `expand_macros.rl` | Multiple files |
| Reference `docs/DEAD_CODE_REPORT.md` and `docs/GOLD_STANDARD_ROADMAP.md` from `docs/README.md` | `docs/README.md` |
| Fix `examples/README.md` — create `sample.tex` or remove references | `examples/README.md` |

### 0.2 Eliminate Dead Code

As identified in `docs/DEAD_CODE_REPORT.md`:
- Delete empty stub files: `scanner/latex_macro.rl`, `sandbox/macro_statechart.rl`
- Delete duplicates: `sandbox/macro_expander.c`, `scanner/expand_macros.sh`, `sandbox/expand_macros.sh`, `scanner/build_multi_analyzer.sh`
- Delete broken/abandoned: `sandbox/read_tok.py`, `sandbox/resolve_ref_labels.py`
- Delete `docs/issues/` directory (26 upstream mirror files)
- Remove unused code: commented-out `main()` in `murmur3.c`, `EMIT` macro in `scanner.rl` if unused, dead globals in `macro_expander.c`
- Decision: archive `include/old/` branch or delete

### 0.3 Fix Makefile

```makefile
# Current (broken): references nonexistent expand_macros.rl and root-level macro_expander.c
macro_expander:
	ragel -m -G2 expand_macros.rl               # FILE DOES NOT EXIST
	$(CC) -O2 -I../include macro_expander.c ... # FILE AT ROOT DOES NOT EXIST

# Fixed:
macro_expander:
	$(CC) -O2 -Iinclude include/macro_expander.c include/regex_util.c -o macro_expander.out -lm
```

### 0.4 Move pyproject.toml to Root

Current: `sandbox/pyproject.toml` — cannot `pip install` from project root.
Fix: Create root-level `pyproject.toml` or `setup.cfg` pointing to `python/` package.

---

## Phase 1: Foundation Hardening (0–3 months)

*Goal: Production-grade correctness and test coverage.*

### 1.1 Macro Expander Bug Fixes

See `docs/MACRO_EXPANSION_PLAN.md` for full detail. Priority order:

1. **Fix `@` in macro names** (~60% of arXiv files affected)
2. **Fix double hash `##` handling** (crashes on `\def\test##1{##1}`)
3. **Add recursive expansion depth limit** (prevent infinite loops)
4. **Fix unused report counters** (wire `--report` mode)

**Success criterion:** arXiv corpus failure rate drops from 8% to <0.1%.

### 1.2 Comprehensive Test Suite

Current test coverage is ~7 integration tests + ~20 macro expander unit tests.
Gold standard requires:

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
test_makeatletter            # \makeatletter\def\@foo{bar}
test_empty_body              # \def\foo{}
test_space_after_macro       # \foo bar (space consumed)
test_newline_handling        # macros across line boundaries
test_multiple_defs           # 1000+ definitions
test_let_chain               # \let\a\b \let\b\c
test_expandafter_chain       # multiple \expandafter

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

# tests/test_graph.py — 10+ unit tests (Phase 2+)
test_single_paper_graph      # Build graph from one .tok file
test_citation_edge           # \cite creates correct edge
test_equation_dedup          # Shared equations create cross-paper edges
test_graph_export            # GraphML export is valid XML

# tests/test_fuzzing.py — Property-based tests
test_random_tex_valid        # Random LaTeX never crashes scanner
test_random_tex_macro        # Random LaTeX never crashes macro expander
test_roundtrip_no_macros     # File without macros → tokenize → identical content
test_token_tree_consistent   # Every parent_id references a valid token_id
```

**Success criterion:** 80%+ line coverage on scanner (generated C), 90%+ on macro_expander.c.

### 1.3 CI/CD Pipeline

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
      - run: gcc -O2 -Iinclude include/macro_expander.c include/regex_util.c -o macro_expander.out -lm
      - run: cd scanner/multi_analyzer && make all
      - run: python -m pytest tests/ -v --cov
      - run: python tests/benchmark_runner.py
```

Additionally:
- **Static analysis:** Add `cppcheck` or `clang-tidy` for C code
- **Linting:** Add `ruff` or `pylint` for Python code
- **Fuzzing:** Add `make fuzz` target using AFL++ or libFuzzer

**Success criterion:** Every PR runs tests and benchmarks. Regression in accuracy or >10%
performance regression blocks merge.

---

## Phase 2: Output and Interoperability (3–6 months)

*Goal: Make the pipeline a drop-in component in any LaTeX processing stack.*

### 2.1 JSON Output Format

Current output is a custom key-value format. Add a `--json` flag:

```json
{
  "version": "1.0",
  "source": {"filepath": "file.tex", "filepath_id": 1425541370},
  "tokens": [
    {
      "token_id": 3328404677,
      "parent_id": 1425541370,
      "offset": 1,
      "length": 71,
      "type": "equation",
      "text": "\\begin{equation}\\label{test}\\frac{x}{2}=\\alpha(y)\\end{equation}",
      "children": [1585187646, 302638469, 20558579]
    }
  ]
}
```

Key additions over current format:
- `children` array (denormalized for query convenience)
- Version field for forward compatibility
- Proper JSON escaping (current format breaks on `"` and `\n` in token text)

### 2.2 Streaming Output

For large corpora, buffering all tokens in memory is unacceptable:

```c
// Current: writes to file after each token (already append-mode)
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
LaTeXScanner *latex_scanner_new(void);
void latex_scanner_free(LaTeXScanner *s);
int latex_scanner_scan(LaTeXScanner *s, const char *tex, size_t len,
                       void (*on_token)(const LaTeXToken *, void *), void *ctx);

// macro_expander.h — Already exists (include/macro_expander.h)
// Already provides: macro_expander_t, create, destroy, feed, get_output, feed_cb, reset, macro_count
```

**Success criterion:** Third-party C/Python/Rust programs can `#include "latex_scanner.h"`
and tokenize LaTeX.

### 2.4 Python Package

```bash
pip install latex-analysis-pipeline
```

```python
from latex_analysis import LaTeXScanner, MacroExpander, LaTeXGraph

scanner = LaTeXScanner()
for token in scanner.scan_file("paper.tex"):
    print(f"{token.type}: {token.text[:50]}...")

expander = MacroExpander()
expanded = expander.expand(r"\newcommand{\foo}{bar}\foo")
assert expanded == "bar"

graph = LaTeXGraph()
graph.add_tok_file("paper.tok")
graph.export_graphml("paper.graphml")
```

**Success criterion:** `pip install` works on Linux, macOS, and Windows (via WSL or native).

### 2.5 Docker Image

```dockerfile
FROM debian:bookworm-slim
RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc libc6-dev ragel make python3 libigraph-dev
COPY . /opt/latex-analysis-pipeline
RUN cd /opt/latex-analysis-pipeline/scanner && make
ENTRYPOINT ["/opt/latex-analysis-pipeline/scanner/scanner.out"]
```

```bash
docker run --rm -v $(pwd):/data latex-analyzer /data/input.tex tex
```

### 2.6 Graph Output (igraph Integration)

See `docs/IGRAPH_INTEGRATION.md` for full detail.

Build a graph analysis tool that converts `.tok` token trees into igraph graphs,
enabling cross-document queries:

```bash
# Build the corpus graph from .tok files
./graph_analyzer --build corpus/*.tok --output corpus.graphml

# Find equation clusters (equations shared across papers)
./graph_analyzer --query equation-clusters --min-share 2 corpus/*.tok

# Export for Neo4j
./graph_analyzer --export-neo4j corpus/*.tok --output-dir neo4j_import/
```

**Graph models supported:**
- Document Relationship Graph (papers connected by citations, shared equations, symbols)
- Token Tree → Property Graph (full paper structure with cross-document edges)
- Equation Flow Graph (derivation dependencies within a paper)

**Success criterion:** Full HEP-TH 2003 corpus (20K files) → igraph graph in <60 seconds.

---

## Phase 3: Semantic Completeness (6–12 months)

*Goal: Handle the full TeX language, not just common constructions.*

### 3.1 Catcode-Aware Parsing

Current scanner treats all characters literally. Real TeX has dynamic category codes.

```
Implement a catcode table (256 entries × 16 catcodes) and make the
scanner catcode-aware. This enables:
- Verbatim detection (catcode change for \, {, }, etc.)
- Proper comment handling (% only when catcode 14)
- \makeatletter / \makeatother (@ as letter)
- \verb|...| detection
```

### 3.2 Full Macro Expansion with TeX Semantics

Current `include/macro_expander.c` is token-at-a-time but has gaps:

```
Must fix:
1. @ in macro names (HIGH — 60% of arXiv files affected)
2. Double hash ## in bodies (HIGH — causes crashes)
3. Recursive expansion limit (MEDIUM — prevents infinite loops)
4. Catcode change support: \makeatletter, \makeatother (MEDIUM)

Must add:
5. \csname...\endcsname (dynamic command name construction)
6. Conditionals: \if, \ifx, \ifnum, \else, \fi
7. \input / \include (multi-file documents)
8. \verb scanning
9. \begingroup / \endgroup (group tracking)
```

See `docs/MACRO_EXPANSION_PLAN.md` for the full implementation plan.

### 3.3 Math Mode Awareness

Current scanner treats `$...$` as a flat token. Gold standard requires:

- Distinguish inline math ($...$) from display math ($$...$$, \[...\])
- Track math mode nesting ($a + $b$ + c$ is legal)
- Detect subscripts (_) and superscripts (^) within math
- Recognize \left...\right pairs
- Handle \text{}, \mathrm{}, \mathbf{} within math

### 3.4 Bibliography and Citation Completeness

| Feature | Current | Target |
|---------|---------|--------|
| `\cite` | ✅ | ✅ |
| `\ref`, `\label` | ✅ | ✅ |
| `\citep`, `\citet`, natbib | ❌ | ✅ |
| `\autocite`, `\textcite`, biblatex | ❌ | ✅ |
| `\nocite` | ❌ | ✅ |
| `\bibitem` with optional arg | ❌ | ✅ |
| Multiple bibliographies | ❌ | ✅ |
| .bib file parsing | ❌ | ✅ |

---

## Phase 4: Validation and Compliance (6–12 months, parallel with Phase 3)

*Goal: Make the pipeline a truth-enforcement tool, not just an extractor.*

### 4.1 Cross-Reference Validator

Current state: `tests/validation_tests.py` does basic `\ref` → `\label` matching.

Gold standard:
- `\ref` → `\label` resolution with line/column error reporting
- `\cite` → bibliography key resolution
- `\pageref`, `\eqref`, `\autoref` variant support
- Forward references (label defined after ref)
- Duplicate label detection
- Orphaned label detection (defined but never referenced)
- Cross-file references (e.g., `\externaldocument`)
- Machine-readable error output: `--format=json`, `--format=sarif`

### 4.2 Style Compliance Engine

Add a rule engine for institutional style guides:

```yaml
# .latex-compliance.yaml
rules:
  - name: require-amsthm
    pattern: "\\\\usepackage{amsthm}"
    severity: warning
  - name: no-eqnarray
    pattern: "\\\\begin{eqnarray}"
    severity: error
  - name: max-nesting-depth
    value: 10
    severity: warning
```

### 4.3 Equation Deduplication

Across a corpus, detect duplicate and near-duplicate equations:
- Hash-based exact match (murmur3 of math body, already computed)
- Normalization pass: strip whitespace, normalize variable names
- Structural hash: replace identifiers with placeholders, hash the structure
- Cluster by structural hash → report near-duplicates

---

## Phase 5: Analytics and Scale (12–18 months)

*Goal: Corpus-wide analytics at industrial scale.*

### 5.1 Incremental Processing

Store token tree with file hash. On file change, mmap new version, diff against old,
re-scan only changed byte ranges. Preserve token_ids for unchanged regions.

### 5.2 Property Graph Output (Neo4j / igraph)

Connect papers by shared constructs using the graph infrastructure from Phase 2.6:

```cypher
// Find papers connected by shared equations
MATCH (p1:Paper)-[:CONTAINS]->(eq:Equation)<-[:CONTAINS]-(p2:Paper)
WHERE p1 <> p2
RETURN p1, p2, eq, count(*) AS shared_eqs
```

### 5.3 NLP Integration

Merge `sandbox/qa/benepar_qa.py` into the main pipeline:
- Variable definition extraction as a first-class token type: "definition"
- Symbol → text-description linking (parent_id from definition to math token)
- Derivation step detection: "proof" environments with sequential equations

### 5.4 REST API / gRPC Service

```bash
POST /v1/scan
Body: {"tex": "\\begin{equation}...\\end{equation}", "options": {...}}
Response: {"tokens": [...], "errors": [...], "stats": {...}}

POST /v1/validate
Body: {"tex": "..."}
Response: {"valid": false, "errors": [{"type": "unresolved_ref", ...}]}
```

---

## Phase 6: Ecosystem and Community (18–24 months)

*Goal: Become the standard tool that other projects depend on.*

### 6.1 Comparison and Benchmarking Framework

| Tool | Approach | Token Acc. | Speed (1k docs) | Nesting | Macros |
|------|----------|-----------|----------------|---------|--------|
| latex-analysis-pipeline | Ragel FSM + C | 99%+ | 5.7s | ✅ | ✅ |
| LaTeXML | Perl → XML | TBD | TBD | TBD | TBD |
| pandoc | Haskell → AST | TBD | TBD | TBD | TBD |
| tex2py | Python → AST | TBD | TBD | TBD | TBD |
| plasTeX | Python → DOM | TBD | TBD | TBD | TBD |

### 6.2 Standard Output Schema

Propose a community standard for LaTeX token representation:
`latex-token-schema` — version 1.0, JSON Schema for tokenized LaTeX documents.

### 6.3 Plugin Architecture

```
plugins/
├── ieee_style.py       # IEEE-specific compliance rules
├── acm_style.py        # ACM-specific rules
├── arxiv_compliance.py # arXiv submission requirements
└── pharma_gxp.py       # Pharma GxP document rules
```

### 6.4 Published Benchmarks and Academic Reference

- Publish a paper or tech report describing the approach
- Submit to JOSS (Journal of Open Source Software) or similar
- Register on Zenodo with DOI

---

## Success Metrics Summary

| Phase | Metric | Target |
|-------|--------|--------|
| 0 | Dead code | 0 lines |
| 0 | Build | All targets green, no warnings |
| 0 | Docs | Zero broken references, all paths correct |
| 1 | Test coverage | >80% line, >90% branch |
| 1 | arXiv corpus pass rate | >99.9% (macro expander) |
| 1 | CI/CD | Green on every commit |
| 2 | JSON output | Validated against JSON Schema |
| 2 | Python package | `pip install` on all platforms |
| 2 | Docker image | <100 MB, one-command usage |
| 2 | Graph analysis | HEP-TH 2003 → igraph graph in <60s |
| 3 | Macro correctness | Pass TeXbook test suite, 0% arXiv failures |
| 3 | Math awareness | All math sub-structures detected |
| 3 | Citation completeness | All major bibliography packages supported |
| 4 | Cross-reference accuracy | 100% on clean corpus |
| 4 | Compliance engine | Pluggable rules, user-extensible |
| 5 | Processing speed | 1M documents / hour (single machine) |
| 5 | Memory | Constant memory, <100 MB for any file |
| 6 | Adoption | Used by ≥1 publisher, ≥1 pharma company |
| 6 | Citation | ≥1 academic reference |

---

## Dependency Map

```
Phase 0: Docs & Housekeeping
  ├── 0.1 Fix documentation references ──────────────── (1 week)
  ├── 0.2 Delete dead code ───────────────────────────── (1 day)
  ├── 0.3 Fix Makefile ───────────────────────────────── (1 day)
  └── 0.4 Move pyproject.toml ───────────────────────── (1 day)
         │
Phase 1: Foundation Hardening
  ├── 1.1 Macro expander bug fixes ──────────────────── (2-4 weeks)
  │      ├── @ in names (HIGH)
  │      ├── ## handling (HIGH)
  │      ├── Recursive limit (MEDIUM)
  │      └── Report counters (LOW)
  ├── 1.2 Comprehensive test suite ───────────────────── (ongoing)
  └── 1.3 CI/CD pipeline ─────────────────────────────── (1 week)
         │
Phase 2: Output & Interoperability
  ├── 2.1 JSON output format ────────────────────────── (2 weeks)
  ├── 2.2 Streaming output ──────────────────────────── (1 week)
  ├── 2.3 C library API ──────────────────────────────── (2 weeks)
  ├── 2.4 Python bindings ────────────────────────────── (1 week)
  ├── 2.5 Docker image ───────────────────────────────── (1 week)
  └── 2.6 Graph output (igraph) ──────────────────────── (3-4 weeks)
         │
Phase 3: Semantic Completeness ─────────────────── (parallel with Phase 4)
  ├── 3.1 Catcode-aware parsing
  ├── 3.2 Full macro expansion (continued)
  ├── 3.3 Math mode awareness
  └── 3.4 Bibliography completeness
         │
Phase 4: Validation & Compliance ────────────────── (parallel with Phase 3)
  ├── 4.1 Cross-reference validator
  ├── 4.2 Style compliance engine
  └── 4.3 Equation deduplication
         │
Phase 5: Analytics & Scale
  ├── 5.1 Incremental processing
  ├── 5.2 Property graph output
  ├── 5.3 NLP integration
  └── 5.4 REST API
         │
Phase 6: Ecosystem & Community
  ├── 6.1 Benchmarking framework
  ├── 6.2 Standard schema
  ├── 6.3 Plugin architecture
  └── 6.4 Published benchmarks
```

---

## Immediate Next Steps (This Week)

1. **Delete dead code** per Phase 0.2
2. **Fix Makefile** `macro_expander` target per Phase 0.3
3. **Fix all `macro_expander` → `macro_expander`** references in docs, scripts, Makefile
4. **Fix `@` in macro names** (blocker for Phase 1.1)
5. **Fix `##` in macro bodies** (crasher for Phase 1.1)
6. **Add recursive expansion limit** (Phase 1.1)
7. **Tag v0.2.1** with cleaned codebase and critical macro fixes

---

*This roadmap is a living document. As each phase is completed, update it with
learnings, revised estimates, and new opportunities discovered along the way.*
