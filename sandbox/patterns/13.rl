#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p13; main := /[A-Z][a-z][a-z]/ . ' ' . /[0-9][0-9]:[0-9][0-9]/ '\n'; write data; }%%
int main(void){static const char *t[]={"Mon 12:30\n","Cat 99:99\n","MON 12:30\n","Mon12:30\n"};static const int e[]={1,1,0,0};int n=4,cs;const char *p,*pe,*eof;
printf("Pattern 13: Regex Literals\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p13_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
