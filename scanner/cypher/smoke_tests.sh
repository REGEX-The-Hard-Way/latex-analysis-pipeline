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

# 16. CONTAINS operator
run_query "CONTAINS string search" \
    "CREATE (a:Item {name:'Hello World'}) CREATE (b:Item {name:'Foo Bar'}) MATCH (n:Item) WHERE n.name CONTAINS 'World' RETURN n.name;" \
    "Hello World"

# 17. Multi-hop path
run_query "Multi-hop (a)-->(b)-->(c)" \
    "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) CREATE (c:Item {name:'C'}) CREATE (a)-[:LINK]->(b) CREATE (b)-[:LINK]->(c) MATCH (x:Item)-[:LINK]->(y:Item)-[:LINK]->(z:Item) WHERE x.name = 'A' RETURN z.name;" \
    "C"

# 18. COUNT(*) aggregation
run_query "COUNT(*) aggregation" \
    "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) CREATE (c:Item {name:'C'}) MATCH (n:Item) RETURN COUNT(*);" \
    "3"

# 19. IN operator
run_query "IN operator" \
    "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) CREATE (c:Item {name:'C'}) MATCH (n:Item) WHERE n.name IN ['A', 'C'] RETURN n.name;" \
    "A"

# 20. ORDER BY DESC
run_query "ORDER BY DESC" \
    "CREATE (a:Item {name:'A', val:1}) CREATE (b:Item {name:'B', val:3}) CREATE (c:Item {name:'C', val:2}) MATCH (n:Item) RETURN n.name, n.val ORDER BY n.val DESC LIMIT 2;" \
    "B"

# 21. Filepath property via interned lookup
run_query "Filepath property" \
    "CREATE (a:Item {name:'test'}) MATCH (n:Item) RETURN n.name;" \
    "test"

# 22. Arithmetic + in WHERE
run_query "Arithmetic +" \
    "CREATE (a:Item {val:10}) CREATE (b:Item {val:5}) MATCH (n:Item) WHERE n.val + 5 > 12 RETURN n.val;" \
    "10"

# 23. Arithmetic * in WHERE
run_query "Arithmetic *" \
    "CREATE (a:Item {val:3}) CREATE (b:Item {val:2}) MATCH (n:Item) WHERE n.val * 2 = 6 RETURN n.val;" \
    "3"

# 24. IS NOT NULL
run_query "IS NOT NULL" \
    "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:''}) MATCH (n:Item) WHERE n.name IS NOT NULL RETURN n.name;" \
    "A"

# 25. SET label assignment
run_query "SET label" \
    "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) SET a:Featured MATCH (n:Featured) RETURN n.name;" \
    "A"

# 26. Error recovery (garbage before valid query)
run_query "Error recovery" \
    "GARBAGE CLAUSE; CREATE (a:Item {name:'X'}) MATCH (n:Item) RETURN n.name;" \
    "X"

# 27-31: Sidecar integration tests (only if sidecar-small.json exists)
if [ -f ../sidecar-small.json ]; then
    echo ""
    echo "--- Sidecar integration tests ---"

    # 27. Parent-child: display math children exist
    run_query "display_math has children" \
        "MATCH (dm:display_2_math)-[:PARENT_OF]->(c:Token) RETURN c LIMIT 1;" \
        "row"

    # 28. math_sub tokens exist
    run_query "math_sub tokens exist" \
        "MATCH (m:math_sub) RETURN m LIMIT 1;" \
        "row"

    # 29. math_sup tokens exist
    run_query "math_sup tokens exist" \
        "MATCH (m:math_sup) RETURN m LIMIT 1;" \
        "row"

    # 30. frac tokens exist
    run_query "frac tokens exist" \
        "MATCH (f:frac) RETURN f LIMIT 1;" \
        "row"

    # 31. braces tokens are children of frac
    run_query "braces child of display_math" \
        "MATCH (dm:display_2_math)-[:PARENT_OF]->(b:braces) RETURN b LIMIT 1;" \
        "row"
fi

# 32-36: Math context awareness (requires make ctx_sidecar first)
echo ""
echo "--- Math context tests ---"
CTX_FILE="/tmp/context_test/sidecar.json"
if [ -f "$CTX_FILE" ]; then
    run_query_ctx() {
        local desc="$1"
        local query="$2"
        local expected="$3"
        local out
        out=$(echo "$query" | "$REPL" --sidecar "$CTX_FILE" 2>/dev/null) || true
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

    run_query_ctx "math_var count — prose x excluded" \
        "MATCH (m:math_var) RETURN COUNT(*) as cnt;" \
        "| 4"

    run_query_ctx "display_math has math_var children" \
        "MATCH (dm:display_math)-[:PARENT_OF]->(mv:math_var) RETURN mv LIMIT 1;" \
        "row"

    run_query_ctx "inline math block has children" \
        "MATCH (m:math)-[:PARENT_OF]->(c) RETURN COUNT(*) as cnt;" \
        "| 3"

    run_query_ctx "greek inside display_2_math" \
        "MATCH (dm:display_2_math)-[:PARENT_OF]->(g:math_greek) RETURN g LIMIT 1;" \
        "row"

    run_query_ctx "math_op inside inline math" \
        "MATCH (m:math)-[:PARENT_OF]->(op:math_op) RETURN op LIMIT 1;" \
        "row"
else
    echo "  (skipped — run 'make ctx_sidecar' first)"
fi

echo ""
echo "=== Results: ${PASS} passed, ${FAIL} failed ==="
exit $((FAIL > 0 ? 1 : 0))
