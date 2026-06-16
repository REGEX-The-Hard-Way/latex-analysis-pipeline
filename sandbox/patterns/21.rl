/* Pattern 21: Action Chaining — >begin @accum %finish on word parser */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p21;
  word = [a-zA-Z]+ >{printf("B ");} @{printf("%c",fc);} %{printf(" E");};
  main := word '\n';
  write data; }%%
int main(void){int cs;const char *p,*pe,*eof;
printf("Pattern 21: Action Chaining\n\n");
p="hello\n";pe=p+6;eof=pe;
%% write init;
%% write exec;
printf("\n");
return 0;}
