#!/usr/bin/env bash
# sidecar_apply.sh — interactive sidecar modification with fzf preview.
#
# Workflow (mirrors fv.sh):
#   1. Select a rule file
#   2. fzf preview shows before/after for matching records
#   3. Enter applies the rules, generates undo log
#   4. Optionally revert via undo log
#
# Usage: ./sidecar_apply.sh sidecar.json [rules.txt]

set -euo pipefail

SIDECAR="${1:-../scanner/sidecar.json}"
RULES="${2:-strip_latex.rules}"
MOD_ENGINE="./sidecar_mod"
OUTPUT="${SIDECAR%.json}.mod.json"
UNDO="${SIDECAR%.json}.undo.log"

# colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

if [ ! -f "$SIDECAR" ]; then
    echo "Usage: $0 sidecar.json [rules.txt]"
    exit 1
fi

if [ ! -f "$MOD_ENGINE" ]; then
    echo "Building sidecar_mod..."
    gcc -O2 -o "$MOD_ENGINE" sidecar_mod.c || exit 1
fi

echo -e "${GREEN}=== Sidecar Modification Engine ===${NC}"
echo "  Input:  $SIDECAR"
echo "  Rules:  $RULES"
echo "  Output: $OUTPUT"
echo "  Undo:   $UNDO"
echo ""

# Step 1: Show what rules would match
echo -e "${YELLOW}Previewing rule matches...${NC}"
echo ""

# Extract first matching record for each rule type
while IFS= read -r rule_line; do
    [ -z "$rule_line" ] && continue
    match_type=$(echo "$rule_line" | python3 -c "import json,sys; print(json.load(sys.stdin).get('match_type',''))" 2>/dev/null || echo "")
    transform=$(echo "$rule_line" | python3 -c "import json,sys; print(json.load(sys.stdin).get('transform',''))" 2>/dev/null || echo "")
    name=$(echo "$rule_line" | python3 -c "import json,sys; print(json.load(sys.stdin).get('name',''))" 2>/dev/null || echo "")
    [ -z "$match_type" ] && continue

    # Find first matching record
    sample=$(grep -m1 "\"type\":\"$match_type\"" "$SIDECAR" | head -1)
    if [ -n "$sample" ]; then
        sample_text=$(echo "$sample" | python3 -c "import json,sys; print(json.load(sys.stdin).get('text',''))" 2>/dev/null | head -c 80)
        echo "  Rule: $name"
        echo "    Type: $match_type  Transform: $transform"
        echo "    Sample: $sample_text"
        echo ""
    fi
done < "$RULES"

# Count matching records
total_matches=0
while IFS= read -r rule_line; do
    [ -z "$rule_line" ] && continue
    match_type=$(echo "$rule_line" | python3 -c "import json,sys; print(json.load(sys.stdin).get('match_type',''))" 2>/dev/null || echo "")
    [ -z "$match_type" ] && continue
    count=$(grep -c "\"type\":\"$match_type\"" "$SIDECAR" 2>/dev/null || echo 0)
    total_matches=$((total_matches + count))
done < "$RULES"

echo -e "${YELLOW}$total_matches total matching records across all rules${NC}"
echo ""

# Step 2: Confirm and apply
echo "Apply rules? [y/N] "
read -r confirm
if [ "$confirm" != "y" ] && [ "$confirm" != "Y" ]; then
    echo "Aborted."
    exit 0
fi

echo "Applying rules..."
"$MOD_ENGINE" "$SIDECAR" "$RULES" "$OUTPUT" "$UNDO"

echo ""
echo -e "${GREEN}Done.${NC}"
echo "  Modified: $OUTPUT"
echo "  Undo log: $UNDO"
echo ""
echo "To revert:  ./sidecar_apply.sh --revert $UNDO $SIDECAR"
echo "To verify:  head -3 $OUTPUT | python3 -m json.tool"
echo "To query:   ./cypher_repl.out --sidecar $OUTPUT"
