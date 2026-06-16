/* Pattern 80: C++ Lexer — tokenize identifiers, integers, floats, hex, operators, strings, comments */
#include <stdio.h>
#include <string.h>
#include <assert.h>

enum {
    TK_ID = 256, TK_INT, TK_FLOAT, TK_HEX, TK_STRING,
    TK_COMMENT, TK_LCOMMENT, TK_EQ, TK_NE, TK_AND, TK_OR,
    TK_WS
};
const char *tok_names[] = {
    [TK_ID-256]="IDENT",[TK_INT-256]="INT",[TK_FLOAT-256]="FLOAT",
    [TK_HEX-256]="HEX",[TK_STRING-256]="STR",[TK_COMMENT-256]="CMT",
    [TK_LCOMMENT-256]="LCMT",[TK_EQ-256]="EQ",[TK_NE-256]="NE",
    [TK_AND-256]="AND",[TK_OR-256]="OR",[TK_WS-256]="WS",
};

int tok_type;
const char *ts, *te;
int act;

%%{ machine p80;

main := |*
    [a-zA-Z_][a-zA-Z0-9_]*          => { printf("%s(%.*s) ", tok_names[TK_ID-256],     (int)(te-ts), ts); fbreak; };
    '0' [xX] [0-9a-fA-F]+           => { printf("%s(%.*s) ", tok_names[TK_HEX-256],    (int)(te-ts), ts); fbreak; };
    [0-9]+ '.' [0-9]+               => { printf("%s(%.*s) ", tok_names[TK_FLOAT-256],  (int)(te-ts), ts); fbreak; };
    [0-9]+                          => { printf("%s(%.*s) ", tok_names[TK_INT-256],    (int)(te-ts), ts); fbreak; };
    '"' ([^"\\] | /\\./)* '"'       => { printf("%s(%.*s) ", tok_names[TK_STRING-256], (int)(te-ts), ts); fbreak; };
    '/*' any* '*/'                  => { printf("%s(%.*s) ", tok_names[TK_COMMENT-256],(int)(te-ts), ts); fbreak; };
    '//' [^\n]* '\n'                => { printf("%s(%.*s) ", tok_names[TK_LCOMMENT-256],(int)(te-ts),ts); fbreak; };
    '=='                            => { printf("%s", tok_names[TK_EQ-256]); fbreak; };
    '!='                            => { printf("%s", tok_names[TK_NE-256]); fbreak; };
    '&&'                            => { printf("%s", tok_names[TK_AND-256]); fbreak; };
    '||'                            => { printf("%s", tok_names[TK_OR-256]); fbreak; };
    space+;
*|;

write data;
}%%

int main(void){int cs;const char *p,*pe,*eof;
printf("Pattern 80: C++ Lexer (12 token types)\n\n");
const char *src = "int x=42; float y=3.14; /* block */ // line\n"
                  "char *s=\"hello\"; if(x!=0&&y>0) return 0xFF;\n";
p=src;pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
printf("-> %s ok\n",cs>=p80_first_final?"ACCEPT":"FAIL");
assert(cs>=p80_first_final);
printf("\nAll tests passed.\n");return 0;}
