/*
 * cypher_lexer.rl — Ragel-based Cypher tokenizer
 * Tokenizes openCypher syntax for the query REPL.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "cypher_parser.h"

%%{
    machine cypher_lexer;
    write data;
}%%

static int dispatch_keyword(cypher_token_t *t) {
    char upper[MAX_STR];
    int i;
    for (i = 0; t->str[i]; i++)
        upper[i] = (t->str[i] >= 'a' && t->str[i] <= 'z') ? t->str[i] - 32 : t->str[i];
    upper[i] = '\0';

    if (!strcmp(upper, "MATCH"))      return TOK_MATCH;
    if (!strcmp(upper, "RETURN"))     return TOK_RETURN;
    if (!strcmp(upper, "WHERE"))      return TOK_WHERE;
    if (!strcmp(upper, "CREATE"))     return TOK_CREATE;
    if (!strcmp(upper, "DELETE"))     return TOK_DELETE;
    if (!strcmp(upper, "SET"))        return TOK_SET;
    if (!strcmp(upper, "MERGE"))      return TOK_MERGE;
    if (!strcmp(upper, "ORDER"))      return TOK_ORDER;
    if (!strcmp(upper, "BY"))         return TOK_BY;
    if (!strcmp(upper, "LIMIT"))      return TOK_LIMIT;
    if (!strcmp(upper, "SKIP"))       return TOK_SKIP;
    if (!strcmp(upper, "AND"))        return TOK_AND;
    if (!strcmp(upper, "OR"))         return TOK_OR;
    if (!strcmp(upper, "NOT"))        return TOK_NOT;
    if (!strcmp(upper, "XOR"))        return TOK_XOR;
    if (!strcmp(upper, "IN"))         return TOK_IN;
    if (!strcmp(upper, "IS"))         return TOK_IS;
    if (!strcmp(upper, "AS"))         return TOK_AS;
    if (!strcmp(upper, "DISTINCT"))   return TOK_DISTINCT;
    if (!strcmp(upper, "OPTIONAL"))   return TOK_OPTIONAL;
    if (!strcmp(upper, "DETACH"))     return TOK_DETACH;
    if (!strcmp(upper, "CONTAINS"))   return TOK_CONTAINS;
    if (!strcmp(upper, "STARTS"))     return TOK_STARTS;
    if (!strcmp(upper, "ENDS"))       return TOK_ENDS;
    if (!strcmp(upper, "WITH"))       return TOK_WITH;
    if (!strcmp(upper, "ON"))         return TOK_ON;
    if (!strcmp(upper, "CASE"))       return TOK_CASE;
    if (!strcmp(upper, "WHEN"))       return TOK_WHEN;
    if (!strcmp(upper, "THEN"))       return TOK_THEN;
    if (!strcmp(upper, "ELSE"))       return TOK_ELSE;
    if (!strcmp(upper, "END"))        return TOK_END;
    if (!strcmp(upper, "DESC"))       return TOK_DESC;
    if (!strcmp(upper, "ASC"))        return TOK_ASC;
    if (!strcmp(upper, "COUNT"))      return TOK_COUNT;
    if (!strcmp(upper, "EXISTS"))     return TOK_EXISTS;
    if (!strcmp(upper, "REMOVE"))     return TOK_REMOVE;
    if (!strcmp(upper, "TRUE"))       { t->ival = 1; return TOK_BOOL; }
    if (!strcmp(upper, "FALSE"))      { t->ival = 0; return TOK_BOOL; }
    if (!strcmp(upper, "NULL"))       return TOK_NULL;
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

        main := |*
            wsp+ => { /* skip */ };

            newline => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_SEMI;
                    tok_count++;
                }
            };

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

            '-' '--' => {
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_MINUS;
                    tok_count++;
                }
                if (tok_count < max_tokens) {
                    tokens[tok_count].type = TOK_DASH;
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
