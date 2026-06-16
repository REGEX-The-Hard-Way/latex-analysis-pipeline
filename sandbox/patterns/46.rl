#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p46; action buf { buf = p; } action emit { printf(" %.*s", (int)(p-buf), buf); } main := [a-z]+ >buf %emit ( ' ' [a-z]+ >buf %emit )* '\n'; write data; }%%
int main(void){int cs;const char *p,*pe,*eof,*buf;
static const char *t[]={"hello world\n","abc\n"};static const int e[]={1,1};int n=2;
printf("Pattern 46: String Accumulation $buf %%emit\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p46_first_final;printf("\n  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
