/* Pattern 40: fnext*return_to — ftargs captures return context for non-local return */
#include <stdio.h>
#include <string.h>
#include <assert.h>
int return_to;
%%{ machine p40;
  one := 'one' @{printf("ONE "); fnext *return_to;};
  two := 'two' @{printf("TWO "); fnext *return_to;};
  main := ( '1' @{return_to = ftargs; fnext one;} | '2' @{return_to = ftargs; fnext two;} | '\n' )*;
  write data; }%%
int main(void){int cs,stack[8],top=0;const char *p,*pe,*eof;
printf("Pattern 40: fnext with ftargs Return Context\n\n");
p="1one2two\n";pe=p+9;eof=pe;
%% write init;
%% write exec;
printf("-> %s\n",cs>=p40_first_final?"ACCEPT":"FAIL");
return 0;}
