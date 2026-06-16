#!/bin/bash
# cypher_bugs.sh — Find real Cypher bugs
REPL="./cypher_repl.out"
PASS=0; FAIL=0; TN=0

check() {
    local desc="$1" query="$2" expected="$3"
    TN=$((TN+1))
    local out
    out=$(echo "$query" | timeout 5 "$REPL" 2>/dev/null) || true
    if echo "$out" | grep -qF -- "$expected"; then
        echo "  PASS #$TN $desc"; PASS=$((PASS+1))
    else
        echo "  FAIL #$TN $desc"
        echo "       expected: $expected"
        echo "       got: $(echo "$out" | tr '\n' ' ' | head -c 150)"
        FAIL=$((FAIL+1))
    fi
}

echo "=== Cypher Bug Hunt ==="

# --- STRING equality vs number comparison ---
check "String = number (should be no match)" \
  "CREATE (a:B1 {v:10}) MATCH (n:B1) WHERE n.v = 'hello' RETURN n.v;" "0 row"

check "Number = string property no match" \
  "CREATE (a:B2 {name:'hello'}) MATCH (n:B2) WHERE n.name = 42 RETURN n.name;" "0 row"

# --- RETURN expression on non-existent property ---
check "RETURN missing property shows 0" \
  "CREATE (n:B3 {v:10}) MATCH (n:B3) RETURN n.missing;" "0"

# --- Aggregation on empty groups ---
check "AVG on empty" \
  "MATCH (n:B4) RETURN AVG(n.val);" "0"

check "MIN on empty" \
  "MATCH (n:B5) RETURN MIN(n.val);" "0"

check "MAX on empty" \
  "MATCH (n:B6) RETURN MAX(n.val);" "0"

# --- Multiple CREATEs + multi-pattern MATCH ---
check "Multi-pattern MATCH cross product" \
  "CREATE (a:B7 {n:'A'}) CREATE (b:B7 {n:'B'}) MATCH (x:B7), (y:B7) RETURN x.n, y.n;" "A"

# --- CREATE edge with target properties ---
check "CREATE compound edge, target has props" \
  "CREATE (a:B8 {n:'A'})-[:X]->(b:B8 {n:'B',v:42}) MATCH (s:B8)-[:X]->(t:B8) WHERE s.n='A' RETURN t.n, t.v;" "B"

# --- WHERE with complex boolean ---
check "WHERE NOT (a OR b)" \
  "CREATE (a:B9 {v:10}) MATCH (n:B9) WHERE NOT (n.v = 5 OR n.v = 20) RETURN n.v;" "10"

check "WHERE (a AND b) OR c" \
  "CREATE (a:B10 {v:10,n:'A'}) CREATE (b:B10 {v:20,n:'B'}) MATCH (n:B10) WHERE (n.v > 5 AND n.n = 'A') OR n.n = 'B' RETURN n.n;" "A"

# --- DELETE with comma-separated nodes ---
check "DELETE multiple nodes" \
  "CREATE (a:B11 {n:'A'}) CREATE (b:B11 {n:'B'}) DELETE a, b MATCH (n:B11) RETURN n.n;" "0 row"

# --- SET property to expression ---
check "SET to arithmetic expression" \
  "CREATE (a:B12 {v:1}) SET a.v = 5 + 3 MATCH (n:B12) RETURN n.v;" "8"

# --- MERGE with relationship ---
check "MERGE with edge" \
  "MERGE (a:B13 {n:'A'})-[:K]->(b:B13 {n:'B'}) RETURN a.n, b.n;" "A"

# --- ORDER BY multiple columns ---
check "ORDER BY two columns" \
  "CREATE (a:B14 {n:'A',v:3}) CREATE (b:B14 {n:'A',v:1}) CREATE (c:B14 {n:'B',v:2}) MATCH (n:B14) RETURN n.n, n.v ORDER BY n.n, n.v;" "1"

# --- RETURN * on node with properties ---
check "RETURN * shows values" \
  "CREATE (n:B15 {name:'X',v:42}) MATCH (n:B15) RETURN *;" "42"

# --- WHERE on edge traversal target ---
check "WHERE on edge target property" \
  "CREATE (a:B16 {n:'A'})-[:R]->(b:B16 {n:'B',v:99}) MATCH (s:B16)-[:R]->(t:B16) WHERE t.v = 99 RETURN s.n;" "A"

# --- CONTAINS on non-existent property ---
check "CONTAINS on missing property returns false" \
  "CREATE (a:B17 {name:'test'}) MATCH (n:B17) WHERE n.missing CONTAINS 'x' RETURN n.name;" "0 row"

# --- STARTS WITH on non-existent property ---
check "STARTS WITH on missing property returns false" \
  "CREATE (a:B18 {name:'test'}) MATCH (n:B18) WHERE n.missing STARTS WITH 'x' RETURN n.name;" "0 row"

# --- WHERE with arithmetic comparison both sides ---
check "WHERE a + b = c" \
  "CREATE (a:B19 {x:3,y:4}) MATCH (n:B19) WHERE n.x + n.y = 7 RETURN n.x;" "3"

# --- RETURN with modulo ---
check "RETURN modulo" \
  "CREATE (n:B20 {v:17}) MATCH (n:B20) RETURN n.v % 5;" "2"

# --- RETURN with division ---
check "RETURN division" \
  "CREATE (n:B21 {v:10}) MATCH (n:B21) RETURN n.v / 3;" "3.33333"

# --- Aggregation with WHERE filter ---
check "SUM with WHERE filter" \
  "CREATE (a:B22 {v:10}) CREATE (b:B22 {v:20}) CREATE (c:B22 {v:30}) MATCH (n:B22) WHERE n.v > 10 RETURN SUM(n.v);" "50"

# --- Variable-length path by 2 ---
check "Var-len path exact 2" \
  "CREATE (a:B23 {n:'A'}) CREATE (b:B23 {n:'B'}) CREATE (c:B23 {n:'C'}) CREATE (a)-[:X]->(b) CREATE (b)-[:X]->(c) MATCH (s:B23)-[:X*2]->(t:B23) WHERE s.n='A' RETURN t.n;" "C"

# --- OPTIONAL MATCH basic ---
check "OPTIONAL MATCH finds node" \
  "CREATE (a:B24 {n:'A'}) MATCH OPTIONAL (n:B24) RETURN n.n;" "A"

check "OPTIONAL MATCH no match" \
  "MATCH OPTIONAL (n:B25) RETURN n.n;" "0 row"

echo ""
echo "=== Results: $PASS passed, $FAIL failed, $TN total ==="
