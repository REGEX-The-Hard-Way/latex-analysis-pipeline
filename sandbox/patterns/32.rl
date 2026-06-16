/* Pattern 32: Scanner Priority — keyword before identifier (longest match) */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p32;
  main := |*
    'while' => { printf("KW:while "); };
    'if' => { printf("KW:if "); };
    [a-zA-Z][a-zA-Z0-9_]* => { printf("ID:%.*s ",(int)(te-ts),ts); };
    digit+ => { printf("NUM:%.*s ",(int)(te-ts),ts); };
    [ \t\n]+;
  *|;
  write data; }%%
int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
printf("Pattern 32: Scanner Priority\n\n");
char buf[]="while if while1 foo\n";
p=buf;pe=p+strlen(buf);eof=pe;
%% write init;
%% write exec;
printf("\n");
return 0;}
