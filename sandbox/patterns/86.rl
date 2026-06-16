/* Pattern 86: Nested Brace Parser — recursive { } matching with stack */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p86;
  action push { stack[depth++] = *p; }
  action pop  {
    if(depth > 0 && stack[depth-1] == '{' && *p == '}') depth--;
    else { cs = p86_error; p = pe-1; }
  }
  action check {
    if(depth > 0) cs = p86_error;
  }
  non_brace = (any-'{' - '}');
  main := ('{' @push (non_brace | '{' @push | '}' @pop)* '}' @pop) '\n' @check;
  write data;
}%%
int main(void){int cs; const char *p,*pe,*eof;
  struct {const char *s; int ok;} t[]={
    {"{}\n",1},
    {"{a{b}c}\n",1},
    {"{{{{}}}}\n",1},
    {"{x}{y}\n",1},
    {"{unclosed\n",0},
    {"}}\n",0},
    {"{a{b{c}d}e}\n",1},
    {"{a{b{c}d}e}f\n",0},
    {"}\n",0},
    {"abc\n",0},
  }; int n=10; char stack[256]; int depth;
  printf("Pattern 86: Nested Brace Parser\n\n");
  for(int i=0;i<n;i++){p=t[i].s;pe=p+strlen(p);eof=pe; depth=0;
    printf("Input: %s",t[i].s);
    %% write init;
    %% write exec;
    int r=cs>=p86_first_final && depth==0;
    printf("  -> %s (expect %s) %s\n",r?"ACCEPT":"FAIL",t[i].ok?"ACCEPT":"FAIL",r==t[i].ok?"ok":"FAIL");
    assert(r==t[i].ok);
  }
  printf("\nAll %d tests passed.\n",n);return 0;
}
