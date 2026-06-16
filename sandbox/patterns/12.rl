#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p12; main := 'hello' '\n' | "world" '\n' | 0x48 0x65 0x6C 0x6C '\n'; write data; }%%
int main(void){static const char *t[]={"hello\n","world\n","Hell\n","foo\n","HELL\n"};static const int e[]={1,1,1,0,0};int n=5,cs;const char *p,*pe,*eof;
printf("Pattern 12: String Literals\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p12_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
