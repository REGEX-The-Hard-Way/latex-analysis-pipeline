#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p10; main := (any** :>> 'END' '\n' | any* 'END' '\n'); write data; }%%
int main(void){static const char *t[]={"END\n","abcEND\n","abcEND\n","\n"};static const int e[]={1,1,1,0};int n=4,cs;const char *p,*pe,*eof;
printf("Pattern 10: Greedy Star\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p10_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
