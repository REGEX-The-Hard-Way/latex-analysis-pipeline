#include <stdio.h>
#include <string.h>
#include <assert.h>
struct nfa_bp_rec { long state; const char *p; long popTrans; long q; };
struct nfa_bp_rec nfa_bp[1024];
long nfa_len = 0;
long nfa_count = 0;
%%{ machine p54;
action psh { nfa_bp[nfa_len].q = q; }
action pop { ({ q = nfa_bp[nfa_len].q; 1; }) }
action ini { ({ q = 0; 1; }) }
action stay { ({ 1; }) }
action repeat { ({ ++q < 2; }) }
action exit { ({ ++q >= 3; }) }
main := :nfa( digit, psh, pop, ini, stay, repeat, exit ): '\n';
write data; }%%
int main(void){int cs;long q=0;const char *p,*pe,*eof; setbuf(stdout,NULL);
static const char *t[]={"1\n","12\n","123\n","1234\n"};static const int e[]={1,0,0,0};int n=4;
printf("54 final\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;nfa_len=0;nfa_count=0;
%% write init;
%% write exec;
int r=cs>=p54_first_final;printf("  %d '%s' -> %s ok\n",i,t[i],r?"ACCEPT":"FAIL");assert(r==e[i]);}
printf("done\n");return 0;}
