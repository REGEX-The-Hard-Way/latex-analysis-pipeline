# Cypher Query Syntax Reference

## Supported Clauses

### CREATE
```
CREATE (var:Label {key: value, ...})
CREATE (var:Label1:Label2 {key: 'string', num: 42, flt: 3.14, neg: -5})
CREATE (src)-[:REL_TYPE]->(dst)
CREATE (src)-[:REL_TYPE]->(dst {key: val})    -- target node with properties
CREATE (src)<-[:REL_TYPE]-(dst)
```

- Variables (`var`) are scoped to the query. Reusing a variable refers to the same node.
- Labels are case-sensitive alphanumeric. Multiple labels use AND semantics.
- Property keys are case-sensitive identifiers.
- Values: strings (single or double quotes), integers, floats, negative numbers, booleans, NULL.
- Relation types are optional; defaults to `REL`.
- Target node in compound CREATE gets labels and properties set correctly.

### MATCH
```
MATCH (var:Label)
MATCH (var:Label:Label2)              -- AND semantics for multiple labels
MATCH (var:Label {key: value})        -- inline property filter
MATCH (src:Label)-[:TYPE]->(tgt:Label)
MATCH (src:Label)<-[:TYPE]-(tgt:Label)
MATCH (var)                           -- all nodes (full scan)
MATCH (a:Label), (b:Label2)           -- comma-separated patterns (cross-product)
MATCH OPTIONAL (var:Label)            -- parsed, not executed as outer-join
MATCH (a)-[:REL*1..3]->(b)            -- variable-length paths (BFS)
MATCH (a)-[:REL*]->(b)                -- any-length paths (unbounded BFS)
MATCH (a)-[:REL*2]->(b)               -- exact 2-hop paths
MATCH (a)-[:T1|T2]->(b)               -- type union (first type matched in executor)
MATCH (a)-[:R]->(b)-[:S]->(c)         -- multi-hop up to 8 hops
```

### WHERE
```
MATCH (n:Paper) WHERE n.year > 2020
MATCH (n:Paper) WHERE n.author = 'Smith'
MATCH (n:Paper) WHERE n.year >= 2020 AND n.year <= 2024
MATCH (n:Paper) WHERE n.author = 'Smith' OR n.title = 'Quantum'
MATCH (n:Paper) WHERE n.author = 'Smith' XOR n.year > 2023
MATCH (n:Paper) WHERE NOT n.status = 'draft'
MATCH (n:Paper) WHERE n.name CONTAINS 'Quantum'
MATCH (n:Paper) WHERE n.name STARTS WITH 'Intro'
MATCH (n:Paper) WHERE n.name ENDS WITH 'duction'
MATCH (n:Paper) WHERE n.year IN [2020, 2021, 2022]
MATCH (n:Paper) WHERE n.key IS NULL
MATCH (n:Paper) WHERE n.key IS NOT NULL
MATCH (n:Item) WHERE n.val + 5 > 10        -- arithmetic in WHERE
MATCH (n:Item) WHERE n.val ^ 2 = 9         -- power operator
MATCH (n:Item) WHERE -n.val < -5           -- unary minus
MATCH (n:Item) WHERE (n.val + 5) * 2 > 20  -- parenthesized expressions
MATCH (n:Item) WHERE true                  -- boolean literal
MATCH (n:Item) WHERE CASE WHEN n.val > 5 THEN true ELSE false END  -- CASE in WHERE
```

Supported operators: `=`, `<>`, `<`, `>`, `<=`, `>=`, `AND`, `OR`, `XOR`, `NOT`, `+`, `-`, `*`, `/`, `%`, `^`, `IS NULL`, `IS NOT NULL`, `IN`, `CONTAINS`, `STARTS WITH`, `ENDS WITH`.

### RETURN
```
RETURN var.key
RETURN var.key1, var.key2
RETURN var.key AS alias
RETURN DISTINCT var.key
RETURN *
RETURN n.val + 5                       -- arithmetic expressions
RETURN n.val ^ 2                       -- power
RETURN -n.val                          -- unary minus
RETURN (n.val + 5) * 2                 -- parenthesized
RETURN CASE WHEN n.val > 5 THEN 1 ELSE 0 END   -- CASE expression
RETURN COUNT(*), SUM(n.val), AVG(n.val), MIN(n.val), MAX(n.val)  -- aggregation
```

### ORDER BY
```
RETURN n.title ORDER BY n.year
RETURN n.title ORDER BY n.year DESC
RETURN n.title ORDER BY n.year ASC, n.author DESC
RETURN n.title ORDER BY n.year SKIP 5 LIMIT 10
```

Uses qsort (O(n log n)) with multi-key support.

### LIMIT / SKIP
```
RETURN n.title LIMIT 10
RETURN n.title SKIP 5 LIMIT 10
RETURN n.title ORDER BY n.year LIMIT 5
```

Default row limit: 200 (configurable via MAX_ROWS in cypher_parser.h).

### SET
```
MATCH (n:Paper {title:'Old'})
SET n.title = 'New'
SET n.year = 2025, n.author = 'Jones'
SET n:NewLabel                         -- add label to existing node
SET n.val = -10                        -- negative values
```

SET overwrites existing properties (last-set wins via backwards property scan).

### DELETE / DETACH DELETE
```
MATCH (n:Paper {title:'Obsolete'})
DELETE n

MATCH (n:Paper)-[r:CITES]->(m)
DETACH DELETE n
```

- `DELETE`: removes labels and properties from the node.
- `DETACH DELETE`: also removes incident edges.

### MERGE
```
MERGE (n:Person {name:'Alice'}) RETURN n.name;
MERGE (n:Person {name:'Alice'})-[:KNOWS]-(m:Person {name:'Bob'}) RETURN n.name, m.name;
MERGE (n:Item {id:123, price:9.99})    -- float properties
MERGE (n:Person:Athlete {name:'Dave'}) -- multi-label
```

Match-or-create semantics. Idempotent — running the same MERGE twice creates one node.

### REMOVE
```
MATCH (n:Item {name:'X'})
REMOVE n.tag
REMOVE n.a, n.b                        -- multiple properties
```

Removes properties from nodes (sets to empty).

### UNWIND
```
UNWIND [1, 2, 3] AS x RETURN x;
UNWIND ['a', 'b', 'c'] AS name RETURN name;
UNWIND [1, 2, 3, 4, 5] AS x RETURN x SKIP 2 LIMIT 2;
```

### WITH
```
MATCH (n:Paper) WHERE n.year > 2020
WITH n.title AS t, n.year AS y
RETURN t, y ORDER BY y
```

Parsed with basic column alias forwarding. Full result pipelining pending.

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

### Dot Commands
```
.help, .h           show available commands and syntax
.history, .hist     browse command history with fzf
.schema             show all labels with node counts
.stats              show node/edge/property counts
.exit, .quit        exit the REPL
```

### Multi-line Queries
```
cypher> CREATE (n:Item {name:'Multi'})
cypher> MATCH (n:Item)
cypher> WHERE n.name = 'Multi'
cypher> RETURN n.name;
```
Newlines are consumed as whitespace; semicolons delimit queries.

## Limitations

- **No UNION** — one MATCH/RETURN per query
- **No MERGE ON MATCH/CREATE SET** — basic MERGE only
- **OPTIONAL MATCH** — parsed but treated as required
- **No subqueries or EXISTS** — not supported
- **No ALL/ANY/NONE/SINGLE predicates** — not implemented
- **No list comprehension** — not implemented
- **No CALL/YIELD procedures** — not supported
- **No parameters ($param)** — not implemented
- **Type union** — executor matches first type only
- **COUNT is a keyword token** — SUM/AVG/MIN/MAX are matched as regular identifiers
- **Hardcoded 200-row result limit** — configurable via MAX_ROWS
- **Full scans are slow** — always use label filters with `MATCH (n:Label)`

## Query Examples

```cypher
-- label lookup
MATCH (l:label) RETURN l.text LIMIT 5;

-- text search
MATCH (s:section) WHERE s.text CONTAINS 'Introduction' RETURN s.text;

-- arithmetic filter
MATCH (n:Item) WHERE n.val + 5 > 10 RETURN n.val;

-- list membership
MATCH (n:Item) WHERE n.name IN ['A', 'C'] RETURN n.name;

-- multi-hop traversal
MATCH (x:Item)-[:LINK]->(y:Item)-[:LINK]->(z:Item) RETURN x.name, z.name;

-- variable-length paths
MATCH (s:V)-[:X*2]->(t:V) RETURN t.name;

-- aggregation
MATCH (n:Sale) RETURN COUNT(*), SUM(n.amount), AVG(n.amount);

-- CASE in RETURN
MATCH (n:Item) RETURN CASE WHEN n.val > 5 THEN 'high' ELSE 'low' END;

-- SET with overwrite
CREATE (a:Item {n:'A'}) SET a.n = 'B' MATCH (n:Item) RETURN n.n;

-- sort with pagination
MATCH (l:label) RETURN l.text ORDER BY l.text LIMIT 10;

-- XOR logic
MATCH (n:Item) WHERE n.val > 5 XOR n.val < 3 RETURN n.val;
```
