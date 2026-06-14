# Lean4 Theorem Prover Pipeline Roadmap

Prove mathematical equivalence by extracting structured facts from LaTeX
and passing them to Lean4 for formal verification.

## Pipeline Overview

```
LaTeX Source (.tex)
     │
     ▼
Scanner (scanner.rl + latex.rl)
  ── EMIT_BLOCK for all math operators ──→  Parent-child parse tree (AST)
     │                                      in sidecar.json
     ▼
Benepar (sandbox/qa/benepar_qa.py)
  ── spaCy constituency parser ──→  Variable definitions, NP defs,
     │                               equation LHS/RHS extraction
     ▼
Tree Fingerprint (tools/tree_fingerprint.c)
  ── Walk parent-child tree ──→  Structural signatures, child-type counts
     │
     ▼
Semantic Extractor (to build)
  ── Map AST nodes to semantic types ──→  (variable, integer, real, set,
     │                                     function, binary-op, relation)
     ▼
Lean4 Code Generator (to build)
  ── Generate .lean source ──→  `theorem eq_1 : f(x) = x^2 := ...`
     │
     ▼
Lean4 Verifier (to install)
  ── `lean --run proof.lean` ──→  ✅ Verified / ❌ Counterexample
```

## Phase 1: Rich Math AST (current work)

### 1a. Add missing math tokens to latex.rl

The scanner currently uses `EMIT` (flat) for `frac` and has no dedicated tokens for
binary operators, relations, or function names. Adding `EMIT_BLOCK` for all math
constructs gives us the parse tree.

| Category | Tokens to add | Priority |
|----------|--------------|----------|
| Binary operators | `+`, `-`, `\times`, `\cdot`, `\pm`, `\div`, `\lt`, `\gt`, `\leq`, `\geq` | HIGH |
| Relations | `=`, `\equiv`, `\approx`, `\sim`, `\propto`, `\neq`, `\ll`, `\gg` | HIGH |
| Functions | `\sin`, `\cos`, `\tan`, `\log`, `\ln`, `\exp`, `\lim`, `\sum`, `\prod`, `\int` | HIGH |
| Delimiters | `\left`, `\right`, `\langle`, `\rangle`, `\lfloor`, `\rfloor` | MEDIUM |
| Accents | `\bar`, `\tilde`, `\hat`, `\vec`, `\overline`, `\underline` | MEDIUM |
| Greek | `\alpha`-`\omega`, `\Gamma`-`\Omega` | MEDIUM |
| Sets | `\subset`, `\supset`, `\subseteq`, `\in`, `\notin`, `\emptyset` | MEDIUM |
| Arrows | `\to`, `\rightarrow`, `\Rightarrow`, `\mapsto` | LOW |
| Sizing | `\frac` → `EMIT_BLOCK` | HIGH |

### 1b. EMIT_BLOCK for frac

Change line 107 of scanner.rl:
```
-  frac => { EMIT("frac"); };
+  frac => { EMIT_BLOCK("frac", 5 , 1 ); };
```

This produces children for `\frac{a}{b}`: two `braces` tokens with the numerator
and denominator, connected via `parent_id` to the frac parent.

### 1c. EMIT_BLOCK for inline math operators

Add scanner patterns for binary operators and relations WITHIN math mode
(inside `$...$` or `$$...$$`). The key challenge: the scanner needs to distinguish
math-mode tokens from regular text. Options:

1. **Context flag**: Set `in_math = 1` when entering math mode, emit typed
   operator tokens only when `in_math`.
2. **Recursive scanner**: For `EMIT_BLOCK("math", ...)`, the recursive scanner
   call processes inner tokens with math-mode awareness.
3. **Post-processing**: Use benepar to classify tokens after scanning.

## Phase 2: Semantic Extraction

### 2a. Variable typing via benepar

benepar_qa.py already extracts definitions like:
```
"the fine structure constant α"  →  α : real
"the electron mass m_e"          →  m_e : mass
"κ is the normalization factor"  →  κ : factor
```

Extend to extract TYPE information from the NP definitions:
- "constant" / "parameter" → hint of `ℝ`
- "integer" / "whole number" → `ℤ`
- "function" → `(ℝ → ℝ)`
- "set" / "subset" → `Set ℝ`
- "mass" / "velocity" → `ℝ` (physical quantities)

### 2b. Structural type inference

From the scanner's AST, infer types from operator usage:
```
f(x) = x^2
  ├── f is applied to x, so f : (ℝ → ℝ) if x : ℝ
  ├── x appears squared, so x : ℝ (exponent is 2)
  └── = is a relation, so both sides have same type
```

### 2c. Equation graph construction

Build a graph of named entities (variables, constants) connected by relationships
(definitions, equalities, inequalities). Edges carry the relation type (=, <, ≤, ≈).

## Phase 3: Lean4 Translation

### 3a. Type mapping

| LaTeX / Math | Lean4 type |
|-------------|-----------|
| `x`, `y`, `z` (scalar) | `ℝ` or `ℤ` (from context) |
| `f`, `g`, `h` (function) | `ℝ → ℝ` |
| `=`, `\\equiv` | `=` |
| `\\approx`, `\\sim` | `≈` |
| `\\leq`, `\\geq` | `≤`, `≥` |
| `x^2` | `x ^ 2` |
| `sqrt{x}` | `Real.sqrt x` |
| `\\frac{a}{b}` | `a / b` (in ℝ) |
| `\\sin`, `\\cos` | `Real.sin`, `Real.cos` |
| `\\sum`, `\\prod` | `Finset.sum`, `Finset.prod` |

### 3b. Statement generation

For each equation, generate a Lean4 theorem:
```lean4
import Mathlib

theorem eq_sound1_fx_x2 : f x = x ^ 2 := by
  -- extracted from TeX: "where f(x) = x^2 is the quadratic function"
  -- with types inferred: f : ℝ → ℝ, x : ℝ
  -- proof: by definition (axiom from the source text)
  sorry
```

For structural equivalence proofs:
```lean4
theorem structural_eq_fx_fy :
  (λ (f : ℝ → ℝ) (x : ℝ) => f x = x ^ 2) =
  (λ (g : ℝ → ℝ) (y : ℝ) => g y = y ^ 2) := by
  -- these are α-equivalent under variable renaming
  -- Lean4's kernel checks this automatically
  rfl
```

### 3c. Proof generation strategies

1. **α-equivalence**: Same structure, different variables → `rfl` (Lean4 handles
   variable renaming natively).
2. **Definitional equality**: If both expressions reduce to the same normal
   form via β-reduction, `rfl` works.
3. **Algebraic proof**: For cases like `(x+1)^2 = x^2 + 2x + 1`, use Lean4's
   `ring` tactic.
4. **Axiom/assumption**: For definitions given in the source text, use `sorry`
   or make them `axiom` declarations.

## Phase 4: Verification Workflow

### 4a. Interactive proof

1. Scanner produces AST for a pair of "equivalent" equations
2. Lean4 generator produces `.lean` file
3. User runs `lean proof.lean` → sees errors, iteratively fills in `sorry`s
4. User can invoke `ring`, `simp`, `rfl`, or manual tactics

### 4b. Automated proof attempt

For simple equivalences (same structural signature), auto-generate `rfl` proof.
For algebraic equivalences, try `ring` and `simp`. Report which need human
attention.

### 4c. Trusted kernel

Lean4's kernel is small (~2K lines of C++). Proof objects are type-checked
by the kernel, providing 100% certainty. The scanner, benepar, and semantic
extractor produce *candidate* equivalences; only Lean4's kernel provides proof.

## Implementation Order

| Step | Component | Effort | Status |
|------|-----------|--------|--------|
| 1a | Add math tokens to latex.rl | 2-3 hours | ✅ Done |
| 1b | EMIT_BLOCK for frac, sqrt | 30 min | ✅ Done (frac) |
| 1c | EMIT_BLOCK for sum, prod, lim, int | 1 hour | ✅ Done |
| 2 | Regenerate sidecar with math AST | scan time | pending |
| 3 | Tree fingerprint with new tokens | 1 hour | pending |
| 4 | Benepar variable type extraction | 2-3 hours | pending |
| 5 | Lean4 type mapping + code generator | 3-4 hours | pending |
| 6 | Lean4 installation + test on sample | 1 hour | pending |
| 7 | Interactive proof workflow | 2-3 hours | pending |
| 8 | Automated proof (rfl/ring/simp) | 2-3 hours | pending |

## Dependencies

- `latex.rl` — scanner patterns (in project)
- `benepar` / `spaCy` — NLP pipeline (already in project at sandbox/qa/)
- `Lean4` — theorem prover (to install: `curl ... | bash` or `elan`)
- `mathlib4` — Lean4's math library (standard with Lean4 install)
