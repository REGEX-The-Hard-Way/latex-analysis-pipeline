#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p47; action c1 { flag } action one { printf("when c1 matched\n"); } main := [a-z]* '\n' when c1 @one; write data; }%%
int main(void){int cs,flag;const char *p,*pe,*eof;
static const char *t[]={"abc\n","\n"};static const int e[]={1,0};
static const int f[]={1,0};int n=2;
printf("Pattern 47: when Condition\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;flag=f[i];
%% write init;
%% write exec;
int r=cs>=p47_first_final;printf("  Input %d (%d) -> %s ok\n",i,flag,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
