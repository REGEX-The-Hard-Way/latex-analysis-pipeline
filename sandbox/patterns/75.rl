/* Pattern 75: Printf Parser — %[flags][width][.precision][length]conversion */
#include <stdio.h>
#include <string.h>
#include <assert.h>

%%{ machine p75;

action fmt_start  { printf("\n  FORMAT: "); }
action flag       { printf("flag=%c ", fc); }
action width_dig  { printf("w"); }
action prec_dig   { printf("p"); }
action length     { printf("len=%c ", fc); }
action conversion { printf("conv=%c ", fc); }
action literal    { printf("%c", fc); }

format_spec = '%' >fmt_start
    ( [\-+ #0] @flag )*
    ( digit @width_dig )*
    ( '.' ( digit @prec_dig )* )?
    ( 'h' | 'l' | 'L' | 'z' | 'j' | 't' )? @length
    [diouxXeEfFgGaAcspn%] @conversion;

main := ( format_spec | any @literal )*;

write data;
}%%
int main(void){int cs;const char *p,*pe,*eof;
printf("Pattern 75: Printf Parser\n");
p="hello %-20.10ld world %% end";pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
printf("\n-> %s ok\n",cs>=p75_first_final?"ACCEPT":"FAIL");assert(cs>=p75_first_final);
printf("\nAll tests passed.\n");return 0;}
