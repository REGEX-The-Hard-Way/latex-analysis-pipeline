#!/bin/bash
# Pattern 100: Build Integration — complete ragel -> gcc -> run workflow
set -e
D=$(mktemp -d)
cd "$D"

cat > integtest.rl << 'EOF'
%%{ machine integtest;
  method = 'GET' | 'POST' | 'PUT' | 'DELETE';
  slash  = '/';
  pchar  = [a-zA-Z0-9/._\-]*;
  ver    = 'HTTP/1.' [01];
  main  := method ' ' slash pchar ' ' ver '\n';
  write data;
}%%
EOF

cat > main.c << 'EOF'
#include <stdio.h>
#include <string.h>
#include <assert.h>

int run_test(const char *input) {
  int cs; const char *p, *pe, *eof;
  p = input; pe = p + strlen(p); eof = pe;
  #include "parsed.c"
  return cs >= integtest_first_final;
}

int main(void) {
  struct { const char *s; int ok; } t[] = {
    {"GET /index.html HTTP/1.1\n", 1},
    {"POST /api/v1/users HTTP/1.0\n", 1},
    {"PUT /data.json HTTP/1.1\n", 1},
    {"DELETE /tmp/x HTTP/1.1\n", 1},
    {"INVALID /x HTTP/1.1\n", 0},
    {"GET /path HTTP/2.0\n", 0},
  };
  int n = 6;

  printf("Pattern 100: Build Integration\n\n");
  printf("Build workflow: ragel .rl -> .c -> gcc -> executable -> run\n\n");

  for (int i = 0; i < n; i++) {
    int r = run_test(t[i].s);
    printf("  %-35s -> %s (expect %s) %s\n",
           t[i].s, r ? "ACCEPT" : "FAIL",
           t[i].ok ? "ACCEPT" : "FAIL",
           r == t[i].ok ? "ok" : "FAIL");
    assert(r == t[i].ok);
  }
  printf("\nAll %d tests passed.\n", n);
  return 0;
}
EOF

echo "=== Pattern 100: Build Integration ==="
echo ""

echo "# Step 1: Generate C source from Ragel (.rl -> .c)"
echo "$ ragel integtest.rl -o parsed.c"
ragel integtest.rl -o parsed.c
echo "  -> parsed.c: $(wc -l < parsed.c) lines, $(wc -c < parsed.c) bytes"
echo ""

echo "# Step 2: Compile (gcc .c -> elf)"
echo "$ gcc -o integtest main.c"
gcc -o integtest main.c
echo "  -> integtest: $(wc -c < integtest) bytes"
echo ""

echo "# Step 3: Run tests"
echo "$ ./integtest"
./integtest

cd /
rm -rf "$D"
