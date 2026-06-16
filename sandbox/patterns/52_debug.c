#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p52; action init { c = 0; } action inc { c++; } action min { c >= 1 } action max { c < 3 } main := :condstar( digit, init, inc, min, max ): '\n'; write data; }%%
int main(void){int cs,c;const char *p,*pe,*eof; setbuf(stdout,NULL);
char *t[]={"1\n","12\n","123\n","1234\n"};int e[]={1,1,0,0};
printf("52 debug\n");
for(int i=0;i<4;i++){p=t[i];pe=p+strlen(p);eof=pe;c=-1;
printf("[%d] in='%s'\n",i,t[i]);
%% write init;
%% write exec;
int r=cs>=p52_first_final;
printf("  result=%d c=%d cs=%d first=%d\n",r,c,cs,p52_first_final);
assert(r==e[i]);}
return 0;}
