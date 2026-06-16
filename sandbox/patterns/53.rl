#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p53; action init { c = 0; } action inc { c++; } action min { c >= 2 } action max { c < 2 } main := :condplus( digit, init, inc, min, max ): '\n'; write data; }%%
int main(void){int cs,c;const char *p,*pe,*eof;
static const char *t[]={"12\n","123\n","1\n","\n"};static const int e[]={1,0,0,0};int n=4;
printf("Pattern 53: :condplus: Conditional Kleene Plus\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p53_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
