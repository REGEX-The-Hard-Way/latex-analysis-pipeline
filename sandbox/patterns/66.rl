/* Pattern 66: Import Machines from 66_atoms.rl */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p66;
import "66_atoms.rl";

action foo_match { printf("foo "); }
action bar_match { printf("bar "); }
action sp_match  { printf("SP ");  }
action nl_match  { printf("NL");   }

main := (
    foo @foo_match |
    bar @bar_match |
    SP  @sp_match  |
    NL  @nl_match
)+;

write data;
}%%
int main(void){int cs;const char *p,*pe,*eof;const char *d="foo bar foo\n";
printf("Pattern 66: Import Machines\n\n");
p=d;pe=p+strlen(d);eof=pe;
%% write init;
%% write exec;
printf(" -> %s ok\n",cs>=p66_first_final?"ACCEPT":"FAIL");
assert(cs>=p66_first_final);
printf("\nAll tests passed.\n");return 0;}
