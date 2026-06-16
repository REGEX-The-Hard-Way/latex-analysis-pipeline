#!/bin/bash
# cypher_edge_tests.sh — Exercise Cypher edge cases, report failures
REPL="./cypher_repl.out"
PASS=0; FAIL=0; SKIP=0; TN=0

check() {
    local desc="$1" query="$2" expected="$3"
    TN=$((TN+1))
    local out
    out=$(echo "$query" | timeout 5 "$REPL" 2>/dev/null) || true
    if echo "$out" | grep -qF -- "$expected"; then
        echo "  PASS #$TN $desc"
        PASS=$((PASS+1))
    else
        echo "  FAIL #$TN $desc"
        echo "       expected: $expected"
        echo "       got: $(echo "$out" | tr '\n' ' ' | head -c 120)"
        FAIL=$((FAIL+1))
    fi
}

check_no_err() {
    local desc="$1" query="$2" expected="$3"
    TN=$((TN+1))
    local out
    out=$(echo "$query" | timeout 5 "$REPL" 2>/dev/null) || true
    if echo "$out" | grep -qF -- "$expected" && ! echo "$out" | grep -qF 'error'; then
        echo "  PASS #$TN $desc"
        PASS=$((PASS+1))
    else
        echo "  FAIL #$TN $desc"
        echo "       expected: $expected"
        echo "       got: $(echo "$out" | tr '\n' ' ' | head -c 120)"
        FAIL=$((FAIL+1))
    fi
}

echo "=== Cypher Edge Case Tests ==="

# --- NULL / missing properties ---
check     "IS NULL on missing property" \
  "CREATE (n:T1 {}) MATCH (n:T1) WHERE n.name IS NULL RETURN n;" "0 row"

check     "IS NOT NULL on present property" \
  "CREATE (n:T2 {name:'A'}) MATCH (n:T2) WHERE n.name IS NOT NULL RETURN n.name;" "A"

check     "IS NULL on present property returns none" \
  "CREATE (n:T3 {name:'A'}) MATCH (n:T3) WHERE n.name IS NULL RETURN n.name;" "0 row"

# --- Empty result handling ---
check     "MATCH on non-existent label" \
  "MATCH (n:T4) RETURN n;" "0 row"

check     "COUNT(*) on empty" \
  "MATCH (n:T5) RETURN COUNT(*);" "0"

check     "SUM on empty result" \
  "MATCH (n:T6) RETURN SUM(n.val);" "0"

# --- SKIP / LIMIT boundaries ---
check     "LIMIT 0 returns no rows" \
  "CREATE (n:T7 {v:1}) MATCH (n:T7) RETURN n.v LIMIT 0;" "0 row"

check     "SKIP past all returns no rows" \
  "CREATE (n:T8 {v:1}) MATCH (n:T8) RETURN n.v SKIP 10;" "0 row"

check     "LIMIT larger than results" \
  "CREATE (n:T9 {v:1}) MATCH (n:T9) RETURN n.v LIMIT 100;" "1 row"

# --- Multi-node operations ---
check     "CREATE 3 nodes, MATCH all" \
  "CREATE (a:T10 {}) CREATE (b:T10 {}) CREATE (c:T10 {}) MATCH (n:T10) RETURN COUNT(*);" "3"

# --- Variable binding across clauses ---
check     "SET then MATCH in same statement" \
  "CREATE (a:T11 {name:'old'}) SET a.name = 'new' MATCH (n:T11) RETURN n.name;" "new"

check     "SET overwrite twice" \
  "CREATE (a:T12 {v:1}) SET a.v = 5 SET a.v = 3 MATCH (n:T12) RETURN n.v;" "3"

check     "SET label on existing node" \
  "CREATE (a:T13 {name:'A'}) SET a:VIP MATCH (n:VIP) RETURN n.name;" "A"

# --- DELETE variations ---
check     "DELETE clears node" \
  "CREATE (a:T14 {name:'X'}) DELETE a MATCH (n:T14) RETURN n.name;" "0 row"

check     "DETACH DELETE removes edges" \
  "CREATE (a:T15 {n:'A'}) CREATE (b:T15 {n:'B'}) CREATE (a)-[:L]->(b) DETACH DELETE a MATCH (n:T15) RETURN n.n;" "B"

# --- Aggregation edge cases ---
check     "AVG of single value" \
  "CREATE (a:T16 {v:42}) MATCH (n:T16) RETURN AVG(n.v);" "42"

check     "MIN of multiple values" \
  "CREATE (a:T17 {v:10}) CREATE (b:T17 {v:20}) CREATE (c:T17 {v:30}) MATCH (n:T17) RETURN MIN(n.v);" "10"

check     "MAX of multiple values" \
  "CREATE (a:T18 {v:10}) CREATE (b:T18 {v:20}) CREATE (c:T18 {v:30}) MATCH (n:T18) RETURN MAX(n.v);" "30"

check     "SUM of mixed values" \
  "CREATE (a:T19 {v:5}) CREATE (b:T19 {v:15}) MATCH (n:T19) RETURN SUM(n.v);" "20"

# --- DISTINCT ---
check     "DISTINCT deduplicates" \
  "CREATE (a:T20 {v:1}) CREATE (b:T20 {v:1}) CREATE (c:T20 {v:2}) MATCH (n:T20) RETURN DISTINCT n.v ORDER BY n.v;" "1 row"

# --- ORDER BY edge cases ---
check     "ORDER BY DESC on numbers" \
  "CREATE (a:T21 {v:1}) CREATE (b:T21 {v:3}) MATCH (n:T21) RETURN n.v ORDER BY n.v DESC;" "3"

check     "ORDER BY on strings" \
  "CREATE (a:T22 {n:'B'}) CREATE (b:T22 {n:'A'}) MATCH (n:T22) RETURN n.n ORDER BY n.n;" "A"

# --- Expression edge cases ---
check     "WHERE with XOR" \
  "CREATE (a:T23 {v:10}) MATCH (n:T23) WHERE n.v > 5 XOR n.v < 3 RETURN n.v;" "10"

check     "WHERE with unary minus" \
  "CREATE (a:T24 {v:-10}) MATCH (n:T24) WHERE -n.v = 10 RETURN n.v;" "-10"

check     "WHERE boolean false" \
  "CREATE (a:T25 {name:'A'}) MATCH (n:T25) WHERE false RETURN n.name;" "0 row"

check     "WHERE boolean true" \
  "CREATE (a:T26 {name:'A'}) MATCH (n:T26) WHERE true RETURN n.name;" "A"

check     "RETURN arithmetic" \
  "CREATE (n:T27 {v:10}) MATCH (n:T27) RETURN n.v + 5;" "15"

check     "RETURN power" \
  "CREATE (n:T28 {v:2}) MATCH (n:T28) RETURN n.v ^ 3;" "8"

check     "RETURN parenthesized" \
  "CREATE (n:T29 {v:10}) MATCH (n:T29) RETURN (n.v + 5) * 2;" "30"

# --- WHERE string operators ---
check     "STARTS WITH" \
  "CREATE (n:T30 {name:'HelloWorld'}) MATCH (n:T30) WHERE n.name STARTS WITH 'Hello' RETURN n.name;" "HelloWorld"

check     "ENDS WITH" \
  "CREATE (n:T31 {name:'HelloWorld'}) MATCH (n:T31) WHERE n.name ENDS WITH 'World' RETURN n.name;" "HelloWorld"

check     "CONTAINS" \
  "CREATE (n:T32 {name:'HelloWorld'}) MATCH (n:T32) WHERE n.name CONTAINS 'loWo' RETURN n.name;" "HelloWorld"

# --- IN list ---
check     "IN list of strings" \
  "CREATE (a:T33 {n:'A'}) CREATE (b:T33 {n:'B'}) MATCH (n:T33) WHERE n.n IN ['A','C'] RETURN n.n;" "A"

check     "IN list of numbers" \
  "CREATE (a:T34 {v:10}) CREATE (b:T34 {v:20}) MATCH (n:T34) WHERE n.v IN [10,30] RETURN n.v;" "10"

# --- UNWIND ---
check     "UNWIND with integers" \
  "UNWIND [1,2,3] AS x RETURN x;" "1"

check     "UNWIND with LIMIT" \
  "UNWIND [1,2,3,4,5] AS x RETURN x LIMIT 2;" "2"

check     "UNWIND with SKIP" \
  "UNWIND [1,2,3,4,5] AS x RETURN x SKIP 2 LIMIT 2;" "3"

# --- MERGE ---
check     "MERGE creates new" \
  "MERGE (n:T35 {name:'Alice'}) RETURN n.name;" "Alice"

check     "MERGE idempotent" \
  "MERGE (n:T36 {id:1}) MERGE (n:T36 {id:1}) RETURN count(n);" "1"

# --- REMOVE ---
check_no_err "REMOVE existing property" \
  "CREATE (n:T37 {name:'X',tag:'Y'}) REMOVE n.tag MATCH (n:T37) RETURN n.name;" "X"

check     "REMOVE non-existent property" \
  "CREATE (n:T38 {name:'A'}) REMOVE n.other MATCH (n:T38) RETURN n.name;" "A"

# --- Error recovery ---
check     "Recovers from garbage clause" \
  "GARBAGE; CREATE (n:T40 {name:'X'}) MATCH (n:T40) RETURN n.name;" "X"

# --- Edge creation variations ---
check     "CREATE edge with existing nodes" \
  "CREATE (a:T41 {n:'A'}) CREATE (b:T41 {n:'B'}) CREATE (a)-[:R]->(b) MATCH (x:T41)-[:R]->(y:T41) RETURN y.n;" "B"

check     "Any-direction edge --" \
  "CREATE (a:T42 {n:'A'}) CREATE (b:T42 {n:'B'}) CREATE (a)-[:R]->(b) MATCH (x:T42)--(y:T42) WHERE x.n='A' RETURN y.n;" "B"

# --- Multi-label ---
check     "Multi-label node matching" \
  "CREATE (n:T43:A:B {name:'ml'}) MATCH (x:B) RETURN x.name;" "ml"

# --- Multi-hop ---
check     "Triple hop" \
  "CREATE (a:T44 {n:'A'}) CREATE (b:T44 {n:'B'}) CREATE (c:T44 {n:'C'}) CREATE (a)-[:X]->(b) CREATE (b)-[:X]->(c) MATCH (x:T44)-[:X]->(y:T44)-[:X]->(z:T44) WHERE x.n='A' RETURN z.n;" "C"

echo ""
echo "=== Results: $PASS passed, $FAIL failed, $TN total ==="
exit $((FAIL > 0 ? 1 : 0))
