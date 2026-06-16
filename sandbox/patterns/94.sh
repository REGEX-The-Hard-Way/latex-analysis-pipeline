#!/bin/bash
# Pattern 94: Graphviz DOT — shell script: ragel -V -p to generate DOT
set -e
D=$(mktemp -d)
cd "$D"

cat > vistest.rl << 'EOF'
%%{ machine vistest;
  http := ( 'http' 's'? '://' ) >{printf("protocol\n");};
  host := ( [a-zA-Z0-9._-]+ ) >{printf("host\n");};
  main := http host '\n';
  write data;
}%%
EOF

echo "=== Pattern 94: Graphviz DOT (-V -p) ==="
echo ""

echo "Generating Graphviz DOT with ragel -V -p..."
ragel -V -p vistest.rl -o vistest.dot 2>/dev/null
echo ""

echo "DOT file contents:"
echo "---8<---"
cat vistest.dot
echo "---8<---"
echo ""

echo "Graph info:"
grep -c 'node\|edge\|shape=\|fontname' vistest.dot 2>/dev/null || true
echo ""

echo "Transitions: $(grep -c '\->' vistest.dot)"
echo "Nodes: $(grep -c '\[shape=' vistest.dot)"

if command -v dot &>/dev/null; then
  echo "Running dot to produce SVG..."
  dot -Tsvg vistest.dot -o vistest.svg 2>/dev/null && echo "  -> vistest.svg created" || echo "  dot failed"
else
  echo "(graphviz 'dot' not installed — SVG not generated)"
fi

cd /
rm -rf "$D"
