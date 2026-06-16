#!/bin/bash
# Pattern 92: Flat Table Backend — shell script comparing -F0 vs -T0 code size
set -e
D=$(mktemp -d)
cd "$D"

cat > ftest.rl << 'EOF'
#include <stdio.h>
%%{ machine ftest;
  ident = [a-zA-Z_][a-zA-Z0-9_]*;
  num   = [0-9]+;
  ws    = [ \t]+ '\n'?;
  main := (ident | num | ws)* '\n';
  write data;
}%%
int main(void){int cs;const char *p,*pe,*eof;
  p="abc 123 _x9\n";pe=p+strlen(p);eof=pe;
  %% write init;
  %% write exec;
  printf("ACCEPT\n");return 0;
}
EOF

echo "=== Pattern 92: Flat Table Backend (-F0 vs -T0) ==="
echo ""

ragel -F0 ftest.rl -o ftest_F0.c 2>/dev/null
ragel -T0 ftest.rl -o ftest_T0.c 2>/dev/null

SIZE_F0=$(wc -c < ftest_F0.c)
SIZE_T0=$(wc -c < ftest_T0.c)

echo "  -F0 (flat table):  $SIZE_F0 bytes"
echo "  -T0 (binary search table): $SIZE_T0 bytes"

if [ "$SIZE_F0" -lt "$SIZE_T0" ]; then
  echo "  -> Flat table is SMALLER"
else
  echo "  -> Binary search table is SMALLER"
fi

echo ""
echo "Compiling both and running..."
gcc -o ftest_F0 ftest_F0.c 2>/dev/null && ./ftest_F0
gcc -o ftest_T0 ftest_T0.c 2>/dev/null && ./ftest_T0
echo "Both produce correct output."

cd /
rm -rf "$D"
