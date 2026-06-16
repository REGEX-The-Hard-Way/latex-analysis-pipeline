#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p50; main := digit when { c++ < 5 } (digit when { c++ < 5 })* '\n'; write data; }%%
int main(void){int cs,c;const char *p,*pe,*eof;
static const char *t[]={"1234\n","12345\n","1234567\n"};static const int e[]={1,1,0};int n=3;
printf("Pattern 50: Inline Conditions\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;c=0;
%% write init;
%% write exec;
int r=cs>=p50_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
