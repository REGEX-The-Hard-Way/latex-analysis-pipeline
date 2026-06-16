/* Pattern 74: Complete Atoi — classic ('-'@see_neg | '+')? (digit@add_digit)+ >begin %finish */
#include <stdio.h>
#include <string.h>
#include <assert.h>

int neg, value;

%%{ machine p74;

action begin {
    neg = 0;
    value = 0;
}

action see_neg {
    neg = 1;
}

action add_digit {
    value = value * 10 + (fc - '0');
}

action finish {
    if (neg) value = -value;
    printf("value=%d\n", value);
}

atoi = (
    ('-'@see_neg | '+')? (digit @add_digit)+
) >begin %finish;

main := atoi '\n';

write data;
}%%
int main(void){int cs;const char *p,*pe,*eof;
printf("Pattern 74: Complete Atoi\n\n");
const char *tests[] = {"1\n","12\n","222\n","+2123\n","-99\n","-12321\n","x\n"};
int expect[] = {1,1,1,1,1,1,0};
int n = 7;
for(int i=0;i<n;i++){p=tests[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p74_first_final;printf("  %-8s -> %s ok\n",tests[i],r?"ACCEPT":"FAIL");assert(r==expect[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
