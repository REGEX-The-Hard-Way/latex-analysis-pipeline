-- Cypher REPL Demo History
-- Load with: ./cypher_repl.out --sidecar sidecar.json
-- Type .help for commands. Up/Down arrows browse history. Ctrl-R for fzf.

-- 1. Label lookup (hash index, sub-millisecond)
MATCH (l:label) RETURN l.text LIMIT 5;

-- 2. Count labels
MATCH (l:label) RETURN COUNT(*);

-- 3. Sorted + paginated
MATCH (l:label) RETURN l.text ORDER BY l.text LIMIT 10;

-- 4. Section titles
MATCH (s:section) RETURN s.text;

-- 5. Text search via CONTAINS
MATCH (s:section) WHERE s.text CONTAINS 'Introduction' RETURN s.text;

-- 6. STARTS WITH prefix match
MATCH (m:math) WHERE m.text STARTS WITH '$\\frac' RETURN m.text LIMIT 5;

-- 7. ENDS WITH suffix match
MATCH (m:math) WHERE m.text ENDS WITH '}$' RETURN m.text LIMIT 5;

-- 8. Arithmetic in WHERE
MATCH (t:Token) WHERE t.offset + 100 < t.length RETURN t.text LIMIT 5;

-- 9. IN list operator
MATCH (t:Token) WHERE t.offset IN [0, 1, 20, 150] RETURN t.text LIMIT 5;

-- 10. IS NOT NULL
MATCH (c:cite) WHERE c.text IS NOT NULL RETURN c.text LIMIT 5;

-- 11. AND / OR combinators
MATCH (t:Token) WHERE t.offset > 1000 AND t.length > 50 RETURN t.text LIMIT 5;

-- 12. Edge traversal
MATCH (eq:equation)-[:PARENT_OF]->(sub:math_sub) RETURN sub.text LIMIT 10;

-- 13. Multi-hop traversal
MATCH (eq:equation)-[:PARENT_OF]->(sub:label) RETURN sub.text LIMIT 10;

-- 14. Filepath lookup (interned, zero duplication)
MATCH (l:label) RETURN l.filepath, l.text LIMIT 3;

-- 15. Schema inspection
.schema
.stats
.help

-- 16. CREATE node in graph store
CREATE (n:Test {title:'Hello', year:2025}) RETURN n.title, n.year;

-- 17. SET property on existing node
CREATE (a:Item {name:'Old'}) SET a.name = 'New' MATCH (n:Item) RETURN n.name;

-- 18. SET label on existing node
CREATE (a:Item {name:'Featured'}) SET a:Featured MATCH (n:Featured) RETURN n.name;

-- 19. DETACH DELETE
CREATE (a:Item {name:'Keep'}) CREATE (b:Item {name:'Del'}) CREATE (a)-[:LINK]->(b)
DETACH DELETE b MATCH (n:Item) RETURN n.name;

-- 20. Error recovery (parser skips garbage to next semicolon)
GARBAGE CLAUSE; MATCH (l:label) RETURN l.text LIMIT 2;
