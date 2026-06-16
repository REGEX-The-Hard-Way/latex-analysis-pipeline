#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p49; main := '(' @{n++;} ( '(' @{n++;} | ')' when {n>0} @{n--;} | [a-z] )* ')' when {n==1} @{n--;} '\n'; write data; }%%
int main(void){int cs,n;const char *p,*pe,*eof;
static const char *t[]={"()\n","(a)\n","(a(b)c)\n","((a)(b))\n","(()\n","())\n"};
static const int e[]={1,1,1,1,0,0};int n2=6;
printf("Pattern 49: Condition Balancer\n\n");
for(int i=0;i<n2;i++){p=t[i];pe=p+strlen(p);eof=pe;n=0;
%% write init;
%% write exec;
int r=cs>=p49_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n2);return 0;}
