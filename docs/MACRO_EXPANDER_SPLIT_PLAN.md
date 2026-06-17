# Modular Ragel-Based Macro Expanders

## Goal

Replace the monolithic `scanner/newcommand.c` (599 lines, all types in one file)
with separate Ragel state machine programs — one per macro definition command.
Each program is a standalone stdin → stdout filter.

## Programs

| Program | Handles | Key syntax distinction |
|---------|---------|----------------------|
| `def_expander.rl` | `\def`, `\gdef` | Inline `#1#2` params between name and body |
| `newcommand_expander.rl` | `\newcommand` | `[n]` arg spec, errors if already defined |
| `renewcommand_expander.rl` | `\renewcommand` | `[n]` arg spec, errors if NOT defined |
| `providecommand_expander.rl` | `\providecommand` | `[n]` arg spec, only defines if absent |
| `edef_expander.rl` | `\edef`, `\xdef` | Body is fully expanded at definition time |

## Pipeline order (TeX-like: primitives first, LaTeX commands later)

```
paper.tex
  → def_expander.out
  → edef_expander.out
  → newcommand_expander.out
  → renewcommand_expander.out
  → providecommand_expander.out
  → expanded.tex
```

A driver script iterates the pipeline until fixpoint (typically 2-3 passes)
to handle chained expansion (e.g., `\newcommand` body contains `\def` macros).

## Architecture per program

Each `.rl` file uses Ragel scanner mode (`|*`, priority-by-position), following
the same style as `scanner/scanner.rl`:

```
%%{ machine def_expander;

letter = [a-zA-Z];
at     = '@';
cmdname = (letter | at)+;

action store_def     { /* store macro in khash */ }
action expand_macro  { /* lookup, read args, expand  */ }
action skip_skip     { /* brace-count to skip other def */ }

main := |*

  # 1. Our definition type
  '\\def' space* cmdname param_spec* braces   => store_def;
  '\\gdef' space* cmdname param_spec* braces  => store_def;

  # 2. Our macros when used
  cmdname                                    => expand_macro;

  # 3. Other definition types — output verbatim (skip)
  '\\newcommand' skip_def                   => output_verbatim;
  '\\renewcommand' skip_def                 => output_verbatim;
  '\\providecommand' skip_def               => output_verbatim;
  '\\edef' skip_def                         => output_verbatim;
  '\\xdef' skip_def                         => output_verbatim;
  '\\let' skip_def                          => output_verbatim;
  '\\long' space* def_rest                  => output_verbatim;
  '\\outer' space* def_rest                 => output_verbatim;

  # 4. Pass-through
  any                                        => { putchar(*ts); };

*|;

write data;
}%%
```

### Braced group matching (used by skip_def)

From `latex.rl` pattern (line 799-802):
```
left_brace  = '{' @{n++; };
right_brace = '}' @{n--; };
brace_body  = any - (left_brace | right_brace);
braces      = '{' @{n=0;}
                (left_brace | right_brace | brace_body)*
                :> '}' when{!n};
```

Max nesting depth: 30. Error if exceeded.

### Skip pattern for each definition type

| Command | Skip shape |
|---------|-----------|
| `\def`, `\gdef`, `\edef`, `\xdef` | `cmdname param_spec* braces` |
| `\newcommand`, `\renewcommand`, `\providecommand` | `(braces \| cmdname) ('[' digit+ ']')? braces` |
| `\long`, `\outer` | prefix, then the rest of the def |
| `\let` | `cmdname space* cmdname` |

### Macro storage (klib khash)

```
#include "khashl.h"

typedef struct {
    char *body;       // body text, \x01..\x09 for #1..#9
    int argc;          // number of parameters
    int body_len;      // byte length of body
} macro_entry;

KHASH_MAP_INIT_STR(macro_table, macro_entry)
khash_t(macro_table) *macros;
```

### Expansion mechanism

When a stored macro `\foo` is encountered:

1. If `argc == 0`: body is output directly
2. If `argc > 0`: read `argc` arguments (each is either `{...}` or a single token)
   - Substitute `\x01` → first arg, etc.
   - Output the expanded body
3. For `\edef`/`\xdef`: body is pre-expanded at store time by scanning for stored
   macro references and expanding them before inserting into the table

### Output buffer

For re-insertion of expanded text, a small static buffer (64KB) suffices since
each program only expands its own macros (no deep recursive expansion within
a single program, except `edef`).

## Build system

In `scanner/Makefile`:

```makefile
CC = gcc
CFLAGS = -g -I../include -Icypher/vendor/klib

# Generic: .rl → .c → .out
def_expander.out: def_expander.rl
	ragel -m -G2 def_expander.rl
	$(CC) $(CFLAGS) -O2 def_expander.c -o def_expander.out

newcommand_expander.out: newcommand_expander.rl
	ragel -m -G2 newcommand_expander.rl
	$(CC) $(CFLAGS) -O2 newcommand_expander.c -o newcommand_expander.out

# ... same for each

# Pipeline helper
pipeline_expand:
	cat $(INPUT) | ./def_expander.out | ./newcommand_expander.out \
	  | ./renewcommand_expander.out | ./providecommand_expander.out \
	  | ./edef_expander.out > $(OUTPUT)

expand_all: def_expander.out edef_expander.out \
            newcommand_expander.out renewcommand_expander.out \
            providecommand_expander.out
```

## Testing

Each program gets its own test class in `tests/`, running the individual binary
via subprocess (same pattern as existing `test_macro_expander.py`):

| Test file | Tests |
|-----------|-------|
| `tests/test_def_expander.py` | `\def` with params, `##`, `\long`, `\outer`, comments |
| `tests/test_edef_expander.py` | `\edef` body-expansion, `\xdef` |
| `tests/test_newcommand_expander.py` | `\newcommand` with/without `[n]`, braces, uniqueness |
| `tests/test_renewcommand_expander.py` | overwrite existing, error on missing |
| `tests/test_providecommand_expander.py` | existence-check |
| `tests/test_pipeline.py` | integration: pipe all stages, verify against existing tests |

Verification: all 100+ tests from `test_macro_expander.py` and
`test_macro_expander_extended.py` must pass when run through the full pipeline.

## Implementation order

| Phase | File | What it handles | Tests to pass |
|-------|------|----------------|---------------|
| 1 | `def_expander.rl` | `\def`/`\gdef` with params, `##` | TestDef (8 cases) |
| 2 | `newcommand_expander.rl` | `\newcommand` with `[n]`, uniqueness | TestNewcommand (4 cases) |
| 3 | `renewcommand_expander.rl` | `\renewcommand`, existence check | Overwrite/cases |
| 4 | `providecommand_expander.rl` | `\providecommand` | Existence cases |
| 5 | `edef_expander.rl` | `\edef`/`\xdef` expansion-at-def | Body-expansion cases |
| 6 | Pipeline driver script | iteration until fixpoint | All 100+ existing tests |
| 7 | 10K corpus validation | Real-world test | Existing corpus |

## Error handling

- Max nesting depth: 30 (error if exceeded)
- Malformed braces in own or other definitions: exit 1 with error message
- `\newcommand` on existing name: error exit 1
- `\renewcommand` on missing name: error exit 1
- Unknown definition type: output verbatim (not an error)
- `\long`/`\outer` prefixes: strip and handle the inner definition

## Relation to existing code

The existing `scanner/newcommand.c` stays in place during development for
regression comparison. Once all programs pass all tests, `newcommand.c`
can be deprecated. The Python `MacroExpander` class and test fixtures
will be updated to use the pipeline instead of the single binary.
