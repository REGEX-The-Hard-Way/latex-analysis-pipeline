#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p4; main := '/*' (any-'\*')* '*/'; write data; }%%
int main(void){static const char *t[]={"/* comment */","/**/","/* x y */","/* unmatched","/ /","abc","/* a * b */"};static const int e[]={1,1,1,0,0,0,0};int n=7,cs;const char *p,*pe,*eof;
printf("Pattern  4: Negated (C comments)\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p4_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
