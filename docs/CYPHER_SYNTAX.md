# Cypher Query Syntax Reference

## Supported Clauses

### CREATE
```
CREATE (var:Label {key: value, ...})
CREATE (var:Label1:Label2 {key: 'string', num: 42, flt: 3.14})
CREATE (src)-[:REL_TYPE]->(dst)
CREATE (src)<-[:REL_TYPE]-(dst)
```

- Variables (`var`) are scoped to the query. Reusing a variable refers to the same node.
- Labels are case-sensitive alphanumeric.
- Property keys are case-sensitive identifiers.
- Values: strings (single or double quotes), integers, floats.
- Relation types are optional; defaults to `REL`.

### MATCH
```
MATCH (var:Label)
MATCH (var:Label:Label2)              -- AND semantics for multiple labels
MATCH (var:Label {key: value})        -- inline property filter
MATCH (src:Label)-[:TYPE]->(tgt:Label)
MATCH (var)                           -- all nodes (full scan)
MATCH OPTIONAL (var:Label)            -- parsed, not executed
```

### WHERE
```
MATCH (n:Paper) WHERE n.year > 2020
MATCH (n:Paper) WHERE n.author = 'Smith'
MATCH (n:Paper) WHERE n.year >= 2020 AND n.year <= 2024
MATCH (n:Paper) WHERE n.author = 'Smith' OR n.title = 'Quantum'
```

Supported operators: `=`, `<>`, `<`, `>`, `<=`, `>=`, `AND`, `OR`, `NOT`.

### RETURN
```
RETURN var.key
RETURN var.key1, var.key2
RETURN var.key AS alias
RETURN DISTINCT var.key
RETURN *
```

### ORDER BY
```
RETURN n.title ORDER BY n.year
RETURN n.title ORDER BY n.year DESC
RETURN n.title ORDER BY n.year ASC, n.author DESC
```

### LIMIT / SKIP
```
RETURN n.title LIMIT 10
RETURN n.title SKIP 5 LIMIT 10
RETURN n.title ORDER BY n.year LIMIT 5
```

### SET
```
MATCH (n:Paper {title:'Old'})
SET n.title = 'New'
SET n.year = 2025, n.author = 'Jones'
```

Note: SET appends new property values. Due to the columnar property model,
the first value written for a key takes precedence on read.

### DELETE / DETACH DELETE
```
MATCH (n:Paper {title:'Obsolete'})
DELETE n

MATCH (n:Paper)-[r:CITES]->(m)
DETACH DELETE n
```

- `DELETE`: removes labels and properties from the node.
- `DETACH DELETE`: also removes incident edges.

### WITH
```
MATCH (n:Paper) WHERE n.year > 2020
WITH n.title AS t, n.year AS y
RETURN t, y ORDER BY y
```

Parsed, basic pipelining support. Pipes intermediate results to subsequent clauses.

## REPL Usage

### Modes
```
./cypher_repl.out                              interactive REPL (readline)
./cypher_repl.out --sidecar sidecar.json       load sidecar, then interactive
./cypher_repl.out --scan sidecar.json          mmap direct query, no import
./cypher_repl.out -i --sidecar sidecar.json    force interactive (even piped)
echo "MATCH ... RETURN ..." | ./cypher_repl.out --scan sidecar.json   pipe mode
./cypher_repl.out file.cypher                   batch mode (execute file)
```

### Readline Navigation
| Key | Action |
|-----|--------|
| Up / Down | Navigate command history |
| Left / Right | Move cursor within line |
| Ctrl-A / Ctrl-E | Jump to start/end of line |
| Ctrl-W | Delete word backward |
| Ctrl-U | Delete to start of line |
| Ctrl-R | Open fzf history browser (same as `.hist`) |
| Tab | (future: autocomplete) |

History auto-loaded from `~/.cypher_history` on startup, saved on exit (1000-line cap).

### Dot Commands
```
.help, .h           show available commands and syntax
.history, .hist     browse command history with fzf
.schema             show all labels with node counts
.stats              show node/edge/property counts
.exit, .quit        exit the REPL
```

### History
Queries are saved to `~/.cypher_history`. Use `.history` to browse with fzf:
- Fuzzy-search past queries
- Preview pane shows full query text
- Enter re-executes the selected query

## Query Examples

```cypher
-- count labels
MATCH (l:label) RETURN l.text;

-- find sections mentioning "Introduction"  
MATCH (s:section) WHERE s.text = '\section{Introduction}' RETURN s.text;

-- cite references from a specific paper
MATCH (c:cite) WHERE c.text LIKE '%ashcroft%' RETURN c.text;

-- sub-expressions within equation environments
MATCH (eq:equation)-[:PARENT_OF]->(sub:math_sub) RETURN sub.text;

-- most common math expressions (sorted)
MATCH (m:math) RETURN m.text ORDER BY m.text LIMIT 10;

-- find labels and their source files
MATCH (l:label) RETURN l.filepath, l.text;
```

## Limitations

- **No subqueries or EXISTS** — parsed but not executed
- **No aggregation (COUNT, SUM, AVG)** — return raw rows
- **No UNION** — one MATCH/RETURN per query
- **No MERGE** — parsed but not executed  
- **Property overwrite** — SET appends; first match wins on read
- **Full scans are slow** — always use label filters with `MATCH (n:Label)`
- **Hardcoded 200-row result limit** — applies to all queries
- **Bubble sort** — ORDER BY is O(n²), acceptable up to ~500 rows
