#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p42; main := 'h' @{printf("h");fbreak;} 'e' @{printf("e");fbreak;} 'l' @{printf("l");fbreak;} 'l' @{printf("l");fbreak;} 'o' @{printf("o");fbreak;}; write data; }%%
int main(void){int cs;const char *p,*pe,*eof;
static const char *t[]={"hello","helo"};static const int e[]={1,0};int n=2;
printf("Pattern 42: fbreak/fnbreak\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
unsigned nchars=0;
while(p<pe){
%% write exec;
if(cs==p42_error)break;nchars++;}
printf("\n");
int ok=(nchars==5&&cs>=p42_first_final);
printf("  Input %d -> %s ok\n",i,ok?"ACCEPT":"FAIL");assert(ok==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
