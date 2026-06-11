# Issues Analysis and TODO List

This document analyzes `issues_list.json` which contains GitHub issues/PRs from the **upstream** `allofphysicsgraph/latex-in-arxiv` repository.

## Repository Relationship

**Important:** This repository (`latex-analysis-pipeline`) is a fork of `allofphysicsgraph/latex-in-arxiv`. The `issues_list.json` file contains issues from the upstream repository, not this fork.

The upstream repository is located at: https://github.com/allofphysicsgraph/latex-in-arxiv

This fork (`latex-analysis-pipeline`) contains updated/rewritten code with:
- Consolidated duplicate files
- Improved build system
- Enhanced documentation
- Modular include structure

## Issue Categories

### 1. Dependency Updates (Both Open)
| # | Title | Status | Relevance |
|---|-------|--------|-----------|
| #31 | Bump pillow from 11.3.0 to 12.1.1 | Open | Python dependency management |
| #30 | Bump fonttools from 4.58.1 to 4.61.0 | Open | Python dependency management |

### 2. Research Questions (All Open)
| # | Title | Status | Connection to latex-analysis-pipeline |
|---|-------|--------|--------------------------------------|
| #28 | benchmark the timing and accuracy of each option | Open | **HIGH** - Scanner benchmarks |
| #26 | find similarity scores for bibliographic entries... | Open | Related to citation analysis |
| #25 | Find expressions that are equivalent | Open | Related to equation deduplication |
| #23 | identify potential schemas for a property graph | Open | Architectural design |
| #22 | searching Latex for features of derivations | Open | Token querying |
| #21 | correlate the use of variables across instances | Open | Variable tracking |
| #20 | associate symbols with text description | Open | Word-sense disambiguation |
| #18 | Extract abstract syntax tree from LaTeX math | Open | Multi-analyzer pipeline |
| #17 | find papers that contain derivations | Open | Content searching |
| #16 | graph of papers based on equations | Open | Citation/relation graphs |
| #14 | survey other tools for LaTeX processing | Open | **HIGH** - Project research |
| #13 | reconcile expressions across different papers | Open | Cross-document analysis |
| #12 | Identify variable definitions in LaTeX | Open | **HIGH** - sandbox/qa/benepar_qa.py |
| #11 | tokenize LaTeX bibliography entries | Open | **MEDIUM** - Citation tokens |
| #10 | expand LaTeX macros | Open | **HIGH** - macro_expander.cpp |
| #9 | detect errors, make attempts to correct... | Open | Validation tools |
| #8 | measure accuracy of finding math equations | Open | **HIGH** - Scanner accuracy |
| #5 | replace Latex string variables... | Open | Variable substitution |
| #4 | improve converting Latex to SymPy | Open | Math processing |
| #3 | definitions of each symbol across papers | Open | Symbol tracking |
| #2 | categories for each paper | Open | Classification |

### 3. Questions (Open)
| # | Title |
|---|-------|
| #19 | find related papers by math equation overlap |
| #6 | Compare data structures for Physics Derivation Graph |

## Connection Matrix

### High Relevance to latex-analysis-pipeline

| Issue | Files Relevant |
|-------|----------------|
| #28 | `benchmarks/README.md`, `scanner/Makefile` |
| #14 | `README.md`, `docs/ARCHITECTURE.md` |
| #12 | `sandbox/qa/benepar_qa.py` |
| #10 | `scanner/macro_expander.cpp`, `scanner/latex_macros.h` |
| #8 | `scanner/sound1.tex`, test infrastructure |
| #11 | Token types for `cite`, `bibitem` |

### Medium Relevance

| Issue | Notes |
|-------|-------|
| #26, #25, #13 | Citation/bibliography processing |
| #22, #21, #20 | NLP/token analysis |
| #17, #16 | Output format extensions |
| #9 | Validation error handling |
| #5, #4, #3 | Math processing extensions |
| #2, #6, #23 | Schema/architecture |

## TODO List

### Priority 1: Benchmark Infrastructure
- [ ] #28: Implement benchmark timing and accuracy measurement
- [ ] Create automated benchmark runner
- [ ] Add accuracy validation for tokenizer output
- [ ] Document benchmark results in `benchmarks/README.md`

### Priority 2: Scanner Enhancements
- [ ] #8: Create test suite for math equation detection accuracy
- [ ] #10: Integrate macro expansion into tokenizer pipeline
- [ ] #11: Add comprehensive bibliography tokenization tests
- [ ] #9: Implement error detection and correction logging

### Priority 3: Variable/Derivation Analysis
- [ ] #12: Investigate `benepar_qa.py` integration
- [ ] #20, #21: Add symbol-to-text mapping utilities
- [ ] #25, #26: Implement expression similarity scoring
- [ ] #5: Create variable substitution utilities

### Priority 4: Graph/Structure
- [ ] #16: Document output format for graph construction
- [ ] #23: Design property graph schema
- [ ] #22: Add derivation feature search capabilities
- [ ] #2, #3, #6: Evaluate data structures for scalability

### Priority 5: Documentation & Research
- [ ] #14: Update project with comparison to other tools
- [ ] #17, #18: Add AST extraction documentation
- [ ] #13: Document cross-paper reconciliation approach
- [ ] #4: Research SymPy integration opportunities

## Resolved Issues

**None of the 25 issues/PRs in `issues_list.json` are resolved.** All items are in "open" state.

### Dependency PRs
- #30 and #31 are dependency update PRs that are still open
- These are from `allofphysicsgraph/latex-in-arxiv` (different repo from latex-analysis-pipeline)
- Our project doesn't have Python dependencies in the same way

## Test Cases for Relevant Issues

### Issue #8: Measure accuracy of finding math equations

```python
# test_math_detection.py
import unittest

class TestMathDetection(unittest.TestCase):
    """Test cases for math equation detection accuracy."""
    
    def test_inline_math_detection(self):
        """Test detection of $...$ inline math."""
        input_tex = r"Text with $E = mc^2$ inline."
        expected_count = 1
        # Verify scanner detects exactly 1 math token
        
    def test_display_math_detection(self):
        """Test detection of equation environments."""
        input_tex = r"\begin{equation}E = mc^2\end{equation}"
        expected_types = ['equation']
        # Verify scanner emits equation token
        
    def test_nested_math_detection(self):
        """Test math inside nested environments."""
        input_tex = r"\begin{align}\frac{a}{b}\end{align}"
        # Verify nested math is captured

if __name__ == '__main__':
    unittest.main()
```

### Issue #10: Expand LaTeX macros

```python
# test_macro_expansion.py
import unittest
from macro_expander import expand

class TestMacroExpansion(unittest.TestCase):
    """Test cases for LaTeX macro expansion."""
    
    def test_simple_macro(self):
        """Test \newcommand{\foo}{body} expansion."""
        input_tex = r"\newcommand{\foo}{bar}\foo"
        expected = "bar"
        result = expand(input_tex)
        self.assertEqual(result, expected)
        
    def test_macro_with_args(self):
        """Test macro with arguments."""
        input_tex = r"\newcommand{\add}[2]{#1 + #2}\add{x}{y}"
        expected = "x + y"
        result = expand(input_tex)
        self.assertEqual(result, expected)

if __name__ == '__main__':
    unittest.main()
```

### Issue #11: Tokenize bibliography entries

```python
# test_bibliography_tokens.py
import unittest

class TestBibliographyTokens(unittest.TestCase):
    """Test cases for bibliography tokenization."""
    
    def test_cite_token(self):
        """Test \cite{...} tokenization."""
        input_tex = r"\cite{key1,key2}"
        expected_type = 'cite'
        
    def test_bibitem_token(self):
        """Test \bibitem tokenization."""
        input_tex = r"\bibitem{key} Author, Title"
        expected_type = 'bibitem'
        
    def test_bibliography_environment(self):
        """Test thebibliography environment."""
        input_tex = r"\begin{thebibliography}\bibitem{key}\end{thebibliography}"
        expected_type = 'thebibliography'

if __name__ == '__main__':
    unittest.main()
```

## Recommendations

1. **Create test infrastructure** in `tests/` directory
2. **Implement CI/CD benchmarks** for performance tracking
3. **Integrate relevant issues** into project roadmap
4. **Link repository** by adding analysis to project documentation

## Files Created/Modified

| File | Description |
|------|-------------|
| `docs/ISSUES_ANALYSIS.md` | This file |
| `docs/README.md` | Updated with issues analysis link |
| `tests/test_issues.py` | Test cases for relevant issues |
| `tests/README.md` | Test documentation |
| `include/regex_util.*` | Consolidated duplicates |
| `README.md` | Updated project documentation |
| `docs/ARCHITECTURE.md` | Architecture documentation |
| `docs/TOKEN_TYPES.md` | Token types reference |