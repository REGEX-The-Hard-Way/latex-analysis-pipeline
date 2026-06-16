#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p9; main := ('a'{4} '\n' | 'a'{,5} '\n' | 'a'{3,} '\n' | 'a'{2,5} '\n'); write data; }%%
int main(void){static const char *t[]={"aaaa\n","aaa\n","aaaaaa\n","aa\n","a\n","abc\n"};static const int e[]={1,1,1,1,1,0};int n=6,cs;const char *p,*pe,*eof;
printf("Pattern  9: Count Repetition\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p9_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
