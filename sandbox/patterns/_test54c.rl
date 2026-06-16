#include <stdio.h>
#include <string.h>
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
action exit { ({ ++q >= 2; }) }
main := :nfa( digit, psh, pop, ini, stay, repeat, exit ): '\n';
write data; }%%
int main(void){int cs;long q=0;const char *p,*pe,*eof; setbuf(stdout,NULL);
for(int len=0;len<=10;len++){char buf[32];memset(buf,'1',len);buf[len]='\n';buf[len+1]=0;
p=buf;pe=p+strlen(buf);eof=pe;nfa_len=0;
%% write init;
%% write exec;
printf("len=%d -> cs=%d first=%d p-pe=%ld\n",len,cs,p54_first_final,p-pe);}
return 0;}
