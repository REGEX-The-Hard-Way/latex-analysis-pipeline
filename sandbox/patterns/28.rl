/* Pattern 28: Error Recovery — $! fhold+fgoto garble_line to skip bad lines */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p28;
  garble_line := [^\n]* '\n' @{printf("SKIP "); fgoto main;};
  main := 'hello' @{printf("OK\n");} $!{fhold;fgoto garble_line;} '\n';
  write data; }%%
int main(void){int cs,stack[8],top=0;const char *p,*pe,*eof;
printf("Pattern 28: Error Recovery ($!)\n\n");
p="bad\nhello\n";pe=p+11;eof=pe;
%% write init;
%% write exec;
printf("\n");
return 0;}
