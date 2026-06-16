#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p3; main := [a-zA-Z_][a-zA-Z0-9_]* '\n'; write data; }%%
int main(void){static const char *t[]={"abc\n","_foo\n","X123\n","_123\n","123\n","\n"};static const int e[]={1,1,1,1,0,0};int n=6,cs;const char *p,*pe,*eof;
printf("Pattern  3: Char Class\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p3_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
