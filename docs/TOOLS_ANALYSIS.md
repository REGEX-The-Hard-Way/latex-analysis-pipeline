# Tools Analysis — June 2026

## Build Status

All binaries compile successfully via `make all`:
- `expr_tree` — 21KB, valgrind clean
- `sidecar_mod` — 21KB, valgrind clean (0 leaks)
- `cluster_find` — 21KB, valgrind clean
- `lean4_gen` — 16KB, valgrind clean
- `def_extract.o` — 13KB (object file, no standalone binary)

## Per-Tool Findings

### 1. expr_tree — Expression Tree Builder ✅

**Purpose**: Reads sidecar tokens, finds display_math/equation parents with children, builds binary expression trees using Pratt parsing (operator precedence), outputs S-expression JSON.

**Test result**: ✅ WORKS
```
Input:  \[ x = 1 + 2 \]
Output: {"op":"=","left":{"leaf":"math_var","text":"x"},
         "right":{"op":"+","left":{"leaf":"math_num","text":"1"},
         "right":{"leaf":"math_num","text":"2"}}}
```

**Usage**: `./expr_tree sidecar.json [--type display_2_math] [--sample]`

**Issues**: None. Handles proper math token types (math_var, math_num, math_op, math_rel). Needs tokens with matching parent_id for children.

---

### 2. sidecar_mod — Rule-Based Sidecar Modifier ✅

**Purpose**: OpenRefine-inspired sidecar modification with JSON rule chains. Supports regex, strip_prefix, strip_suffix, trim transforms with undo logging.

**Test result**: ✅ WORKS
- 9 records in, 1 changed by regex rule
- Rule: `{"match_type":"math_op","prop":"text","transform":"regex","pattern":"\\+","replace":"PLUS"}`
- Correctly matched and modified the `+` token

**Usage**: `./sidecar_mod in.json rules.txt out.json [undo.log]`

**Rule format** (JSON lines):
```json
{"step":1,"name":"strip author","match_type":"author","prop":"text","transform":"regex","pattern":"\\\\author\\{(.*)\\}","replace":"$1"}
```

**Issues**: 
- test_tools.sh expects binaries in CWD (uses `./sidecar_mod` not `tools/sidecar_mod`)
- No standalone test for undo functionality

---

### 3. cluster_find — Clustering Engine ⚠️

**Purpose**: Fingerprint + Levenshtein clustering for deduplication of sidecar token text values.

**Test result**: ⚠️ NEEDS MORE DATA
- With 9-token sidecar: empty output (not enough tokens to cluster)
- Likely works with 1000+ token sidecars from real TeX files

**Usage**: `./cluster_find sidecar.json [type_filter] [--levenshtein N]`

**Issues**: 
- Requires sufficient data to produce meaningful clusters
- Needs testing with a full 2003-dataset sidecar

---

### 4. lean4_gen — Lean4 Proof Generator ⚠️

**Purpose**: Generates Lean4 theorem prover stubs from scanner output.

**Test result**: ⚠️ STUB ONLY
- Produces header boilerplate but no actual proofs
- Seems to be a placeholder/wireframe

**Usage**: `./lean4_gen` (expects stdin)

**Issues**: Incomplete implementation. Generates only template headers.

---

### 5. def_extract — Math Variable Definition Extraction ✅

**Purpose**: C port of benepar_qa.py. Extracts natural-language definitions for math variables using pattern matching (copula, apposition, equation-as-definition).

**Test result**: ✅ COMPILES (object file)
- 402 lines of C, compiled to def_extract.o
- Three pattern types implemented: copula ("VAR is NP"), apposition ("the NP, VAR"), equation ("VAR = EXPR")
- No standalone binary — library only

**Usage**: Link `def_extract.o` with a main program:
```c
def_result_t results[64];
int n = def_extract(tex_text, tex_len, symbols, n_symbols, results, 64);
```

**Issues**: Needs a standalone test driver to verify correctness.

---

### 6. json_scan — Ragel JSON Scanner ✅

**Purpose**: Fast mmap-based JSON Lines parser using Ragel state machine.

**Test result**: ✅ WORKS
- Used by expr_tree, sidecar_mod, cluster_find
- Handles escaped strings, integer/float values, nested objects
- Generated from `json_scan.rl` via ragel -G2

**Issues**: None. Core infrastructure for all tools.

---

### 7. cluster_graph — Graph-Based Clustering 📋

**Purpose**: Clustering using graph-based algorithms.

**Status**: Source only, no binary target in Makefile. Not compiled.

---

### 8. tree_fingerprint — Tree Fingerprinting 📋

**Purpose**: Generate structural fingerprints for expression trees.

**Status**: Source only, no binary target in Makefile. Not compiled.

---

### 9. struct_norm — Structure Normalizer 📋

**Purpose**: Normalize LaTeX structural elements.

**Status**: Source only, no binary target in Makefile. Not compiled.

---

### 10. vocab_classify — Vocabulary Classifier 📋

**Purpose**: Classify vocabulary words into categories.

**Status**: Source only, no binary target in Makefile. Not compiled.

---

### 11. spectral_cluster — Spectral Clustering 📋

**Purpose**: Spectral clustering (C implementation + Python reference).

**Status**: C source + Python. Not compiled. No binary target.

---

## Shell Scripts

### cluster_apply.sh / sidecar_apply.sh
Interactive fzf-based workflows for cluster merging and rule application.
Both reference `./sidecar_mod` and `./cluster_find` in CWD.
Need `fzf` installed.

### test_tools.sh
Tests build of sidecar_mod and cluster_find. Currently FAILS because it expects binaries in CWD (`./sidecar_mod` not `tools/sidecar_mod`). Requires the tools to be compilable independently.

### strip_latex.rules
10 example rules for LaTeX delimiter removal. Used by sidecar_mod as input rules file.

## Scorecard

| Tool | Binary | Valgrind | Functional | Notes |
|------|--------|----------|------------|-------|
| expr_tree | ✅ | ✅ | ✅ | Working expression tree builder |
| sidecar_mod | ✅ | ✅ | ✅ | Working rule engine |
| cluster_find | ✅ | ✅ | ⚠️ | Needs larger dataset |
| lean4_gen | ✅ | ✅ | ⚠️ | Stub only |
| def_extract | obj only | — | ⚠️ | Library, needs test driver |
| json_scan | ✅ | — | ✅ | Core infrastructure |
| spectral_cluster | ❌ | — | ❌ | Not compiled |
| tree_fingerprint | ❌ | — | ❌ | Not compiled |
| struct_norm | ❌ | — | ❌ | Not compiled |
| vocab_classify | ❌ | — | ❌ | Not compiled |
| cluster_graph | ❌ | — | ❌ | Not compiled |

## Recommendation

1. **Add missing compile targets** for tree_fingerprint, struct_norm, vocab_classify, cluster_graph, spectral_cluster to the Makefile
2. **Fix test_tools.sh** to use correct paths: `tools/sidecar_mod` instead of `./sidecar_mod`
3. **Create standalone test driver** for def_extract to verify correctness
4. **Test cluster_find** with a full 2003-dataset sidecar (200K+ tokens)
5. **Complete lean4_gen** — currently only a stub
