/* Pattern 26: Global Error Actions — >err $err %err on 'hello' parser */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p26;
  main := ('hello' >err{printf(">ERR ");} $err{printf("$ERR ");} %err{printf("%%ERR ");}) '\n';
  write data; }%%
int main(void){int cs;const char *p,*pe,*eof;
printf("Pattern 26: Global Error Actions\n\n");
p="helx\n";pe=p+5;eof=pe;
%% write init;
%% write exec;
printf(" -> error state reached\n");
return 0;}
