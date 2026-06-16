#!/bin/bash
# Pattern 93: Binary Search Backend — demonstrating -T0 default backend
set -e
D=$(mktemp -d)
cd "$D"

cat > bintest.rl << 'EOF'
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine bintest;
  action classify {
    const char *types[] = {"alpha","digit","punct","space","nl"};
    const char *tp = (p[-1]>='a'&&p[-1]<='z')?"alpha":
                     (p[-1]>='0'&&p[-1]<='9')?"digit":
                     (p[-1]=='\n')?"nl":"punct";
    printf("  '%c' -> %s\n", p[-1], tp);
  }
  main := any+ >{printf("Classifying chars:\n");} $classify;
  write data;
}%%
int main(void){int cs;const char *p,*pe,*eof;
  const char *input="a9!\n";
  p=input;pe=p+strlen(input);eof=pe;
  %% write init;
  %% write exec;
  printf("\n  -> %s\n",cs>=bintest_first_final?"ACCEPT":"FAIL");
  return 0;
}
EOF

echo "=== Pattern 93: Binary Search Backend (-T0 default) ==="
echo ""
echo "-T0 is the default backend (binary search on transitions)."
echo "It generates compact tables good for machines with many transitions."
echo ""

ragel -T0 bintest.rl -o bintest.c
echo "Generated code lines: $(wc -l < bintest.c)"
gcc -o bintest bintest.c
./bintest

cd /
rm -rf "$D"
