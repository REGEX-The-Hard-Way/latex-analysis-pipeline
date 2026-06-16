/* Pattern 71: Pull-Style Scanner — scan() returning one token via fbreak */
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct Scanner {
    int cs, act;
    const char *ts, *te;
    const char *p, *pe;
    const char *eof;
};

struct Scanner sc;

%%{ machine p71;
main := |*
    [a-zA-Z_][a-zA-Z0-9_]*  => {
        printf("ID(%.*s) ", (int)(sc.te - sc.ts), sc.ts);
        fbreak;
    };
    [0-9]+                  => {
        printf("NUM(%.*s) ", (int)(sc.te - sc.ts), sc.ts);
        fbreak;
    };
    [+\-*/]                 => {
        printf("OP(%c) ", fc);
        fbreak;
    };
    space+;
*|;
write data;
}%%

int scan(void) {
    if (sc.p == sc.pe && sc.p == sc.eof)
        return 0;
    %% variable p sc.p;
    %% variable pe sc.pe;
    %% variable eof sc.eof;
    %% variable ts sc.ts;
    %% variable te sc.te;
    %% variable act sc.act;
    %% variable cs sc.cs;
    %% write exec;
    return 1;
}

int main(void){
printf("Pattern 71: Pull-Style Scanner\n\n");

%% variable cs sc.cs;
%% write init;

const char *input = "foo 123 + bar 456";
sc.p = input;
sc.pe = input + strlen(input);
sc.eof = sc.pe;

while (scan()) {
    if (sc.cs == p71_error) { printf("ERR "); break; }
}
printf("-> %s ok\n", sc.cs >= p71_first_final ? "ACCEPT" : "FAIL");
assert(sc.cs >= p71_first_final);
printf("\nAll tests passed.\n");return 0;}
