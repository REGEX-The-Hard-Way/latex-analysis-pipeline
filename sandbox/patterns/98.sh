#!/bin/bash
# Pattern 98: ASM Direct Backend — shell script generating ragel ASM output
set -e
D=$(mktemp -d)
cd "$D"

cat > asmtest.rl << 'EOF'
#include <stdio.h>
%%{ machine asmtest;
  main := ( 'GET' | 'PUT' | 'DEL' ) ' /' [a-zA-Z0-9/_]+ '\n';
  write data;
}%%
int main(void){int cs;const char *p,*pe,*eof;
  p="GET /index.html\n";pe=p+strlen(p);eof=pe;
  %% write init;
  %% write exec;
  printf("  -> %s\n",cs>=asmtest_first_final?"ACCEPT":"FAIL");
  return 0;
}
EOF

echo "=== Pattern 98: ASM Direct Backend (-G2) ==="
echo ""

echo "Generating x86-64 assembly with ragel-asm..."
ragel-asm -G2 asmtest.rl -o asmtest.s 2>/dev/null || {
  echo "ragel-asm not available, using ragel -G2..."
  ragel -G2 asmtest.rl -o asmtest.c 2>/dev/null && echo "Generated C+goto instead."
}
LINES=$(wc -l < asmtest.s 2>/dev/null || echo 0)
echo "Generated $LINES lines."

if [ "$LINES" -gt 5 ]; then
  echo ""
  echo "First 30 lines:"
  head -30 asmtest.s
  echo ""
  echo "Assembly stats:"
  echo "  Labels: $(grep -c ':$' asmtest.s || echo 0)"
  echo "  Jump instructions: $(grep -c -E 'jmp|j[a-z]+' asmtest.s || echo 0)"
else
  echo "  (minimal or no ASM output — ragel-asm may need separate binary)"
fi

echo ""
echo "To assemble and run (Linux x86-64):"
echo "  as asmtest.s -o asmtest.o && ld asmtest.o -o asmtest && ./asmtest"

cd /
rm -rf "$D"
