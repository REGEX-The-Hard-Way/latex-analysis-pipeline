#!/bin/bash
# Pattern 91: GCC Computed Goto — shell script creating+running ragel -G2 example
set -e
D=$(mktemp -d)
cd "$D"

cat > g2test.rl << 'EOF'
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine g2test; main := ( 'foo' | 'bar' | 'baz' ) '\n'; write data; }%%
int main(void){int cs;const char *p,*pe,*eof;
  const char *t[]={"foo\n","bar\n","baz\n","bad\n",""};
  int e[]={1,1,1,0,0},n=5;
  printf("GCC Computed Goto (-G2) Demo\n\n");
  for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
    %% write init;
    %% write exec;
    int r=cs>=g2test_first_final;
    printf("  %-6s -> %s (expect %s) %s\n",t[i],r?"ACCEPT":"FAIL",e[i]?"ACCEPT":"FAIL",r==e[i]?"ok":"FAIL");
    assert(r==e[i]);
  }
  printf("\nAll tests passed.\n");return 0;
}
EOF

echo "=== Pattern 91: GCC Computed Goto (-G2) ==="
echo ""
echo "Generating code with -G2 (computed goto backend)..."
ragel -G2 g2test.rl -o g2test.c
echo "Compiling with gcc..."
gcc -o g2test g2test.c
echo "Running..."
./g2test

cd /
rm -rf "$D"
