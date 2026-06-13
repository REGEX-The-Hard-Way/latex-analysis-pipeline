#!/bin/bash
# Smoke tests for cypher_repl.out - validates CREATE, MATCH, RETURN, WHERE, edges
set -euo pipefail

REPL="./cypher_repl.out"
PASS=0
FAIL=0
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

run_query() {
    local desc="$1"
    local query="$2"
    local expected="$3"
    local out
    out=$(echo "$query" | "$REPL" 2>/dev/null) || true
    if echo "$out" | grep -qF "$expected"; then
        echo -e "${GREEN}PASS${NC} $desc"
        PASS=$((PASS + 1))
    else
        echo -e "${RED}FAIL${NC} $desc"
        echo "  expected to contain: $expected"
        echo "  got: $(echo "$out" | head -5)"
        FAIL=$((FAIL + 1))
    fi
}

echo "=== Cypher REPL Smoke Tests ==="
echo ""

# 1. Basic CREATE + RETURN
run_query "CREATE node + RETURN properties" \
    "CREATE (n:Paper {title:'Test', year:2024}) RETURN n.title, n.year;" \
    "Test"

# 2. Multiple CREATEs + MATCH all
run_query "Multiple CREATEs + MATCH all" \
    "CREATE (a:Paper {title:'A'}) CREATE (b:Paper {title:'B'}) MATCH (n:Paper) RETURN n.title;" \
    "A"

# 3. WHERE equality
run_query "WHERE with ==" \
    "CREATE (a:Paper {title:'A', author:'X'}) CREATE (b:Paper {title:'B', author:'Y'}) MATCH (n:Paper) WHERE n.author = 'Y' RETURN n.title;" \
    "B"

# 4. WHERE numeric greater-than
run_query "WHERE with >" \
    "CREATE (a:Paper {title:'A', score:10}) CREATE (b:Paper {title:'B', score:5}) CREATE (c:Paper {title:'C', score:15}) MATCH (n:Paper) WHERE n.score > 7 RETURN n.title;" \
    "A"

# 5. WHERE numeric less-than
run_query "WHERE with <" \
    "CREATE (a:Paper {title:'A', score:10}) CREATE (b:Paper {title:'B', score:5}) MATCH (n:Paper) WHERE n.score < 7 RETURN n.title;" \
    "B"

# 6. WHERE >=
run_query "WHERE with >=" \
    "CREATE (a:Paper {title:'A', year:2020}) CREATE (b:Paper {title:'B', year:2025}) MATCH (n:Paper) WHERE n.year >= 2021 RETURN n.title;" \
    "B"

# 7. WHERE <=
run_query "WHERE with <=" \
    "CREATE (a:Paper {title:'A', year:2020}) CREATE (b:Paper {title:'B', year:2025}) MATCH (n:Paper) WHERE n.year <= 2020 RETURN n.title;" \
    "A"

# 8. Edges
run_query "CREATE edge + MATCH relationship" \
    "CREATE (a:Paper {title:'S'}) CREATE (b:Paper {title:'T'}) CREATE (a)-[:CITES]->(b) MATCH (src:Paper)-[:CITES]->(tgt:Paper) RETURN src.title, tgt.title;" \
    "S"

# 9. Multi-node MATCH with return *
run_query "CREATE + MATCH with RETURN *" \
    "CREATE (n:Paper {title:'P'}) MATCH (p:Paper) RETURN *;" \
    "1 row"

# 10. WHERE with AND
run_query "WHERE with AND" \
    "CREATE (a:Paper {title:'A', year:2020, author:'X'}) CREATE (b:Paper {title:'B', year:2024, author:'X'}) MATCH (n:Paper) WHERE n.author = 'X' AND n.year > 2021 RETURN n.title;" \
    "B"

# 11. Multiple properties in CREATE
run_query "CREATE with numeric prop" \
    "CREATE (n:Paper {title:'Test', pages:42}) RETURN n.title, n.pages;" \
    "42"

# 12. Multiple labels on node
run_query "CREATE with two labels" \
    "CREATE (n:Paper:Featured {title:'F'}) CREATE (m:Paper {title:'R'}) MATCH (x:Featured) RETURN x.title;" \
    "F"

# 13. MATCH without label (all nodes)
run_query "MATCH all nodes (no label)" \
    "CREATE (a:Paper {title:'A'}) CREATE (b:Paper {title:'B'}) MATCH (n) RETURN n.title;" \
    "A"

# 14. WHERE integer prop matching in node pattern
run_query "Node pattern prop match (integer)" \
    "CREATE (a:Paper {title:'A', year:2024}) CREATE (b:Paper {title:'B', year:2023}) MATCH (n:Paper {year:2024}) RETURN n.title;" \
    "A"

# 15. String prop matching in node pattern
run_query "Node pattern prop match (string)" \
    "CREATE (a:Paper {title:'A', author:'X'}) CREATE (b:Paper {title:'B', author:'Y'}) MATCH (n:Paper {author:'X'}) RETURN n.title;" \
    "A"

echo ""
echo "=== Results: ${PASS} passed, ${FAIL} failed ==="
exit $((FAIL > 0 ? 1 : 0))
