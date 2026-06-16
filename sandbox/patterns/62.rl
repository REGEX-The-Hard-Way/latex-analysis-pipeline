/* Pattern 62: getkey — parse array of structs by type field */
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct { int type; const char *name; } Token;
Token tokens[] = {
    { 'A', "alpha" },
    { 'B', "beta"  },
    { 'Z', "zeta"  },
    { 0,   NULL     },
};
#define TOKEN_COUNT 3

%%{
machine p62;
alphtype int;
getkey fpc->type;

action begin { printf("{"); }
action token { printf(" %s", fpc->name); }
action end   { printf(" }\n"); }

main := ( any+ $token ) >begin %end;

write data;
}%%
int main(void){int cs;Token *p,*pe,*eof;p=tokens;pe=p+TOKEN_COUNT;eof=pe;
printf("Pattern 62: getkey — struct array parsing\n\n");
printf("  Tokens:");
%% write init;
%% write exec;
if(cs>=p62_first_final)printf("ACCEPT\n");else printf("FAIL\n");
assert(cs>=p62_first_final);
printf("\nAll tests passed.\n");return 0;}
