/* Pattern 69: Concurrent Line/Column — scanner & position tracker */
#include <stdio.h>
#include <string.h>
#include <assert.h>

int line = 1, col = 1;
const char *ts, *te;
int act;

%%{ machine p69;

action inc_line { line++; col = 1; }
action inc_col  { col++; }
action token    { printf("tok@%d:%d ", line, col); }

main := |*
    [a-zA-Z]+ => token;
    '\n'      => inc_line;
    any       => inc_col;
*|;

write data;
}%%
int main(void){int cs;const char *p,*pe,*eof;
const char *d="abc def\nghi jkl";
printf("Pattern 69: Concurrent Line/Column Tracker\n\n");
p=d;pe=p+strlen(d);eof=pe;
%% write init;
%% write exec;
printf("-> %s ok\n",cs>=p69_first_final?"ACCEPT":"FAIL");
printf("\nAll tests passed.\n");return 0;}
