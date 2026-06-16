/* Pattern 38: fncall/fnret — named call/return dispatch */
#include <stdio.h>
#include <string.h>
#include <assert.h>
char comm;
%%{ machine p38;
  action dispatch {
    if (comm == 'a') fncall alpha_comm;
    else if (comm == 'd') fncall digit_comm;
  }
  alpha_comm := 'ALPHA' @{printf("ALPHA ");} any @{fhold;fnret;};
  digit_comm := 'DIGIT' @{printf("DIGIT ");} any @{fhold;fnret;};
  cmd = [ad] @{comm=fc;} ' ' @dispatch;
  main := cmd '\n';
  write data; }%%
int main(void){int cs,stack[8],top=0;const char *p,*pe,*eof;
printf("Pattern 38: fncall/fnret\n\n");
p="d DIGIT\n";pe=p+8;eof=pe;
%% write init;
%% write exec;
printf("-> %s\n",cs>=p38_first_final?"ACCEPT":"FAIL");
return 0;}
