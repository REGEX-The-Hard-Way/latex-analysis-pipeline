/* Pattern 88: Multi-Language Demo — C version showing Ruby/OCaml/ASM equivalents */
/* Ruby:   %% write data;  -> same directive, generates Ruby code
 * OCaml:  %% write data;  -> generates OCaml code
 * ASM:    %% write data;  -> ragel -G2 gives x86-64 ASM output
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p88;
  action echo { printf("  char: %c\n", *p); }
  main := ( [a-z] @echo | [0-9] @echo | '!' @echo )+ '\n';
  write data;
}%%
int main(void){int cs; const char *p,*pe,*eof;
  struct {const char *s; int ok;} t[]={
    {"abc123!\n",1},
    {"hello987world!\n",1},
    {"ABC\n",0},
    {"\n",0},
  }; int n=4;
  printf("Pattern 88: Multi-Language Demo\n\n");
  printf("C version compiled. Ruby/OCaml/ASM would use same .rl with:\n");
  printf("  Ruby: ragel -R input.rl\n");
  printf("  OCaml: ragel -O input.rl\n");
  printf("  ASM x86-64: ragel -G2 input.rl\n\n");
  for(int i=0;i<n;i++){p=t[i].s;pe=p+strlen(p);eof=pe;
    printf("Input: %s",t[i].s);
    %% write init;
    %% write exec;
    int r=cs>=p88_first_final;
    printf("  -> %s (expect %s) %s\n",r?"ACCEPT":"FAIL",t[i].ok?"ACCEPT":"FAIL",r==t[i].ok?"ok":"FAIL");
    assert(r==t[i].ok);
  }
  printf("\nAll %d tests passed.\n",n);return 0;
}
