#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p7; action ok { matched=1; } main := ((empty | zlen) 'A' 'B' '\n') %ok; write data; }%%
int main(void){static const char *t[]={"AB\n","A\n","\n"};static const int e[]={1,0,0};int n=3,cs,matched;const char *p,*pe,*eof;
printf("Pattern  7: Empty/Zlen\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;matched=0;
%% write init;
%% write exec;
printf("  Input %d -> %s ok\n",i,matched?"ACCEPT":"FAIL");assert(matched==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
