/* Pattern 99: OCaml VarFeature Concept — C switch-based simulation with OCaml comments */
#include <stdio.h>
#include <string.h>
#include <assert.h>

/*
 * In Ragel OCaml backend (-O), variables like cs, p, pe, eof are passed
 * through the generated function. This C program simulates that pattern:
 *
 *   (* OCaml equivalent: *)
 *   let ragel_exec cs p pe eof =
 *     match cs with
 *     | 0 -> ... (* error *)
 *     | 1 -> ... (* start *)
 *     | 2 -> ... (* match *) ...
 */

typedef enum { S_ERROR, S_START, S_A, S_B, S_ACCEPT } State;

static State step(State cs, char c) {
  switch (cs) {
    case S_START:
      if (c == 'a') return S_A;
      if (c == 'b') return S_B;
      return S_ERROR;
    case S_A:
      if (c == 'a') return S_A;
      if (c == 'b') return S_ACCEPT;
      return S_ERROR;
    case S_B:
      if (c == 'b') return S_B;
      if (c == 'a') return S_ACCEPT;
      return S_ERROR;
    case S_ACCEPT:
      return S_ERROR;
    default: return S_ERROR;
  }
}

int main(void) {
  printf("Pattern 99: OCaml VarFeature Concept\n\n");
  printf("Simulating Ragel OCaml generator pattern.\n");
  printf("OCaml backend uses match/cs pattern instead of goto.\n\n");

  struct { const char *s; int ok; } t[] = {
    {"ab", 1}, {"ba", 1}, {"aaab", 1},
    {"abc", 0}, {"", 0}, {"aba", 0},
  };
  int n = 6;

  for (int i = 0; i < n; i++) {
    State cs = S_START;
    const char *p = t[i].s, *pe = p + strlen(p);
    while (p < pe && cs != S_ERROR) { cs = step(cs, *p); p++; }
    int r = (cs == S_ACCEPT && p == pe);
    printf("  \"%s\" -> %s (expect %s) %s\n",
           t[i].s, r ? "ACCEPT" : "FAIL",
           t[i].ok ? "ACCEPT" : "FAIL",
           r == t[i].ok ? "ok" : "FAIL");
    assert(r == t[i].ok);
  }
  printf("\nAll %d tests passed.\n", n);
  return 0;
}
