# Math AST Improvement Plan

## Mapping PS.g4 Grammar to Ragel Scanner Enhancements

This document provides a structured plan to evolve `latex.rl` and `scanner.rl` from flat
lexical math tokenization into a hierarchical expression parser, modeled on the `PS.g4`
ANTLR grammar (638 lines, fully specified LaTeX math AST grammar).

---

## 1. Gap Analysis: Current State vs PS.g4 Target

### 1.1 What scanner.rl Already Handles

| PS.g4 Construct | scanner.rl Status | Gaps |
|----------------|-------------------|------|
| Inline math `$...$` | `inline_math` → `EMIT_BLOCK("math")` | Works. Interior scanned recursively. |
| Display math `\[...\]`, `$$...$$` | `display_math`, `display_2_math` | Works. |
| Equation environment | `equation` → `EMIT_BLOCK` | Works. Interior scanned recursively. |
| Align/gather/split/multline | `align`, `gather`, `split`, `multline` etc. | Works. Interior scanned recursively. |
| Fraction `\frac{a}{b}` | `frac` → `EMIT_BLOCK("frac",5,0)` | Works. |
| Binomial `\binom`, `\choose` | **Missing** | Whole PS.g4 binom rule needed. |
| Sqrt `\sqrt{...}`, `\sqrt[n]{...}` | `sqrt` defined in latex.rl but **not emitted in scanner.rl** | Add emission. |
| Sub/superscript `_`, `^` | `math_sub`, `math_sup` (guarded by `g_in_math`) | Emits flat tokens; no nesting. |
| Sum/prod/lim/int with limits | `sum_full`, `prod_full`, `lim_full`, `int_full` | Captured as `EMIT_BLOCK`. |
| Math operators (+,-,*,×,·,÷) | `math_op` | Flat tokens; no precedence. |
| Math relations (=,<,>,≤,≥,≡,≠) | `math_rel` | Flat tokens; no nesting. |
| Math functions (sin,cos,tan,log,ln,exp,...) | `math_fn` | Flat tokens; no args parsing. |
| Greek letters | `math_greek` | Complete for common set. |
| Special symbols (∂,∞,∅,∇,ℏ,∀,∃,∈,→,...) | `math_sym` | Covers ~30 common symbols. |
| Numbers | `math_num` | Only integer digits. |
| Variables (single letter) | `math_var` | Only `alpha` (single-letter). |
| Matrices `\begin{matrix}...\end{matrix}` | `matrix`, `pmatrix`, `bmatrix`, `vmatrix` as `EMIT_BLOCK` | No row/cell parsing. |
| Cases `\begin{cases}` | `cases` → `EMIT_BLOCK` | Interior treated as generic. |
| Subarray `\begin{subarray}` | `subarray` → `EMIT_BLOCK` | Interior not specially parsed. |
| Parentheses `(expr)` | `parens` → `EMIT_BLOCK` | Works. |
| Braces `{expr}` | `braces` → `EMIT` | Emits text, no expression scoping. |
| Display math `\begin{displaymath}` | `displaymath` → `EMIT_BLOCK` | Works. |
| Math environment `\begin{math}` | `math` → `EMIT_BLOCK` | Works. |

### 1.2 PS.g4 Constructs Entirely Missing from scanner.rl

| PS.g4 Construct | Description | Priority |
|----------------|-------------|----------|
| `\lgroup`, `\rgroup` | Group delimiters | Medium |
| `\lbrace`, `\rbrace` | Brace command aliases | Low |
| `\lbrack`, `\rbrack` | Bracket command aliases | Low |
| `\lvert`, `\rvert`, `\vert` | Vertical bar variants | High |
| `\|` (norm) | Double-vertical norm bars | High |
| `\lfloor`, `\rfloor`, `\llcorner`, `\lrcorner` | Floor delimiters | Medium |
| `\lceil`, `\rceil`, `\ulcorner`, `\urcorner` | Ceiling delimiters | Medium |
| `\left`, `\right`, `\mleft`, `\mright` | Scalable delimiters | High |
| `\operatorname{name}` | Named operator | High |
| `\floor`, `\ceil` | Floor/ceil functions | Medium |
| `\gcd`, `\lcm` | GCD/LCM functions | Medium |
| `\max`, `\min`, `\det` | Already in `math_fn` | Done |
| `\eye`, `\zeros`, `\ones`, `\diag` | Matrix constructors | Medium |
| `\norm`, `\rank`, `\trace`, `\tr` | Linear algebra functions | Medium |
| `\rref`, `\hstack`, `\vstack` | Matrix operations | Low |
| `\nullspace`, `\diagonalize`, `\eigenvals`, `\eigenvectors`, `\svd` | Advanced LA | Low |
| `\mathit{text}` | Math italic text | Medium |
| `\binom`, `\tbinom`, `\dbinom` | Binomial coefficient | High |
| `\choose` | Binomial (plain TeX) | High |
| `\mod` | Modulo operator | Low |
| `\times`, `\cdot`, `\div`, `\ast`, `\star`, `\circ`, `\bullet` | Most in `math_op` | Partial |
| `\oplus`, `\otimes`, `\odot` | In `math_op` | Done |
| `\cap`, `\cup`, `\setminus` | In `math_op` | Done |
| `\wedge`, `\vee`, `\land`, `\lor` | Some in `math_op` | Partial |
| `\over` | Fraction (plain TeX) | Low |
| Accent commands | `\hat`, `\tilde`, `\bar`, `\vec`, `\dot`, `\ddot`, `\widehat`, `\widetilde`, `\overline`, `\underline`, `\overrightarrow`, `\bm`, `\boldsymbol`, `\mathbb`, `\mathcal`, `\mathbf`, `\mathsf`, `\mathtt`, `\mathrm`, `\mathfrak`, `\mathscr`, `\text`, `\textit` | High |
| `\variable{name}` | Variable definition | Medium |
| `\xrightarrow`, `\xRightarrow` | Extensible arrows | Medium |
| `<->`, `<=>`, `\leftrightarrow`, `\Leftrightarrow` | Transform exchange arrows | Low |
| `^T`, `^{\top}`, `'` (transpose) | Transpose notation | Medium |
| `!` (factorial) | Postfix operator | Low |
| `e` / `\exponentialE` | Euler's number | Low |
| Scientific notation `1.23E-4` | E-notation numbers | Low |
| Percentage `50\%` | Percent numbers | Low |
| Decimal numbers `1,234.56` | Formatted numbers | Medium |
| Differential `d` | Differential operator | Low |
| Elementary row transforms | Matrix transform notation | Low |
| `\eval` at points | Evaluation-at notation | Low |

---

## 2. Phased Implementation Plan

### Phase A: Complete the Lexical Token Vocabulary (Week 1–2)

**Goal:** Ensure every atomic mathematical symbol from PS.g4 is recognized as a token.

#### A.1 Delimiter Completeness

Add to `latex.rl` the PS.g4 delimiter vocabulary:

```ragel
# New: PS.g4 delimiters
l_group   = '\\lgroup';
r_group   = '\\rgroup';
l_brace_cmd = '\\lbrace';
r_brace_cmd = '\\rbrace';
lbrack_cmd   = '\\lbrack';
rbrack_cmd   = '\\rbrack';
l_vert    = '\\lvert';
r_vert    = '\\rvert';
vert_cmd  = '\\vert';
norm      = '\\|';              # \| — double vertical for norms
l_floor   = '\\lfloor';
r_floor   = '\\rfloor';
ll_corner = '\\llcorner';
lr_corner = '\\lrcorner';
l_ceil    = '\\lceil';
r_ceil    = '\\rceil';
ul_corner = '\\ulcorner';
ur_corner = '\\urcorner';
left_cmd  = '\\left';
right_cmd = '\\right';
mleft_cmd = '\\mleft';
mright_cmd = '\\mright';
```

**scanner.rl emission (Phase A only — flat tokens):**
```ragel
norm          => { if (g_in_math) EMIT("norm"); };
l_floor       => { if (g_in_math) EMIT("l_floor"); };
r_floor       => { if (g_in_math) EMIT("r_floor"); };
l_ceil        => { if (g_in_math) EMIT("l_ceil"); };
r_ceil        => { if (g_in_math) EMIT("r_ceil"); };
left_cmd      => { if (g_in_math) EMIT("left_cmd"); };
right_cmd     => { if (g_in_math) EMIT("right_cmd"); };
mleft_cmd     => { if (g_in_math) EMIT("mleft_cmd"); };
mright_cmd    => { if (g_in_math) EMIT("mright_cmd"); };
```

#### A.2 Missing Operators

Extend `math_op` in scanner.rl to cover all PS.g4 operators:

```ragel
math_op = '+' | '-' | '\\times' | '\\cdot' | '\\pm' | '\\mp'
        | '\\div' | '\\ast' | '\\star' | '\\circ' | '\\bullet'
        | '\\oplus' | '\\otimes' | '\\odot'
        | '\\cap' | '\\cup' | '\\setminus'
        | '\\wedge' | '\\vee' | '\\land' | '\\lor'
        # NEW:
        | '\\bigcirc' | '\\bigtriangleup' | '\\bigtriangledown'
        | '\\sqcap' | '\\sqcup' | '\\uplus' | '\\amalg'
        | '\\dagger' | '\\ddagger' | '\\wr'
        | '\\diamond' | '\\bigcirc' | '\\triangleleft' | '\\triangleright';
```

#### A.3 Missing Relations

Extend `math_rel`:

```ragel
math_rel = '=' | '\\lt' | '\\gt' | '\\leq' | '\\geq' | '\\le' | '\\ge'
         | '\\leqslant' | '\\geqslant'
         | '\\equiv' | '\\approx' | '\\sim' | '\\propto'
         | '\\simeq' | '\\cong' | '\\neq' | '\\ne' | '<' | '>'
         | '\\ll' | '\\gg' | '\\perp' | '\\parallel'
         | '\\not\\equiv'
         # NEW from PS.g4:
         | '\\doteq' | '\\bowtie' | '\\models' | '\\dashv' | '\\vdash'
         | '\\smile' | '\\frown' | '\\asymp' | '\\mid'
         | '\\succ' | '\\prec' | '\\succeq' | '\\preceq'
         | '\\sqsubseteq' | '\\sqsupseteq' | '\\ni'
         | '\\subset' | '\\supset' | '\\subseteq' | '\\supseteq';
```

#### A.4 Missing Functions

Extend `math_fn`:

```ragel
math_fn = '\\sin' | '\\cos' | '\\tan' | '\\csc' | '\\sec' | '\\cot'
        | '\\arcsin' | '\\arccos' | '\\arctan' | '\\arccsc' | '\\arcsec' | '\\arccot'
        | '\\sinh' | '\\cosh' | '\\tanh'
        | '\\arsinh' | '\\arcosh' | '\\artanh'
        | '\\arcsinh' | '\\arccosh' | '\\arctanh'
        | '\\log' | '\\ln' | '\\exp'
        | '\\lim' | '\\sum' | '\\prod' | '\\int' | '\\oint'
        | '\\max' | '\\min' | '\\sup' | '\\inf' | '\\det'
        | '\\gcd' | '\\lcm' | '\\deg'
        | '\\dim' | '\\hom' | '\\ker' | '\\Pr'
        # NEW from PS.g4:
        | '\\floor' | '\\ceil'
        | '\\arg' | '\\argmax' | '\\argmin'
        | '\\operatorname'                 # Will be handled separately
        | '\\iint' | '\\iiint' | '\\iiiint'  # Multiple integrals
        | '\\idotsint';
```

#### A.5 Complete Greek and Symbol Sets

Add to `math_greek` (already comprehensive):

```ragel
# Add any missing:
| '\\digamma' | '\\varkappa'
```

Add to `math_sym` (extend with PS.g4 OTHER_SYMBOL set):

```ragel
math_sym = '\\partial' | '\\infty' | '\\emptyset' | '\\varnothing'
         | '\\nabla' | '\\triangle' | '\\angle' | '\\square'
         | '\\hbar' | '\\ell' | '\\wp' | '\\Im' | '\\Re'
         | '\\forall' | '\\exists' | '\\nexists' | '\\neg' | '\\lnot'
         | '\\in' | '\\notin' | '\\ni' | '\\subset' | '\\supset'
         | '\\subseteq' | '\\supseteq' | '\\to' | '\\rightarrow'
         | '\\leftarrow' | '\\leftrightarrow' | '\\Rightarrow' | '\\Leftarrow'
         | '\\mapsto' | '\\longmapsto' | '\\longrightarrow'
         | '\\cdots' | '\\vdots' | '\\ddots'
         | '\\therefore' | '\\because' | '\\diamond'
         # NEW from PS.g4 complete set:
         | '\\Bbbk' | '\\bigstar' | '\\measuredangle' | '\\sphericalangle'
         | '\\eth' | '\\diagdown' | '\\diagup'
         | '\\clubsuit' | '\\diamondsuit' | '\\heartsuit' | '\\spadesuit'
         | '\\Diamond' | '\\complement' | '\\Finv' | '\\Game'
         | '\\triangledown' | '\\vartriangle' | '\\blacklozenge' | '\\lozenge'
         | '\\blacksquare' | '\\mho' | '\\blacktriangle'
         | '\\sharp' | '\\flat' | '\\natural'
         | '\\imath' | '\\jmath' | '\\hslash'
         | '\\backprime' | '\\circledS' | '\\surd'
         | '\\prime' | '\\backprime'
         | '\\aleph' | '\\beth' | '\\gimel' | '\\daleth'
         | '\\digamma';
```

#### A.6 Accent Pattern Support

Add to `latex.rl`:

```ragel
# Accent commands — wrap their argument
accent_cmd = '\\acute' | '\\bar' | '\\overline' | '\\breve' | '\\check'
           | '\\widecheck' | '\\dot' | '\\ddot' | '\\grave' | '\\hat'
           | '\\tilde' | '\\widetilde' | '\\vec' | '\\overrightarrow'
           | '\\bm' | '\\boldsymbol' | '\\text' | '\\textit'
           | '\\mathbb' | '\\mathbin' | '\\mathbf' | '\\mathcal'
           | '\\mathclap' | '\\mathclose' | '\\mathellipsis'
           | '\\mathfrak' | '\\mathinner' | '\\mathit'
           | '\\mathnormal' | '\\mathop' | '\\mathopen'
           | '\\mathord' | '\\mathpunct' | '\\mathrel'
           | '\\mathring' | '\\mathrlap' | '\\mathrm'
           | '\\mathscr' | '\\mathsf' | '\\mathsterling' | '\\mathtt';

accent = accent_cmd braces;
```

**scanner.rl emission:**
```ragel
accent => { if (g_in_math) EMIT("accent"); };
```

#### A.7 Number Format Extensions

Extend `math_num`:

```ragel
# PS.g4 NUMBER: digit+ (comma digit digit digit)*  | digit* . digit+
math_num = digit+ (',' digit digit digit)* ('.' digit+)?;
# SCIENTIFIC: math_num 'E' [+-]? digit+
math_scientific = math_num [eE] [+-]? digit+;
# PERCENT: math_num '\%'
math_percent = math_num '\\%';
```

**scanner.rl emission:**
```ragel
math_scientific => { if (g_in_math) EMIT("scientific"); };
math_percent    => { if (g_in_math) EMIT("percent"); };
```

---

### Phase B: Expression Structure — Build Nested ASTs (Week 3–6)

**Goal:** Replace flat token emission with hierarchical expression grouping, mirroring
PS.g4's recursive descent grammar: `expr → additive → mp → unary → postfix → exp → comp → atom`.

#### B.1 Strategy: "Group and Nest" with EMIT_BLOCK

The key mechanism is `EMIT_BLOCK`, which already handles recursive scanning.
We extend it to emit expression-group tokens that wrap their interior:

| PS.g4 Rule | Ragel Pattern | Emission Type |
|-----------|---------------|---------------|
| `expr` / `additive` | Chains of `+`/`-` separated sub-expressions | `EMIT_BLOCK("expr")` with interior re-scanned |
| `mp` (multiplicative) | Chains of `*`, `\times`, `\cdot`, `/`, `\div` | `EMIT_BLOCK("mp")` with interior re-scanned |
| `group` | Any bracket/paren/brace delimited expression | `EMIT_BLOCK("group")` |
| `abs_group` | `|expr|`, `\lvert...\rvert`, `\vert...\vert` | `EMIT_BLOCK("abs")` |
| `norm_group` | `\|expr\|` | `EMIT_BLOCK("norm_group")` |
| `floor_group` | `\lfloor...\rfloor`, `\llcorner...\lrcorner` | `EMIT_BLOCK("floor")` |
| `ceil_group` | `\lceil...\rceil`, `\ulcorner...\urcorner` | `EMIT_BLOCK("ceil")` |
| `frac` | `\frac{numerator}{denominator}` | `EMIT_BLOCK("frac",5,0)` (already exists) |
| `binom` | `\binom{top}{bottom}`, `{top\choose bottom}` | `EMIT_BLOCK("binom")` |
| `matrix` | `\begin{matrix}...&...\\...\end{matrix}` | `EMIT_BLOCK("matrix")` with cell parsing |
| `det` | `\begin{vmatrix}...\end{vmatrix}` | `EMIT_BLOCK("det")` |
| `func` | `\sin(x)`, `\log(x+1)`, `\max(a,b)` | `EMIT_BLOCK("function")` |
| `sqrt` | `\sqrt[n]{expr}` | `EMIT_BLOCK("sqrt")` |
| `accent` | `\hat{x}`, `\vec{v}`, `\widetilde{abc}` | `EMIT_BLOCK("accent")` |
| `exp` (exponentiation) | `base^{exponent}` with optional subscript | `EMIT_BLOCK("exp")` |
| `eval_at` | `expr\|_{sub}^{sup}` | `EMIT_BLOCK("eval_at")` |

#### B.2 Implementation in latex.rl

**Expression grouping (operator chains):**

```ragel
# An expression is a chain of additive terms
# expr := term (add_op term)*
add_op = ('+' | '-') @{n++;};

# A term is a chain of multiplicative factors  
mp_op = ('*' | '\\times' | '\\cdot' | '/' | '\\div') @{m++;};

# Match: expr (op expr)* where both sides are non-trivial
# Strategy: When we see op followed by another atom, wrap the whole chain
expr_chain = (atom_or_group (add_op atom_or_group)+) @{g_expr_depth++;};
mp_chain   = (atom_or_group (mp_op atom_or_group)+) @{g_expr_depth++;};
```

**Bracket/group matching (extend existing `parens` pattern):**

```ragel
# Universal group: any delimited expression
group =
    '(' any+ :> ')'            # Parentheses (already exists as 'parens')
  | '[' any+ :> ']'            # Brackets
  | '{' any+ :> '}'            # Braces (already exists as 'braces')
  | '\\lgroup' any+ :> '\\rgroup'
  | '\\lbrace' any+ :> '\\rbrace'
  | '\\lbrack' any+ :> '\\rbrack'
  | '\\left' . any+ :> '\\right' .
  | '\\mleft' . any+ :> '\\mright' .;
```

**Norm, absolute, floor, ceiling groups:**

```ragel
abs_group = ('|' any+ :> '|')
          | ('\\lvert' any+ :> '\\rvert')
          | ('\\vert' any+ :> '\\vert');

norm_group = '\\|' any+ :> '\\|';

floor_group = ('\\lfloor' any+ :> '\\rfloor')
            | ('\\llcorner' any+ :> '\\lrcorner');

ceil_group = ('\\lceil' any+ :> '\\rceil')
           | ('\\ulcorner' any+ :> '\\urcorner');
```

**Matrix pattern (already exists as block, add cell-level parsing):**

```ragel
# Extended matrix — detect & and \\ inside to split into rows/cells
matrix_row = ((any - ('&'|'\\\\'|'\\end'))+ ('&' (any - ('&'|'\\\\'|'\\end'))+)*);
matrix_body = matrix_row ('\\\\' matrix_row)*;
```

**Function call pattern:**

```ragel
# Function name followed by arguments
# \sin(x)  or \sin x  (noparen form)
func_single = math_fn (math_sub? math_sup?)? (braces | parens);
func_multi  = math_fn (math_sub? math_sup?)? parens;  # \gcd(a,b)
func_sqrt   = '\\sqrt' brackets? braces;              # \sqrt[n]{expr}
```

**Binomial pattern:**

```ragel
# \binom{top}{bottom} or {top \choose bottom}
binom = ('\\binom' | '\\tbinom' | '\\dbinom') braces braces
      | braces '\\choose' braces;
```

#### B.3 Implementation in scanner.rl

Replace flat token emissions with structured EMIT_BLOCK calls:

```ragel
# Expression chains → EMIT_BLOCK for recursive interior parsing
expr_chain => { EMIT_BLOCK("expr",0,0); };
mp_chain   => { EMIT_BLOCK("mp",0,0); };

# Groups → EMIT_BLOCK (most already use this pattern)
abs_group   => { EMIT_BLOCK("abs",1,1); };
norm_group  => { EMIT_BLOCK("norm",2,2); };
floor_group => { EMIT_BLOCK("floor",7,7); };
ceil_group  => { EMIT_BLOCK("ceil",6,6); };

# Function calls → EMIT_BLOCK
func_single => { EMIT_BLOCK("function",0,0); };
func_multi  => { EMIT_BLOCK("function",0,0); };
func_sqrt   => { EMIT_BLOCK("sqrt",5,0); };

# Binomial → EMIT_BLOCK
binom => { EMIT_BLOCK("binom",0,0); };

# Matrices → EMIT_BLOCK (already handled, add cell parsing)
matrix      => { EMIT_BLOCK("matrix",14,12); };
pmatrix     => { EMIT_BLOCK("pmatrix",15,13); };
bmatrix     => { EMIT_BLOCK("bmatrix",15,13); };
vmatrix     => { EMIT_BLOCK("det",15,13); };

# Exponents → EMIT_BLOCK (wraps base^exp)
exp_expr    => { EMIT_BLOCK("exp",0,0); };

# Evaluation-at → EMIT_BLOCK
eval_at     => { EMIT_BLOCK("eval_at",0,0); };

# Transpose
transpose   => { if (g_in_math) EMIT("transpose"); };

# Factorial
factorial   => { if (g_in_math) EMIT("factorial"); };
```

#### B.4 C Runtime: Expression Nesting Stack

The current `g_in_math` flag (boolean) tracks whether we are in math mode.
We need to extend this to a depth counter so we can distinguish:

```c
// In scanner.rl C preamble:
static int g_in_math = 0;
static int g_expr_depth = 0;    // NEW: expression nesting depth
static int g_additive_depth = 0; // NEW: additive chain depth
static int g_mp_depth = 0;       // NEW: multiplicative chain depth
static int g_group_depth = 0;    // NEW: bracket group depth
```

Each `EMIT_BLOCK` increment/decrement cycle naturally tracks entry/exit
of expression scopes. The C-side can use these to annotate emitted tokens
with depth metadata:

```c
#define EMIT_BLOCK_EXPR(t, prefix_len, suffix_len, depth_var) do { \
    depth_var++; \
    /* ... existing EMIT_BLOCK logic ... */ \
    depth_var--; \
} while(0)
```

---

### Phase C: Operator Precedence and AST Construction (Week 7–10)

**Goal:** The emitted token stream should allow deterministic AST reconstruction by
respecting PS.g4's operator precedence: `(expr → additive → mp → unary → exp → comp → atom)`.

#### C.1 Strategy: Parent-Child Token ID Chains

The existing EMIT system already generates a `parent_id` for each token.
The scanner's recursive descent naturally builds the tree:

```
\author{...}          ← parent_id = filepath_id
  \frac{a+b}{c-d}     ← parent_id = author's token_id (recursive block)
    a                 ← parent_id = frac's token_id
    +                 ← parent_id = frac's token_id (flat in this scope)
    b
    c
    -
    d
```

For the math AST, we enhance this by adding **intermediate grouping tokens**
that correspond to the PS.g4 grammar rules:

```
expr                          ← expresses the full expression
├── additive                  ← groups additive chain
│   ├── mp                    ← groups multiplicative chain
│   │   ├── unary
│   │   │   └── atom (3)
│   │   └── * (operator)
│   │   └── atom (x)
│   └── + (operator)
│   └── mp
│       └── atom (2)
└── relation
    └── ...
```

Each nesting level corresponds to a different token type. The depth counters
from Phase B enable deterministic assignment of parent-child relationships.

#### C.2 Token Type Hierarchy for AST Nodes

| AST Level | Token Type | Parent Type | Children |
|-----------|-----------|-------------|----------|
| Top | `expr` | math environment | `relation` or `additive` |
| Relation | `relation` | `expr` | left `expr`, operator, right `expr` |
| Additive | `additive` | `expr` or `relation` | `mp` chains separated by `+`/`-` |
| Multiplicative | `mp` | `additive` | `unary` chains separated by `*`/`\times`/`\cdot`/`/` |
| Unary | `unary` | `mp` | sign + `postfix` |
| Postfix | `postfix` | `unary` | `exp` + optional `!`, `^T`, `\eval`|
| Exponent | `exp` | `postfix` | base `comp` + superscript `expr` + optional subscript |
| Component | `comp` | `exp` | `group` \| `func` \| `frac` \| `binom` \| `matrix` \| `atom` |
| Atom | `atom` | `comp` | `math_var` \| `math_greek` \| `math_sym` \| `math_num` \| `accent` |
| Group | `group` \| `abs` \| `norm` \| `floor` \| `ceil` | `comp` | interior `expr` |
| Function | `function` | `comp` | `math_fn` + argument `expr`(s) |
| Fraction | `frac` | `comp` | `numer` + `denom` (both `expr`) |
| Binomial | `binom` | `comp` | `upper` + `lower` (both `expr`) |
| Matrix | `matrix` \| `pmatrix` \| `bmatrix` \| `det` | `comp` | `matrix_row`* |
| Matrix Row | `matrix_row` | `matrix` | `matrix_cell`* |
| Cell | `matrix_cell` | `matrix_row` | `expr` |
| Accent | `accent` | `atom` | `accent_cmd` + base `expr` |
| Sqrt | `sqrt` | `atom` | optional `root` + `base` (both `expr`) |

#### C.3 Scanner-side Token Emission with Precedence

The key idea: the scanner emits **opening and closing markers** around each
precedence level, so downstream parsers can reconstruct the tree without
a full grammar:

```ragel
# When we detect an addition/subtraction chain:
expr_add_begin = atom_or_group '+' | atom_or_group '-' @{g_additive_depth=1;};
expr_add_end   = atom_or_group when{g_additive_depth>0};

# Scanner emission:
expr_add_begin => { EMIT("additive_begin"); g_additive_depth++; };
expr_add_end   => { EMIT("additive_end"); g_additive_depth--; };
```

This creates a bracketed token stream:
```
additive_begin
  atom (x)
  math_op (+)
  atom (y)
additive_end
```

#### C.4 Downward Operator Detection in Ragel

For the additive/multiplicative chains, the Ragel pattern detects
when a binary operator follows a term:

```ragel
# Binary expression detection:
# We've just completed an atom_or_group, and the next token is +/-
binary_add  = (atom_or_group when{g_additive_depth==0})
              [ ]* ('+'|'-') [ ]*
              @{g_additive_depth=1;} .* ;
binary_mp   = (atom_or_group when{g_mp_depth==0})
              [ ]* ('*'|'\\times'|'\\cdot'|'/'|'\\div') [ ]*
              @{g_mp_depth=1;} .* ;
```

---

### Phase D: Advanced Constructs (Week 11–14)

#### D.1 Matrix/Determinant with Cell Parsing

Current: `matrix` emits as `EMIT_BLOCK`, interior has no cell awareness.

**Improvement:** Split the interior on `&` and `\\`:

```ragel
# In latex.rl:
matrix_col_delim = '&';
matrix_row_delim = '\\\\';

# Recursive: inside a matrix block, scan each row separately
matrix_row_body = (any - (matrix_col_delim|matrix_row_delim|'\\end'))+;
matrix_row = matrix_row_body (matrix_col_delim matrix_row_body)*;
```

**In scanner.rl:**
```ragel
# Inside matrix EMIT_BLOCK, the recursive scanner will see:
matrix_col_delim => { EMIT("matrix_col_delim"); };
matrix_row_delim => { EMIT("matrix_row_delim"); };
```

The downstream parser reads: cells between `&`, rows between `\\`.

#### D.2 Function Argument Parsing

```ragel
# \sin(x+1) → emit function block wrapping interior
# \sin x    → no-paren form: just the next atom
# \gcd(a,b) → multi-arg: comma-separated
# \lim_{x\to 0} f(x) → limit with bound

# Multi-arg detection:
func_multi_arg = math_fn math_sub? math_sup? parentheses;
# Comma inside function args:
func_arg_sep = ',' when{g_in_func_args};
```

Set `g_in_func_args` flag when inside function parentheses.

#### D.3 Sum/Product/Integral with Bounds

Current `sum_full`, `prod_full`, `int_full`, `lim_full` already handle bounds.
Improvement: emit the bounds as separate child tokens:

```ragel
# sum_full already emits EMIT_BLOCK("sum",...)
# Inside, sub/sup are handled; add explicit bound tokens:
sum_bound_lower = '_' braces when{g_in_sum};
sum_bound_upper = '^' braces when{g_in_sum};
integral_diff  = 'd' alpha when{g_in_int};  # dx, dy, dt
```

#### D.4 Variable Definitions

```ragel
# \variable{name} — PS.g4 construct
variable_cmd = '\\variable' braces;
# Optional subscript
variable_sub = '_' (braces | alpha | digit);
# Complete variable: \variable{X}_{i} → emit as variable_def
```

---

### Phase E: Output Format and Integration (Week 15+)

#### E.1 Enhanced Token JSON Output

Each emitted token already carries: `filepath`, `filepath_id`, `token_id`,
`parent_id`, `offset`, `length`, `type`, `text`. For math AST tokens, add:

```json
{
  "type": "additive",
  "token_id": 12345,
  "parent_id": 12300,
  "depth": 2,
  "children": ["12346", "12348"],
  "operator_count": 1
}
```

The recursive scanning via `EMIT_BLOCK` automatically establishes
parent-child relationships through the `parent_id` field.

#### E.2 Token Type Summary (New Math Types)

| Type | Description | PS.g4 Rule |
|------|-------------|-----------|
| `abs` | Absolute value `\|expr\|` | `abs_group` |
| `norm_group` | Norm `\\|expr\\|` | `norm_group` |
| `floor` | Floor `\lfloor expr \rfloor` | `floor_group` |
| `ceil` | Ceiling `\lceil expr \rceil` | `ceil_group` |
| `additive` | Addition/subtraction chain | `additive` |
| `multiplicative` | Multiplication/division chain | `mp` |
| `relation` | Equality/inequality chain | `relation` |
| `unary` | Unary +/- expression | `unary` |
| `exp` | Exponentiation `base^{exp}` | `exp` |
| `binom` | Binomial `\binom{n}{k}` | `binom` |
| `function` | Function call `\sin(x)` | `func` |
| `matrix_row` | Matrix row | `matrix_row` |
| `matrix_cell` | Matrix cell | (implied by `&`) |
| `accent` | Accented expression | `accent` |
| `sqrt` | Square root | `\sqrt` |
| `eval_at` | Evaluation-at-bar | `eval_at` |
| `differential` | `dx`, `dy` differential | `DIFFERENTIAL` |
| `scientific` | E-notation number | `E_NOTATION` |
| `percent` | Percentage number | `PERCENT_NUMBER` |
| `variable_def` | `\variable{name}` | `VARIABLE` |
| `transform` | Matrix row transform | `elementary_transform` |
| `transpose` | `^T`, `^{\top}` | `transpose` |
| `factorial` | `!` postfix operator | `BANG` |
| `norm_delim` | `\|` token | `NORM` |
| `l_floor`, `r_floor` | Floor delimiters | `L_FLOOR`, `R_FLOOR` |
| `l_ceil`, `r_ceil` | Ceiling delimiters | `L_CEIL`, `R_CEIL` |
| `left_cmd`, `right_cmd` | Scalable delimiters | `L_LEFT`, `R_RIGHT` |

---

## 3. Implementation Order by Impact

| Priority | Phase | Effort | Impact |
|----------|-------|--------|--------|
| **P0** | A.1 Delimiters (norm, abs, floor, ceil) | 2 days | High — enables expression grouping |
| **P0** | A.6 Accent support | 1 day | High — common in papers |
| **P0** | B.2 Group detection (abs, norm, floor, ceil as EMIT_BLOCK) | 3 days | High — nested expression parsing |
| **P1** | A.4 Missing functions (floor, ceil, gcd, lcm) | 1 day | Medium |
| **P1** | A.5 Complete symbol set | 1 day | Medium |
| **P1** | B.4 Binomial support | 1 day | Medium |
| **P1** | B.2 sqrt emission fix | 1 day | Medium |
| **P1** | A.7 Number formats (decimal, scientific) | 1 day | Low-Medium |
| **P2** | C.1 Additive chain detection | 3 days | High — operator precedence |
| **P2** | C.1 Multiplicative chain detection | 2 days | High |
| **P2** | B.2 Function call parsing | 2 days | Medium |
| **P3** | D.1 Matrix cell parsing | 2 days | Medium |
| **P3** | D.3 Integral/sum bound parsing | 2 days | Medium |
| **P4** | A.2-A.3 Missing operators/relations | 1 day | Low |
| **P4** | D.4 Variable definitions | 1 day | Low |
| **P4** | D.1 Transform notation | 1 day | Low |

---

## 4. Testing Strategy

### 4.1 Test Corpus

- Use existing 1079 `.tex` files in `scanner/2003/` as the test corpus
- Target: run scanner on all files, verify zero crashes or parse errors
- Measure: token coverage — what fraction of math constructs are captured

### 4.2 Validation Approach

For each new token type:
1. Create a minimal `.tex` file with the construct
2. Run `scanner.out` and verify the token appears with correct type
3. Verify parent-child relationships match the expected AST structure
4. Add to regression test suite

### 4.3 Coverage Metrics

| Metric | Current | Phase A Target | Phase B Target | Phase C Target |
|--------|---------|---------------|----------------|----------------|
| Math symbol coverage | ~60 symbols | ~120 symbols | ~120 symbols | ~120 symbols |
| Expression grouping | Flat tokens only | Flat + group blocks | Group + chain blocks | Full precedence |
| Operator precedence | None | None | Partial (block nesting) | Full (parent_id chains) |
| Matrix cell parsing | None | None | Row/cell detection | Cell-level tokens |
| Function args | None | None | Arg block emission | Multi-arg support |
| Accent parsing | Partial (2 cmds) | Full (30+ cmds) | Full | Full |

---

## 5. Risks and Mitigations

| Risk | Mitigation |
|------|-----------|
| Ragel state explosion with complex math patterns | Keep patterns modular; use `include` directives; limit backtracking |
| Performance degradation from recursive EMIT_BLOCK | Mathematical expressions are typically short (<500 chars); recursion depth is bounded |
| Ambiguous parsing (e.g., `a/b` vs `\frac{a}{b}`) | Follow PS.g4 precedence rules exactly; add conflict resolution notes |
| Unicode handling in math symbols | Maintain current ISO-8859-1/UTF-8 fallback encoding logic |
| Breaking existing non-math token extraction | Math-only changes guarded by `g_in_math` flag; no impact on prose/environments |
| Ragel v6 vs v7 syntax compatibility | Use currently installed Ragel version; document version requirement |
