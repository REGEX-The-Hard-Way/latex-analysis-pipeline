#!/bin/bash
# Pattern 96: Minimization Levels — shell script comparing -n/-m/-l/-e state counts
set -e
D=$(mktemp -d)
cd "$D"

cat > mintest.rl << 'EOF'
%%{ machine mintest;
  v4 = 'IPv4:' digit+ '.' digit+ '.' digit+ '.' digit+ '\n';
  v6 = 'IPv6:' xdigit+ (':' xdigit+)+ '\n';
  mac = 'MAC:' xdigit{2} (':' xdigit{2}){5} '\n';
  addr = 'addr:' any+ '\n';
  main := v4 | v6 | mac | addr;
  write data;
}%%
EOF

echo "=== Pattern 96: Minimization Levels (-n/-m/-l/-e) ==="
echo ""

for lvl in -n -m -l -e; do
  echo "Level $lvl (none/minimize/exact_lookup/exact):"
  ragel -s $lvl mintest.rl 2>&1 | grep -E 'fsm-states|key_offsets|table-data'
  echo ""
done

echo "Higher minimization = fewer states, longer compile time."
cd /
rm -rf "$D"
