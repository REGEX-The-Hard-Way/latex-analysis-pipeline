#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p44; action recover { if(++ercount<4){fexec save;fgoto main;} } main := 'hello' @err(recover) '\n'; write data; }%%
int main(void){int cs,ercount;const char *p,*pe,*eof;const char *save;
static const char *t[]={"hello\n","helo\n"};static const int e[]={1,0};int n=2;
printf("Pattern 44: fexec Instruction\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;save=p;ercount=0;
%% write init;
%% write exec;
int r=cs>=p44_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
