/* Pattern 30: Scanner Tokenization — |* patterns with => actions */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p30;
  main := |*
    [a-zA-Z_][a-zA-Z0-9_]* => { printf("IDENT:%.*s ",(int)(te-ts),ts); };
    digit+ => { printf("NUM:%.*s ",(int)(te-ts),ts); };
    [+\-*/] => { printf("SYM:%c ",*ts); };
    [ \t\n]+;
  *|;
  write data; }%%
int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
printf("Pattern 30: Scanner Tokenization\n\n");
char buf[]="foo + 42 bar\n";
p=buf;pe=p+strlen(buf);eof=pe;
%% write init;
%% write exec;
printf("\n");
return 0;}
