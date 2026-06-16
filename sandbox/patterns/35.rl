/* Pattern 35: Scanner Token Boundaries — $~ (from-start) $* (to-start) */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p35;
  main := |*
    'hello' $~{printf("FS ");} $*{printf("TS ");} => { printf("TOK\n"); };
    [a-z]+ => { printf("TOK\n"); };
    [ \t\n]+;
  *|;
  write data; }%%
int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
printf("Pattern 35: Scanner Token Boundaries ($~ $*)\n\n");
char buf[]="hello abc\n";
p=buf;pe=p+strlen(buf);eof=pe;
%% write init;
%% write exec;
return 0;}
