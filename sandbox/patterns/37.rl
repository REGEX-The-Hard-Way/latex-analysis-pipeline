/* Pattern 37: Dynamic Dispatch — fentry()+fcall based on command char */
#include <stdio.h>
#include <string.h>
#include <assert.h>
char comm;
%%{ machine p37;
  action dispatch {
    if (comm == 'a') fcall alpha_comm;
    else if (comm == 'd') fcall digit_comm;
  }
  alpha_comm := 'ALPHA' @{printf("ALPHA ");} any @{fhold;fret;};
  digit_comm := 'DIGIT' @{printf("DIGIT ");} any @{fhold;fret;};
  cmd = [ad] @{comm=fc;} ' ' @dispatch;
  main := cmd '\n';
  write data; }%%
int main(void){int cs,stack[8],top=0;const char *p,*pe,*eof;
printf("Pattern 37: Dynamic Dispatch (fentry+fcall)\n\n");
p="a ALPHA\n";pe=p+8;eof=pe;
%% write init;
%% write exec;
printf("-> %s\n",cs>=p37_first_final?"ACCEPT":"FAIL");
return 0;}
