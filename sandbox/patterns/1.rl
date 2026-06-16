#include <assert.h>
#include <stdio.h>
#include <string.h>
%%{
  machine p1;
  main:= 'hello' '\n';
  write data;
}%% 

int main(void) {
  static const char *t[] = {"hello\n", "hello"};
  static const int e[] = {1, 0};
  int n = 2, cs;
  const char *p, *pe, *eof;
  printf("Pattern  1: Basic Literal\n\n");
  for (int i = 0; i < n; i++) {
    p = t[i];
    pe = p + strlen(p);
    eof = pe;
    %% write init;
    %% write exec;
    int r = cs >= p1_first_final;
    printf("  Input %d -> %s ok\n", i, r ? "ACCEPT" : "FAIL");
    assert(r == e[i]);
  }
  printf("\nAll %d tests passed.\n", n);
  return 0;
}
