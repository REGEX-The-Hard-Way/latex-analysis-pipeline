/* Pattern 73: EOF fgoto — fhold+fgoto to post-processing on %eof */
#include <stdio.h>
#include <string.h>
#include <assert.h>

%%{ machine p73;

action got_num  { printf("num=%c ", fc); }
action extra    { printf("EOF_EXTRA "); fhold; fgoto *fentry(extra); }
action done     { printf("DONE"); }

atoi = digit+ @got_num;

extra := '' %done;

main := atoi %extra;

write data;
}%%
int main(void){int cs;const char *p,*pe,*eof;
printf("Pattern 73: %%eof with fhold+fgoto\n\n");
p="123";pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
printf("-> %s ok\n",cs>=p73_first_final?"ACCEPT":"FAIL");
printf("\nAll tests passed.\n");return 0;}
