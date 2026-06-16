/* Pattern 24: Epsilon Transitions — fnext/fgoto jumps between named states */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p24;
  sub := 'b';
  action eps_a { fgoto sub; }
  main := 'a' @eps_a;
  write data; }%%
int main(void){int cs,stack[8],top=0;const char *p,*pe,*eof;
printf("Pattern 24: Epsilon Transitions\n\n");
p="ab";pe=p+2;eof=pe;
%% write init;
%% write exec;
printf("  ab -> %s\n",cs>=p24_first_final?"ACCEPT":"FAIL");
return 0;}
