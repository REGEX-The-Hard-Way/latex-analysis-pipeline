# Macro Expansion Verification Plan

## 2003 arXiv hep-th Dataset Analysis

**1019 files**, **~50K macro definitions** across the corpus.

### Files Scanned: 50 sample

| Metric | Count |
|--------|-------|
| Total `\newcommand` / `\def` / `\newenvironment` | 2,886 |
| Total `\label` | 1,541 |
| Total `\ref` | 1,579 |
| Total `\cite` | 1,567 |
| Total `\includegraphics` | 26 |

### Compilation Blockers (pdflatex would fail on these)

| Blocker Type | Files Affected | Examples |
|-------------|---------------|----------|
| Missing images (.eps, .ps, .pdf) | 9 / 50 | `fig1.eps`, `auto.eps`, `press.ps` |
| Missing `\include`/`\input` files | 5 / 50 | `title2.tex`, `psfig.sty`, `tcilatex` |
| Missing `.bib` files | 4 / 50 | `univ`, `short`, `refd5conf` |
| **Fully self-contained** | **32 / 50** | No external dependencies |

### Document Classes
- `article` — 22 files
- `revtex4` / `revtex` — 14 files
- `iopart`, `kluwer`, `JHEP3` — remaining

## pdflatex vs Our Macro Expander

Since pdflatex is not available in this environment, verification proceeds in two phases:

### Phase 1: Structural Verification (no pdflatex)

1. **Count macros defined**: Scanner detects `\newcommand`, `\renewcommand`, `\def`, `\newenvironment`
2. **Count macros expanded**: Macro expander processes each definition and replaces usage
3. **Compare token counts**: Original vs expanded file should have different token counts
4. **Check for unexpanded `\newcommand`**: Expanded output should not contain `\newcommand` (success) or should have fewer (partial)

### Phase 2: pdflatex Comparison (when available)

1. Run `pdflatex -interaction=nonstopmode` on each file
2. Compare macro-expanded output tokens with pdflatex-compiled output
3. Flag differences in: expanded macro bodies, `\ref` resolution, `\cite` resolution

### Phase 3: Full Corpus Run

```bash
# Run macro expansion on all 1019 files
cd scanner
for f in 2003/03*.tex_cleaned.tex; do
    ./macro_expander.out "$f" > "${f%.tex}_expanded.tex" 2>/dev/null
done

# Tokenize expanded files
for f in 2003/*_expanded.tex; do
    ./scanner.out --json "$f" tex 2>/dev/null
done

# Compare token type distributions original vs expanded
```

## Expected Differences

1. **Macro-free files**: Token counts should be identical (minus the `\newcommand` definitions themselves)
2. **Files with simple macros** (`\def\foo{bar}`): `\foo` tokens replaced with their expansion
3. **Files with argument macros** (`\newcommand{\foo}[1]{#1}...`): Arguments substituted
4. **Files with `\let`, `\expandafter`**: Complex expansions may differ from pdflatex

## Pre-processing Pipeline

```
Original .tex
    ↓ strip_non_ascii.out
Cleaned .tex
    ↓ macro_expander.out
Expanded .tex
    ↓ scanner.out --json
sidecar.json  →  Cypher query engine
```

## Known Limitations

- `\input`/`\include` not followed (files may not exist)
- `\bibliography` not resolved (no .bib files)
- Images not checked for actual content (only existence)
- Complex macro interactions may not match pdflatex exactly
- pdflatex not available for direct comparison in this environment
