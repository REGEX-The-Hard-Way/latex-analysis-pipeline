# Macro Expansion Plan

## Current State

The macro expander lives at `include/macro_expander.c` (907 lines) with its header at
`include/macro_expander.h`. It is a **token-at-a-time** expander with proper TeX semantics
(catcodes, grouping, expansion control). It has a C library API (`macro_expander_t` struct
with `feed`, `get_output`, `feed_cb`, `reset`) and a standalone `main()` that operates as
a stdin→stdout filter with `--report` mode.

Build command: `gcc -O2 -Iinclude include/macro_expander.c include/regex_util.c -o macro_expander.out -lm`

### What Works
- `\newcommand` / `\renewcommand` / `\providecommand` with braced and unbraced names
- `\newcommand\name[n]{body}` with argument count
- `\def`, `\edef`, `\xdef` with inline parameter markers `#1..#9`
- `\let` (copy definitions)
- `\expandafter` (token reordering)
- `\aftergroup` (queue token past group end)
- `\noexpand` (suppress expansion of next token)
- Macro followed by letter (no accidental expansion)
- Nested braces in macro body
- `--report` mode for listing all macro definitions found

### What is Broken (from DEBUG_NOTES.md and testing)

| Issue | Severity | Symptom |
|-------|----------|---------|
| `@` in macro names (`\def\@foo`) | **HIGH** | Fails to parse 60% of real arXiv files that use internal LaTeX macros |
| Double hash `##` | **HIGH** | Crashes on `\def\test##1{##1}` — expects brace, gets `#` |
| Catcode table is fixed (128 entries), not dynamic | **MEDIUM** | `\makeatletter` / `\makeatother` not supported, `\verb` not handled |
| No conditional support (`\if`, `\ifx`, `\ifnum`) | MEDIUM | Cannot expand documents with TeX conditionals |
| No `\csname...\endcsname` | MEDIUM | Cannot construct command names dynamically |
| No `\begingroup` / `\endgroup` tracking | MEDIUM | Group nesting not tracked, `\aftergroup` may misbehave |
| `input_push_file()` unimplemented | LOW | No `\input` / `\include` support for multi-file documents |
| `g_parsed_defs` globals are declared but never written | LOW | Report mode counters are dead code |
| No expansion limits | LOW | No safety limit on recursive expansion (infinite loop risk) |

### Current Failure Rate
- **Original upstream code:** 189/1019 files crashed
- **Current code:** 80/1019 files still have issues (~8%)
- **Primary remaining cause:** `@` in macro names and `##` handling

---

## Phase 1: Critical Bug Fixes (Week 1-2)

### 1.1 Fix `@` in Macro Names

**Problem:** LaTeX allows `@` in command names as "internal" macros (activated by
`\makeatletter`). Around 60% of arXiv papers use `\@`-prefixed macros.

**Root cause (in `include/macro_expander.c`):**
- The scanning/lexing function that identifies macro names only accepts `[A-Za-z]` characters.
- `@` is classified as "other" (catcode 12) instead of "letter" (catcode 11).

**Fix:**
```c
// In the catcode initialization block (cat_code array setup):
// Change catcode of '@' from OTHER to LETTER when makeatletter is active.
// For now, simplest fix: always treat '@' as a letter.

// Find the tokenizer's character classification and add:
if (c == '@') return CMD_LETTER;  // or equivalent

// Better: add catcode-awareness so \makeatletter/\makeatother work properly.
```

**Success criterion:** All arXiv files with `\def\@foo` no longer crash.

### 1.2 Fix Double Hash `##`

**Problem:** In TeX, `##` in a macro body represents a literal `#` character (catcode 6
parameter character, doubled to produce one). Currently the code treats each `#` as
starting a parameter reference.

**Root cause (in `learn_body()` or equivalent):**
```c
// Current (broken) behavior:
if (c == '#') {
    c = get_next_char();           // reads '1'..'9' as parameter number
    // stores as INTERNAL_PARAM_REF
}
// This fails on '##' because it tries to treat the second '#' as a parameter number.
```

**Fix:**
```c
if (c == '#') {
    c = get_next_char();
    if (c == '#') {
        // Double hash: emit a literal '#' character
        *ep++ = '#';
    } else if (c >= '1' && c <= '9') {
        // Single hash + digit: parameter reference
        *ep++ = PARAM_BASE + (c - '1');
    } else {
        // Error: bare '#' not followed by '#' or digit
        fprintf(stderr, "Warning: stray # in macro body\n");
    }
}
```

**Success criterion:** `tests/test_macro_expander.py::TestDef::test_double_hash` passes.

### 1.3 Add Recursive Expansion Limit

**Problem:** No safety limit on recursive expansion. `\def\a{\a}\a` will expand forever
and crash with stack overflow or infinite loop.

**Fix:** Add a global expansion depth counter to `macro_expander_t`:
```c
#define MAX_EXPANSION_DEPTH 1000

struct macro_expander {
    // ... existing fields ...
    int expansion_depth;
};

// In get_x_token() or the main expansion loop:
if (me->expansion_depth > MAX_EXPANSION_DEPTH) {
    fprintf(stderr, "macro_expander: expansion depth exceeded (possible infinite loop)\n");
    me->scanner_status = SCAN_ERROR;
    return;
}
```

**Success criterion:** `\def\a{\a}\a` produces a controlled error, not a crash.

### 1.4 Fix Unused Report Counters

**Problem:** `g_parsed_defs`, `g_parsed_lets`, `g_parsed_expandafter`, `g_parsed_noexpand`,
`g_parsed_aftergroup` are declared as globals but never incremented.

**Fix (two options):**
- Option A: Remove them entirely (they are unused).
- Option B: Wire them into the expansion path so `--report` mode actually counts usage.

Prefer Option B since `--report` mode is a genuinely useful feature.

---

## Phase 2: Correctness Hardening (Week 3-6)

### 2.1 Catcode Awareness

**Problem:** The current scanner uses a fixed 128-entry `cat_code` table initialized at
startup. Real TeX macros (`\makeatletter`, `\makeatother`, `\verb`) change catcodes
dynamically.

**Implementation plan:**
1. Make the catcode table a property of `macro_expander_t` (already partially done:
   `int cat_code[128]` exists in the struct).
2. Implement `\makeatletter` (set `@` catcode to 11/letter) and `\makeatother`
   (set `@` catcode to 12/other).
3. Implement `\verb` scanning: after `\verb`, the next character becomes the delimiter;
   everything until the next delimiter is treated as verbatim text (all catcodes 12).
4. Implement `\begingroup` / `\endgroup` with catcode stack save/restore.

**Data structure:**
```c
#define CATCODE_SAVE_STACK_SIZE 256

typedef struct {
    int cat_code[128];   // per-group catcode table
    // ... other saved state ...
} catcode_save_t;

struct macro_expander {
    // ...
    int cat_code[128];
    catcode_save_t catcode_stack[CATCODE_SAVE_STACK_SIZE];
    int catcode_stack_ptr;
    // ...
};
```

**Success criterion:** `\makeatletter\def\@foo{bar}\makeatother\@foo` expands to `bar`.

### 2.2 Multi-Token Parameter Absorption

**Problem:** When a macro takes arguments `#1..#9`, the current implementation does
not correctly handle multi-token arguments in all cases (e.g., `\def\foo#1{#1}\foo{abc def}`
should produce `abc def`).

**Fix:** Ensure argument absorption follows TeX rules:
- If the parameter is undelimited (just `#1`), absorb either a single token or a
  brace group `{...}`.
- If the parameter is delimited (`#1:`), absorb everything up to the delimiter.
- Handle nested braces correctly within delimited arguments.

### 2.3 Edge Case Tests

Add comprehensive tests for all known edge cases:

```python
# tests/test_macro_expander.py — new test cases
def test_at_in_name(self):           # \def\@foo{bar}
def test_double_hash(self):          # \def\test##1{##1}
def test_makeatletter(self):         # \makeatletter\def\@foo{bar}
def test_nested_braces_body(self):   # \def\foo{{a}{b}}
def test_braces_in_args(self):       # \foo{a{b}c}
def test_recursive_limit(self):      # \def\a{\a}\a should error
def test_empty_body(self):           # \def\foo{}
def test_space_after_macro(self):    # \foo bar (space consumed)
def test_newline_handling(self):     # macros across line boundaries
def test_unicode_in_body(self):      # UTF-8 in macro replacements
def test_multiple_defs(self):        # 1000+ definitions
def test_let_chain(self):            # \let\a\b \let\b\c
def test_expandafter_chain(self):    # multiple \expandafter
```

---

## Phase 3: Feature Completeness (Month 2-3)

### 3.1 `\csname...\endcsname`

Implement dynamic command name construction:
```latex
\csname mycommand\endcsname   % equivalent to \mycommand
\csname @ifundefined\endcsname{foo}{bar}  % internal LaTeX API
```

**Implementation:** When `\csname` is encountered, enter a special scanning mode that
collects tokens (fully expanded) until `\endcsname`, then looks up the resulting string
as a command name and expands it.

### 3.2 TeX Conditionals (`\if`, `\ifx`, `\ifnum`, etc.)

Implement conditional expansion:
```c
// Conditionals follow the same pattern:
// \if<condition> <true text> \else <false text> \fi

// Track conditional nesting with a stack:
int cond_stack[COND_STACK_SIZE];
int cond_ptr;  // -1 = not in conditional

// When \if\ifx\ifnum is encountered:
//   1. Evaluate the condition
//   2. Push result onto cond_stack
//   3. Skip to \else or \fi if condition is false

// When \else is encountered:
//   1. Toggle the current condition state
//   2. Skip to \fi if we just entered the false branch

// When \fi is encountered:
//   1. Pop cond_stack
```

**Key insight:** Conditionals are evaluated during expansion, not during tokenization.
The expander must be able to skip balanced `\if...\fi` groups efficiently.

### 3.3 `\input` and `\include` Support

Add multi-file document support:
1. Maintain a file include stack.
2. When `\input{file}` or `\include{file}` is encountered, open the file and push its
   content onto the input stack.
3. Close the file when exhausted.
4. Support `\endinput` to stop reading the current file early.

**Security:** Implement a `--max-include-depth` flag to prevent recursive `\input` attacks.

### 3.4 `\verb` Scanning

After `\verb`, the next character defines the delimiter. Everything until the next
occurrence of that delimiter is literal text (no macro expansion, no special characters).

```c
// In the token scanner:
if (match("\\verb")) {
    char delim = next_char();
    // Switch to verbatim mode
    while ((c = next_char()) != delim && c != EOF) {
        emit_literal(c);  // all characters are literal
    }
    // Exit verbatim mode
}
```

---

## Phase 4: Library Hardening (Month 3-4)

### 4.1 Thread Safety

The current `macro_expander_t` struct has no thread-local state. To make it thread-safe:
1. Ensure all state is in the struct (already done).
2. Remove all global/static variables (check for any remaining in `main()` function).
3. Document that each thread needs its own `macro_expander_t*`.

### 4.2 Memory Safety

- Audit all `strndup` / `malloc` calls for proper `free`.
- Add bounds checking on all buffers.
- Fuzz test with AFL++ or libFuzzer.

### 4.3 Error Recovery

Current behavior on error: `exit(1)`. Change to:
1. Set an error flag on the `macro_expander_t` struct.
2. Return partial output (everything expanded before the error).
3. Provide `macro_expander_get_error()` to retrieve error details.

### 4.4 Performance Optimization

- Profile the hot path (`get_x_token`, `print_token`).
- Consider replacing the linked-list token pool with a flat array + free list.
- Use `__builtin_expect` for branch prediction hints.
- Add a fast path for input without any macros.

---

## Build System Fixes

### Current broken targets

The Makefile `macro_expander` target references `expand_macros.rl` (does not exist) and
`macro_expander.c` (at root, does not exist).

### Fixed build commands

```makefile
# In scanner/Makefile or a root-level Makefile:
macro_expander:
	$(CC) -O2 -Iinclude include/macro_expander.c include/regex_util.c -o macro_expander.out -lm

# For the library (no main):
macro_expander_lib:
	$(CC) -O2 -c -Iinclude include/macro_expander.c -o macro_expander.o
	$(CC) -O2 -c include/regex_util.c -o regex_util.o
	ar rcs libmacro_expander.a macro_expander.o regex_util.o
```

---

## Success Metrics

| Metric | Current | Target | Phase |
|--------|---------|--------|-------|
| arXiv failure rate | ~8% (80/1019) | <0.1% | 1 |
| `@` in macro names | Fails | Passes | 1 |
| `##` in bodies | Crashes | Correct | 1 |
| Recursive safety | None | Controlled error | 1 |
| Catcode awareness | Fixed table | Dynamic (makeatletter/verb) | 2 |
| Conditional support | None | `\if`/`\ifx`/`\ifnum`/`\fi` | 3 |
| `\csname...\endcsname` | None | Fully supported | 3 |
| `\input`/`\include` | None | Fully supported | 3 |
| Thread safety | Untested | Guaranteed | 4 |
| Memory safety | Not fuzzed | Fuzz-tested 1B+ iterations | 4 |
| Test count | ~20 | 100+ | All |

---

## Dependency Map

```
                    ┌─────────────────┐
                    │  @ in names     │ ← Phase 1.1 (blocker for arXiv corpus)
                    │  (bloker)       │
                    └────────┬────────┘
                             │
                    ┌────────▼────────┐
                    │  ## handling    │ ← Phase 1.2 (common crash)
                    │  (bloker)       │
                    └────────┬────────┘
                             │
                    ┌────────▼────────┐
                    │  Catcode        │ ← Phase 2.1 (prerequisite for makeatletter)
                    │  awareness      │
                    └────────┬────────┘
                             │
              ┌──────────────┼──────────────┐
              │              │              │
     ┌────────▼─────┐ ┌─────▼──────┐ ┌─────▼──────────┐
     │  Conditionals │ │  \csname   │ │  \input/include│
     │  (Phase 3.2)  │ │ (Phase 3.1)│ │ (Phase 3.3)    │
     └────────────────┘ └────────────┘ └────────────────┘
                             │
                    ┌────────▼────────┐
                    │  Library        │
                    │  Hardening      │ ← Phase 4
                    │  (thread/mem/   │
                    │   error recov.) │
                    └─────────────────┘
```

Phase 1 is the critical path. Until it is complete, the macro expander cannot be used
reliably on real-world arXiv corpus data.
