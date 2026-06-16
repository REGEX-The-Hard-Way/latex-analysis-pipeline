#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p14; day = /[0-3][0-9]/; month = /[A-Z][a-z][a-z]/; year = /[0-9][0-9][0-9][0-9]/; main := day . ' ' . month . ' ' . year '\n'; write data; }%%
int main(void){static const char *t[]={"12 Jan 2023\n","31 Dec 1999\n","00 AAA 0000\n","12Jan2023\n"};static const int e[]={1,1,0,0};int n=4,cs;const char *p,*pe,*eof;
printf("Pattern 14: Concatenation\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p14_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
