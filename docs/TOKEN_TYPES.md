# Supported Token Types

This document lists all token types supported by the LaTeX Analysis Pipeline.

## Document Structure Tokens

These tokens represent structural elements of LaTeX documents.

| Type | Pattern | Description |
|------|---------|-------------|
| `begin_doc` | `\begin{document}` | Document environment start |
| `end_doc` | `\end{document}` | Document environment end |
| `document` | `\begin{document}...\end{document}` | Full document block |
| `article` | `\begin{article}...\end{article}` | Article environment |
| `book` | `\begin{book}...\end{book}` | Book environment |
| `section` | `\section{...}` | Section heading |
| `subsection` | `\subsection{...}` | Subsection heading |

## Mathematical Tokens

These tokens represent mathematical constructs.

| Type | Pattern | Description |
|------|---------|-------------|
| `equation` | `\begin{equation}...\end{equation}` | Equation environment (starred variants supported) |
| `math` | `$...$` | Inline math |
| `frac` | `\frac{x}{y}` | Fraction command |
| `parens` | `(nested)` | Parenthesized expression |
| `braces` | `{content}` | Brace group |

## Bibliography Tokens

| Type | Pattern | Description |
|------|---------|-------------|
| `cite` | `\cite{key}` | Citation command |
| `ref` | `\ref{label}` | Reference command |
| `label` | `\label{name}` | Label command |
| `bibitem` | `\bibitem{key}` | Bibliography item |

## Environment Blocks

These are block-level environments that can contain nested content.

### Theorem-like
| Type | Environment |
|------|-------------|
| `theorem` | `theorem`, `thm` |
| `lemma` | `lemma`, `lema` |
| `corollary` | `corollary`, `cor` |
| `proposition` | `proposition`, `prop`, `prp` |
| `definition` | `definition`, `defin`, `defi` |
| `example` | `example`, `ex`, `exa`, `exam` |
| `proof` | `proof`, `pf`, `proofof` |

### Proof Structure
| Type | Environment |
|------|-------------|
| `proof` | `proof`, `pf` |
| `observation` | `observation`, `obs` |
| `conjecture` | `conjecture`, `conj` |

### Lists and Tables
| Type | Environment |
|------|-------------|
| `itemize` | `itemize`, `compactitem` |
| `enumerate` | `enumerate` |
| `description` | `description` |
| `table` | `table`, `tablenotes` |
| `tabular` | `tabular`, `tabularx` |
| `figure` | `figure`, `wrapfigure` |

### AMS Math Environments
| Type | Environment |
|------|-------------|
| `align` | `align`, `gather`, `multline` |
| `alignat` | `alignat` |
| `array` | `array`, `matrix`, `pmatrix`, `bmatrix`, `vmatrix`, `Vmatrix` |

### Graphics and TikZ
| Type | Environment |
|------|-------------|
| `tikzpicture` | `tikzpicture`, `tikzcd` |
| `picture` | `picture` |
| `figure` | `figure` |

## Text Blocks

| Type | Pattern | Description |
|------|---------|-------------|
| `abstract` | `\begin{abstract}...\end{abstract}` | Abstract environment |
| `ack` | Various acknowledgment variants | Acknowledgment environments |
| `titlepage` | `titlepage` | Title page environment |
| `verbatim` | `verbatim` | Verbatim environment |

## Code and Listings

| Type | Environment | Description |
|------|-------------|-------------|
| `lstlisting` | `lstlisting` | LaTeX listings environment |
| `code` | `code` | Code environment |

## Configuration

| Type | Pattern | Description |
|------|---------|-------------|
| `usepackage` | `\usepackage{...}` | Package import |
| `author` | `\author{...}` | Author declaration |
| `title` | `\title{...}` | Title declaration |

## Adding New Token Types

To add support for a new LaTeX environment:

1. Add the environment pattern to `latex.rl`:

```ragel
newenv_begin = '\\begin{newenv}' @{n++; };
newenv_end  = '\\end{newenv}' @{n--; };
newenv_body = any+ - (newenv_begin|newenv_end);
newenv = '\\begin{newenv}' @{n=1;}
         (newenv_begin|newenv_end|newenv_body)*
         newenv_end :> any when{!n};
```

2. Add the emission handler in `scanner.rl`:

```ragel
newenv => { EMIT_BLOCK("newenv",1,0); };
```

3. Rebuild the scanner:

```bash
make clean && make scanner
```