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
run_ne() {
    # assert expected NOT in output
    local desc="$1" query="$2" unexpected="$3"
    TN=$((TN+1))
    local out
    out=$(echo "$query" | "$REPL" 2>/dev/null) || true
    if ! echo "$out" | grep -qF -- "$unexpected"; then
        echo "PASS #$TN $desc"; PASS=$((PASS+1))
    else
        echo "FAIL #$TN $desc"; echo "  found unexpected: $unexpected"; FAIL=$((FAIL+1))
    fi
}

echo "=== Cypher Final Test Suite ==="
echo ""

# GROUP 1: CREATE + RETURN (1-10)
echo "--- CREATE + RETURN ---"
run "CREATE with string" "CREATE (n:Test1 {title:'Hello'}) RETURN n.title;" "Hello"
run "CREATE with integer" "CREATE (n:Test2 {pages:42}) RETURN n.pages;" "42"
run "CREATE with float" "CREATE (n:Test3 {price:9.99}) RETURN n.price;" "9.99"
run "CREATE with two labels" "CREATE (n:Test4:VIP {title:'F'}) MATCH (x:VIP) RETURN x.title;" "F"
run "CREATE multi-property" "CREATE (n:Test5 {a:'T',b:2024,c:300}) RETURN n.a, n.b;" "T"
run "Multiple CREATE nodes" "CREATE (a:Test6 {name:'A'}) CREATE (b:Test6 {name:'B'}) MATCH (n:Test6) RETURN n.name;" "A"
run "CREATE with zero" "CREATE (n:Test7 {val:0}) MATCH (n:Test7) WHERE n.val = 0 RETURN n.val;" "0"
run "CREATE with negative" "CREATE (n:Test8 {val:-5}) MATCH (n:Test8) WHERE n.val = -5 RETURN n.val;" "-5"
run "CREATE with empty string" "CREATE (n:Test9 {name:''}) MATCH (n:Test9) RETURN n.name;" "1 row"
run "CREATE with hex" "CREATE (n:Test10 {val:255}) MATCH (n:Test10) WHERE n.val = 255 RETURN n.val;" "255"

# GROUP 2: MATCH + WHERE (11-30)
echo "--- MATCH + WHERE ---"
run "WHERE = string" "CREATE (a:M1 {name:'A'}) CREATE (b:M1 {name:'B'}) MATCH (n:M1) WHERE n.name = 'B' RETURN n.name;" "B"
run "WHERE = integer" "CREATE (a:M2 {val:10}) CREATE (b:M2 {val:20}) MATCH (n:M2) WHERE n.val = 20 RETURN n.val;" "20"
run "WHERE > integer" "CREATE (a:M3 {val:10}) CREATE (b:M3 {val:20}) MATCH (n:M3) WHERE n.val > 10 RETURN n.val;" "20"
run "WHERE < integer" "CREATE (a:M4 {val:10}) CREATE (b:M4 {val:20}) MATCH (n:M4) WHERE n.val < 15 RETURN n.val;" "10"
run "WHERE >= integer" "CREATE (a:M5 {val:10}) CREATE (b:M5 {val:20}) MATCH (n:M5) WHERE n.val >= 20 RETURN n.val;" "20"
run "WHERE <= integer" "CREATE (a:M6 {val:10}) CREATE (b:M6 {val:20}) MATCH (n:M6) WHERE n.val <= 10 RETURN n.val;" "10"
run "WHERE <> integer" "CREATE (a:M7 {val:10}) CREATE (b:M7 {val:20}) MATCH (n:M7) WHERE n.val <> 10 RETURN n.val;" "20"
run "WHERE <> string" "CREATE (a:M8 {name:'A'}) CREATE (b:M8 {name:'B'}) MATCH (n:M8) WHERE n.name <> 'A' RETURN n.name;" "B"
run "WHERE AND" "CREATE (a:M9 {name:'A',val:10}) CREATE (b:M9 {name:'B',val:20}) MATCH (n:M9) WHERE n.val > 5 AND n.name = 'B' RETURN n.name;" "B"
run "WHERE OR" "CREATE (a:M10 {name:'A',v:1}) CREATE (b:M10 {name:'B',v:2}) MATCH (n:M10) WHERE n.name = 'A' OR n.name = 'B' RETURN n.name;" "A"
run "WHERE NOT" "CREATE (a:M11 {name:'A'}) CREATE (b:M11 {name:'B'}) MATCH (n:M11) WHERE NOT n.name = 'A' RETURN n.name;" "B"
run "WHERE arithmetic +" "CREATE (a:M12 {val:10}) MATCH (n:M12) WHERE n.val + 5 > 12 RETURN n.val;" "10"
run "WHERE arithmetic -" "CREATE (a:M13 {val:10}) MATCH (n:M13) WHERE n.val - 3 = 7 RETURN n.val;" "10"
run "WHERE arithmetic *" "CREATE (a:M14 {val:3}) MATCH (n:M14) WHERE n.val * 2 = 6 RETURN n.val;" "3"
run "WHERE arithmetic /" "CREATE (a:M15 {val:10}) MATCH (n:M15) WHERE n.val / 2 = 5 RETURN n.val;" "10"
run "WHERE modulo %" "CREATE (a:M16 {val:10}) MATCH (n:M16) WHERE n.val % 3 = 1 RETURN n.val;" "10"
run "WHERE power ^" "CREATE (a:M17 {val:3}) MATCH (n:M17) WHERE n.val ^ 2 = 9 RETURN n.val;" "3"
run "WHERE unary minus" "CREATE (a:M18 {val:10}) MATCH (n:M18) WHERE -n.val < -5 RETURN n.val;" "10"
run "WHERE parenthesized" "CREATE (a:M19 {val:10}) MATCH (n:M19) WHERE (n.val + 5) > 12 RETURN n.val;" "10"
run "WHERE nested parens" "CREATE (a:M20 {val:10}) MATCH (n:M20) WHERE ((n.val + 5)) > 12 RETURN n.val;" "10"

# GROUP 3: WHERE special operators (31-45)
echo "--- WHERE special ---"
run "CONTAINS" "CREATE (a:W1 {name:'Hello World'}) MATCH (n:W1) WHERE n.name CONTAINS 'World' RETURN n.name;" "Hello World"
run "STARTS WITH" "CREATE (a:W2 {name:'Hello World'}) MATCH (n:W2) WHERE n.name STARTS WITH 'Hello' RETURN n.name;" "Hello World"
run "ENDS WITH" "CREATE (a:W3 {name:'Hello World'}) MATCH (n:W3) WHERE n.name ENDS WITH 'World' RETURN n.name;" "Hello World"
run "IN list string" "CREATE (a:W4 {name:'A'}) CREATE (b:W4 {name:'B'}) CREATE (c:W4 {name:'C'}) MATCH (n:W4) WHERE n.name IN ['A','C'] RETURN n.name;" "A"
run "IN list numeric" "CREATE (a:W5 {val:10}) CREATE (b:W5 {val:20}) MATCH (n:W5) WHERE n.val IN [10, 30] RETURN n.val;" "10"
run "IS NOT NULL" "CREATE (a:W6 {name:'A'}) CREATE (b:W6 {}) MATCH (n:W6) WHERE n.name IS NOT NULL RETURN n.name;" "A"
run "XOR true" "CREATE (a:W7 {val:10}) MATCH (n:W7) WHERE n.val > 3 XOR n.val > 20 RETURN n.val;" "10"
run "XOR false" "CREATE (a:W8 {val:10}) MATCH (n:W8) WHERE n.val = 10 XOR n.val = 10 RETURN n.val;" "0 row"
run "Boolean true" "CREATE (a:W9 {name:'A'}) MATCH (n:W9) WHERE true RETURN n.name;" "A"
run "Boolean false" "CREATE (a:W10 {name:'A'}) MATCH (n:W10) WHERE false RETURN n.name;" "0 row"
run "NOT parenthesized" "CREATE (a:W11 {val:10}) MATCH (n:W11) WHERE NOT (n.val = 5) RETURN n.val;" "10"
run "AND OR NOT combo" "CREATE (a:W12 {name:'A',v:10}) CREATE (b:W12 {name:'B',v:5}) MATCH (n:W12) WHERE (n.v > 3 AND NOT n.v > 9) OR n.name = 'A' RETURN n.name;" "A"
run "Unary plus" "CREATE (a:W13 {val:10}) MATCH (n:W13) WHERE +n.val = 10 RETURN n.val;" "10"
run "Float comparison" "CREATE (a:W14 {val:3.14}) MATCH (n:W14) WHERE n.val > 3.0 RETURN n.val;" "3.14"
run "WHERE false returns 0" "CREATE (a:W15 {name:'X'}) MATCH (n:W15) WHERE false RETURN n.name;" "0 row"

# GROUP 4: RETURN variants (46-58)
echo "--- RETURN ---"
run "RETURN *" "CREATE (n:R1 {name:'X'}) MATCH (n:R1) RETURN *;" "1 row"
run "RETURN AS alias" "CREATE (n:R2 {name:'X'}) MATCH (n:R2) RETURN n.name AS lbl;" "lbl"
run "RETURN multiple cols" "CREATE (n:R3 {name:'X',v:1}) MATCH (n:R3) RETURN n.name, n.v;" "X"
run "RETURN DISTINCT" "CREATE (a:R4 {name:'A'}) CREATE (b:R4 {name:'A'}) MATCH (n:R4) RETURN DISTINCT n.name;" "1 row"
run "RETURN arith +" "CREATE (n:R5 {val:10}) MATCH (n:R5) RETURN n.val + 5;" "15"
run "RETURN arith *" "CREATE (n:R6 {val:3}) MATCH (n:R6) RETURN n.val * 4;" "12"
run "RETURN power" "CREATE (n:R7 {val:2}) MATCH (n:R7) RETURN n.val ^ 3;" "8"
run "RETURN unary minus" "CREATE (n:R8 {val:5}) MATCH (n:R8) RETURN -n.val;" "-5"
run "RETURN unary plus" "CREATE (n:R9 {val:5}) MATCH (n:R9) RETURN +n.val;" "5"
run "RETURN parens" "CREATE (n:R10 {val:10}) MATCH (n:R10) RETURN (n.val + 5) * 2;" "30"
run "RETURN arith chain" "CREATE (n:R11 {val:10}) MATCH (n:R11) RETURN n.val + 5 * 2;" "20"
run "RETURN mod" "CREATE (n:R12 {val:10}) MATCH (n:R12) RETURN n.val % 7;" "3"
run "RETURN divide" "CREATE (n:R13 {val:10}) MATCH (n:R13) RETURN n.val / 3;" "3.33333"

# GROUP 5: ORDER BY (59-72)
echo "--- ORDER BY ---"
run "ORDER BY ASC" "CREATE (a:O1 {v:3}) CREATE (b:O1 {v:1}) CREATE (c:O1 {v:2}) MATCH (n:O1) RETURN n.v ORDER BY n.v;" "1"
run "ORDER BY DESC" "CREATE (a:O2 {v:1}) CREATE (b:O2 {v:3}) MATCH (n:O2) RETURN n.v ORDER BY n.v DESC;" "3"
run "ORDER BY string ASC" "CREATE (a:O3 {n:'B'}) CREATE (b:O3 {n:'A'}) MATCH (n:O3) RETURN n.n ORDER BY n.n;" "A"
run "ORDER BY string DESC" "CREATE (a:O4 {n:'A'}) CREATE (b:O4 {n:'B'}) MATCH (n:O4) RETURN n.n ORDER BY n.n DESC;" "B"
run "ORDER BY + LIMIT" "CREATE (a:O5 {v:1}) CREATE (b:O5 {v:3}) CREATE (c:O5 {v:2}) MATCH (n:O5) RETURN n.v ORDER BY n.v LIMIT 2;" "2"
run "ORDER BY + SKIP" "CREATE (a:O6 {v:1}) CREATE (b:O6 {v:2}) CREATE (c:O6 {v:3}) MATCH (n:O6) RETURN n.v ORDER BY n.v SKIP 1;" "2"
run "ORDER BY DESC + LIMIT" "CREATE (a:O7 {v:1}) CREATE (b:O7 {v:3}) MATCH (n:O7) RETURN n.v ORDER BY n.v DESC LIMIT 2;" "3"
run "ORDER BY + SKIP + LIMIT" "CREATE (a:O8 {v:1}) CREATE (b:O8 {v:2}) CREATE (c:O8 {v:3}) MATCH (n:O8) RETURN n.v ORDER BY n.v SKIP 1 LIMIT 1;" "2"
run "ORDER BY multi-key" "CREATE (a:O9 {n:'A',v:3}) CREATE (b:O9 {n:'A',v:1}) MATCH (n:O9) RETURN n.n, n.v ORDER BY n.n, n.v;" "1"
run "ORDER BY + DISTINCT" "CREATE (a:O10 {v:1}) CREATE (b:O10 {v:2}) CREATE (c:O10 {v:2}) MATCH (n:O10) RETURN DISTINCT n.v ORDER BY n.v;" "1"
run "ORDER BY DESC no match" "CREATE (a:O11 {v:1}) MATCH (n:O11) WHERE n.v = 99 RETURN n.v ORDER BY n.v;" "0 row"
run "ORDER BY with AS" "CREATE (a:O12 {v:3}) MATCH (n:O12) RETURN n.v AS val ORDER BY val;" "3"

# GROUP 6: LIMIT/SKIP (73-78)
echo "--- LIMIT SKIP ---"
run "LIMIT 1 on multi" "CREATE (a:L1 {}) CREATE (b:L1 {}) MATCH (n:L1) RETURN n LIMIT 1;" "1 row"
run "LIMIT 0" "CREATE (a:L2 {name:'X'}) MATCH (n:L2) RETURN n.name LIMIT 0;" "0 row"
run "SKIP all" "CREATE (a:L3 {name:'A'}) MATCH (n:L3) RETURN n.name SKIP 10;" "0 row"
run "SKIP + LIMIT" "CREATE (a:L4 {n:'A'}) CREATE (b:L4 {n:'B'}) CREATE (c:L4 {n:'C'}) MATCH (n:L4) RETURN n.n SKIP 1 LIMIT 1;" "B"
run "LIMIT > count" "CREATE (a:L5 {name:'A'}) MATCH (n:L5) RETURN n.name LIMIT 100;" "1 row"
run "SKIP + LIMIT exact" "CREATE (a:L6 {n:'A'}) CREATE (b:L6 {n:'B'}) CREATE (c:L6 {n:'C'}) MATCH (n:L6) RETURN n.n SKIP 1 LIMIT 1;" "B"

# GROUP 7: Edge traversal (79-95)
echo "--- Edges ---"
run "CREATE edge" "CREATE (a:E1 {n:'S'})-[:LINK]->(b:E1 {n:'T'}) MATCH (src:E1)-[:LINK]->(tgt:E1) RETURN src.n;" "S"
run "Edge with WHERE source" "CREATE (a:E2 {n:'S'})-[:LINK]->(b:E2 {n:'T'}) MATCH (src:E2)-[:LINK]->(tgt:E2) WHERE src.n = 'S' RETURN tgt.n;" "T"
run "Left-directed" "CREATE (a:E3 {n:'S'}) CREATE (b:E3 {n:'T'}) CREATE (a)-[:LINK]->(b) MATCH (src:E3)<-[:LINK]-(tgt:E3) RETURN src.n, tgt.n;" "T"
run "Any type --" "CREATE (a:E4 {n:'S'}) CREATE (b:E4 {n:'T'}) CREATE (a)-[:LINK]->(b) MATCH (src:E4)--(tgt:E4) RETURN src.n;" "S"
run "Multi-hop 2" "CREATE (a:E5 {n:'A'}) CREATE (b:E5 {n:'B'}) CREATE (c:E5 {n:'C'}) CREATE (a)-[:X]->(b) CREATE (b)-[:X]->(c) MATCH (x:E5)-[:X]->(y:E5)-[:X]->(z:E5) WHERE x.n = 'A' RETURN z.n;" "C"
run "Multi-hop 3" "CREATE (a:E6 {n:'A'}) CREATE (b:E6 {n:'B'}) CREATE (c:E6 {n:'C'}) CREATE (d:E6 {n:'D'}) CREATE (a)-[:H]->(b) CREATE (b)-[:H]->(c) CREATE (c)-[:H]->(d) MATCH (w:E6)-[:H]->(x:E6)-[:H]->(y:E6)-[:H]->(z:E6) WHERE w.n = 'A' RETURN z.n;" "D"
run "No matching edge" "CREATE (a:E7 {n:'A'}) CREATE (b:E7 {n:'B'}) MATCH (src:E7)-[:X]->(tgt:E7) RETURN src.n;" "0 row"
run "Edge with property on source" "CREATE (a:E8 {n:'A',v:42})-[:R]->(b:E8 {n:'B'}) MATCH (s:E8)-[:R]->(t:E8) WHERE s.v = 42 RETURN t.n;" "B"
run "Edge with property on target" "CREATE (a:E9 {n:'A',v:42})-[:R]->(b:E9 {n:'B'}) MATCH (s:E9)-[:R]->(t:E9) WHERE s.n = 'A' RETURN t.n;" "B"
run "Edge both props" "CREATE (a:E10 {n:'A'}) CREATE (b:E10 {n:'B'}) CREATE (a)-[:T]->(b) MATCH (s:E10)-[:T]->(t:E10) WHERE s.n = 'A' RETURN t.n;" "B"

# GROUP 8: Aggregation (96-100)
echo "--- Aggregation ---"
run "COUNT(*) 3" "CREATE (a:A1 {}) CREATE (b:A1 {}) CREATE (c:A1 {}) MATCH (n:A1) RETURN COUNT(*);" "3"
run "COUNT(*) with WHERE" "CREATE (a:A2 {v:10}) CREATE (b:A2 {v:20}) MATCH (n:A2) WHERE n.v > 10 RETURN COUNT(*);" "1"
run "COUNT(*) with label" "CREATE (a:A3:Active {}) CREATE (b:A3 {}) MATCH (n:Active) RETURN COUNT(*);" "1"
run "COUNT(*) empty" "MATCH (n:A4) RETURN COUNT(*);" "0"
run "COUNT(*) 2" "CREATE (a:A5 {}) CREATE (b:A5 {}) MATCH (n:A5) RETURN COUNT(*);" "2"

# GROUP 9: MERGE (101-108)
echo "--- MERGE ---"
run "MERGE create" "MERGE (n:M1 {name:'Alice'}) RETURN n.name;" "Alice"
run "MERGE with int" "MERGE (n:M2 {id:123}) RETURN n.id;" "123"
run "MERGE idempotent" "MERGE (n:M3 {id:1}) MERGE (n:M3 {id:1}) RETURN count(n);" "1"
run "MERGE two nodes" "MERGE (a:M4 {id:1}) MERGE (b:M4 {id:2}) RETURN count(*);" "2"
run "MERGE with edge" "MERGE (a:M5 {name:'A'})-[:K]->(b:M5 {name:'B'}) RETURN a.name, b.name;" "A"
run "MERGE multi-label" "MERGE (n:M6:P:Athlete {name:'Dave'}) RETURN n.name;" "Dave"
run "MERGE existing" "CREATE (n:M7 {name:'Bob'}) MERGE (n:M7 {name:'Bob'}) RETURN n.name;" "Bob"
run "MERGE float value" "MERGE (p:M8 {price:9.99}) RETURN p.price;" "9.99"

# GROUP 10: DELETE (109-114)
echo "--- DELETE ---"
run "DELETE node" "CREATE (a:D1 {n:'Del'}) DELETE a MATCH (n:D1) RETURN n.n;" "0 row"
run "DETACH DELETE" "CREATE (a:D2 {n:'Keep'}) CREATE (b:D2 {n:'Del'}) DETACH DELETE b MATCH (n:D2) RETURN n.n;" "Keep"
run "DETACH DELETE edges" "CREATE (a:D3 {n:'A'}) CREATE (b:D3 {n:'B'}) CREATE (a)-[:X]->(b) DETACH DELETE a MATCH (n:D3) RETURN n.n;" "B"
run "DELETE then verify" "CREATE (a:D4 {n:'X'}) DELETE a MATCH (n:D4) RETURN n.n;" "0 row"
run "No DETACH needed for no edges" "CREATE (a:D5 {n:'X'}) DELETE a MATCH (n:D5) RETURN n.n;" "0 row"

# GROUP 11: SET (115-122)
echo "--- SET ---"
run "SET property string" "CREATE (a:S1 {name:'Old'}) SET a.name = 'New' MATCH (n:S1) RETURN n.name;" "New"
run "SET property numeric" "CREATE (a:S2 {val:1}) SET a.val = 42 MATCH (n:S2) RETURN n.val;" "42"
run "SET label" "CREATE (a:S3 {name:'A'}) SET a:VIP MATCH (n:VIP) RETURN n.name;" "A"
run "SET multi props" "CREATE (a:S4 {n:'A',v:1}) SET a.n = 'B', a.v = 2 MATCH (n:S4) RETURN n.n, n.v;" "B"
run "SET overwrite" "CREATE (a:S5 {n:'A'}) SET a.n = 'X' SET a.n = 'Y' MATCH (n:S5) RETURN n.n;" "Y"
run "SET to same value" "CREATE (a:S6 {name:'A'}) SET a.name = 'A' MATCH (n:S6) RETURN n.name;" "A"
run "SET float" "CREATE (a:S7 {v:1}) SET a.v = 3.14159 MATCH (n:S7) RETURN n.v;" "3.14159"
run "SET negative" "CREATE (a:S8 {v:1}) SET a.v = -10 MATCH (n:S8) RETURN n.v;" "-10"

# GROUP 12: UNWIND (123-132)
echo "--- UNWIND ---"
run "UNWIND ints" "UNWIND [1,2,3] AS x RETURN x;" "1"
run "UNWIND strings" "UNWIND ['a','b','c'] AS x RETURN x;" "a"
run "UNWIND + LIMIT" "UNWIND [1,2,3,4,5] AS x RETURN x LIMIT 2;" "2"
run "UNWIND + SKIP" "UNWIND [1,2,3,4,5] AS x RETURN x SKIP 2 LIMIT 2;" "3"
run "UNWIND + SKIP + LIMIT" "UNWIND [1,2,3,4,5] AS x RETURN x SKIP 1 LIMIT 3;" "2"
run "UNWIND single" "UNWIND [42] AS x RETURN x;" "42"
run "UNWIND float" "UNWIND [1.5, 2.5] AS x RETURN x LIMIT 1;" "1.5"
run "UNWIND + ALIAS" "UNWIND [1,2] AS val RETURN val AS num;" "num"
run "UNWIND empty check" "UNWIND [1,2,3] AS x RETURN x LIMIT 0;" "0 row"

# GROUP 13: Error recovery (133-140)
echo "--- Error recovery ---"
run "Garbage then valid" "FOO BAR; CREATE (a:Z1 {name:'X'}) MATCH (n:Z1) RETURN n.name;" "X"
run "Empty MATCH" "MATCH (n:Z2) RETURN n;" "0 row"
run "WHERE no matches" "CREATE (a:Z3 {name:'A'}) MATCH (n:Z3) WHERE n.name = 'Z' RETURN n.name;" "0 row"
run "Garbage then MATCH" "XYZZY; MATCH (n:Z4) RETURN n;" "0 row"
run "Multi-line with garbage" "FOO;
CREATE (n:Z5 {name:'ML'})
MATCH (n:Z5) RETURN n.name;" "ML"

# GROUP 14: Multi-line (141-145)
echo "--- Multi-line ---"
run "ML CREATE+MATCH+RETURN" "CREATE (n:Q1 {name:'ML'})
MATCH (n:Q1) RETURN n.name;" "ML"
run "ML MATCH+WHERE+RETURN" "CREATE (a:Q2 {val:42})
MATCH (n:Q2)
WHERE n.val > 40
RETURN n.val;" "42"

# GROUP 15: Edge cases (146-165)
echo "--- Edge cases ---"
run "Backtick id" "CREATE (n:U1 {t:'Yes'}) MATCH (\`n\`:U1) RETURN \`n\`.t;" "Yes"
run "Zero-len property" "CREATE (n:U2 {name:''}) MATCH (n:U2) RETURN n.name;" "1 row"
run "Long property value" "CREATE (n:U3 {name:'ABCDEFGHIJKLMNOPQRSTUVWXYZ'}) MATCH (n:U3) WHERE n.name CONTAINS 'KLM' RETURN n.name;" "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
run "Multiple labels" "CREATE (n:U4:A:B:C {name:'trip'}) MATCH (x:C) RETURN x.name;" "trip"
run "STARTS WITH prefix" "CREATE (n:U5 {name:'abcdef'}) MATCH (n:U5) WHERE n.name STARTS WITH 'abc' RETURN n.name;" "abcdef"
run "ENDS WITH suffix" "CREATE (n:U6 {name:'abcdef'}) MATCH (n:U6) WHERE n.name ENDS WITH 'def' RETURN n.name;" "abcdef"
run "CONTAINS special char" "CREATE (n:U7 {name:'a_b-c'}) MATCH (n:U7) WHERE n.name CONTAINS '_' RETURN n.name;" "a_b-c"
run "Multi AND conditions" "CREATE (a:U8 {n:'A',v:10,t:'x'}) MATCH (n:U8) WHERE n.n = 'A' AND n.v = 10 AND n.t = 'x' RETURN n.n;" "A"
run "Multi OR conditions" "CREATE (a:U9 {v:1}) CREATE (b:U9 {v:2}) CREATE (c:U9 {v:3}) MATCH (n:U9) WHERE n.v = 1 OR n.v = 3 RETURN n.v;" "1"
run "Pattern properties" "CREATE (a:U10 {n:'A',v:10}) MATCH (n:U10 {n:'A',v:10}) RETURN n.n;" "A"
run "All nodes no label" "CREATE (a:U11 {n:'A'}) CREATE (b:U11 {n:'B'}) MATCH (n) RETURN n.n;" "A"
run "MATCH on absent label" "MATCH (n:U12) RETURN n;" "0 row"
run "MATCH then SET" "CREATE (a:U13 {n:'Old'}) SET a.n = 'New' MATCH (m:U13) RETURN m.n;" "New"
run "Comparison with arithmetic on both sides" "CREATE (a:U14 {v:10}) MATCH (n:U14) WHERE n.v + n.v = 20 RETURN n.v;" "10"
run "Complex WHERE arithmetic" "CREATE (a:U15 {v:10}) MATCH (n:U15) WHERE n.v * 3 + 2 = 32 RETURN n.v;" "10"
run "CREATE many properties" "CREATE (n:U16 {a:1,b:2,c:3,d:4}) MATCH (n:U16) WHERE n.a = 1 AND n.b = 2 RETURN n.c;" "3"
run "CREATE single-quote string" "CREATE (n:U17 {name:'it''s'}) MATCH (n:U17) WHERE n.name CONTAINS 'it' RETURN n.name;" "it"
run "CREATE with esc seq" "CREATE (n:U18 {name:'a\\nb'}) MATCH (n:U18) RETURN n.name;" "a\\nb"
run "NOT IS NULL" "CREATE (a:U19 {name:'A'}) MATCH (n:U19) WHERE n.name IS NOT NULL RETURN n.name;" "A"
run "IS NULL with missing prop" "CREATE (a:U20 {}) MATCH (n:U20) WHERE n.name IS NULL RETURN n.name;" "0"

# GROUP 16: REMOVE (166-173)
echo "--- REMOVE ---"
run "REMOVE property" "CREATE (n:V1 {name:'X',val:1}) REMOVE n.val MATCH (n:V1) RETURN n.name;" "X"
run "REMOVE nonexistent" "CREATE (n:V2 {name:'A'}) REMOVE n.unknown MATCH (n:V2) RETURN n.name;" "A"
run "REMOVE then verify gone" "CREATE (n:V3 {name:'X',tag:'Y'}) REMOVE n.tag MATCH (n:V3) RETURN n.name;" "X"
run "REMOVE last property" "CREATE (n:V4 {name:'X'}) REMOVE n.name MATCH (n:V4) WHERE n.name IS NULL RETURN n.name;" "0 row"

# GROUP 17: Compound operations (174-190)
echo "--- Compound ---"
run "CREATE + MATCH same var" "CREATE (n:C1 {name:'X'}) MATCH (n:C1) RETURN n.name;" "X"
run "CREATE + SET + MATCH" "CREATE (a:C2 {n:'A'}) SET a.n = 'B' MATCH (n:C2) RETURN n.n;" "B"
run "CREATE + MERGE + MATCH" "CREATE (a:C3 {n:'A'}) MERGE (b:C3 {n:'B'}) MATCH (n:C3) RETURN n.n;" "A"
run "SET + DELETE other" "CREATE (a:C4 {n:'A'}) CREATE (b:C4 {n:'B'}) DELETE b MATCH (n:C4) RETURN n.n;" "A"
run "MERGE then SET" "MERGE (n:C5 {id:1}) SET n.v = 99 MATCH (n:C5) RETURN n.v;" "99"
run "CREATE edge then DETACH" "CREATE (a:C6 {n:'A'}) CREATE (b:C6 {n:'B'}) CREATE (a)-[:X]->(b) DETACH DELETE a MATCH (n:C6) RETURN n.n;" "B"
run "Multi-clause pipeline" "CREATE (a:C7 {n:'A',v:1}) CREATE (b:C7 {n:'B',v:2}) MATCH (n:C7) WHERE n.v = 2 RETURN n.n;" "B"
run "Complex WHERE + ORDER" "CREATE (a:C8 {n:'A',v:3}) CREATE (b:C8 {n:'B',v:1}) CREATE (c:C8 {n:'C',v:2}) MATCH (n:C8) WHERE n.v > 1 RETURN n.n ORDER BY n.n;" "A"
run "RETURN DISTINCT + LIMIT" "CREATE (a:C9 {v:1}) CREATE (b:C9 {v:1}) CREATE (c:C9 {v:2}) MATCH (n:C9) RETURN DISTINCT n.v ORDER BY n.v LIMIT 1;" "1"
run "WHERE + ORDER + SKIP + LIMIT" "CREATE (a:C10 {n:'A',v:1}) CREATE (b:C10 {n:'B',v:2}) CREATE (c:C10 {n:'C',v:3}) MATCH (n:C10) RETURN n.n ORDER BY n.n SKIP 1 LIMIT 1;" "B"

# GROUP 18: expression edge cases (191-200)
echo "--- Expression edge ---"
run "Power of power" "CREATE (n:X1 {v:2}) MATCH (n:X1) RETURN n.v ^ 2 ^ 3;" "64"
run "Arith paren precedence" "CREATE (n:X2 {v:10}) MATCH (n:X2) WHERE n.v * (2 + 3) = 50 RETURN n.v;" "10"
run "XOR with AND combo" "CREATE (a:X3 {v:10}) MATCH (n:X3) WHERE n.v > 5 AND n.v < 15 XOR n.v = 10 RETURN n.v;" "0 row"
run "WHERE IN empty list" "CREATE (a:X4 {name:'A'}) MATCH (n:X4) WHERE n.name IN [] RETURN n.name;" "0 row"
run "RETURN with all ops" "CREATE (n:X5 {v:10}) MATCH (n:X5) RETURN n.v * 2 + 3 - 1;" "22"
run "WHERE <> number" "CREATE (a:X6 {v:10}) CREATE (b:X6 {v:20}) MATCH (n:X6) WHERE n.v <> 20 RETURN n.v;" "10"
run "WHERE IS NULL on present" "CREATE (a:X7 {name:'A'}) MATCH (n:X7) WHERE n.name IS NULL RETURN n.name;" "0 row"
run "RETURN negative result" "CREATE (n:X8 {v:5}) MATCH (n:X8) RETURN n.v - 10;" "-5"
run "WHERE double negative" "CREATE (a:X9 {v:10}) MATCH (n:X9) WHERE NOT n.v < 5 RETURN n.v;" "10"
run "Complex multi-AND" "CREATE (a:X10 {a:1,b:2,c:3,d:4,e:5}) MATCH (n:X10) WHERE n.a=1 AND n.b=2 AND n.c=3 AND n.d=4 RETURN n.e;" "5"

# GROUP 19: Extra coverage to reach 200 (173-200)
echo "--- Extra coverage ---"
run "REMOVE multi properties" "CREATE (n:Z1 {a:'x',b:'y'}) REMOVE n.a, n.b MATCH (n:Z1) RETURN n.a;" "1 row"
run "MERGE with SET after" "MERGE (n:Z2 {id:99}) SET n.tag = 'ok' MATCH (n:Z2) RETURN n.tag;" "ok"
run "DELETE where label" "CREATE (a:Z3:Temp {name:'X'}) DELETE a MATCH (n:Temp) RETURN n.name;" "0 row"
run "CREATE unlabeled MATCH all" "CREATE (a {name:'anon'}) MATCH (n) RETURN n.name;" "anon"
run "WHERE <> on string" "CREATE (a:Z4 {tag:'abc'}) CREATE (b:Z4 {tag:'def'}) MATCH (n:Z4) WHERE n.tag <> 'def' RETURN n.tag;" "abc"
run "SKIP 0 + LIMIT" "CREATE (a:Z5 {v:1}) CREATE (b:Z5 {v:2}) MATCH (n:Z5) RETURN n.v ORDER BY n.v SKIP 0 LIMIT 1;" "1"
run "RETURN * with alias" "CREATE (n:Z6 {name:'X',val:1}) MATCH (n:Z6) RETURN *;" "1 row"
run "WHERE numeric OR string" "CREATE (a:Z7 {n:'A',v:10}) CREATE (b:Z7 {n:'B',v:5}) MATCH (n:Z7) WHERE n.v = 10 OR n.n = 'B' RETURN n.n;" "A"
run "XOR with OR" "CREATE (a:Z8 {v:10}) MATCH (n:Z8) WHERE n.v > 20 OR n.v = 10 XOR n.v = 10 RETURN n.v;" "0 row"
run "NOT with boolean" "CREATE (a:Z9 {name:'A'}) MATCH (n:Z9) WHERE NOT false RETURN n.name;" "A"
run "WHERE arith chain" "CREATE (a:Z10 {v:10}) MATCH (n:Z10) WHERE n.v * 2 + 5 - 3 = 22 RETURN n.v;" "10"
run "WHERE division compare" "CREATE (a:Z11 {v:20}) MATCH (n:Z11) WHERE n.v / 4 = 5 RETURN n.v;" "20"
run "WHERE power compare" "CREATE (a:Z12 {v:4}) MATCH (n:Z12) WHERE n.v ^ 3 = 64 RETURN n.v;" "4"
run "ORDER BY with WHERE" "CREATE (a:Z13 {v:3}) CREATE (b:Z13 {v:1}) CREATE (c:Z13 {v:2}) MATCH (n:Z13) WHERE n.v > 1 RETURN n.v ORDER BY n.v;" "2"
run "UNWIND with WHERE-like" "UNWIND [5,10,15] AS x RETURN x LIMIT 2;" "5"
run "COUNT(*) with DISTINCT" "CREATE (a:Z14 {v:1}) CREATE (b:Z14 {v:1}) MATCH (n:Z14) RETURN DISTINCT n.v;" "1 row"
run "SET overwrite numeric" "CREATE (a:Z15 {v:1}) SET a.v = 5 SET a.v = 3 MATCH (n:Z15) RETURN n.v;" "3"
run "CREATE with relation both props" "CREATE (a:Z16 {n:'A',v:1})-[:E]->(b:Z16 {n:'B',v:2}) MATCH (s:Z16)-[:E]->(t:Z16) WHERE s.n='A' RETURN t.n;" "B"
run "DELETE node with label" "CREATE (a:Z17:Del {n:'X'}) DELETE a MATCH (n:Del) RETURN n.n;" "0 row"
run "WHERE CONTAINS exact" "CREATE (a:Z18 {name:'match'}) MATCH (n:Z18) WHERE n.name CONTAINS 'match' RETURN n.name;" "match"
run "WHERE STARTS WITH edge" "CREATE (a:Z19 {name:'prefix_suffix'}) MATCH (n:Z19) WHERE n.name STARTS WITH 'prefix' RETURN n.name;" "prefix_suffix"
run "WHERE ENDS WITH edge" "CREATE (a:Z20 {name:'abc_end'}) MATCH (n:Z20) WHERE n.name ENDS WITH '_end' RETURN n.name;" "abc_end"
run "RETURN * on multi-label" "CREATE (n:Z21:A:B {name:'multi'}) MATCH (n:A) RETURN *;" "1 row"
run "ORDER BY DESC + SKIP + LIMIT" "CREATE (a:Z22 {v:1}) CREATE (b:Z22 {v:2}) CREATE (c:Z22 {v:3}) MATCH (n:Z22) RETURN n.v ORDER BY n.v DESC SKIP 1 LIMIT 1;" "2"
run "LIMIT with WHERE" "CREATE (a:Z23 {v:1}) CREATE (b:Z23 {v:2}) CREATE (c:Z23 {v:3}) MATCH (n:Z23) WHERE n.v >= 2 RETURN n.v ORDER BY n.v LIMIT 1;" "2"
run "IN with multiple matches" "CREATE (a:Z24 {v:1}) CREATE (b:Z24 {v:2}) CREATE (c:Z24 {v:3}) MATCH (n:Z24) WHERE n.v IN [1,3] RETURN n.v ORDER BY n.v;" "1"
run "RETURN expr compare" "CREATE (n:Z25 {v:10}) MATCH (n:Z25) WHERE n.v + n.v > 15 RETURN n.v;" "10"
run "RETURN mod expr" "CREATE (n:Z26 {v:17}) MATCH (n:Z26) RETURN n.v % 5;" "2"

echo ""
echo "========================================="
echo "  $PASS passed, $FAIL failed, $TN total"
echo "========================================="
exit $((FAIL > 0 ? 1 : 0))
