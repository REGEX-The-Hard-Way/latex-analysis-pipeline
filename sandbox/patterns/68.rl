/* Pattern 68: Concurrent Machines — simultaneous pattern matching */
#include <stdio.h>
#include <string.h>
#include <assert.h>

%%{ machine p68;

action char_token   { printf("CHAR[%c] ", fc); }
action word_token   { printf("WORD ");          }
action comment_token{ printf("CMT ");           }
action string_token { printf("STR ");           }
action space_token  { /* skip */               }

main := (
    [a-zA-Z] @char_token |
    [a-zA-Z]+ @word_token |
    '"' [^"]* '"' @string_token |
    '/*' any* '*/' @comment_token |
    space+
)*;

write data;
}%%
int main(void){int cs;const char *p,*pe,*eof;
const char *d="abc \"hello\" /*c*/ x def";
printf("Pattern 68: Concurrent Machines\n\n");
p=d;pe=p+strlen(d);eof=pe;
%% write init;
%% write exec;
printf("-> %s ok\n",cs>=p68_first_final?"ACCEPT":"FAIL");
assert(cs>=p68_first_final);
printf("\nAll tests passed.\n");return 0;}
