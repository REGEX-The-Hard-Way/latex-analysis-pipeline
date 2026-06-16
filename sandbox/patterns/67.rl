/* Pattern 67: Include Machines from 67_lib.rl */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p67;
include lib1 "67_lib.rl";
include lib2 "67_lib.rl";

main := (
    a1 | b1 | c2 | d2
)*;

write data;
}%%
int main(void){int cs;const char *p,*pe,*eof;const char *d="ABCD";
printf("Pattern 67: Include Machines\n\n");
p=d;pe=p+strlen(d);eof=pe;
%% write init;
%% write exec;
printf(" -> %s ok\n",cs>=p67_first_final?"ACCEPT":"FAIL");
assert(cs>=p67_first_final);
printf("\nAll tests passed.\n");return 0;}
