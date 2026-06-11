# Examples

This directory contains example inputs and corresponding outputs from the LaTeX Analysis Pipeline.

## Example: Basic Tokenization

**Input (`sample.tex`):**
```latex
\begin{equation}
  \label{test}
  \frac{x}{2} = \alpha(y)
\end{equation}
```

**Output (`.tok`):**
```
{<filepath:sample.tex,filepath_id:1425541370,token_id:3328404677,
 parent_id:1425541370,offset:1,length:71,type:equation,
 tok:\begin{equation}\n \label{test}\n  \frac{x}{2}=\\alpha(y)\n\end{equation}}

{<filepath:sample.tex,filepath_id:1425541370,token_id:1585187646,
 parent_id:3328404677,offset:18,length:12,type:label,
 tok:\label{test}}

{<filepath:sample.tex,filepath_id:1425541370,token_id:302638469,
 parent_id:3328404677,offset:17,length:11,type:frac,
 tok:\frac{x}{2}}

{<filepath:sample.tex,filepath_id:1425541370,token_id:20558579,
 parent_id:3328404677,offset:52,length:3,type:parens,
 tok:(y)}
```

## Example: Sentence Segmentation

**Input (`.tok` output):**
```
{<filepath:doc.tex,filepath_id:1,...token_id:123,parent_id:1,offset:1,length:10,type:abstract,tok:\begin{abstract}...\end{abstract}}
```

**Output (`clean.sent`):**
```
This is a sentence in the abstract.
Another sentence follows.
```

## Running Examples

```bash
cd scanner
./scanner.out sample.tex tex > output.tok
./sent_split.out < output.tok > output.sent
```