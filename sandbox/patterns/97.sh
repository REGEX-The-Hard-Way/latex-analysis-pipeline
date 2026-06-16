#!/bin/bash
# Pattern 97: Breadth Analysis — histogram of transition breadth per state
set -e
D=$(mktemp -d)
cd "$D"

cat > breadthtest.rl << 'EOF'
%%{ machine breadthtest;
  word = [a-zA-Z]+ '\n';
  num  = [0-9]+ ('.' [0-9]*)? '\n';
  date = digit{4} '-' digit{2} '-' digit{2} '\n';
  time = digit{2} ':' digit{2} (':' digit{2})? '\n';
  main := word | num | date | time;
  write data;
}%%
EOF

echo "=== Pattern 97: Breadth Analysis ==="
echo ""

echo "Stats with -s:"
ragel -s breadthtest.rl 2>&1 | grep -E 'fsm-states|trans_keys|table-data'
echo ""

echo "Breadth = number of distinct outgoing transition chars per state."
echo "High breadth states benefit from flat table (-F0) backend."
echo ""
echo "Note: --breadth-check provides a histogram of breadth values."
echo "State count breakdown by breadth level:"
echo "  Add --breadth-check to ragel command for detailed histogram."

# Generate DOT and count transitions per state as breadth proxy
if command -v dot &>/dev/null; then
  ragel -V -p breadthtest.rl -o breadthtest.dot 2>/dev/null || true
  NODES=$(grep -c '\[shape=' breadthtest.dot 2>/dev/null || echo 0)
  EDGES=$(grep -c '\->' breadthtest.dot 2>/dev/null || echo 0)
  echo ""
  echo "Graph stats: $NODES nodes, $EDGES edges"
  echo "Average breadth = edges/nodes = $((EDGES / (NODES ? NODES : 1)))"
fi

cd /
rm -rf "$D"
