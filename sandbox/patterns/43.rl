#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p43; main := |* 'hello' => { printf("hello "); fhold; }; 'world' => { printf("world "); fhold; }; any => { printf("? "); }; *|; write data; }%%
int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
static const char *t[]={"helloworld","helloXworld"};static const int e[]={1,1};int n=2;
printf("Pattern 43: fhold Instruction\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
printf("\n");
int r=cs>=p43_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
