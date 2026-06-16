/*
 * cypher_parse_ragel.rl — Ragel-based replacement for cypher_parse().
 *
 * Replaces the hand-written while-loop in cypher_parse() with a deterministic
 * Ragel scanner DFA. All parse_*() clause parsers are linked from cypher_parser.c.
 *
 * Compile: ragel -G2 cypher_parse_ragel.rl → cypher_parse_ragel.c
 * Link:    gcc ... cypher_parser.c cypher_parse_ragel.c ...
 *
 * The cypher_parse() in this file replaces the one in cypher_parser.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cypher_parser.h"

/* Clause parsers from cypher_parser.c (declared, not defined here) */
extern cypher_token_t *toks;
extern int tn, ti;
extern const char **err;
static cypher_ast_t *parse_clause(void); /* defined in cypher_parser.c */

/* ---- Ragel scanner ---- */
%%{
    machine cypher_clause_scan;
    write data;
}%%

cypher_ast_t *cypher_parse(cypher_token_t *tokens, int n, const char **error) {
    toks = tokens; tn = n; ti = 0;
    err = error; *error = NULL;

    /* token types → byte array */
    static unsigned char buf[MAX_TOKENS];
    int i; for (i = 0; i < n && i < MAX_TOKENS; i++)
        buf[i] = (unsigned char)tokens[i].type;

    const unsigned char *p  = buf;
    const unsigned char *pe = buf + i;
    const unsigned char *eof = pe;
    int cs, act;

    cypher_ast_t *head = NULL, *tail = NULL;

    %%{
        action clause_action {
            cypher_ast_t *cl = parse_clause();
            if (cl) {
                if (!head) head = tail = cl;
                else { tail->next = cl; tail = cl; }
            }
            /* sync ti to p position after parse_clause() advances it */
        }

        /* clause-starting tokens — all others silently consumed by 'any' */
        main := |*
            32  => clause_action;  /* TOK_MATCH    */
            53  => clause_action;  /* TOK_OPTIONAL */
            33  => clause_action;  /* TOK_RETURN   */
            35  => clause_action;  /* TOK_CREATE   */
            37  => clause_action;  /* TOK_SET      */
            36  => clause_action;  /* TOK_DELETE   */
            54  => clause_action;  /* TOK_DETACH   */
            38  => clause_action;  /* TOK_MERGE    */
            44  => clause_action;  /* TOK_UNWIND   */
            43  => clause_action;  /* TOK_WITH     */
            68  => clause_action;  /* TOK_REMOVE   */
            any;
        *|;
    }%%

    %% write init;
    %% write exec;

    return head;
}
