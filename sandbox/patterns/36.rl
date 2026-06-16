/* Pattern 36: fcall/fret — parity-based call with fhold+fret */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p36;
  action check {
    int d = fc - '0';
    if (d & 1) fcall odd;
    else fcall even;
  }
  even := 'even' @{printf("EVEN ");} any @{fhold;fret;};
  odd := 'odd' @{printf("ODD ");} any @{fhold;fret;};
  main := digit+ ' ' @check '\n';
  write data; }%%
int main(void){int cs,stack[8],top=0;const char *p,*pe,*eof;
printf("Pattern 36: fcall/fret\n\n");
p="2 even\n";pe=p+7;eof=pe;
%% write init;
%% write exec;
printf("-> %s\n",cs>=p36_first_final?"ACCEPT":"FAIL");
return 0;}
