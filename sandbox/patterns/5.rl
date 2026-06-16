#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p5; main := (any '\n' | any* '\n' | [^\n]* '\n'); write data; }%%
int main(void){static const char *t[]={"A\n","hello world\n","\n","",".\n"};static const int e[]={1,1,1,0,1};int n=5,cs;const char *p,*pe,*eof;
printf("Pattern  5: Dot/Any\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p5_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
