#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p45; main := |* [a-z]+ => { printf("=> word: %.*s\n", (int)(te-ts), ts); }; [0-9]+ >{ts=p;} %{te=p;} { printf("ts/te num: %.*s\n", (int)(te-ts), ts); }; space => { printf("<space>\n"); }; *|; write data; }%%
int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
static const char *t[]={"abc 123","abc 123 xyz"};static const int e[]={1,1};int n=2;
printf("Pattern 45: => vs Explicit ts/te\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p45_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
