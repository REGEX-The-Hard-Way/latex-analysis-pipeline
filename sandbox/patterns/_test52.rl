#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p52; action init { c = 0; } action inc { c++; } action min { c >= 1 } action max { c < 2 } main := :condstar( digit, init, inc, min, max ): '\n'; write data; }%%
int main(void){int cs,c;const char *p,*pe,*eof; setbuf(stdout,NULL);
static const char *t[]={"\n","1\n","12\n"};static const int e[]={0,1,0};int n=3;
printf("52 final\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p52_first_final;printf("  %d '%s' -> %s ok\n",i,t[i],r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("done\n");return 0;}
