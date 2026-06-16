#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p15;
keyword = 'if' | 'else' | 'for' | 'while';
identifier = [a-zA-Z_][a-zA-Z0-9_]*;
main := (identifier & !keyword) '\n';
write data;
}%%
int main(void){static const char *t[]={"abc\n","if\n","else\n","for\n","myVar\n","while\n"};static const int e[]={1,0,0,0,1,0};int n=6,cs;const char *p,*pe,*eof;
printf("Pattern 15: Intersection &\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p15_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
