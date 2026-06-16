#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p48; action setcnt { cnt = fc - '0'; } action cntgt0 { cnt > 0 } action dec { cnt--; } main := digit @setcnt (digit when cntgt0 @dec)* outwhen !cntgt0 '\n'; write data; }%%
int main(void){int cs,cnt;const char *p,*pe,*eof;
static const char *t[]={"3123\n","21a\n","11\n","0\n"};static const int e[]={1,0,1,1};int n=4;
printf("Pattern 48: outwhen Condition\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p48_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
