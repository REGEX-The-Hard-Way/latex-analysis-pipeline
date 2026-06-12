# Dead Code & Unused Files Report

Generated 2026-06-12 from a full project audit.

---

## Summary

| Category | Count | Action |
|----------|-------|--------|
| Orphaned/old source files | 4 | Remove or archive |
| Duplicate scripts | 2 | Consolidate |
| Incomplete/stub implementations | 3 | Finish or remove |
| Broken/abandoned scripts | 2 | Fix or remove |
| Placeholder directories | 1 | Populate or remove |
| Stale documentation references | 5+ | Update or remove |
| Upstream-issue mirror files | 22 | Archive or remove |
| Missing referenced files | 3 | Create or update paths |

---

## 1. Orphaned / Old Source Files

### `include/old/macro_expander.c` (1400+ lines) and `include/old/macro_expander.out`
- **Status:** Explicitly placed in `old/` directory — these are the original `macro_expander.c` from upstream `allofphysicsgraph/latex-in-arxiv`.
- **Why dead:** The active implementation is the root-level `macro_expander.c` (985 lines), which was rewritten with proper macro handling (`\def`, `\edef`, `\xdef`, `\let`, `\expandafter`, `\aftergroup`, `\noexpand`).
- **Action:** These are already isolated in `old/`. Consider deleting them or moving them to an `archive/` branch.

### `scanner/latex_macro.rl` (156 lines)
- **Status:** Incomplete Ragel-based macro expander.
- **Why dead:** All action blocks are empty stubs (`action collect_keyword { }`, `action store_macro { }`, etc.). The machine definition is purely structural with no actual behavior. The active macro expander is `macro_expander.c` in C, not Ragel.
- **Action:** Delete, or annotate with a `// TODO` if this is intended as future work.

### `sandbox/macro_statechart.rl` (202 lines)
- **Status:** Experimental Ragel statechart-based macro expander.
- **Why dead:** Unfinished. Comments say "Macro expansion would go here…" and only counts definitions. Uses C++ (`iostream`, `using namespace std`) which is inconsistent with the rest of the C codebase.
- **Action:** Either complete the implementation or delete. If kept as research, move to a `research/` or `experiments/` directory with a README explaining its status.

---

## 2. Duplicate Scripts

### `scanner/expand_macros.sh` (24 lines) — duplicate of root `expand_macros.sh`
- **Status:** Identical content. Both reference `./macro_expander.out` which does not exist as a separate binary (the actual binary is `./macro_expander.out`).
- **Why dead:** The root-level `expand_macros.sh` is the canonical one. The scanner copy is redundant.
- **Action:** Delete `scanner/expand_macros.sh`. Fix the root `expand_macros.sh` to reference `./macro_expander.out` instead of `./macro_expander.out`.

### `scanner/build_multi_analyzer.sh` (61 lines)
- **Status:** Shell wrapper that runs `make` in the `multi_analyzer/` directory.
- **Why dead:** The `scanner/multi_analyzer/Makefile` already handles the build. This script adds no value beyond what `cd multi_analyzer && make` does.
- **Action:** Delete, or merge its diagnostic output into the Makefile as an `info` target.

---

## 4. Placeholder Directories

### `examples/`
- **Contains:** Only a `README.md` (53 lines).
- **Why dead:** The README references `sample.tex` which does not exist. No actual example input or output files are present.
- **Action:** Either add real example files (`sample.tex`, `sample.tok`, `sample.sent`) or remove the directory and fold the README content into the main `README.md`.

---

## 5. Stale Documentation References

### Missing file: `macro_expander.out`
- Referenced in: `README.md` (line 126), `docs/ARCHITECTURE.md` (line 134), `docs/macro_expander.md` (multiple), `scanner/Makefile` (line 16), `expand_macros.sh` (line 13), `scanner/expand_macros.sh` (line 13), `tests/test_issues.py` (line 91).
- **Reality:** The binary is `macro_expander.out`. There is no `macro_expander.out` target in the Makefile and no `macro_expander.c` source.
- **Action:** Either (a) rename `macro_expander.c`/`macro_expander.out` to `macro_expander.c`/`macro_expander.out` consistently, or (b) update all references to use `macro_expander.out`.

### Missing file: `issues_list.json`
- Referenced in: `docs/ISSUES_ANALYSIS.md`, `tests/README.md`, `tests/test_issues.py`.
- **Reality:** Listed in `.gitignore` (line 62-65, duplicated). This is a snapshot from the upstream `allofphysicsgraph/latex-in-arxiv` repo that was never committed.
- **Action:** Either commit it or remove all references to it.

### Missing file: `sample.tex`
- Referenced in: `examples/README.md`.
- **Action:** Create it or remove the reference.

### Missing Makefile target: `make expand_macros`
- Referenced in: `docs/macro_expander.md` (line 17).
- **Reality:** The Makefile has `macro_expander` target (not `expand_macros`), and it references a non-existent source file `expand_macros.rl`.
- **Action:** Fix the Makefile `macro_expander` target to actually work, or remove it if `macro_expander.c` is the canonical macro expander.

---

## 7. Unused/Dead Code Paths in Active Files

### `macro_expander.c` — unused global variables
- `g_parsed_defs`, `g_parsed_lets`, `g_parsed_expandafter`, `g_parsed_noexpand`, `g_parsed_aftergroup` (lines 872-876): These globals are declared and set to 0 but never incremented outside of their initialization. The `print_report()` function doesn't use them.
- **Action:** Remove, or implement the counting logic.

### `scanner/file_mmap.c` — `file_write_string()` (line 70-77)
- **Status:** Declared in `file_mmap.h` but never called anywhere in the codebase.
- **Action:** Remove if unused, or document as public API.

### `include/regex_util.c` — `regex_macro_expand()` (line 266-305)
- **Status:** Declared in `regex_util.h` but never called. The macro expansion is handled entirely by `macro_expander.c`.
- **Action:** Remove or mark as deprecated.

### `include/regex_util.h` — `regex_split()` and `regex_free_split()` (lines 35-40)
- **Status:** Declared and implemented in `regex_util.c` but never called anywhere in the project.
- **Action:** Remove or keep as public API with documentation.

### `scanner/murmur3.c` — commented-out `main()` (lines 75-81)
- **Status:** Dead test code in a comment block.
- **Action:** Remove the comment block.

### `scanner/scanner.rl` — `EMIT` macro (line 7-18)
- **Status:** The `EMIT` macro (leaf token, no recursion) is defined but never used in the pattern actions. All token emissions use `EMIT_BLOCK`.
- **Action:** Remove `EMIT` or keep for documentation purposes.

---

## 8. Inconsistent Naming Conventions

| Issue | Details |
|-------|---------|
| `macro_expander.c` vs `macro_expander` | The file is named `macro_expander.c` but referenced as `macro_expander` in docs and scripts |
| `scanner.out` vs `scanner` | The Makefile builds `scanner.out` but `README.md` sometimes refers to `./scanner` |
| `multi_analyzer.out` vs `multi_analyzer` | Binary naming inconsistency in docs |
| `sent_split.out` vs `sent_split` | Same pattern |

---

## Recommended Cleanup Order

1. **Immediate (safe deletes):**
   - Delete `sandbox/statechart.rl` (unrelated Ragel demo)
   - Delete `scanner/expand_macros.sh` (duplicate)
   - Delete `scanner/build_multi_analyzer.sh` (redundant)
   - Delete `docs/issues/` (22 upstream mirror files)
   - Remove the duplicate `issues_list.json` line from `.gitignore`
   - Remove the commented-out `main()` from `scanner/murmur3.c`

2. **Short-term (fix or consolidate):**
   - Fix all `macro_expander.out` references to point to `macro_expander.out`, or rename the binary
   - Delete or fix `scanner/latex_macro.rl` (empty stub actions)
   - Delete or fix `sandbox/macro_statechart.rl` (incomplete)
   - Delete redundant `scanner/resolve_ref_labels.py` (superseded by tests)
   - Fix or delete `scanner/read_tok.py` (broken)

3. **Medium-term (decisions needed):**
   - Decide fate of `include/old/` — archive or delete
   - Populate or remove `examples/`
   - Commit or stop referencing `issues_list.json`
   - Clean up unused globals in `macro_expander.c`
   - Remove or document unused `regex_util` functions

**Total dead/useless lines: ~2,415 lines of code + ~450 lines of duplicated upstream-issue files.**
