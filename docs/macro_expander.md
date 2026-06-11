# Macro Expander Documentation

## Overview

The `macro_expander` tool expands LaTeX `\newcommand`, `\def`, `\renewcommand`, and `\providecommand` macros using a regex-based two-pass algorithm.

## Usage

### Pipe Mode (Default)
```bash
cat input.tex | ./macro_expander.out > output.tex
```

### Build
```bash
cd scanner
make expand_macros
```

## Supported Macros

### `\newcommand`
```latex
\newcommand{\mycmd}{replacement text}   % braced form
\newcommand\mycmd{replacement text}      % unbraced form
\newcommand{\mycmd}[1]{with #1}          % with argument count
\newcommand\mycmd[1]{with #1}
```

### `\renewcommand`
```latex
\renewcommand{\mycmd}{new replacement}
\renewcommand\mycmd{new replacement}
```

### `\def`
```latex
\def\mycmd{replacement}
\def \mycmd{replacement}
```

### `\providecommand`
```latex
\providecommand{\mycmd}{replacement}
\providecommand\mycmd{replacement}
```

## Implementation

The implementation uses:
- **Pass 1**: Collect macro definitions into internal storage, build clean output string
- **Pass 2**: Expand macros using regex substitution

### Files
- `scanner/macro_expander.c` - Main implementation
- `include/regex_util.c` - Regex utilities including `regex_macro_expand()`
- `include/regex_util.h` - Header file

## Status

| Feature | Status |
|---------|--------|
| `\newcommand\{\name\}{body}` | ✅ |
| `\newcommand\name{body}` | ✅ |
| `\newcommand\name[n]{body}` | ✅ |
| `\renewcommand\name{body}` | ✅ |
| `\def\name{body}` | ✅ |
| `\providecommand\name{body}` | ✅ |
| Macro followed by letter (no expand) | ✅ |
| Nested braces in body | ✅ |