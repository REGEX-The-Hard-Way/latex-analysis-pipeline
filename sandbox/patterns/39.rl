/* Pattern 39: fgoto/fnext — error skip with garble_line, resume with fgoto main */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p39;
  garble_line := [^\n]* '\n' @{printf("SKIP "); fgoto main;};
  main := 'GOOD' @{printf("OK\n");} $!{fhold;fgoto garble_line;} '\n';
  write data; }%%
int main(void){int cs,stack[8],top=0;const char *p,*pe,*eof;
printf("Pattern 39: fgoto/fnext Error Recovery\n\n");
p="BAD\nGOOD\n";pe=p+9;eof=pe;
%% write init;
%% write exec;
printf("done\n");
return 0;}
