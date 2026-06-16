/* Pattern 31: Silent Scanner — whitespace/comments consumed silently */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p31;
  main := |*
    '//' [^\n]* '\n';
    [ \t\n]+;
    [a-zA-Z]+ => { printf("WORD "); };
    digit+ => { printf("NUM "); };
  *|;
  write data; }%%
int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
printf("Pattern 31: Silent Scanner\n\n");
char buf[]="hello // comment\n 123 world\n";
p=buf;pe=p+strlen(buf);eof=pe;
%% write init;
%% write exec;
printf("\n");
return 0;}
