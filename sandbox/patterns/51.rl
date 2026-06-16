#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p51; action c1 { thresh } action set_t { thresh = fc - '0'; } main := digit @set_t digit+ when c1 '\n'; write data; }%%
int main(void){int cs,thresh;const char *p,*pe,*eof;
static const char *t[]={"510\n","55\n","50\n","5\n"};static const int e[]={1,1,1,0};int n=4;
printf("Pattern 51: Parameterized Conditions\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;thresh=0;
%% write init;
%% write exec;
int r=cs>=p51_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
