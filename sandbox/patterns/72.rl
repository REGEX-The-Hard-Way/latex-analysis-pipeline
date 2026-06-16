/* Pattern 72: EOF Actions — >eof $eof %eof <>eof on 'hello' machine */
#include <stdio.h>
#include <string.h>
#include <assert.h>

%%{ machine p72;

action start_eof  { printf(" >eof"); }
action all_eof    { printf(" $eof"); }
action final_eof  { printf(" %%eof"); }
action not_eof    { printf(" <>eof"); }

main := (
    'hello' >eof start_eof $eof all_eof %eof final_eof <>eof not_eof
);

write data;
}%%

void test(const char *s, int expect) {
    int cs; const char *p,*pe,*eof;
    p=s; pe=p+strlen(s); eof=pe;
    printf("  \"%s\" -> ", s);
    %% write init;
    %% write exec;
    int r = cs >= p72_first_final;
    printf(" %s ok\n", r?"ACCEPT":"FAIL");
    assert(r == expect);
}

int main(void){
printf("Pattern 72: EOF Actions (eof=pe)\n\n");
test("hello", 1);
test("hell",  0);
test("h",     0);
test("",      0);
test("hello!",0);
printf("\nAll 5 tests passed.\n");return 0;}
