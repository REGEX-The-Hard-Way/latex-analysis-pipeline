#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p16; main := ([a-z]+ - 'abc') '\n' | ([a-z]+ -- 'bc') '\n'; write data; }%%
int main(void){static const char *t[]={"abc\n","xyz\n","abcd\n","xbc\n","hello\n"};static const int e[]={0,1,1,1,1};int n=5,cs;const char *p,*pe,*eof;
printf("Pattern 16: Subtraction (- and --)\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p16_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
