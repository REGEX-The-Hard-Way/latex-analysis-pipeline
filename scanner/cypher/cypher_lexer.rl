/*
 * cypher_lexer.rl — Ragel-based Cypher tokenizer
 * Tokenizes openCypher syntax for the query REPL.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cypher_parser.h"

%%{
    machine cypher_lexer;
    write data;
}%%

static int dispatch_keyword(cypher_token_t *t) {
    (void)t;
    return TOK_IDENT;
}

int cypher_lex(const char *in, int len, cypher_token_t *tokens, int max_tokens) {
    const char *p = in, *pe = in + len, *eof = pe;
    const char *ts, *te;
    int act, cs;
    int tok_count = 0;

    %%{
        newline = '\n' | '\r\n' | '\r';
        wsp = [ \t];
        dgt = [0-9];
        letter = [A-Za-z];
        id_char = letter | dgt | '_';
        id = letter id_char*;

        KW_MATCH    = [Mm][Aa][Tt][Cc][Hh];
        KW_RETURN   = [Rr][Ee][Tt][Uu][Rr][Nn];
        KW_WHERE    = [Ww][Hh][Ee][Rr][Ee];
        KW_CREATE   = [Cc][Rr][Ee][Aa][Tt][Ee];
        KW_DELETE   = [Dd][Ee][Ll][Ee][Tt][Ee];
        KW_SET      = [Ss][Ee][Tt];
        KW_MERGE    = [Mm][Ee][Rr][Gg][Ee];
        KW_UNWIND   = [Uu][Nn][Ww][Ii][Nn][Dd];
        KW_ORDER    = [Oo][Rr][Dd][Ee][Rr];
        KW_BY       = [Bb][Yy];
        KW_LIMIT    = [Ll][Ii][Mm][Ii][Tt];
        KW_SKIP     = [Ss][Kk][Ii][Pp];
        KW_AND      = [Aa][Nn][Dd];
        KW_OR       = [Oo][Rr];
        KW_NOT      = [Nn][Oo][Tt];
        KW_XOR      = [Xx][Oo][Rr];
        KW_IN       = [Ii][Nn];
        KW_IS       = [Ii][Ss];
        KW_AS       = [Aa][Ss];
        KW_DISTINCT = [Dd][Ii][Ss][Tt][Ii][Nn][Cc][Tt];
        KW_OPTIONAL = [Oo][Pp][Tt][Ii][Oo][Nn][Aa][Ll];
        KW_DETACH   = [Dd][Ee][Tt][Aa][Cc][Hh];
        KW_CONTAINS = [Cc][Oo][Nn][Tt][Aa][Ii][Nn][Ss];
        KW_STARTS   = [Ss][Tt][Aa][Rr][Tt][Ss];
        KW_ENDS     = [Ee][Nn][Dd][Ss];
        KW_WITH     = [Ww][Ii][Tt][Hh];
        KW_ON       = [Oo][Nn];
        KW_CASE     = [Cc][Aa][Ss][Ee];
        KW_WHEN     = [Ww][Hh][Ee][Nn];
        KW_THEN     = [Tt][Hh][Ee][Nn];
        KW_ELSE     = [Ee][Ll][Ss][Ee];
        KW_END      = [Ee][Nn][Dd];
        KW_DESC     = [Dd][Ee][Ss][Cc];
        KW_ASC      = [Aa][Ss][Cc];
        KW_COUNT    = [Cc][Oo][Uu][Nn][Tt];
        KW_EXISTS   = [Ee][Xx][Ii][Ss][Tt][Ss];
        KW_REMOVE   = [Rr][Ee][Mm][Oo][Vv][Ee];
        KW_TRUE     = [Tt][Rr][Uu][Ee];
        KW_FALSE    = [Ff][Aa][Ll][Ss][Ee];
        KW_NULL     = [Nn][Uu][Ll][Ll];
        KW_CALL     = [Cc][Aa][Ll][Ll];
        KW_YIELD    = [Yy][Ii][Ee][Ll][Dd];
        KW_ALL      = [Aa][Ll][Ll];
        KW_ANY      = [Aa][Nn][Yy];
        KW_NONE     = [Nn][Oo][Nn][Ee];
        KW_SINGLE   = [Ss][Ii][Nn][Gg][Ll][Ee];
        KW_UNION    = [Uu][Nn][Ii][Oo][Nn];

        main := |*
            wsp+ => { /* skip */ };

            newline => { /* skip, consumed like whitespace */ };

            ';' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_SEMI;
                    tok_count++;
                }
            };

            ',' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_COMMA;
                    tok_count++;
                }
            };

            '..' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_DOTDOT;
                    tok_count++;
                }
            };

            '.' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_DOT;
                    tok_count++;
                }
            };

            ':' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_COLON;
                    tok_count++;
                }
            };

            '(' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_LPAREN;
                    tok_count++;
                }
            };

            ')' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_RPAREN;
                    tok_count++;
                }
            };

            '[' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_LBRACKET;
                    tok_count++;
                }
            };

            ']' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_RBRACKET;
                    tok_count++;
                }
            };

            '{' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_LBRACE;
                    tok_count++;
                }
            };

            '}' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_RBRACE;
                    tok_count++;
                }
            };

            '=' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_EQ;
                    tok_count++;
                }
            };

            '<>' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_NEQ;
                    tok_count++;
                }
            };

            '+=' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_PLUSEQ;
                    tok_count++;
                }
            };

            '<=' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_LE;
                    tok_count++;
                }
            };

            '>=' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_GE;
                    tok_count++;
                }
            };

            '<' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_LT;
                    tok_count++;
                }
            };

            '>' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_GT;
                    tok_count++;
                }
            };

            '+' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_PLUS;
                    tok_count++;
                }
            };

            '-' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_MINUS;
                    tok_count++;
                }
            };

            '--' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_DASH;
                    tok_count++;
                }
            };

            '*' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_STAR;
                    tok_count++;
                }
            };

            '/' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_SLASH;
                    tok_count++;
                }
            };

            '%' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_PCT;
                    tok_count++;
                }
            };

            '^' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_CARET;
                    tok_count++;
                }
            };

            '$' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_DOLLAR;
                    tok_count++;
                }
            };

            '|' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_PIPE;
                    tok_count++;
                }
            };

            '`' [^`]* '`' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_IDENT;
                    int l = (int)(te - ts - 2);
                    if (l >= MAX_STR) l = MAX_STR - 1;
                    memcpy(tokens[tok_count].str, ts + 1, l);
                    tokens[tok_count].str[l] = '\0';
                    tok_count++;
                }
            };

            '"' ([^"\\] | '\\' any)* '"' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_STRING;
                    int l = (int)(te - ts - 2);
                    if (l >= MAX_STR) l = MAX_STR - 1;
                    memcpy(tokens[tok_count].str, ts + 1, l);
                    tokens[tok_count].str[l] = '\0';
                    tok_count++;
                }
            };

            "'" ([^'\\] | '\\' any)* "'" => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_STRING;
                    int l = (int)(te - ts - 2);
                    if (l >= MAX_STR) l = MAX_STR - 1;
                    memcpy(tokens[tok_count].str, ts + 1, l);
                    tokens[tok_count].str[l] = '\0';
                    tok_count++;
                }
            };

            '0x' [0-9A-Fa-f]+ => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_INTEGER;
                    tokens[tok_count].ival = (int)strtoll(ts, NULL, 16);
                    tok_count++;
                }
            };

            dgt+ '.' dgt+ => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_FLOAT;
                    tokens[tok_count].fval = strtod(ts, NULL);
                    tok_count++;
                }
            };

            dgt+ => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_INTEGER;
                    tokens[tok_count].ival = (int)strtol(ts, NULL, 10);
                    tok_count++;
                }
            };

            KW_MATCH    => { tokens[tok_count].type = TOK_MATCH;    tok_count++; };
            KW_RETURN   => { tokens[tok_count].type = TOK_RETURN;   tok_count++; };
            KW_WHERE    => { tokens[tok_count].type = TOK_WHERE;    tok_count++; };
            KW_CREATE   => { tokens[tok_count].type = TOK_CREATE;   tok_count++; };
            KW_DELETE   => { tokens[tok_count].type = TOK_DELETE;   tok_count++; };
            KW_SET      => { tokens[tok_count].type = TOK_SET;      tok_count++; };
            KW_MERGE    => { tokens[tok_count].type = TOK_MERGE;    tok_count++; };
            KW_UNWIND   => { tokens[tok_count].type = TOK_UNWIND;   tok_count++; };
            KW_ORDER    => { tokens[tok_count].type = TOK_ORDER;    tok_count++; };
            KW_BY       => { tokens[tok_count].type = TOK_BY;       tok_count++; };
            KW_LIMIT    => { tokens[tok_count].type = TOK_LIMIT;    tok_count++; };
            KW_SKIP     => { tokens[tok_count].type = TOK_SKIP;     tok_count++; };
            KW_AND      => { tokens[tok_count].type = TOK_AND;      tok_count++; };
            KW_OR       => { tokens[tok_count].type = TOK_OR;       tok_count++; };
            KW_NOT      => { tokens[tok_count].type = TOK_NOT;      tok_count++; };
            KW_XOR      => { tokens[tok_count].type = TOK_XOR;      tok_count++; };
            KW_IN       => { tokens[tok_count].type = TOK_IN;       tok_count++; };
            KW_IS       => { tokens[tok_count].type = TOK_IS;       tok_count++; };
            KW_AS       => { tokens[tok_count].type = TOK_AS;       tok_count++; };
            KW_DISTINCT => { tokens[tok_count].type = TOK_DISTINCT; tok_count++; };
            KW_OPTIONAL => { tokens[tok_count].type = TOK_OPTIONAL; tok_count++; };
            KW_DETACH   => { tokens[tok_count].type = TOK_DETACH;   tok_count++; };
            KW_CONTAINS => { tokens[tok_count].type = TOK_CONTAINS; tok_count++; };
            KW_STARTS   => { tokens[tok_count].type = TOK_STARTS;   tok_count++; };
            KW_ENDS     => { tokens[tok_count].type = TOK_ENDS;     tok_count++; };
            KW_WITH     => { tokens[tok_count].type = TOK_WITH;     tok_count++; };
            KW_ON       => { tokens[tok_count].type = TOK_ON;       tok_count++; };
            KW_CASE     => { tokens[tok_count].type = TOK_CASE;     tok_count++; };
            KW_WHEN     => { tokens[tok_count].type = TOK_WHEN;     tok_count++; };
            KW_THEN     => { tokens[tok_count].type = TOK_THEN;     tok_count++; };
            KW_ELSE     => { tokens[tok_count].type = TOK_ELSE;     tok_count++; };
            KW_END      => { tokens[tok_count].type = TOK_END;      tok_count++; };
            KW_DESC     => { tokens[tok_count].type = TOK_DESC;     tok_count++; };
            KW_ASC      => { tokens[tok_count].type = TOK_ASC;      tok_count++; };
            KW_COUNT    => { tokens[tok_count].type = TOK_COUNT;    tok_count++; };
            KW_EXISTS   => { tokens[tok_count].type = TOK_EXISTS;   tok_count++; };
            KW_REMOVE   => { tokens[tok_count].type = TOK_REMOVE;   tok_count++; };
            KW_TRUE     => { tokens[tok_count].type = TOK_BOOL; tokens[tok_count].ival = 1; tok_count++; };
            KW_FALSE    => { tokens[tok_count].type = TOK_BOOL; tokens[tok_count].ival = 0; tok_count++; };
            KW_NULL     => { tokens[tok_count].type = TOK_NULL;     tok_count++; };
            KW_CALL     => { tokens[tok_count].type = TOK_CALL;     tok_count++; };
            KW_YIELD    => { tokens[tok_count].type = TOK_YIELD;    tok_count++; };
            KW_ALL      => { tokens[tok_count].type = TOK_ALL;      tok_count++; };
            KW_ANY      => { tokens[tok_count].type = TOK_ANY;      tok_count++; };
            KW_NONE     => { tokens[tok_count].type = TOK_NONE;     tok_count++; };
            KW_SINGLE   => { tokens[tok_count].type = TOK_SINGLE;   tok_count++; };
            KW_UNION    => { tokens[tok_count].type = TOK_UNION;    tok_count++; };

            id => {
                if (tok_count < max_tokens) {
                    int l = (int)(te - ts);
                    if (l >= MAX_STR) l = MAX_STR - 1;
                    memcpy(tokens[tok_count].str, ts, l);
                    tokens[tok_count].str[l] = '\0';
                    tokens[tok_count].type = dispatch_keyword(&tokens[tok_count]);
                    tok_count++;
                }
            };

            '//' [^\n]* => { /* skip line comment */ };
            '/*' any* :>> '*/' => { /* skip block comment */ };
        *|;
    }%%

    %% write init;
    %% write exec noend;

    return tok_count;
}
