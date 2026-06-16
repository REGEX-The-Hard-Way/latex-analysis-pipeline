#!/bin/bash
# Pattern 95: FSM Stats — shell script: ragel -s on various machines
set -e
D=$(mktemp -d)
cd "$D"

cat > ident.rl << 'EOF'
%%{ machine ident;
  ident = [a-zA-Z_][a-zA-Z0-9_]*;
  main := ident '\n';
  write data;
}%%
EOF

cat > comment.rl << 'EOF'
%%{ machine comment;
  comment = '/*' (any-'\*')* '*/';
  main := comment;
  write data;
}%%
EOF

cat > url.rl << 'EOF'
%%{ machine url;
  scheme = [a-z]+ '://';
  host   = [a-zA-Z0-9._\-]+;
  path   = '/' [a-zA-Z0-9/._\-]*;
  main := scheme host path? '\n';
  write data;
}%%
EOF

cat > ip.rl << 'EOF'
%%{ machine ip;
  octet = digit{1,3};
  main := octet '.' octet '.' octet '.' octet '\n';
  write data;
}%%
EOF

echo "=== Pattern 95: FSM Stats (-s) ==="
echo ""

for m in ident comment url ip; do
  echo "Machine: $m"
  ragel -s "${m}.rl" 2>&1 | grep -E 'fsm-states|trans_keys|table-data'
  echo ""
done

echo "All stats generated."
cd /
rm -rf "$D"
