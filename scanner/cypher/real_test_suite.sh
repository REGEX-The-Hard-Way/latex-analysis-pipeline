#!/bin/bash
# real_test_suite.sh — Honest Cypher test suite.
# Tests EVERY feature: working, partial, and missing.
# Each test is tagged: PASS (works), FAIL (should work but doesn't),
#   KNOWN_BROKEN (not yet implemented), REGRESSION (was working, now broken)

REPL="./cypher_repl.out"
PASS=0; FAIL=0; KNOWN_BROKEN=0; REGRESSION=0; TN=0

check() {
    local tag="$1" desc="$2" query="$3" expected="$4"
    TN=$((TN+1))
    local out
    out=$(echo "$query" | timeout 5 "$REPL" 2>/dev/null) || true
    if echo "$out" | grep -qF -- "$expected"; then
        case "$tag" in
            PASS) echo "  PASS #$TN $desc"; PASS=$((PASS+1)) ;;
            KNOWN_BROKEN) echo "  FIXED! #$TN $desc (was broken, now works)"; PASS=$((PASS+1)) ;;
            REGRESSION) echo "  FIXED! #$TN $desc (regression resolved)"; PASS=$((PASS+1)) ;;
        esac
    else
        case "$tag" in
            PASS) echo "  REGRESSION #$TN $desc"; echo "       expected: $expected"; echo "       got: $(echo "$out" | tr '\n' ' ' | head -c 120)"; REGRESSION=$((REGRESSION+1)) ;;
            KNOWN_BROKEN) echo "  KNOWN_BROKEN #$TN $desc (not yet implemented)"; KNOWN_BROKEN=$((KNOWN_BROKEN+1)) ;;
            REGRESSION) echo "  STILL_BROKEN #$TN $desc"; REGRESSION=$((REGRESSION+1)) ;;
        esac
    fi
}

echo "========================================="
echo "  HONEST CYPHER TEST SUITE"
echo "========================================="
echo ""
echo "Legend: PASS = works | KNOWN_BROKEN = not implemented yet | REGRESSION = was working, now broken"
echo ""

# ==========================================
# GROUP 1: WORKING FEATURES (should all PASS)
# ==========================================
echo "--- WORKING FEATURES (should all PASS) ---"

check PASS "CREATE node with string property" \
  "CREATE (n:T1 {name:'hello'}) RETURN n.name;" "hello"

check PASS "CREATE node with integer property" \
  "CREATE (n:T2 {val:42}) RETURN n.val;" "42"

check PASS "CREATE node with float property" \
  "CREATE (n:T3 {price:9.99}) RETURN n.price;" "9.99"

check PASS "CREATE node with negative value" \
  "CREATE (n:T4 {val:-5}) MATCH (n:T4) WHERE n.val = -5 RETURN n.val;" "-5"

check PASS "CREATE two nodes, MATCH both" \
  "CREATE (a:T5 {n:'A'}) CREATE (b:T5 {n:'B'}) MATCH (n:T5) RETURN n.n;" "A"

check PASS "MATCH with WHERE = string" \
  "CREATE (a:T6 {n:'A'}) CREATE (b:T6 {n:'B'}) MATCH (n:T6) WHERE n.n = 'B' RETURN n.n;" "B"

check PASS "MATCH with WHERE > numeric" \
  "CREATE (a:T7 {v:10}) CREATE (b:T7 {v:20}) MATCH (n:T7) WHERE n.v > 10 RETURN n.v;" "20"

check PASS "MATCH with WHERE AND" \
  "CREATE (a:T8 {n:'A',v:10}) CREATE (b:T8 {n:'B',v:20}) MATCH (n:T8) WHERE n.v > 5 AND n.n = 'B' RETURN n.n;" "B"

check PASS "MATCH with WHERE OR" \
  "CREATE (a:T9 {n:'A',v:1}) CREATE (b:T9 {n:'B',v:2}) MATCH (n:T9) WHERE n.n = 'A' OR n.n = 'B' RETURN n.n;" "A"

check PASS "MATCH with WHERE NOT" \
  "CREATE (a:T10 {n:'A'}) CREATE (b:T10 {n:'B'}) MATCH (n:T10) WHERE NOT n.n = 'A' RETURN n.n;" "B"

check PASS "MATCH with WHERE XOR" \
  "CREATE (a:T11 {v:10}) MATCH (n:T11) WHERE n.v > 5 XOR n.v < 3 RETURN n.v;" "10"

check PASS "MATCH with WHERE IS NOT NULL" \
  "CREATE (a:T12 {n:'A'}) CREATE (b:T12 {}) MATCH (n:T12) WHERE n.n IS NOT NULL RETURN n.n;" "A"

check PASS "MATCH with WHERE IS NULL" \
  "CREATE (a:T13 {}) MATCH (n:T13) WHERE n.n IS NULL RETURN n.n;" "0"

check PASS "MATCH with WHERE IN list" \
  "CREATE (a:T14 {n:'A'}) CREATE (b:T14 {n:'B'}) CREATE (c:T14 {n:'C'}) MATCH (n:T14) WHERE n.n IN ['A','C'] RETURN n.n;" "A"

check PASS "MATCH with WHERE CONTAINS" \
  "CREATE (a:T15 {name:'Hello World'}) MATCH (n:T15) WHERE n.name CONTAINS 'World' RETURN n.name;" "Hello World"

check PASS "MATCH with WHERE STARTS WITH" \
  "CREATE (a:T16 {name:'Hello World'}) MATCH (n:T16) WHERE n.name STARTS WITH 'Hello' RETURN n.name;" "Hello World"

check PASS "MATCH with WHERE ENDS WITH" \
  "CREATE (a:T17 {name:'Hello World'}) MATCH (n:T17) WHERE n.name ENDS WITH 'World' RETURN n.name;" "Hello World"

check PASS "MATCH with WHERE arithmetic in expression" \
  "CREATE (a:T18 {v:10}) MATCH (n:T18) WHERE n.v + 5 > 12 RETURN n.v;" "10"

check PASS "MATCH with WHERE parenthesized" \
  "CREATE (a:T19 {v:10}) MATCH (n:T19) WHERE (n.v + 5) > 12 RETURN n.v;" "10"

check PASS "MATCH with WHERE power" \
  "CREATE (a:T20 {v:3}) MATCH (n:T20) WHERE n.v ^ 2 = 9 RETURN n.v;" "3"

check PASS "MATCH with WHERE boolean true" \
  "CREATE (a:T21 {n:'A'}) MATCH (n:T21) WHERE true RETURN n.n;" "A"

check PASS "MATCH with WHERE boolean false" \
  "CREATE (a:T22 {n:'A'}) MATCH (n:T22) WHERE false RETURN n.n;" "0 row"

check PASS "MATCH with multi-label" \
  "CREATE (n:T23:A:B {name:'ml'}) MATCH (x:B) RETURN x.name;" "ml"

check PASS "MATCH all nodes (no label)" \
  "CREATE (a:T24 {n:'A'}) CREATE (b:T24 {n:'B'}) MATCH (n) RETURN n.n;" "A"

check PASS "MATCH on empty label returns nothing" \
  "MATCH (n:T25) RETURN n;" "0 row"

check PASS "RETURN with alias" \
  "CREATE (n:T26 {name:'X'}) MATCH (n:T26) RETURN n.name AS label;" "label"

check PASS "RETURN DISTINCT" \
  "CREATE (a:T27 {v:1}) CREATE (b:T27 {v:1}) CREATE (c:T27 {v:2}) MATCH (n:T27) RETURN DISTINCT n.v ORDER BY n.v;" "2"

check PASS "RETURN arithmetic expression" \
  "CREATE (n:T28 {v:10}) MATCH (n:T28) RETURN n.v + 5;" "15"

check PASS "RETURN with ORDER BY ASC" \
  "CREATE (a:T29 {v:3}) CREATE (b:T29 {v:1}) MATCH (n:T29) RETURN n.v ORDER BY n.v;" "1"

check PASS "RETURN with ORDER BY DESC" \
  "CREATE (a:T30 {v:1}) CREATE (b:T30 {v:3}) MATCH (n:T30) RETURN n.v ORDER BY n.v DESC;" "3"

check PASS "RETURN with LIMIT" \
  "CREATE (a:T31 {v:1}) CREATE (b:T31 {v:2}) MATCH (n:T31) RETURN n.v ORDER BY n.v LIMIT 1;" "1"

check PASS "RETURN with SKIP" \
  "CREATE (a:T32 {v:1}) CREATE (b:T32 {v:2}) MATCH (n:T32) RETURN n.v ORDER BY n.v SKIP 1;" "2"

check PASS "RETURN with SKIP + LIMIT" \
  "CREATE (a:T33 {v:1}) CREATE (b:T33 {v:2}) CREATE (c:T33 {v:3}) MATCH (n:T33) RETURN n.v ORDER BY n.v SKIP 1 LIMIT 1;" "2"

check PASS "RETURN with CASE expression" \
  "CREATE (n:T34 {v:10}) MATCH (n:T34) RETURN CASE WHEN n.v > 5 THEN 1 ELSE 0 END;" "1"

check PASS "COUNT(*) aggregation" \
  "CREATE (a:T35 {}) CREATE (b:T35 {}) CREATE (c:T35 {}) MATCH (n:T35) RETURN COUNT(*);" "3"

check PASS "SUM aggregation" \
  "CREATE (a:T36 {v:10}) CREATE (b:T36 {v:20}) MATCH (n:T36) RETURN SUM(n.v);" "30"

check PASS "AVG aggregation" \
  "CREATE (a:T37 {v:10}) CREATE (b:T37 {v:20}) MATCH (n:T37) RETURN AVG(n.v);" "15"

check PASS "MIN aggregation" \
  "CREATE (a:T38 {v:10}) CREATE (b:T38 {v:20}) MATCH (n:T38) RETURN MIN(n.v);" "10"

check PASS "MAX aggregation" \
  "CREATE (a:T39 {v:10}) CREATE (b:T39 {v:20}) MATCH (n:T39) RETURN MAX(n.v);" "20"

check PASS "Multi-aggregate in one query" \
  "CREATE (a:T40 {v:10}) CREATE (b:T40 {v:20}) MATCH (n:T40) RETURN COUNT(*), SUM(n.v), AVG(n.v);" "30"

check PASS "CREATE edge between nodes" \
  "CREATE (a:T41 {n:'A'}) CREATE (b:T41 {n:'B'}) CREATE (a)-[:L]->(b) MATCH (x:T41)-[:L]->(y:T41) RETURN y.n;" "B"

check PASS "CREATE edge any direction --" \
  "CREATE (a:T42 {n:'A'}) CREATE (b:T42 {n:'B'}) CREATE (a)-[:L]->(b) MATCH (x:T42)--(y:T42) WHERE x.n='A' RETURN y.n;" "B"

check PASS "CREATE compound edge with target props" \
  "CREATE (a:T43 {n:'A'})-[:L]->(b:T43 {n:'B',v:42}) MATCH (s:T43)-[:L]->(t:T43) WHERE s.n='A' RETURN t.n, t.v;" "B"

check PASS "Multi-hop (2 edges)" \
  "CREATE (a:T44 {n:'A'}) CREATE (b:T44 {n:'B'}) CREATE (c:T44 {n:'C'}) CREATE (a)-[:X]->(b) CREATE (b)-[:X]->(c) MATCH (x:T44)-[:X]->(y:T44)-[:X]->(z:T44) WHERE x.n='A' RETURN z.n;" "C"

check PASS "SET property on existing node" \
  "CREATE (a:T45 {n:'old'}) SET a.n = 'new' MATCH (n:T45) RETURN n.n;" "new"

check PASS "SET label on existing node" \
  "CREATE (a:T46 {n:'A'}) SET a:VIP MATCH (n:VIP) RETURN n.n;" "A"

check PASS "DELETE node" \
  "CREATE (a:T47 {n:'X'}) DELETE a MATCH (n:T47) RETURN n.n;" "0 row"

check PASS "DETACH DELETE node with edges" \
  "CREATE (a:T48 {n:'A'}) CREATE (b:T48 {n:'B'}) CREATE (a)-[:X]->(b) DETACH DELETE a MATCH (n:T48) RETURN n.n;" "B"

check PASS "DELETE multiple nodes" \
  "CREATE (a:T49 {n:'A'}) CREATE (b:T49 {n:'B'}) DELETE a, b MATCH (n:T49) RETURN n.n;" "0 row"

check PASS "MERGE create new node" \
  "MERGE (n:T50 {name:'Alice'}) RETURN n.name;" "Alice"

check PASS "MERGE idempotent" \
  "MERGE (n:T51 {id:1}) MERGE (n:T51 {id:1}) RETURN count(n);" "1"

check PASS "MERGE with edge" \
  "MERGE (a:T52 {n:'A'})-[:K]->(b:T52 {n:'B'}) RETURN a.n, b.n;" "A"

check PASS "REMOVE property" \
  "CREATE (n:T53 {n:'X',tag:'Y'}) REMOVE n.tag MATCH (n:T53) RETURN n.n;" "X"

check PASS "UNWIND list" \
  "UNWIND [1,2,3] AS x RETURN x;" "1"

check PASS "UNWIND with LIMIT" \
  "UNWIND [1,2,3,4,5] AS x RETURN x LIMIT 2;" "2"

check PASS "UNWIND with SKIP" \
  "UNWIND [1,2,3,4,5] AS x RETURN x SKIP 2 LIMIT 2;" "3"

check PASS "Variable-length path [*2]" \
  "CREATE (a:T54 {n:'A'}) CREATE (b:T54 {n:'B'}) CREATE (c:T54 {n:'C'}) CREATE (a)-[:X]->(b) CREATE (b)-[:X]->(c) MATCH (s:T54)-[:X*2]->(t:T54) WHERE s.n='A' RETURN t.n;" "C"

check PASS "Error recovery from garbage" \
  "GARBAGE CLAUSE; CREATE (n:T55 {n:'X'}) MATCH (n:T55) RETURN n.n;" "X"

check PASS "Multi-line query" \
  "CREATE (n:T56 {n:'ML'})
MATCH (n:T56) RETURN n.n;" "ML"

check PASS "SET with arithmetic RHS" \
  "CREATE (a:T57 {v:1}) SET a.v = 5 + 3 MATCH (n:T57) RETURN n.v;" "8"

check PASS "WHERE on edge target property" \
  "CREATE (a:T58 {n:'A'})-[:R]->(b:T58 {n:'B',v:99}) MATCH (s:T58)-[:R]->(t:T58) WHERE t.v = 99 RETURN s.n;" "A"

# ==========================================
# GROUP 2: KNOWN BROKEN / NOT IMPLEMENTED
# ==========================================
echo ""
echo "--- NOT YET IMPLEMENTED (expected to fail) ---"

check KNOWN_BROKEN "UNION two queries" \
  "CREATE (a:U1 {v:1}) MATCH (n:U1) RETURN n.v UNION CREATE (b:U1 {v:2}) MATCH (n:U1) RETURN n.v;" "| 1"

check KNOWN_BROKEN "UNION ALL" \
  "CREATE (a:U2 {v:1}) MATCH (n:U2) RETURN n.v UNION ALL MATCH (n:U2) RETURN n.v;" "| 2"

check KNOWN_BROKEN "WITH pipelining (results flow between clauses)" \
  "CREATE (a:U3 {v:1}) CREATE (b:U3 {v:2}) MATCH (n:U3) WITH n.v AS val WHERE val > 1 RETURN val;" "2"

check KNOWN_BROKEN "OPTIONAL MATCH outer join (NULL for no match)" \
  "CREATE (a:U4 {n:'A'}) MATCH OPTIONAL (n:U4) MATCH OPTIONAL (m:U5) RETURN n.n;" "A"

check KNOWN_BROKEN "MERGE ON MATCH SET" \
  "CREATE (n:U6 {id:1}) MERGE (n:U6 {id:1}) ON MATCH SET n.updated = 1 RETURN n.updated;" "1"

check KNOWN_BROKEN "MERGE ON CREATE SET" \
  "MERGE (n:U7 {id:99}) ON CREATE SET n.created = 1 RETURN n.created;" "1"

check KNOWN_BROKEN "CALL procedure" \
  "CALL db.labels();" "Token"

check KNOWN_BROKEN "SET variable = value" \
  "CREATE (a:U8 {}) SET a = 5 RETURN a;" "1 row"

check KNOWN_BROKEN "SET += list append" \
  "CREATE (a:U9 {tags:['a']}) SET a.tags = a.tags + ['b'] RETURN a.tags;" "b"

check KNOWN_BROKEN "EXISTS pattern in WHERE" \
  "CREATE (a:U10 {n:'A'})-[:X]->(b:U10 {n:'B'}) MATCH (n:U10) WHERE EXISTS((n)-[:X]->()) RETURN n.n;" "A"

check KNOWN_BROKEN "Parameters \$param" \
  "CREATE (n:U11 {name:'test'}) MATCH (n:U11) WHERE n.name = \$name RETURN n.name;" "test"

check KNOWN_BROKEN "ALL predicate" \
  "CREATE (a:U12 {v:10}) MATCH (n:U12) WHERE ALL(x IN [5,10,15] WHERE x >= n.v) RETURN n.v;" "10"

check KNOWN_BROKEN "List comprehension" \
  "RETURN [x IN [1,2,3] WHERE x > 1 | x * 2];" "4"

check KNOWN_BROKEN "CASE as standalone clause" \
  "CREATE (n:U13 {v:10}) MATCH (n:U13) CASE WHEN n.v > 5 THEN 'big' ELSE 'small' END;" "big"

check KNOWN_BROKEN "List slicing [0..2]" \
  "RETURN [10,20,30,40][0..2];" "10"

check KNOWN_BROKEN "COALESCE function" \
  "CREATE (n:U14 {}) MATCH (n:U14) RETURN COALESCE(n.missing, 'default');" "default"

check KNOWN_BROKEN "Pattern variable assignment p = (a)-->(b)" \
  "CREATE (a:U15 {n:'A'})-[:X]->(b:U15 {n:'B'}) MATCH p = (x:U15)-->(y:U15) RETURN p;" "1 row"

check KNOWN_BROKEN "Relationship properties in CREATE" \
  "CREATE (a:U16 {n:'A'})-[:R {weight:0.5}]->(b:U16 {n:'B'}) MATCH (a)-[r:R]->(b) RETURN r.weight;" "0.5"

# ==========================================
# GROUP 3: EDGE CASES THAT SHOULD WORK
# ==========================================
echo ""
echo "--- EDGE CASES (should work, may have bugs) ---"

check PASS "STRING = number comparison returns no rows" \
  "CREATE (a:E1 {v:10}) MATCH (n:E1) WHERE n.v = 'hello' RETURN n.v;" "0 row"

check PASS "WHERE on missing property via CONTAINS" \
  "CREATE (a:E2 {name:'test'}) MATCH (n:E2) WHERE n.missing CONTAINS 'x' RETURN n.name;" "0 row"

check PASS "COUNT(*) on empty result" \
  "MATCH (n:E3) RETURN COUNT(*);" "0"

check PASS "SUM on empty result" \
  "MATCH (n:E4) RETURN SUM(n.val);" "0"

check PASS "AVG on empty result" \
  "MATCH (n:E5) RETURN AVG(n.val);" "0"

check PASS "MIN on empty result" \
  "MATCH (n:E6) RETURN MIN(n.val);" "0"

check PASS "MAX on empty result" \
  "MATCH (n:E7) RETURN MAX(n.val);" "0"

check PASS "LIMIT 0 returns no rows" \
  "CREATE (n:E8 {v:1}) MATCH (n:E8) RETURN n.v LIMIT 0;" "0 row"

check PASS "SKIP past all rows returns empty" \
  "CREATE (n:E9 {v:1}) MATCH (n:E9) RETURN n.v SKIP 10;" "0 row"

check PASS "LIMIT larger than row count" \
  "CREATE (n:E10 {v:1}) MATCH (n:E10) RETURN n.v LIMIT 100;" "1 row"

check PASS "MATCH with multi-pattern comma" \
  "CREATE (a:E11 {n:'A'}) CREATE (b:E11 {n:'B'}) MATCH (x:E11), (y:E11) RETURN x.n, y.n;" "A"

check PASS "RETURN * on node with properties" \
  "CREATE (n:E12 {name:'X',v:42}) MATCH (n:E12) RETURN *;" "42"

check PASS "RETURN division" \
  "CREATE (n:E13 {v:10}) MATCH (n:E13) RETURN n.v / 3;" "3.33333"

check PASS "RETURN modulo" \
  "CREATE (n:E14 {v:17}) MATCH (n:E14) RETURN n.v % 5;" "2"

check PASS "RETURN unary minus" \
  "CREATE (n:E15 {v:5}) MATCH (n:E15) RETURN -n.v;" "-5"

check PASS "MATCH with backtick identifiers" \
  "CREATE (n:\`special\` {name:'yes'}) MATCH (n:\`special\`) RETURN n.name;" "yes"

check PASS "MATCH pattern with inline properties" \
  "CREATE (a:E16 {n:'A',v:10}) MATCH (n:E16 {n:'A',v:10}) RETURN n.n;" "A"

echo ""
echo "========================================="
echo "  RESULTS"
echo "========================================="
echo "  PASS:          $PASS (should all pass)"
echo "  KNOWN_BROKEN:  $KNOWN_BROKEN (not yet implemented)"
echo "  REGRESSION:    $REGRESSION (was working, now broken!)"
echo "  TOTAL:         $TN"
echo ""
if [ $REGRESSION -gt 0 ]; then
    echo "  ⚠ REGRESSIONS DETECTED — features that should work are broken!"
    exit 1
fi
echo "  $(($PASS + $KNOWN_BROKEN)) / $TN expected results match"
