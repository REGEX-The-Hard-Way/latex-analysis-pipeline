# Macro Expander Debugging Notes

## Test Results Summary

| Implementation | Files Tested | Crashes/Failures | Status |
|----------------|--------------|------------------|--------|
| macro_expander.c (original) | 1019 | 189 | Crashes on complex files |
| macro_expander.c (updated) | 1019 | 80 | Still has issues |
| macro_expander.c | 167+ | 100% broken | Missing parameter handling |

## Issues Found in macro_expander.c

### 1. Double Hash (`##`) Not Handled
**Problem**: In LaTeX, `##` in a macro body represents a single `#` character.
The code currently treats each `#` separately.

**Example**:
```
\def\test##1{param: ##1}
```
This should result in a macro named `test` with body `param: #1`.

**Current behavior**: Prints error "expected '{' for macro body (got '#')"

**Root cause**: In `learn_body()` at line 232-234:
```c
if (c == '#') {
    c = get_next_char();           // '1' .. '9'
    *ep++ = c - '1' + _PARAMETER_; // INTERNAL CODE FOR #1, #2, ... #9
}
```

This doesn't handle `##` which should produce a literal `#` followed by the next character.

### 2. Array Declaration Order Problem
**Problem**: Changed array dimensions but used wrong order.

**Original**:
```c
static char macro[MAXMACRONAMELEN][MAXCOMMANDS];
static int body[MAXMACROBODYLEN][MAXCOMMANDS];
```

**My change**:
```c
static char macro[MAXCOMMANDS][MAXMACRONAMELEN];
static int body[MAXCOMMANDS][MAXMACROBODYLEN];
```

**Issue**: The code uses `macro[NEXTFREEMACRO]` and `body[THIS_COMMAND]` where the index 
should be the first dimension. But then `body[THIS_COMMAND][i]` should work...

Actually, looking at usage:
- `strcpy(macro[NEXTFREEMACRO], name);` - correct with my change
- `intcpy(body[NEXTFREEMACRO], expansion);` - expects `expansion` to be an int array

Wait, there's an inconsistency! The original code has:
- `macro` as `char[MAXMACRONAMELEN][MAXCOMMANDS]` - indexed by [name_len][macro_id]
- `body` as `int[MAXMACROBODYLEN][MAXCOMMANDS]` - indexed by [body_pos][macro_id]

This is backwards for how it's used! The code uses `macro[NEXTFREEMACRO]` as if 
`NEXTFREEMACRO` is the first index.

Let me check the original working version again...

### 3. Memory Layout Issue
The original declarations:
```c
static char macro[MAXMACRONAMELEN][MAXCOMMANDS];  // [name_len][cmd_id]
static int body[MAXMACROBODYLEN][MAXCOMMANDS];    // [body_pos][cmd_id]
```

But the code uses:
```c
strcpy(macro[NEXTFREEMACRO], name);  // treats NEXTFREEMACRO as first index
intcpy(body[NEXTFREEMACRO], expansion);  // treats NEXTFREEMACRO as first index
```

This means the original code has a bug! `macro[NEXTFREEMACRO]` where NEXTFREEMACRO 
can be up to MAXCOMMANDS (1024), but macro only has MAXMACRONAMELEN (80) as first dimension.

This is causing out-of-bounds access and should be:
```c
static char macro[MAXCOMMANDS][MAXMACRONAMELEN];
static int body[MAXCOMMANDS][MAXMACROBODYLEN];
```

And the intcpy function needs to be fixed too since body is now [cmd][pos].

## Fixes Needed

1. Fix array declarations to use correct indexing
2. Handle `##` in macro body (convert to literal `#` and don't treat as parameter)
3. Fix intcpy to work with new body array layout

## Progress

- [x] Increase buffer sizes
- [ ] Fix array dimension ordering
- [ ] Handle double hash `##`
- [ ] Fix intcpy function
## Remaining Issues

1. **Macros with `@` in name**: Files like `0301022.tex` have `\def\@foo{...}` which isn't supported
   - LaTeX allows `@` in macro names for "internal" macros
   - Need to modify `learn_macro()` to accept `@` in names

2. **Complex macro definitions**: Some files have deeply nested or malformed macro definitions

## Progress

- [x] Increase buffer sizes
- [x] Fix array dimension ordering  
- [x] Handle double hash `##`
- [x] Handle inline parameters `#1#2#3`
- [ ] Handle `@` in macro names
- [ ] Handle more edge cases

## Recommendations

1. The updated `macro_expander.c` is the best option currently available
2. For production use, consider:
   - Adding support for `@` in macro names
   - Better error handling (skip problematic macros instead of failing)
   - Dynamic buffer allocation for truly large files
