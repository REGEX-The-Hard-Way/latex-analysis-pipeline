#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p41; main := '1' @{ return_to = fcurs; fgoto other; } | '\n'; other := 'other' @{ printf("other\n"); fgoto *return_to; }; write data; }%%
int main(void){int cs,return_to=0;const char *p,*pe,*eof;
static const char *t[]={"1other\n","1otherX\n"};static const int e[]={1,0};int n=2;
printf("Pattern 41: fcurs Variable\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p41_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
