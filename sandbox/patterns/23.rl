/* Pattern 23: Named Patterns — hexdigit/letter patterns combined */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p23;
  hexdig = [0-9a-fA-F];
  letter = [a-zA-Z];
  ident = letter (letter|digit)*;
  hexnum = '0x' hexdig+;
  main := (ident | hexnum) '\n';
  write data; }%%
int main(void){int cs;const char *p,*pe,*eof;
printf("Pattern 23: Named Patterns\n\n");
p="myVar\n";pe=p+6;eof=pe;
%% write init;
%% write exec;
printf("  myVar -> %s\n",cs>=p23_first_final?"ACCEPT":"FAIL");
return 0;}
