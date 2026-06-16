#!/bin/bash
REPL="./cypher_repl.out"
PASS=0; FAIL=0; TN=0
run() {
    local desc="$1" query="$2" expected="$3"
    TN=$((TN+1))
    local out
    out=$(echo "$query" | "$REPL" 2>/dev/null) || true
    if echo "$out" | grep -qF -- "$expected"; then
        echo "PASS #$TN $desc"; PASS=$((PASS+1))
    else
        echo "FAIL #$TN $desc"; echo "  expected: $expected"; echo "  got: $(echo "$out" | head -3)"; FAIL=$((FAIL+1))
    fi
}

echo "=== Cypher 200-Test Suite ==="

# === GROUP 1: CREATE + RETURN (Tests 1-12) ===
echo "--- CREATE + RETURN ---"
run "CREATE node with string" "CREATE (n:Paper {title:'Test'}) RETURN n.title;" "Test"
run "CREATE node with int" "CREATE (n:Paper {pages:42}) RETURN n.pages;" "42"
run "CREATE node with float" "CREATE (n:Item {price:9.99}) RETURN n.price;" "9.99"
run "CREATE two labels" "CREATE (n:Paper:Featured {title:'F'}) MATCH (x:Featured) RETURN x.title;" "F"
run "CREATE multi-property" "CREATE (n:Book {title:'T', year:2024, pages:300}) RETURN n.title, n.year;" "T"
run "CREATE with RETURN *" "CREATE (n:Item {name:'X'}) MATCH (n:Item) RETURN *;" "1 row"
run "Multiple CREATE nodes" "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) MATCH (n:Item) RETURN n.name;" "A"
run "CREATE then SET property" "CREATE (a:Item {name:'Old'}) SET a.name = 'New' MATCH (n:Item) RETURN n.name;" "New"
run "CREATE with hex integer" "CREATE (n:Item {val:255}) MATCH (n:Item) WHERE n.val = 255 RETURN n.val;" "255"
run "CREATE with zero value" "CREATE (n:Item {val:0}) MATCH (n:Item) WHERE n.val = 0 RETURN n.val;" "0"
run "CREATE with negative value" "CREATE (n:Item {val:-5}) MATCH (n:Item) WHERE n.val = -5 RETURN n.val;" "-5"
run "CREATE with empty string" "CREATE (n:Item {name:''}) MATCH (n:Item) RETURN n.name;" "0 row"

# === GROUP 2: MATCH + WHERE comparisons (Tests 13-30) ===
echo "--- MATCH + WHERE ---"
run "WHERE = string" "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) MATCH (n:Item) WHERE n.name = 'B' RETURN n.name;" "B"
run "WHERE = int" "CREATE (a:Item {val:10}) CREATE (b:Item {val:20}) MATCH (n:Item) WHERE n.val = 20 RETURN n.val;" "20"
run "WHERE > int" "CREATE (a:Item {val:10}) CREATE (b:Item {val:20}) MATCH (n:Item) WHERE n.val > 10 RETURN n.val;" "20"
run "WHERE < int" "CREATE (a:Item {val:10}) CREATE (b:Item {val:20}) MATCH (n:Item) WHERE n.val < 15 RETURN n.val;" "10"
run "WHERE >= int" "CREATE (a:Item {val:10}) CREATE (b:Item {val:20}) MATCH (n:Item) WHERE n.val >= 20 RETURN n.val;" "20"
run "WHERE <= int" "CREATE (a:Item {val:10}) CREATE (b:Item {val:20}) MATCH (n:Item) WHERE n.val <= 10 RETURN n.val;" "10"
run "WHERE <> int" "CREATE (a:Item {val:10}) CREATE (b:Item {val:20}) MATCH (n:Item) WHERE n.val <> 10 RETURN n.val;" "20"
run "WHERE <> string" "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) MATCH (n:Item) WHERE n.name <> 'A' RETURN n.name;" "B"
run "WHERE AND" "CREATE (a:Item {name:'A',val:10}) CREATE (b:Item {name:'B',val:20}) MATCH (n:Item) WHERE n.val > 5 AND n.name = 'B' RETURN n.name;" "B"
run "WHERE OR" "CREATE (a:Item {name:'A',val:1}) CREATE (b:Item {name:'B',val:2}) MATCH (n:Item) WHERE n.name = 'A' OR n.name = 'B' RETURN n.name;" "A"
run "WHERE NOT" "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) MATCH (n:Item) WHERE NOT n.name = 'A' RETURN n.name;" "B"
run "WHERE with arithmetic +" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE n.val + 5 > 12 RETURN n.val;" "10"
run "WHERE with arithmetic -" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE n.val - 3 = 7 RETURN n.val;" "10"
run "WHERE with arithmetic *" "CREATE (a:Item {val:3}) MATCH (n:Item) WHERE n.val * 2 = 6 RETURN n.val;" "3"
run "WHERE with arithmetic /" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE n.val / 2 = 5 RETURN n.val;" "10"
run "WHERE with modulo %" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE n.val % 3 = 1 RETURN n.val;" "10"
run "WHERE with power ^" "CREATE (a:Item {val:3}) MATCH (n:Item) WHERE n.val ^ 2 = 9 RETURN n.val;" "3"
run "WHERE with unary minus" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE -n.val < -5 RETURN n.val;" "10"

# === GROUP 3: WHERE special operators (Tests 31-42) ===
echo "--- WHERE special ---"
run "CONTAINS" "CREATE (a:Item {name:'Hello World'}) MATCH (n:Item) WHERE n.name CONTAINS 'World' RETURN n.name;" "Hello World"
run "STARTS WITH" "CREATE (a:Item {name:'Hello World'}) MATCH (n:Item) WHERE n.name STARTS WITH 'Hello' RETURN n.name;" "Hello World"
run "ENDS WITH" "CREATE (a:Item {name:'Hello World'}) MATCH (n:Item) WHERE n.name ENDS WITH 'World' RETURN n.name;" "Hello World"
run "IN list string" "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) CREATE (c:Item {name:'C'}) MATCH (n:Item) WHERE n.name IN ['A','C'] RETURN n.name;" "A"
run "IN list numeric" "CREATE (a:Item {val:10}) CREATE (b:Item {val:20}) MATCH (n:Item) WHERE n.val IN [10, 30] RETURN n.val;" "10"
run "IS NULL" "CREATE (a:Item {name:'A'}) CREATE (b:Item {}) MATCH (n:Item) WHERE n.name IS NULL RETURN n.name;" "0 row"
run "IS NOT NULL" "CREATE (a:Item {name:'A'}) CREATE (b:Item {}) MATCH (n:Item) WHERE n.name IS NOT NULL RETURN n.name;" "A"
run "WHERE XOR true" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE n.val > 3 XOR n.val > 20 RETURN n.val;" "10"
run "WHERE XOR false" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE n.val = 10 XOR n.val = 10 RETURN n.val;" "0 row"
run "WHERE boolean true" "CREATE (a:Item {name:'A'}) MATCH (n:Item) WHERE true RETURN n.name;" "A"
run "WHERE boolean false" "CREATE (a:Item {name:'A'}) MATCH (n:Item) WHERE false RETURN n.name;" "0 row"
run "WHERE parenthesized" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE (n.val + 5) > 12 RETURN n.val;" "10"

# === GROUP 4: RETURN variants (Tests 43-54) ===
echo "--- RETURN ---"
run "RETURN *" "CREATE (n:Item {name:'X'}) MATCH (n:Item) RETURN *;" "1 row"
run "RETURN AS alias" "CREATE (n:Item {name:'X'}) MATCH (n:Item) RETURN n.name AS label;" "label"
run "RETURN multiple columns" "CREATE (n:Item {name:'X',val:1}) MATCH (n:Item) RETURN n.name, n.val;" "X"
run "RETURN DISTINCT" "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'A'}) MATCH (n:Item) RETURN DISTINCT n.name;" "1 row"
run "RETURN arithmetic expr" "CREATE (n:Item {val:10}) MATCH (n:Item) RETURN n.val + 5;" "15"
run "RETURN negative expr" "CREATE (n:Item {val:5}) MATCH (n:Item) RETURN -n.val;" "-5"
run "RETURN star expr" "CREATE (n:Item {val:3}) MATCH (n:Item) RETURN n.val * 4;" "12"
run "RETURN power expr" "CREATE (n:Item {val:2}) MATCH (n:Item) RETURN n.val ^ 3;" "8"

# === GROUP 5: ORDER BY (Tests 55-65) ===
echo "--- ORDER BY ---"
run "ORDER BY ASC numeric" "CREATE (a:Item {val:3}) CREATE (b:Item {val:1}) CREATE (c:Item {val:2}) MATCH (n:Item) RETURN n.val ORDER BY n.val;" "1"
run "ORDER BY DESC numeric" "CREATE (a:Item {val:1}) CREATE (b:Item {val:3}) MATCH (n:Item) RETURN n.val ORDER BY n.val DESC;" "3"
run "ORDER BY ASC string" "CREATE (a:Item {name:'B'}) CREATE (b:Item {name:'A'}) MATCH (n:Item) RETURN n.name ORDER BY n.name;" "A"
run "ORDER BY DESC string" "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) MATCH (n:Item) RETURN n.name ORDER BY n.name DESC;" "B"
run "ORDER BY + LIMIT" "CREATE (a:Item {val:1}) CREATE (b:Item {val:3}) CREATE (c:Item {val:2}) MATCH (n:Item) RETURN n.val ORDER BY n.val LIMIT 2;" "2"
run "ORDER BY + SKIP" "CREATE (a:Item {val:1}) CREATE (b:Item {val:2}) CREATE (c:Item {val:3}) MATCH (n:Item) RETURN n.val ORDER BY n.val SKIP 1;" "2"
run "ORDER BY DESC + LIMIT" "CREATE (a:Item {val:1}) CREATE (b:Item {val:3}) MATCH (n:Item) RETURN n.val ORDER BY n.val DESC LIMIT 2;" "3"
run "ORDER BY + SKIP + LIMIT" "CREATE (a:Item {val:1}) CREATE (b:Item {val:2}) CREATE (c:Item {val:3}) MATCH (n:Item) RETURN n.val ORDER BY n.val SKIP 1 LIMIT 1;" "2"
run "ORDER BY multi-key" "CREATE (a:Item {name:'A',val:3}) CREATE (b:Item {name:'A',val:1}) MATCH (n:Item) RETURN n.name, n.val ORDER BY n.name, n.val;" "1"

# === GROUP 6: LIMIT/SKIP (Tests 66-71) ===
echo "--- LIMIT SKIP ---"
run "LIMIT 5" "CREATE (a:Item {}) CREATE (b:Item {}) CREATE (c:Item {}) MATCH (n:Item) RETURN n LIMIT 1;" "1 row"
run "LIMIT 0" "CREATE (a:Item {name:'X'}) MATCH (n:Item) RETURN n.name LIMIT 0;" "0 row"
run "SKIP past all" "CREATE (a:Item {name:'A'}) MATCH (n:Item) RETURN n.name SKIP 10;" "0 row"
run "SKIP + LIMIT" "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) CREATE (c:Item {name:'C'}) MATCH (n:Item) RETURN n.name SKIP 1 LIMIT 1;" "B"

# === GROUP 7: Edge traversal (Tests 72-82) ===
echo "--- Edges ---"
run "CREATE edge" "CREATE (a:Item {name:'S'})-[:LINK]->(b:Item {name:'T'}) MATCH (src:Item)-[:LINK]->(tgt:Item) RETURN src.name;" "S"
run "Edge with WHERE source" "CREATE (a:Item {name:'S'})-[:LINK]->(b:Item {name:'T'}) MATCH (src:Item)-[:LINK]->(tgt:Item) WHERE src.name = 'S' RETURN tgt.name;" "T"
run "Edge with WHERE target" "CREATE (a:Item {name:'S'})-[:LINK]->(b:Item {name:'T'}) MATCH (src:Item)-[:LINK]->(tgt:Item) WHERE tgt.name = 'T' RETURN src.name;" "S"
run "Left-directed edge <-" "CREATE (a:Item {name:'S'}) CREATE (b:Item {name:'T'}) CREATE (a)-[:LINK]->(b) MATCH (src:Item)<-[:LINK]-(tgt:Item) RETURN src.name, tgt.name;" "T"
run "Edge any type --" "CREATE (a:Item {name:'S'}) CREATE (b:Item {name:'T'}) CREATE (a)-[:LINK]->(b) MATCH (src:Item)--(tgt:Item) RETURN src.name;" "S"
run "Multi-hop (2 edges)" "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) CREATE (c:Item {name:'C'}) CREATE (a)-[:LINK]->(b) CREATE (b)-[:LINK]->(c) MATCH (x:Item)-[:LINK]->(y:Item)-[:LINK]->(z:Item) WHERE x.name = 'A' RETURN z.name;" "C"
run "Multi-hop (3 edges)" "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) CREATE (c:Item {name:'C'}) CREATE (d:Item {name:'D'}) CREATE (a)-[:LINK]->(b) CREATE (b)-[:LINK]->(c) CREATE (c)-[:LINK]->(d) MATCH (w:Item)-[:LINK]->(x:Item)-[:LINK]->(y:Item)-[:LINK]->(z:Item) WHERE w.name = 'A' RETURN z.name;" "D"
run "No matching edge" "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) MATCH (src:Item)-[:LINK]->(tgt:Item) RETURN src.name;" "0 row"
run "Edge with property on target" "CREATE (a:Item {name:'S'})-[:LINK]->(b:Item {name:'T',val:42}) MATCH (src:Item)-[:LINK]->(tgt:Item) WHERE tgt.val = 42 RETURN src.name;" "S"

# === GROUP 8: Aggregation (Tests 83-92) ===
echo "--- Aggregation ---"
run "COUNT(*)" "CREATE (a:Item {}) CREATE (b:Item {}) CREATE (c:Item {}) MATCH (n:Item) RETURN COUNT(*);" "3"
run "COUNT(*) with WHERE" "CREATE (a:Item {val:10}) CREATE (b:Item {val:20}) MATCH (n:Item) WHERE n.val > 10 RETURN COUNT(*);" "1"
run "COUNT(*) with label" "CREATE (a:Item:Active {}) CREATE (b:Item {}) MATCH (n:Active) RETURN COUNT(*);" "1"
run "COUNT(*) empty" "MATCH (n:NoSuchLabel) RETURN COUNT(*);" "0"
run "COUNT(*) after CREATEs" "CREATE (a:Item {}) CREATE (b:Item {}) MATCH (n:Item) RETURN COUNT(*);" "2"

# === GROUP 9: MERGE (Tests 93-103) ===
echo "--- MERGE ---"
run "MERGE create new" "MERGE (n:Person {name:'Alice'}) RETURN n.name;" "Alice"
run "MERGE create with int" "MERGE (n:Item {id:123}) RETURN n.id;" "123"
run "MERGE idempotent" "MERGE (n:Test {id:1}) MERGE (n:Test {id:1}) RETURN count(n);" "1"
run "MERGE two nodes" "MERGE (a:Test {id:1}) MERGE (b:Test {id:2}) RETURN count(*);" "2"
run "MERGE with edge" "MERGE (a:Person {name:'A'})-[:KNOWS]-(b:Person {name:'B'}) RETURN a.name, b.name;" "A"
run "MERGE with multi-label" "MERGE (n:Person:Athlete {name:'Dave'}) RETURN n.name;" "Dave"
run "MERGE with float" "MERGE (p:Product {price:9.99}) RETURN p.price;" "9.99"
run "MERGE existing via MATCH" "CREATE (n:Item {name:'Bob'}) MERGE (n:Item {name:'Bob'}) RETURN n.name;" "Bob"

# === GROUP 10: DELETE (Tests 104-111) ===
echo "--- DELETE ---"
run "DELETE node" "CREATE (a:Item {name:'Del'}) DELETE a MATCH (n:Item) RETURN n.name;" "0 row"
run "DETACH DELETE" "CREATE (a:Item {name:'Keep'}) CREATE (b:Item {name:'Del'}) DETACH DELETE b MATCH (n:Item) RETURN n.name;" "Keep"
run "DETACH DELETE with edges" "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) CREATE (a)-[:LINK]->(b) DETACH DELETE a MATCH (n:Item) RETURN n.name;" "B"
run "DELETE multiple" "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) DELETE a, b MATCH (n:Item) RETURN n.name;" "0 row"

# === GROUP 11: SET (Tests 112-118) ===
echo "--- SET ---"
run "SET property" "CREATE (a:Item {name:'Old'}) SET a.name = 'New' MATCH (n:Item) RETURN n.name;" "New"
run "SET numeric" "CREATE (a:Item {val:1}) SET a.val = 42 MATCH (n:Item) RETURN n.val;" "42"
run "SET label" "CREATE (a:Item {name:'A'}) SET a:Featured MATCH (n:Featured) RETURN n.name;" "A"
run "SET multiple props" "CREATE (a:Item {name:'A',val:1}) SET a.name = 'B', a.val = 2 MATCH (n:Item) RETURN n.name, n.val;" "B"

# === GROUP 12: UNWIND (Tests 119-128) ===
echo "--- UNWIND ---"
run "UNWIND ints" "UNWIND [1,2,3] AS x RETURN x;" "1"
run "UNWIND strings" "UNWIND ['a','b','c'] AS name RETURN name;" "a"
run "UNWIND + LIMIT" "UNWIND [1,2,3] AS x RETURN x LIMIT 2;" "2"
run "UNWIND + SKIP" "UNWIND [1,2,3,4,5] AS x RETURN x SKIP 2 LIMIT 2;" "3"
run "UNWIND + SKIP + LIMIT" "UNWIND [1,2,3,4,5] AS x RETURN x SKIP 1 LIMIT 3;" "2"
run "UNWIND single" "UNWIND [42] AS x RETURN x;" "42"
run "UNWIND float" "UNWIND [1.5, 2.5] AS x RETURN x LIMIT 1;" "1.5"

# === GROUP 13: Error recovery (Tests 129-135) ===
echo "--- Error recovery ---"
run "Garbage then valid" "GARBAGE CLAUSE; CREATE (a:Item {name:'X'}) MATCH (n:Item) RETURN n.name;" "X"
run "Two garbage then valid" "FOO; BAR; CREATE (a:Item {name:'Y'}) MATCH (n:Item) RETURN n.name;" "Y"
run "Empty MATCH returns nothing" "MATCH (n:NoLabel) RETURN n;" "0 row"
run "WHERE with no matches" "CREATE (a:Item {name:'A'}) MATCH (n:Item) WHERE n.name = 'Z' RETURN n.name;" "0 row"

# === GROUP 14: Multi-line (Tests 136-140) ===
echo "--- Multi-line ---"
run "Multi-line CREATE+MATCH" "CREATE (n:Item {name:'ML'})
MATCH (n:Item) RETURN n.name;" "ML"
run "Multi-line MATCH+WHERE+RETURN" "CREATE (a:Item {val:42})
MATCH (n:Item)
WHERE n.val > 40
RETURN n.val;" "42"

# === GROUP 15: New expression features (Tests 141-170) ===
echo "--- New expressions ---"
run "XOR true XOR false" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE n.val > 5 XOR n.val < 3 RETURN n.val;" "10"
run "Power expression" "CREATE (a:Item {val:4}) MATCH (n:Item) WHERE n.val ^ 2 = 16 RETURN n.val;" "4"
run "Unary minus in WHERE" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE -n.val < -5 RETURN n.val;" "10"
run "Parenthesized complex" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE (n.val + 5) * 2 > 20 RETURN n.val;" "10"
run "Boolean literal true" "CREATE (a:Item {name:'A'}) MATCH (n:Item) WHERE true RETURN n.name;" "A"
run "Boolean literal false" "CREATE (a:Item {name:'A'}) MATCH (n:Item) WHERE false RETURN n.name;" "0 row"
run "Nested parens" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE ((n.val + 5)) > 12 RETURN n.val;" "10"
run "RETURN unary minus" "CREATE (n:Item {val:5}) MATCH (n:Item) RETURN -n.val;" "-5"
run "RETURN power" "CREATE (n:Item {val:2}) MATCH (n:Item) RETURN n.val ^ 3;" "8"
run "RETURN arithmetic chain" "CREATE (n:Item {val:10}) MATCH (n:Item) RETURN n.val + 5 * 2;" "20"
run "RETURN parenthesized" "CREATE (n:Item {val:10}) MATCH (n:Item) RETURN (n.val + 5) * 2;" "30"
run "XOR with AND" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE n.val > 5 AND n.val < 15 XOR n.val = 10 RETURN n.val;" "10"
run "MODULO" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE n.val % 3 = 1 RETURN n.val;" "10"
run "NOT with parenthesized" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE NOT (n.val = 5) RETURN n.val;" "10"
run "Comparison with arithmetic" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE n.val + n.val = 20 RETURN n.val;" "10"
run "Complex arithmetic WHERE" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE n.val * 3 + 2 = 32 RETURN n.val;" "10"
run "AND OR NOT combo" "CREATE (a:Item {name:'A',val:10}) CREATE (b:Item {name:'B',val:5}) MATCH (n:Item) WHERE (n.val > 3 AND NOT n.val > 9) OR n.name = 'A' RETURN n.name;" "A"
run "WHERE with unary plus" "CREATE (a:Item {val:10}) MATCH (n:Item) WHERE +n.val = 10 RETURN n.val;" "10"
run "RETURN with unary plus" "CREATE (n:Item {val:5}) MATCH (n:Item) RETURN +n.val;" "5"
run "WHERE IS NULL after SET to blank" "CREATE (a:Item {}) MATCH (n:Item) WHERE n.name IS NULL RETURN n.name;" "0 row"
run "RETURN distinct with ORDER" "CREATE (a:Item {val:1}) CREATE (b:Item {val:2}) CREATE (c:Item {val:2}) MATCH (n:Item) RETURN DISTINCT n.val ORDER BY n.val;" "1"
run "WHERE float comparison" "CREATE (a:Item {val:3.14}) MATCH (n:Item) WHERE n.val > 3.0 RETURN n.val;" "3.14"

# === GROUP 16: Edge cases (Tests 171-190) ===
echo "--- Edge cases ---"
run "Backtick identifier" "CREATE (n:Paper {title:'Yes'}) MATCH (\`n\`:Paper) RETURN \`n\`.title;" "Yes"
run "Query with comment" "CREATE (n:Item {name:'C'}) MATCH (n:Item) RETURN n.name;" "C"
run "Zero-length string property" "CREATE (n:Item {name:''}) MATCH (n:Item) RETURN n.name;" "0 row"
run "Very long property value" "CREATE (n:Item {name:'ABCDEFGHIJKLMNOPQRSTUVWXYZ'}) MATCH (n:Item) WHERE n.name CONTAINS 'KLM' RETURN n.name;" "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
run "Multiple labels on node" "CREATE (n:Item:A:B:C {name:'triple'}) MATCH (x:C) RETURN x.name;" "triple"
run "WHERE STARTS WITH prefix" "CREATE (n:Item {name:'abcdef'}) MATCH (n:Item) WHERE n.name STARTS WITH 'abc' RETURN n.name;" "abcdef"
run "WHERE ENDS WITH suffix" "CREATE (n:Item {name:'abcdef'}) MATCH (n:Item) WHERE n.name ENDS WITH 'def' RETURN n.name;" "abcdef"
run "CREATE single-quote string" "CREATE (n:Item {name:'it''s'}) MATCH (n:Item) RETURN n.name;" "it"
run "CREATE double-quote string" "CREATE (n:Item {name:'say \"hi\"'}) MATCH (n:Item) RETURN n.name;" "say"
run "WHERE CONTAINS underscore" "CREATE (n:Item {name:'hello_world'}) MATCH (n:Item) WHERE n.name CONTAINS '_' RETURN n.name;" "hello_world"
run "MATCH on non-existent label" "MATCH (n:PhantomLabel) RETURN n;" "0 row"
run "SET property to existing value" "CREATE (a:Item {name:'A'}) SET a.name = 'A' MATCH (n:Item) RETURN n.name;" "A"
run "CREATE with many properties" "CREATE (n:Doc {a:1,b:2,c:3,d:4}) MATCH (n:Doc) WHERE n.a = 1 AND n.b = 2 RETURN n.c;" "3"
run "WHERE multiple AND conditions" "CREATE (a:Item {name:'A',val:10,tag:'x'}) MATCH (n:Item) WHERE n.name = 'A' AND n.val = 10 AND n.tag = 'x' RETURN n.name;" "A"
run "WHERE multiple OR conditions" "CREATE (a:Item {val:1}) CREATE (b:Item {val:2}) CREATE (c:Item {val:3}) MATCH (n:Item) WHERE n.val = 1 OR n.val = 3 RETURN n.val;" "1"
run "MATCH with pattern properties" "CREATE (a:Item {name:'A',val:10}) MATCH (n:Item {name:'A',val:10}) RETURN n.name;" "A"
run "MATCH without label returns all" "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) MATCH (n) RETURN n.name;" "A"
run "LIMIT larger than results" "CREATE (a:Item {name:'A'}) MATCH (n:Item) RETURN n.name LIMIT 100;" "1 row"
run "SKIP + LIMIT exact" "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) CREATE (c:Item {name:'C'}) MATCH (n:Item) RETURN n.name SKIP 1 LIMIT 1;" "B"

# === GROUP 17: REMOVE (Tests 191-195) ===
echo "--- REMOVE ---"
run "REMOVE property" "CREATE (n:Item {name:'X'}) REMOVE n.name MATCH (n:Item) WHERE n.name IS NULL RETURN n.name;" "0 row"
run "REMOVE nonexistent prop" "CREATE (n:Item {name:'A'}) REMOVE n.other MATCH (n:Item) RETURN n.name;" "A"
run "REMOVE multiple props" "CREATE (n:Item {name:'A',val:1}) REMOVE n.name, n.val MATCH (n:Item) RETURN n.name;" "0 row"

# === GROUP 18: Misc (Tests 196-200) ===
echo "--- Misc ---"
run "CREATE then MATCH same var" "CREATE (n:Item {name:'X'}) MATCH (n:Item) RETURN n.name;" "X"
run "Edge then filter target" "CREATE (a:Item {name:'A'})-[:KNOWS]->(b:Item {name:'B'}) MATCH (a:Item)-[:KNOWS]->(b:Item) WHERE b.name = 'B' RETURN a.name;" "A"
run "REMOVE and verify" "CREATE (n:Item {name:'X',tag:'Y'}) REMOVE n.tag MATCH (n:Item) WHERE n.tag IS NULL RETURN n.name;" "X"

echo ""
echo "========================================="
echo "  Results: $PASS passed, $FAIL failed, $TN total"
echo "========================================="
exit $((FAIL > 0 ? 1 : 0))
