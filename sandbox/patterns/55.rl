#include <stdio.h>
#include <string.h>
#include <assert.h>
struct nfa_bp_rec { long state; const char *p; long popTrans; };
struct nfa_bp_rec nfa_bp[1024];
long nfa_len = 0;
long nfa_count = 0;
%%{ machine p55;
main := :nfa |*
    "hello" => { printf("hello "); };
    [a-z]+ => { printf("word "); };
    ' ' => { printf("<sp> "); };
*|;
write data; }%%
int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
static const char *t[]={"hello world all"};static const int e[]={1};int n=1;
printf("Pattern 55: NFA Multi-Match\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;nfa_len=0;
%% write init;
%% write exec;
int r=cs>=p55_first_final;printf("\n  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
