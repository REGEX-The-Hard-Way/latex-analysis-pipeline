# Math AST Improvement Plan

## Mapping PS.g4 Grammar to Ragel Scanner → Lean4 Verification

This document defines the path from LaTeX source through a structured math AST to
formal verification in **Lean4**. The end goal is to interface scientific papers
with the Lean theorem prover: infer types, values, and ranges; semantically mark
up named expressions; and prove expression equivalence across papers.

---

## Status Dashboard

| Phase | Description | Status |
|-------|-------------|--------|
| Phase A | Lexical token vocabulary (120+ symbols, 30+ accents, delimiters) | ✅ Done |
| Phase B | Expression group patterns (abs, norm, floor, ceil, binom, sqrt) | ✅ Done |
| Graph | Cross-document cite→bibitem, ref→label, author→child edges | ✅ Done |
| Phase C | Operator precedence chain detection (additive, multiplicative) | ✅ Done |
| Phase D | Matrix cell parsing (`&` / `\\` awareness) | Pending |
| Phase E | Semantic extraction: types, values, ranges, named expressions | Pending |
| Phase F | Lean4 code generation from AST + semantic annotations | Pending |
| Phase G | Cross-paper equivalence proof pipeline | Pending |

---

## 1. Completed Work

### 1.1 Phase A — Lexical Token Vocabulary

**latex.rl** additions:
- 14 delimiter patterns: `l_vert`, `r_vert`, `vert_cmd`, `norm_delim` (`\|`),
  `l_floor`, `r_floor`, `l_ceil`, `r_ceil`, corner variants
- 30 accent commands: `\hat`, `\tilde`, `\vec`, `\bar`, `\dot`, `\ddot`,
  `\widehat`, `\widetilde`, `\overline`, `\overrightarrow`, `\bm`,
  `\boldsymbol`, `\mathbb`, `\mathcal`, `\mathbf`, `\mathsf`, `\mathtt`,
  `\mathrm`, `\mathit`, `\mathfrak`, `\mathscr`, `\text`, `\textit`, etc.
- Expression group patterns: `abs_group`, `norm_group`, `floor_group`, `ceil_group`
- Binomial: `\binom`, `\tbinom`, `\dbinom`, `\choose`
- Square root with optional root: `\sqrt[n]{expr}`

**scanner.rl** additions:
- `math_fn`: +6 functions (`\floor`, `\ceil`, `\arg`, `\argmax`, `\argmin`,
  `\operatorname`, `\iint`, `\iiint`, `\iiiint`)
- `math_op`: +10 operators (`\bigcirc`, `\bigtriangleup`, `\bigtriangledown`,
  `\sqcap`, `\sqcup`, `\uplus`, `\amalg`, `\dagger`, `\ddagger`, `\wr`)
- `math_rel`: +16 relations (`\doteq`, `\bowtie`, `\models`, `\dashv`, `\vdash`,
  `\smile`, `\frown`, `\asymp`, `\mid`, `\succ`, `\prec`, `\succeq`, `\preceq`,
  `\sqsubseteq`, `\sqsupseteq`, `\sqsubset`, `\sqsupset`)
- `math_sym`: +30 symbols (full PS.g4 OTHER_SYMBOL set)
- `math_greek`: +`\varkappa`, `\digamma`
- `math_num`: decimal support (`digit+ (',' digit digit digit)* ('.' digit+)?`)
- `math_scientific`, `math_percent` patterns

### 1.2 Phase B — Expression Group Patterns

All use `EMIT_BLOCK` for recursive interior scanning with proper prefix/suffix:

| Pattern | Emission | Token DB count |
|---------|----------|---------------|
| `abs_group` (`\|x\|`, `\lvert...\rvert`) | `EMIT_BLOCK("abs",1,1)` | 0 (rare in corpus) |
| `norm_group` (`\\|x\\|`) | `EMIT_BLOCK("norm",2,2)` | 0 (rare in corpus) |
| `floor_group` (`\lfloor...\rfloor`) | `EMIT_BLOCK("floor",7,7)` | 0 (rare in corpus) |
| `ceil_group` (`\lceil...\rceil`) | `EMIT_BLOCK("ceil",6,6)` | 0 (rare in corpus) |
| `accent` (`\hat{x}`, `\vec{v}`) | `EMIT("accent")` | 4,462 |
| `binom` (`\binom{n}{k}`) | `EMIT("binom")` | 0 (rare in corpus) |
| `sqrt_full` (`\sqrt[n]{x}`) | `EMIT_BLOCK("sqrt",5,0)` | 0 (flat sqrt exists) |

Note: delimiters (`|`, `\|`, `\lfloor`, `\lceil`) are only recognized inside
math mode (`g_in_math` guard). Corpus has 17,240 `math` blocks, 1,843 `equation`
blocks, and 4,463 `frac` blocks with recursive interior scanning.

### 1.3 Cross-Document Graph

Script: `build_graph.py` — resolves citations, references, and author-childhood.

**graph_edges table:**

| Relationship | Count | Description |
|-------------|-------|-------------|
| `cites` | 157,528 | `\cite{key}` → `\bibitem{key}` |
| — cross-document | 107,030 | Citation resolves to a different paper |
| — unresolved | 5,913 | No matching bibitem found |
| `refers_to` | 107,309 | `\ref{name}` → `\label{name}` |
| `has_child` | 792 | Author → footnote/email/address/thanks chain |

Schema: `(edge_id, source_type, source_key, source_filepath, source_filepath_id,
target_type, target_key, target_filepath, target_filepath_id, target_token_id,
relationship, status)`.

### 1.4 Phase C — Operator Precedence Chains

Script: `phase_c_chains.py` — post-processes flat token streams into bracketed
expression chains following PS.g4 precedence: `expr → additive → mp`.

**Results:** 1,511 chains detected across 10,964 math blocks (3,022 begin/end tokens).

Example — this flat sequence:
```
math_greek(ω)  math_op(+)  math_greek(ω)  math_op(-)  math_num(2)  math_greek(κ)
```
becomes:
```
additive_begin  math_greek(ω)  math_op(+)  math_greek(ω)  math_op(-)
math_num(2)  math_greek(κ)  additive_end
```

All `additive_begin`/`additive_end` tokens are parented to their math block,
enabling tree reconstruction via `parent_id` chains.

---

## 2. Current Database State

Token counts after scanner re-ingestion (`load_tokens.py`, 314K new tokens):

| Token Type | Count | Purpose |
|-----------|-------|---------|
| `math_var` | 75,141 | Single-letter variables |
| `math_sub` | 46,800 | Subscripts |
| `math_num` | 24,529 | Numbers (incl. decimals) |
| `math_op` | 19,653 | Binary operators |
| `math_greek` | 19,260 | Greek letters |
| `math` | 17,240 | Inline math blocks |
| `math_sup` | 15,937 | Superscripts |
| `math_rel` | 12,825 | Relations (=, <, >, ≤, ≥, etc.) |
| `frac` | 4,463 | Fractions with interior scanning |
| `accent` | 4,462 | Accented expressions |
| `math_sym` | 3,900 | Special symbols (∂, ∞, ∇, etc.) |
| `math_fn` | 1,588 | Function names (sin, cos, log, etc.) |
| `equation` | 1,843 | Equation environments |
| `bibitem` | 25,474 | Bibliography entries |
| `graph_edges` | 265,629 | Cross-reference graph edges |

---

## 3. Phase E: Semantic Extraction (Lean4 Bridge)

This is the critical bridge from raw tokens to formal mathematics. The goal is to
annotate the AST with enough semantic information to generate valid Lean4 theorems.

### 3.1 Type Inference

For each variable in the AST, infer its mathematical type from context:

| Evidence | Inferred Type |
|----------|---------------|
| Appears in `\mathbb{R}`, `\in \mathbb{R}` | `ℝ` |
| Appears in `\mathbb{Z}`, `\in \mathbb{Z}` | `ℤ` |
| Appears in `\mathbb{N}`, `\in \mathbb{N}` | `ℕ` |
| Appears in `\mathbb{C}` | `ℂ` |
| Exponent is integer literal (`x^2`, `x^3`) | Base is `ℝ` (or semiring) |
| Appears under `\sqrt` | `ℝ` |
| Appears inside `\sin`, `\cos`, `\log` | Argument is `ℝ` |
| Appears as `f(x)` where f is function | Domain inferred from argument |
| Appears in `\sum_{i=1}^n` | Index is `ℕ` |
| Appears in `\int f(x) dx` | Integration variable is `ℝ` |
| Named "constant", "parameter" in prose | `ℝ` |
| Named "integer", "whole number" in prose | `ℤ` |
| Named "function", "map" in prose | `α → β` |
| Named "set", "subset", "space" in prose | `Set α` |

**Implementation:** Post-processing Python script that:
1. Finds all `math_var`, `math_greek` tokens
2. Looks at their context in the math block (operators applied, surrounding tokens)
3. Cross-references with benepar NLP output for prose definitions
4. Assigns a type annotation as a child token (`type_annotation`) on the variable

### 3.2 Value and Range Inference

| Evidence | Inferred Constraint |
|----------|-------------------|
| `x > 0`, `x \geq 0` | `x : ℝ`, `x ≥ 0` |
| `x \in (0,1)` | `x : ℝ`, `0 < x < 1` |
| `n \in \mathbb{N}` | `n : ℕ` |
| `\forall x \in X` | `x : X` |
| `x \to 0` (in limit) | `x` approaches 0 |
| `x \to \infty` | `x` unbounded above |
| `\|x\| < \epsilon` | `x` in epsilon-neighborhood of 0 |
| `0 \leq x \leq 1` | `x : ℝ`, bounded |

**Implementation:** Pattern matching on inequality chains in the AST. After
Phase C chain detection, inequalities form relation chains that can be parsed:
```
math_var(x) math_rel(≥) math_num(0)  →  constraint: x ≥ 0
```

### 3.3 Named Expression Recognition

Many expressions are known by name. Detecting these enables cross-paper linking
and Lean4 theorem naming.

| Expression Pattern | Canonical Name | Lean4 Target |
|-------------------|----------------|-------------|
| `E = mc^2` | Mass-energy equivalence | `theorem mass_energy : E = m * c ^ 2 := ...` |
| `i\hbar\frac{\partial}{\partial t}\Psi = \hat{H}\Psi` | Schrödinger equation | `theorem schrodinger : ...` |
| `R_{\mu\nu} - \frac{1}{2}Rg_{\mu\nu} = \frac{8\pi G}{c^4}T_{\mu\nu}` | Einstein field equations | `theorem einstein_field : ...` |
| `e^{i\pi} + 1 = 0` | Euler's identity | `theorem euler_identity : ...` |
| `\nabla \cdot \mathbf{E} = \frac{\rho}{\epsilon_0}` | Gauss's law | `theorem gauss_law : ...` |
| `\oint \mathbf{B} \cdot d\mathbf{l} = \mu_0 I` | Ampère's law | `theorem ampere_law : ...` |
| `a^2 + b^2 = c^2` | Pythagorean theorem | `theorem pythagorean : ...` |
| `\frac{d}{dx}f(x) = \lim_{h\to 0}\frac{f(x+h)-f(x)}{h}` | Derivative definition | `theorem derivative_def : ...` |

**Approach — Structural fingerprinting:**

The `tree_fingerprint.c` tool already produces structural hashes from the
parent-child tree. Two expressions with the same structure and named atoms
produce the same fingerprint:

```
display_math
├── math_greek(Ψ)  math_sub(i\hbar)  math_sup(∂/∂t)
├── math_op(=)
├── math_greek(Ĥ)  math_greek(Ψ)
```
→ fingerprint: `display_math:math_greek.2|math_sub.1|math_sup.1|math_op.1`

Combined with **semantic name lookup** (a curated dictionary of known expression
signatures → canonical names), we can tag expressions as known theorems,
equations, or identities.

**Implementation:**
1. Build a fingerprint for each math block
2. Normalize by α-renaming (replace leaf variable text with type-based placeholders)
3. Look up in a curated dictionary of known physics/math expressions
4. Assign a canonical name and Lean4 theorem name
5. Store as `named_expression` edge in `graph_edges`

### 3.4 Semantic Markup Pipeline

```
LaTeX Source
    │
    ▼
scanner.rl → sidecar.tok → authors table (AST tokens)
    │
    ▼
phase_c_chains.py → additive_begin/end bracketing
    │
    ▼
tree_fingerprint.c → structural hashes
    │
    ├──→ Known expression lookup → named_expression annotations
    │
    ▼
benepar_qa.py → NLP variable definitions (type hints from prose)
    │
    ▼
type_inference.py (to build)
    ├── Context-based type assignment
    ├── Value/range constraint extraction
    └── Writes type_annotation child tokens
```

---

## 4. Phase F: Lean4 Code Generation

### 4.1 Expression → Lean4 Translation

Mapping from the annotated AST to Lean4 syntax:

| AST Token | Lean4 Expression |
|-----------|-----------------|
| `math_var(x)` | `x` |
| `math_greek(\alpha)` | `α` (Unicode, Lean4 supports) |
| `math_num(42)` | `(42 : ℝ)` or `42` |
| `math_op(+)` | `+` |
| `math_op(-)` | `-` |
| `math_op(\times)`, `math_op(\cdot)` | `*` |
| `frac{a}{b}` | `a / b` |
| `math_fn(\sin)` | `Real.sin` |
| `math_fn(\cos)` | `Real.cos` |
| `math_fn(\log)` | `Real.log` |
| `math_fn(\exp)` | `Real.exp` |
| `math_fn(\sqrt)` | `Real.sqrt` |
| `math_rel(=)` | `=` |
| `math_rel(\leq)` | `≤` |
| `math_rel(\geq)` | `≥` |
| `math_rel(\neq)` | `≠` |
| `math_rel(\equiv)` | `≡` |
| `math_rel(\approx)` | `≈` (needs `Analysis` import) |
| `math_sup(^2)` | `^ 2` |
| `math_sub(_i)` | `_i` (index notation) |
| `math_fn(\sum)` | `Finset.sum` or `∑` |
| `math_fn(\int)` | `∫` (needs `MeasureTheory`) |
| `math_fn(\lim)` | `Tendsto` (needs `Topology`) |
| `parens(...)` | `(...)` |
| `abs(\|x\|)` | `\|x\|` |
| `binom{n}{k}` | `Nat.choose n k` |
| `floor(...)`, `ceil(...)` | `Int.floor`, `Int.ceil` |

### 4.2 Type-Aware Translation

When type annotations are present, generate typed Lean4:

```lean4
-- Without type inference:
theorem eq1 : f x = x ^ 2 := by
  sorry

-- With type inference (f: ℝ→ℝ, x: ℝ):
theorem eq1 : (f : ℝ → ℝ) (x : ℝ) = (x : ℝ) ^ 2 := by
  sorry
```

### 4.3 Statement Generation

For each named expression or equation, generate:

```lean4
import Mathlib

/-- Equation from paper: 0304245, eq. (3.7) -/
theorem paper_0304245_eq3_7 (x y : ℝ) (h : x > 0) : Real.log (x * y) = Real.log x + Real.log y := by
  -- Proof: from the paper text "by the logarithmic property"
  -- Status: axiom from source
  sorry
```

### 4.4 Proof Strategy Selection

| Evidence | Proof Strategy |
|----------|---------------|
| Structural equivalence (α-renaming) | `rfl` |
| Algebraic identity (expand both sides) | `ring` |
| Simplification | `simp` |
| Inequality chain | `linarith` |
| Definition from source (no proof) | `sorry` (axiom) |
| Mathematical induction | `induction` |
| Case analysis | `cases` |

---

## 5. Phase G: Cross-Paper Equivalence

The ultimate goal: given two expressions from different papers, prove they are
equivalent.

### 5.1 Structural Equivalence via Fingerprints

The `tree_fingerprint.c` tool already normalizes by child-type counts:

```
Paper A:  f(x) = x^2 + 2x + 1
  → fingerprint: "display_math:math_fn.1|parens.1|math_op.3|math_var.3|math_num.2"

Paper B:  g(y) = (y+1)^2
  → fingerprint: "display_math:math_fn.1|parens.1|math_op.2|math_var.2|math_num.1"
```

Different fingerprints → not structurally equivalent at the token level.
Need to expand both sides algebraically.

### 5.2 Algebraic Equivalence via Lean4

For cases where structural fingerprints differ:
1. Generate `.lean` theorem for paper A's expression
2. Generate `.lean` theorem for paper B's expression
3. Generate an equivalence theorem: `theorem cross_paper_eq : expr_A = expr_B := by ...`
4. Run Lean4 with `ring`, `simp`, `field_simp` tactics
5. If Lean4 proves it, the equivalence is verified

### 5.3 Cross-Paper Graph

Extend `graph_edges` with `equivalent_to` edges:

```sql
INSERT INTO graph_edges VALUES (
  'equiv_001',
  'equation', 'eq_3_7', '0304245.tex', ...,
  'equation', 'eq_12',   '0304197.tex', ...,
  'equivalent_to', 'verified_by_lean4'
);
```

This builds a network of known-equivalent expressions across the corpus.
A query can then find all papers containing (or implying) a given mathematical
fact.

### 5.4 Named Expression Network

Expressions tagged with canonical names (Section 3.3) form a cross-paper network:

```
"Schrödinger equation"
├── 0301001.tex, eq. (1) — time-dependent form
├── 0301034.tex, eq. (5) — time-independent form
├── 0302056.tex, eq. (12) — with potential V(x)
└── 0304114.tex, eq. (3.2) — in momentum representation
```

All linked by `named_expression` edges with the canonical name as the key.
A researcher can query: "show me all occurrences of the Schrödinger equation
in this corpus, and whether they are formally equivalent."

---

## 6. Implementation Plan (Next Steps)

### 6.1 Immediate (Phase D)
- Matrix cell parsing: detect `&` and `\\` inside matrix environments
- Emit `matrix_cell`, `matrix_row_delim`, `matrix_col_delim` tokens

### 6.2 Short-term (Phase E — Semantic Extraction)
1. **Type inference script** — `type_inference.py`
   - Context-based: analyze operator usage around each variable
   - NLP-based: consume benepar_qa.py output for prose definitions
   - Writes `type_annotation` tokens in the DB
2. **Value/range extraction** — add to type_inference.py
   - Parse inequality chains (Phase C tokens) for bounds
   - Extract domain restrictions from `\forall`, `\in`, `\subset`
3. **Named expression dictionary** — curated `named_expressions.json`
   - Structural fingerprints → canonical names
   - Initially populate with ~50 common physics/math identities
   - Extensible: users add new patterns as discovered

### 6.3 Medium-term (Phase F — Code Generation)
1. **Lean4 type mapper** — `lean4_emit.py`
   - Token → Lean4 syntax mapping (Section 4.1)
   - Type-aware emission using annotations
2. **Lean4 proof strategy selector**
   - Analyze equation structure → choose tactic
   - Simple: `rfl` for structural equivalence
   - Algebraic: `ring` for polynomial identities
   - Unknown: `sorry` with source annotation
3. **Verify with sample equations** — test on known identities from corpus

### 6.4 Long-term (Phase G — Cross-Paper Proofs)
1. **Cross-paper equivalence checker**
   - Pairwise comparison of named expressions
   - Lean4 verification of equivalence
   - Store verified equivalences in graph_edges
2. **Named expression network browser**
   - Query: "show all occurrences of expression X"
   - Visualization: graph of equivalent expressions across papers
3. **Automated corpus-wide search**
   - For each named expression, find all structurally similar expressions
   - Attempt automated Lean4 proof of equivalence
   - Report verified matches and near-misses

---

## 7. Dependencies

| Component | Status | Location |
|-----------|--------|----------|
| `scanner.rl` + `latex.rl` | ✅ Built (v2) | `scanner/` |
| `build_graph.py` | ✅ Working | `scanner/` |
| `phase_c_chains.py` | ✅ Working | `scanner/` |
| `tree_fingerprint.c` | Exists, needs rebuild | `tools/` |
| `benepar_qa.py` | Exists | `sandbox/qa/` |
| `spaCy` + `benepar` | Needs install | `pip install spacy benepar` |
| `Lean4` | Needs install | `curl ... | bash` (elan) |
| `mathlib4` | Ships with Lean4 | standard |
| `named_expressions.json` | To create | `docs/` |
| `type_inference.py` | To build | `scanner/` |
| `lean4_emit.py` | To build | `scanner/` |

---

## 8. Token Type Reference (Math)

Complete list of math token types emitted by the upgraded scanner:

| Type | PS.g4 Rule | Example |
|------|-----------|---------|
| `math` | inline math | `$x + y$` |
| `display_math` | `\[...\]` | display math |
| `display_2_math` | `$$...$$` | TeX display math |
| `equation` | `\begin{equation}` | numbered equation |
| `align` | `\begin{align}` | aligned equations |
| `math_var` | `LETTER_NO_E` | `x`, `y`, `z` |
| `math_greek` | `GREEK_LETTER` | `\alpha`, `\beta`, `\Gamma` |
| `math_num` | `NUMBER` | `42`, `3.14` |
| `math_scientific` | `E_NOTATION` | `1.5e-3` |
| `math_op` | `ADD`, `SUB`, `CMD_TIMES`, etc. | `+`, `-`, `\times` |
| `math_rel` | `EQUAL`, `LT`, `LTE`, etc. | `=`, `<`, `\leq` |
| `math_fn` | `FUNC_SIN`, `FUNC_LOG`, etc. | `\sin`, `\log`, `\exp` |
| `math_sym` | `SYMBOL`, `OTHER_SYMBOL` | `\partial`, `\infty`, `\nabla` |
| `math_sub` | `UNDERSCORE` | `_i`, `_{ij}` |
| `math_sup` | `CARET` | `^2`, `^{n}` |
| `frac` | `CMD_FRAC` | `\frac{a}{b}` |
| `sqrt` | `FUNC_SQRT` | `\sqrt{x}`, `\sqrt[n]{x}` |
| `binom` | `CMD_BINOM` | `\binom{n}{k}` |
| `abs` | `abs_group` | `\|x\|` |
| `norm` | `norm_group` | `\\|v\\|` |
| `floor` | `floor_group` | `\lfloor x \rfloor` |
| `ceil` | `ceil_group` | `\lceil x \rceil` |
| `accent` | `accent` | `\hat{x}`, `\vec{v}` |
| `sum` | `FUNC_SUM` | `\sum_{i=1}^n` |
| `prod` | `FUNC_PROD` | `\prod_{i=1}^n` |
| `lim` | `FUNC_LIM` | `\lim_{x\to 0}` |
| `int` | `FUNC_INT` | `\int f(x) dx` |
| `additive_begin` | Phase C chain | start of a+b+c chain |
| `additive_end` | Phase C chain | end of a+b+c chain |
| `mp_begin` | Phase C chain | start of a·b·c chain |
| `mp_end` | Phase C chain | end of a·b·c chain |
