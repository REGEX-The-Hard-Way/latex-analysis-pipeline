/* Pattern 25: Action-Only Patterns — zlen >action fires without consuming input */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p25;
  main := 'hello' zlen >{printf("MARKER ");} '\n';
  write data; }%%
int main(void){int cs;const char *p,*pe,*eof;
printf("Pattern 25: Action-Only Patterns\n\n");
p="hello\n";pe=p+6;eof=pe;
%% write init;
%% write exec;
printf("\n");
return 0;}
