#include <assert.h>
#include <stdio.h>
#include <string.h>
%%{
  machine p2;
  main:= ('hello' | 'world' | 'foo' | 'bar')'\n';
  write data;
}%%
int main(void) {
  static const char *t[] = {"hello\n", "world\n", "foo\n", "bar\n",
                            "baz\n",   "hello",   ""};
  static const int e[] = {1, 1, 1, 1, 0, 0, 0};
  int n = 7, cs;
  const char *p, *pe, *eof;
  printf("Pattern  2: Alternation\n\n");
  for (int i = 0; i < n; i++) {
    p = t[i];
    pe = p + strlen(p);
    eof = pe;
    %% write init;
    %% write exec;
    int r = cs >= p2_first_final;
    printf("  Input %d -> %s ok\n", i, r ? "ACCEPT" : "FAIL");
    assert(r == e[i]);
  }
  printf("\nAll %d tests passed.\n", n);
  return 0;
}
