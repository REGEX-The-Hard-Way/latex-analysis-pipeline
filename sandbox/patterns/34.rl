/* Pattern 34: Scanner Leave — % actions fire before => in scanner */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p34;
  main := |*
    'hello' %{printf("(leave_hello) ");} => { printf("TOK:hello\n"); };
    [a-z]+ %{printf("(leave_word) ");} => { printf("TOK:word\n"); };
    [ \t\n]+;
  *|;
  write data; }%%
int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
printf("Pattern 34: Scanner Leave %% Actions\n\n");
char buf[]="hello world\n";
p=buf;pe=p+strlen(buf);eof=pe;
%% write init;
%% write exec;
return 0;}
