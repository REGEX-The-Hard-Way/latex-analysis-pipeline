#!/usr/bin/env bash
# cluster_apply.sh — interactive cluster merge via fzf.
#
# Workflow:
#   1. Select a token type to cluster (e.g., "label", "author", "section")
#   2. Find clusters via fingerprint or Levenshtein
#   3. fzf preview shows cluster members side-by-side
#   4. Enter selects canonical value, applies merge rules
#   5. Generates undo log for reversibility
#
# Usage: ./cluster_apply.sh sidecar.json [type]

set -euo pipefail

SIDECAR="${1:-../scanner/sidecar.json}"
FTYPE="${2:-}"
CLUSTER_BIN="./cluster_find"
MOD_BIN="./sidecar_mod"
CLUSTER_FILE="/tmp/clusters_$$.json"
MERGE_FILE="/tmp/merge_rules_$$.txt"
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

if [ ! -f "$SIDECAR" ]; then
    echo "Usage: $0 sidecar.json [type]"
    exit 1
fi

# Build tools if needed
[ -f "$CLUSTER_BIN" ] || gcc -O2 -o "$CLUSTER_BIN" cluster_find.c
[ -f "$MOD_BIN" ] || gcc -O2 -o "$MOD_BIN" sidecar_mod.c

# List available types
if [ -z "$FTYPE" ]; then
    echo -e "${YELLOW}Available token types:${NC}"
    grep -oP '"type":"\K[^"]+' "$SIDECAR" | sort | uniq -c | sort -rn | head -20
    echo ""
    echo "Usage: $0 $SIDECAR <type>"
    exit 0
fi

echo -e "${GREEN}=== Cluster Merge: $FTYPE ===${NC}"
echo ""

# Step 1: Find clusters
echo "Finding clusters..."
"$CLUSTER_BIN" "$SIDECAR" "$FTYPE" > "$CLUSTER_FILE" 2>/dev/null || true

total=$(wc -l < "$CLUSTER_FILE" 2>/dev/null || echo 0)
if [ "$total" -eq 0 ]; then
    # Try Levenshtein
    echo "  No fingerprint clusters found. Trying Levenshtein (threshold=3)..."
    "$CLUSTER_BIN" "$SIDECAR" "$FTYPE" --levenshtein 3 > "$CLUSTER_FILE" 2>/dev/null || true
    total=$(wc -l < "$CLUSTER_FILE" 2>/dev/null || echo 0)
fi

echo "  Found $total clusters with >1 member"
echo ""
[ "$total" -eq 0 ] && { echo "Nothing to merge."; exit 0; }

# Step 2: Interactive fzf selection
echo -e "${YELLOW}Browse clusters (Enter to select, Esc to skip):${NC}"
echo ""

# Build fzf input: one line per cluster with preview
selected=$(
while IFS= read -r line; do
    canonical=$(echo "$line" | python3 -c "import json,sys; print(json.load(sys.stdin)['canonical'])" 2>/dev/null)
    count=$(echo "$line" | python3 -c "import json,sys; print(json.load(sys.stdin)['count'])" 2>/dev/null)
    algo=$(echo "$line" | python3 -c "import json,sys; print(json.load(sys.stdin)['algorithm'])" 2>/dev/null)
    echo "$canonical|$count|$algo|$line"
done < "$CLUSTER_FILE" | \
fzf --delimiter='|' \
    --preview 'echo {} | cut -d"|" -f4 | python3 -c "
import json,sys
c=json.load(sys.stdin)
print(f\"Canonical: {c[\"canonical\"]}\")
print(f\"Members ({c[\"count\"]}):\")
for m in c[\"members\"]:
    print(f\"  -> {m}\")
"' \
    --preview-window 'down,40%' \
    --header "${FTYPE} clusters (select to merge)" \
    --bind 'enter:execute(echo {} | cut -d"|" -f4 > /tmp/cluster_selected.json)+abort' \
    2>/dev/null
) || true

if [ ! -f /tmp/cluster_selected.json ]; then
    echo "Nothing selected."
    rm -f "$CLUSTER_FILE"
    exit 0
fi

# Step 3: Confirm and apply
echo ""
canonical=$(python3 -c "import json; print(json.load(open('/tmp/cluster_selected.json'))['canonical'])" 2>/dev/null || echo "?")
count=$(python3 -c "import json; print(json.load(open('/tmp/cluster_selected.json'))['count'])" 2>/dev/null || echo "?")
echo -e "${GREEN}Merge $count values → \"$canonical\"?${NC} [y/N] "
read -r confirm
if [ "$confirm" != "y" ] && [ "$confirm" != "Y" ]; then
    echo "Skipped."
    rm -f "$CLUSTER_FILE" /tmp/cluster_selected.json
    exit 0
fi

# Step 4: Build merge rules and apply
python3 -c "
import json
c = json.load(open('/tmp/cluster_selected.json'))
canonical = c['canonical']
with open('$MERGE_FILE', 'w') as f:
    for i, m in enumerate(c['members']):
        if m != canonical:
            # Generate regex to match exact text and replace with canonical
            import re
            escaped_val = re.escape(m)
            f.write(json.dumps({
                'step': i+1,
                'name': f'merge cluster {i+1}',
                'match_type': '$FTYPE',
                'prop': 'text',
                'transform': 'regex',
                'pattern': f'^{escaped_val}$',
                'replace': canonical
            }) + '\n')
"

OUTPUT="${SIDECAR%.json}.merged.json"
UNDO="${SIDECAR%.json}.merged.undo.log"

"$MOD_BIN" "$SIDECAR" "$MERGE_FILE" "$OUTPUT" "$UNDO"

echo ""
echo -e "${GREEN}Done.${NC}"
echo "  Modified: $OUTPUT"
echo "  Undo log: $UNDO"

rm -f "$CLUSTER_FILE" /tmp/cluster_selected.json "$MERGE_FILE"
