# Dead Code & Unused Files Report

Generated 2026-06-12 from a full project audit. Last updated 2026-06-13.

---

## Summary

| Category | Count | Action |
|----------|-------|--------|
| Orphaned/old source files | 3 | Remove or archive |
| Duplicate scripts | 3 | Consolidate |
| Incomplete/stub implementations | 2 | Finish or remove |
| Broken/abandoned scripts | 2 | Fix or remove |
| Empty/placeholder directories | 1 | Populate or remove |
| Stale documentation references | 6 | Update or remove |
| Upstream-issue mirror files | 26 | Archive or remove |
| Missing referenced files | 4 | Create or update paths |
| Unused code in active files | 7 items | Remove |

---

## 1. Orphaned / Old Source Files

### `include/old/newcommand.c` — REMOVED
- **Status:** Deleted. The active implementation is `scanner/newcommand.c`.

### `scanner/latex_macro.rl` — DELETED

### `sandbox/macro_statechart.rl` (202 lines)
- **Status:** Experimental Ragel statechart-based macro expander.
- **Why dead:** Unfinished. Comments say "Macro expansion would go here..." and it only counts definitions. Uses C++ (`iostream`, `using namespace std`) which is inconsistent with the rest of the C codebase.
- **Action:** Delete. If research value remains, move content to `sandbox/README.md` as a note.

### `sandbox/macro_expander.c` (907 lines)
- **Status:** Duplicate of `include/macro_expander.c`.
- **Why dead:** Both files are identical (both 907 lines, same implementation). The canonical version lives in `include/` where the header also resides.
- **Action:** Delete `sandbox/macro_expander.c`.

---

## 2. Duplicate Scripts

### `scanner/expand_macros.sh` (23 lines) — duplicate of root `expand_macros.sh`
- **Status:** Same content as root-level `expand_macros.sh`.
- **Why dead:** The root-level `expand_macros.sh` is the canonical one. Both reference `./macro_expander.out` but that binary does not exist — the actual binary name is `macro_expander.out` (no 'r').
- **Action:** Delete `scanner/expand_macros.sh`. Fix both copies' binary path references.

### `sandbox/expand_macros.sh` (23 lines) — duplicate of root `expand_macros.sh`
- **Status:** Same content as root-level `expand_macros.sh`. Also in `scanner/`.
- **Why dead:** Three identical copies exist.
- **Action:** Delete `sandbox/expand_macros.sh`.

### `scanner/build_multi_analyzer.sh` (61 lines)
- **Status:** Shell wrapper that runs `make` in `multi_analyzer/`.
- **Why dead:** `cd multi_analyzer && make all` does the same thing.
- **Action:** Delete.

---

## 3. Broken / Abandoned Scripts

### `sandbox/read_tok.py`
- **Status:** Attempts to read `.tok` files but has broken parsing logic. Never completed.
- **Action:** Delete.

### `sandbox/resolve_ref_labels.py`
- **Status:** Superseded by `tests/validation_tests.py` which has a proper test-based approach.
- **Action:** Delete.

---

## 4. Empty / Placeholder Directories

### `examples/`
- **Contains:** Only `README.md` (53 lines).
- **Why dead:** The README references `sample.tex` which does not exist. No actual input/output example files are present. The `sidecar.tok` and `clean.sent` files in this directory are referenced nowhere.
- **Action:** Either create real `sample.tex`, `sample.tok`, `sample.sent` files and flip the bracket-order examples to match actual output format, or remove the directory and fold README content into the main `README.md`.

---

## 5. Stale Documentation References

### `macro_expander.out` vs `macro_expander.out`
- **Referenced in:** `README.md` (line 126, 145), `docs/ARCHITECTURE.md` (line 134, 166), `docs/macro_expander.md` (multiple), `scanner/Makefile` (line 16), `expand_macros.sh` (line 13), `scanner/expand_macros.sh` (line 13), `tests/test_issues.py` (line 91).
- **Reality:** The source file is `include/macro_expander.c` and the header is `include/macro_expander.h`. The built binary should be named `macro_expander.out`. All references say `macro_expander.out` which is a misspelling.
- **Action:** (a) Correct all references from `macro_expander.out` to `macro_expander.out`, OR (b) rename the source to `macro_expander.c` and keep the binary as `macro_expander.out`. Either way, **fix all docs to be consistent**.

### `issues_list.json`
- **Referenced in:** `docs/ISSUES_ANALYSIS.md`, `tests/README.md`, `tests/test_issues.py`.
- **Reality:** Listed in `.gitignore` (lines 62-65, duplicated). This is a snapshot from the upstream repo that was never committed. The `docs/issues/` directory contains individual markdown files instead.
- **Action:** Either commit a snapshot or remove all references. Prefer removing references since the `docs/issues/` files serve the same purpose.

### `sample.tex`
- **Referenced in:** `examples/README.md` (lines 7, 51).
- **Action:** Create it or remove the references.

### `make expand_macros` target
- **Referenced in:** `docs/macro_expander.md` (line 17).
- **Reality:** The Makefile has a `macro_expander` target (not `expand_macros`), and it references a non-existent source file `expand_macros.rl`. The actual source is `../include/macro_expander.c`.
- **Action:** Either fix the Makefile target to work with `include/macro_expander.c`, or remove the `macro_expander` target from the Makefile entirely and document the build command as `gcc -O2 -Iinclude include/macro_expander.c include/regex_util.c -o macro_expander.out`.

### `docs/macro_expander.md` ($.55) — Wrong file path
- **States:** `scanner/macro_expander.c`
- **Reality:** The file is at `include/macro_expander.c`.
- **Action:** Fix to `include/macro_expander.c`.

### `docs/ARCHITECTURE.md` — Wrong file path
- **States:** `macro_expander.c` at root, `scanner/macro_expander.cpp` (line 65).
- **Reality:** The file is at `include/macro_expander.c`. It's C, not C++.
- **Action:** Fix both references.

---

## 6. Upstream-Issue Mirror Files

### `docs/issues/` directory (26 files)
- **Status:** 26 individual markdown files mirroring upstream GitHub issues from `allofphysicsgraph/latex-in-arxiv`.
- **Why dead:** They are copies of upstream discussion that has no active relevance to this fork. The `docs/ISSUES_ANALYSIS.md` already summarizes their relevance.
- **Total:** ~450 lines.
- **Action:** Delete entire `docs/issues/` directory. Update `docs/ISSUES_ANALYSIS.md` to note that individual issue files were removed (the analysis document is sufficient).

---

## 7. Unused / Dead Code Paths in Active Files

### `include/regex_util.c` — `regex_macro_expand()` (lines 266-305)
- **Status:** Declared in `regex_util.h` but never called anywhere in the codebase. Macro expansion is handled entirely by `include/macro_expander.c`.
- **Action:** Remove or mark as deprecated with a comment.

### `include/regex_util.h` — `regex_split()` and `regex_free_split()` (lines 35-40)
- **Status:** Declared and implemented in `regex_util.c` but never called.
- **Action:** Remove or document as public API.

### `scanner/file_mmap.c` — `file_write_string()` (lines 70-77)
- **Status:** Declared in `file_mmap.h` but never called anywhere.
- **Action:** Remove or document as public API.

### `scanner/murmur3.c` — commented-out `main()` (lines 75-81)
- **Status:** Dead test code in a comment block.
- **Action:** Remove the comment block.

### `scanner/scanner.rl` — `EMIT` macro (lines 7-18)
- **Status:** The `EMIT` macro (leaf token, no recursion) is defined but never used in pattern actions. All token emissions use `EMIT_BLOCK`.
- **Action:** Remove the `EMIT` macro or keep it annotated with a comment for documentation.

### `docs/issues/` — 26 files with no active purpose
- **Status:** No code files, but they are stale documentation duplicates.
- **Action:** Delete.

---

## 8. Inconsistent Naming Conventions

| Issue | Details |
|-------|---------|
| `macro_expander` vs `macro_expander` | The file is `macro_expander.c` but referenced as `macro_expander` in docs and scripts everywhere |
| `scanner.out` vs `scanner` | Makefile builds `scanner.out` but README sometimes refers to `./scanner` |
| `multi_analyzer.out` vs `multi_analyzer` | Binary naming inconsistency in docs |
| `sent_split.out` vs `sent_split` | Same pattern |
| `include/macro_expander.c` vs root `macro_expander.c` (nonexistent) | README and Makefile both reference a root-level `macro_expander.c` that does not exist |

---

## 9. Build System Issues

### Makefile `macro_expander` target (line 14-16)
```makefile
macro_expander:
	ragel -m -G2 expand_macros.rl
	$(CC) -O2 -I../include macro_expander.c ../include/regex_util.c -o macro_expander.out
```
- `expand_macros.rl` does not exist anywhere in the project.
- `macro_expander.c` at root does not exist.
- The actual files are `include/macro_expander.c` and `include/macro_expander.h`.
- **Action:** Fix the target to: `$(CC) -O2 -Iinclude include/macro_expander.c include/regex_util.c -o macro_expander.out -lm`

### `sandbox/pyproject.toml` location
- The Python package definition is inside `sandbox/`, not at the project root.
- `pip install` will not work without running from `sandbox/`.
- **Action:** Move `pyproject.toml` to the project root, or create a root-level `pyproject.toml` that references the `python/` package directory.

---

## 10. Recommended Cleanup Order

### Immediate (safe deletes — zero risk):
- [x] Deleted `scanner/latex_macro.rl` (empty stub)
- [ ] Delete `sandbox/macro_statechart.rl` (incomplete)
- [ ] Delete `sandbox/macro_expander.c` (duplicate of include/)
- [ ] Delete `scanner/expand_macros.sh` (duplicate)
- [ ] Delete `sandbox/expand_macros.sh` (duplicate)
- [ ] Delete `scanner/build_multi_analyzer.sh` (redundant)
- [ ] Delete `sandbox/read_tok.py` (broken)
- [ ] Delete `sandbox/resolve_ref_labels.py` (superseded)
- [ ] Delete `docs/issues/` directory (26 upstream mirror files)
- [ ] Remove commented-out `main()` from `scanner/murmur3.c`
- [ ] Remove duplicate `issues_list.json` lines from `.gitignore`

### Short-term (fix or consolidate):
- [ ] Fix all `macro_expander.out` references to `macro_expander.out` throughout docs, scripts, and Makefile
- [ ] Fix Makefile `macro_expander` target to link `include/macro_expander.c` directly (no Ragel)
- [ ] Create `sample.tex` for `examples/` or remove references
- [ ] Commit or remove references to `issues_list.json`
- [ ] Fix `docs/macro_expander.md` file paths (say `include/macro_expander.c`, not `scanner/macro_expander.c`)
- [ ] Fix `docs/ARCHITECTURE.md` file paths (say `include/macro_expander.c`, not root-level or `.cpp`)

### Medium-term (decisions needed):
- [x] Deleted `include/old/newcommand.c`
- [ ] Populate `examples/` with real files or remove directory
- [ ] Move `sandbox/pyproject.toml` to root, or create root-level pyproject.toml
- [ ] Remove unused globals from `include/macro_expander.c` (g_parsed_defs etc.)
- [ ] Remove or document unused `regex_util.c` functions (`regex_split`, `regex_macro_expand`)
- [ ] Remove `EMIT` macro from `scanner.rl` if proven unused
- [ ] Remove or document `file_write_string()` from `file_mmap.c`

### Verification:
After cleanup, verify with:
```bash
# No dead references to macro_expander (should map to macro_expander)
grep -r "macro_expander" --include="*.md" --include="*.sh" --include="Makefile" --include="*.py"
# Expected: zero matches (all should be macro_expander)

# No dead file references
for f in expand_macros.rl issues_list.json sample.tex; do
  grep -r "$f" --include="*.md" --include="*.py" --include="*.sh"
done
# Expected: zero matches

# No unused function declarations
grep -rn "regex_split\|regex_macro_expand\|file_write_string" --include="*.c" --include="*.h"
# Expected: declarations in headers, implementations in .c, but no callers outside those files
```

---

## Line Count Summary

| Category | Lines |
|----------|-------|
| Dead/orphaned source files | ~1,750 |
| Duplicate scripts | ~110 |
| Incomplete stubs | ~360 |
| Broken scripts | ~200 |
| Upstream issue mirrors | ~450 |
| Dead code in active files | ~150 |
| **Total dead code** | **~3,020 lines** |

---

*This report should be updated after each cleanup pass. Run the verification commands above after cleaning to confirm progress.*
