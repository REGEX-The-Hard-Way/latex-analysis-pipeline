# Examples

## Tokenization

**Input (`sound1.tex`):**
```latex
\begin{equation}
  \label{test}
  \frac{x}{2} = \alpha(y)
\end{equation}
```

**Run:**
```bash
cd scanner
./scanner.out sound1.tex tex > output.tok
```

**Output (sidecar.tok):**
```
{filepath:sound1.tex,filepath_id:1425541370,token_id:3328404677,
 parent_id:1425541370,offset:1,length:71,type:equation,
 tok:\begin{equation}\n \label{test}\n  \frac{x}{2}=\\alpha(y)\n\end{equation}}

{filepath:sound1.tex,filepath_id:1425541370,token_id:1585187646,
 parent_id:3328404677,offset:18,length:12,type:label,
 tok:\label{test}}
```

## Sentence Segmentation

```bash
cd scanner
./sent_split.out < output.tok > clean.sent
```

## Cypher Queries

```bash
cd scanner/cypher
make
./cypher_repl.out --sidecar ../sidecar.json
```

```cypher
-- Count tokens by type
MATCH (n:equation) RETURN COUNT(*);

-- Find subscripts inside equations
MATCH (eq:equation)-[:PARENT_OF]->(sub:math_sub) RETURN sub.text LIMIT 10;

-- Aggregation
MATCH (n:Token) RETURN COUNT(*), SUM(n.length);
```
