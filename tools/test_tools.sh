#!/usr/bin/env bash
# Test suite for tools/ (sidecar_mod + cluster_find)
set -euo pipefail

PASS=0; FAIL=0
RED='\033[0;31m'; GREEN='\033[0;32m'; NC='\033[0m'

pass() { echo -e "${GREEN}PASS${NC} $1"; PASS=$((PASS+1)); }
fail() { echo -e "${RED}FAIL${NC} $1"; FAIL=$((FAIL+1)); }

MOD=./sidecar_mod
CLUSTER=./cluster_find

# Build
gcc -O2 -o "$MOD" sidecar_mod.c json_scan.o -I. 2>/dev/null && pass "build sidecar_mod" || fail "build sidecar_mod"
gcc -O2 -o "$CLUSTER" cluster_find.c 2>/dev/null && pass "build cluster_find" || fail "build cluster_find"

# Test data
cat > /tmp/tools_test.json << 'EOF'
{"filepath":"test.tex","filepath_id":1,"token_id":1,"parent_id":1,"offset":0,"length":23,"type":"author","text":"\\author{K. Trachenko}"}
{"filepath":"test.tex","filepath_id":1,"token_id":2,"parent_id":1,"offset":30,"length":20,"type":"section","text":"\\section{Introduction}"}
{"filepath":"test.tex","filepath_id":1,"token_id":3,"parent_id":1,"offset":55,"length":11,"type":"label","text":"\\label{sec1}"}
{"filepath":"test.tex","filepath_id":1,"token_id":4,"parent_id":1,"offset":70,"length":11,"type":"label","text":"\\label{sec1}"}
{"filepath":"test.tex","filepath_id":1,"token_id":5,"parent_id":1,"offset":85,"length":12,"type":"cite","text":"\\cite{smith}"}
{"filepath":"test.tex","filepath_id":1,"token_id":6,"parent_id":1,"offset":100,"length":15,"type":"math","text":"$E=mc^2$"}
EOF

# Test 1: sidecar_mod regex transform
$MOD /tmp/tools_test.json strip_latex.rules /tmp/tools_out.json /tmp/tools_undo.log
grep -q '"K. Trachenko"' /tmp/tools_out.json && pass "sidecar_mod: strip author" || fail "sidecar_mod: strip author"
grep -q '"Introduction"' /tmp/tools_out.json && pass "sidecar_mod: strip section" || fail "sidecar_mod: strip section"
grep -q '"sec1"' /tmp/tools_out.json && pass "sidecar_mod: strip label" || fail "sidecar_mod: strip label"
grep -q '"smith"' /tmp/tools_out.json && pass "sidecar_mod: strip cite" || fail "sidecar_mod: strip cite"
grep -q '"E=mc^2"' /tmp/tools_out.json && pass "sidecar_mod: strip math" || fail "sidecar_mod: strip math"

# Test 2: Undo log (6 transforms on 6 records)
grep -q '"original":"\\\\author{K. Trachenko}"' /tmp/tools_undo.log && pass "undo log: captures original" || fail "undo log: captures original"
[ $(wc -l < /tmp/tools_undo.log) -ge 5 ] && pass "undo log: >=5 entries ($(wc -l < /tmp/tools_undo.log))" || fail "undo log: too few entries"

# Test 3: Fingerprint clustering finds duplicates (2 identical labels)
$CLUSTER /tmp/tools_test.json label > /tmp/tools_clusters.json 2>/dev/null
if [ -s /tmp/tools_clusters.json ]; then
    grep -q '"count":2' /tmp/tools_clusters.json && pass "cluster_find: duplicate label cluster" || fail "cluster_find: no count=2"
    grep -q '"fingerprint"' /tmp/tools_clusters.json && pass "cluster_find: algorithm=fingerprint" || fail "cluster_find: no fingerprint"
else
    fail "cluster_find: no output"
fi

# Test 4: Levenshtein clustering
$CLUSTER /tmp/tools_test.json label --levenshtein 2 > /tmp/tools_lev.json 2>/dev/null
if [ -s /tmp/tools_lev.json ]; then
    grep -q '"levenshtein"' /tmp/tools_lev.json && pass "cluster_find: algorithm=levenshtein" || fail "cluster_find: no levenshtein"
else
    fail "cluster_find: no levenshtein output"
fi

# Test 5: JSON validity of output
python3 -c "import json; [json.loads(l) for l in open('/tmp/tools_out.json')]" 2>/dev/null && pass "output is valid JSON" || fail "output is valid JSON"
python3 -c "import json; [json.loads(l) for l in open('/tmp/tools_undo.log')]" 2>/dev/null && pass "undo log is valid JSON" || fail "undo log is valid JSON"

echo ""
echo "=== Results: ${PASS} passed, ${FAIL} failed ==="
exit $((FAIL > 0 ? 1 : 0))
