/*
 * cypher_parser.c — Recursive-descent parser consuming tokens from cypher_lexer.rl.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cypher_parser.h"

static cypher_token_t *toks;
static int tn, ti;
static const char **err;

static cypher_ast_t *new_ast(cypher_ast_type_t t) {
    cypher_ast_t *a = calloc(1, sizeof(cypher_ast_t));
    a->type = t;
    return a;
}

static int peek(int off) { return (ti + off < tn) ? toks[ti + off].type : TOK_EOF; }
static int cur(void) { return peek(0); }
static void advance(void) { if (ti < tn) ti++; }
static int match(int t) { if (cur() == t) { advance(); return 1; } return 0; }
static int expect(int t) {
    if (cur() == t) { advance(); return 1; }
    *err = "unexpected token";
    return 0;
}

static cypher_ast_t *parse_expression(void);

static cypher_ast_t *parse_atom(void) {
    if (cur() == TOK_CASE) {
        advance();
        cypher_ast_t *case_node = new_ast(AST_CASE);
        cypher_ast_t *head = NULL, *tail = NULL;
        while (cur() == TOK_WHEN) {
            advance();
            cypher_ast_t *cond = parse_expression();
            if (!expect(TOK_THEN)) break;
            cypher_ast_t *result = parse_expression();
            cypher_ast_t *w = new_ast(AST_BINARY);
            w->bin.op = TOK_WHEN;
            w->bin.l = cond;
            w->bin.r = result;
            w->next = NULL;
            if (!head) head = tail = w;
            else { tail->next = w; tail = w; }
        }
        cypher_ast_t *else_result = NULL;
        if (match(TOK_ELSE)) else_result = parse_expression();
        expect(TOK_END);
        case_node->bin.l = head;
        case_node->bin.r = else_result;
        return case_node;
    }
    if ((cur() == TOK_IDENT || cur() == TOK_COUNT || cur() == TOK_EXISTS)
        && peek(1) == TOK_LPAREN) {
        cypher_ast_t *fc = new_ast(AST_FUNCALL);
        fc->call.func = new_ast(AST_IDENT);
        if (cur() == TOK_COUNT) strcpy(fc->call.func->str, "COUNT");
        else if (cur() == TOK_EXISTS) strcpy(fc->call.func->str, "EXISTS");
        else strcpy(fc->call.func->str, toks[ti].str);
        advance(); advance(); /* consume ident and '(' */
        cypher_ast_t *args[64]; int na = 0;
        if (cur() != TOK_RPAREN && cur() != TOK_EOF) {
            do {
                if (match(TOK_COMMA)) continue;
                if (match(TOK_STAR)) {
                    cypher_ast_t *star = new_ast(AST_IDENT);
                    strcpy(star->str, "*");
                    args[na++] = star;
                    continue;
                }
                if (match(TOK_DISTINCT)) continue;
                cypher_ast_t *e = parse_expression();
                if (e) args[na++] = e;
            } while (cur() == TOK_COMMA);
        }
        expect(TOK_RPAREN);
        fc->call.args = malloc(sizeof(cypher_ast_t *) * (size_t)(na + 1));
        fc->call.n = na;
        for (int i = 0; i < na; i++) fc->call.args[i] = args[i];
        return fc;
    }
    if (cur() == TOK_LPAREN) {
        advance();
        cypher_ast_t *e = parse_expression();
        expect(TOK_RPAREN);
        return e;
    }
    if (cur() == TOK_IDENT) {
        cypher_ast_t *a = new_ast(AST_IDENT);
        strcpy(a->str, toks[ti].str);
        advance();
        return a;
    }
    if (cur() == TOK_STRING) {
        cypher_ast_t *a = new_ast(AST_STRING);
        strcpy(a->str, toks[ti].str);
        advance();
        return a;
    }
    if (cur() == TOK_INTEGER) {
        cypher_ast_t *a = new_ast(AST_INTEGER);
        a->ival = toks[ti].ival;
        advance();
        return a;
    }
    if (cur() == TOK_MINUS && (peek(1) == TOK_INTEGER || peek(1) == TOK_FLOAT)) {
        advance();
        if (cur() == TOK_INTEGER) {
            cypher_ast_t *a = new_ast(AST_INTEGER);
            a->ival = -toks[ti].ival;
            advance();
            return a;
        }
        if (cur() == TOK_FLOAT) {
            cypher_ast_t *a = new_ast(AST_FLOAT);
            a->fval = -toks[ti].fval;
            advance();
            return a;
        }
    }
    if (cur() == TOK_FLOAT) {
        cypher_ast_t *a = new_ast(AST_FLOAT);
        a->fval = toks[ti].fval;
        advance();
        return a;
    }
    if (cur() == TOK_BOOL) {
        cypher_ast_t *a = new_ast(AST_BOOL);
        a->ival = toks[ti].ival;
        advance();
        return a;
    }
    if (cur() == TOK_NULL) {
        advance();
        return new_ast(AST_NULL);
    }
    if (cur() == TOK_LBRACKET) {
        advance();
        cypher_ast_t *lst = new_ast(AST_LIST);
        cypher_ast_t *items[64]; int ni = 0;
        while (cur() != TOK_RBRACKET && cur() != TOK_EOF) {
            cypher_ast_t *e = parse_expression();
            if (e) items[ni++] = e;
            match(TOK_COMMA);
        }
        expect(TOK_RBRACKET);
        lst->list.n = ni;
        lst->list.items = malloc(sizeof(cypher_ast_t *) * (size_t)(ni + 1));
        for (int i = 0; i < ni; i++) lst->list.items[i] = items[i];
        return lst;
    }
    return NULL;
}

static cypher_ast_t *parse_postfix(cypher_ast_t *a) {
    while (cur() == TOK_DOT || cur() == TOK_LBRACKET) {
        if (cur() == TOK_DOT) {
            advance();
            cypher_ast_t *prop = new_ast(AST_PROP);
            prop->prop.e = a;
            cypher_ast_t *n = new_ast(AST_IDENT);
            if (cur() == TOK_IDENT) { strcpy(n->str, toks[ti].str); advance(); }
            prop->prop.n = n;
            a = prop;
        } else {
            advance(); /* consume '[' */
            cypher_ast_t *idx = parse_expression();
            expect(TOK_RBRACKET);
            cypher_ast_t *b = new_ast(AST_BINARY);
            b->bin.op = TOK_LBRACKET;
            b->bin.l = a;
            b->bin.r = idx;
            a = b;
        }
    }
    return a;
}

/* unary: ('+'|'-')* atom postfix */
static cypher_ast_t *parse_unary(void) {
    if (cur() == TOK_MINUS) {
        advance();
        cypher_ast_t *u = new_ast(AST_UNARY);
        u->una.op = '-';
        u->una.a = parse_unary();
        return u;
    }
    if (cur() == TOK_PLUS) {
        advance();
        return parse_unary();
    }
    cypher_ast_t *a = parse_atom();
    if (!a) return NULL;
    return parse_postfix(a);
}

/* power: unary('^' unary)* */
static cypher_ast_t *parse_power(void) {
    cypher_ast_t *l = parse_unary();
    if (!l) return NULL;
    while (cur() == TOK_CARET) {
        advance();
        cypher_ast_t *r = parse_unary();
        if (!r) break;
        cypher_ast_t *b = new_ast(AST_BINARY);
        b->bin.op = '^'; b->bin.l = l; b->bin.r = r;
        l = b;
    }
    return l;
}

/* term: power('*'|'/'|'%' power)* */
static cypher_ast_t *parse_term(void) {
    cypher_ast_t *l = parse_power();
    if (!l) return NULL;
    while (cur() == TOK_STAR || cur() == TOK_SLASH || cur() == TOK_PCT) {
        int op = cur(); advance();
        cypher_ast_t *r = parse_power();
        if (!r) break;
        cypher_ast_t *b = new_ast(AST_BINARY);
        b->bin.op = (char)op; b->bin.l = l; b->bin.r = r;
        l = b;
    }
    return l;
}

/* arithmetic: term('+'|'-' term)* */
static cypher_ast_t *parse_arithmetic(void) {
    cypher_ast_t *l = parse_term();
    if (!l) return NULL;
    while (cur() == TOK_PLUS || cur() == TOK_MINUS) {
        int op = cur(); advance();
        cypher_ast_t *r = parse_term();
        if (!r) break;
        cypher_ast_t *b = new_ast(AST_BINARY);
        b->bin.op = (char)op; b->bin.l = l; b->bin.r = r;
        l = b;
    }
    return l;
}

static cypher_ast_t *parse_comparison(void) {
    cypher_ast_t *l = parse_arithmetic();
    if (!l) return NULL;

    int op = cur();
    if (op == TOK_EQ || op == TOK_NEQ || op == TOK_LT || op == TOK_GT ||
        op == TOK_LE || op == TOK_GE || op == TOK_IN || op == TOK_CONTAINS ||
        op == TOK_STARTS || op == TOK_ENDS) {
        advance();
        if (op == TOK_STARTS || op == TOK_ENDS) match(TOK_WITH);
        cypher_ast_t *r = parse_arithmetic();
        cypher_ast_t *b = new_ast(AST_BINARY);
        b->bin.op = (char)op;
        b->bin.l = l;
        b->bin.r = r;
        return b;
    }
    if (op == TOK_IS) {
        advance();
        int not = match(TOK_NOT);
        cypher_ast_t *b = new_ast(AST_BINARY);
        if (match(TOK_NULL)) {
            b->bin.op = not ? TOK_NEQ : TOK_EQ;
            b->bin.l = l;
            b->bin.r = new_ast(AST_NULL);
        }
        return b;
    }
    return l;
}

static cypher_ast_t *parse_not(void) {
    if (match(TOK_NOT)) {
        cypher_ast_t *a = new_ast(AST_NOT);
        a->bin.l = parse_comparison();
        return a;
    }
    return parse_comparison();
}

static cypher_ast_t *parse_and(void) {
    cypher_ast_t *l = parse_not();
    while (match(TOK_AND)) {
        cypher_ast_t *b = new_ast(AST_BINARY);
        b->bin.op = TOK_AND;
        b->bin.l = l;
        b->bin.r = parse_not();
        l = b;
    }
    return l;
}

static cypher_ast_t *parse_xor(void) {
    cypher_ast_t *l = parse_and();
    while (match(TOK_XOR)) {
        cypher_ast_t *b = new_ast(AST_BINARY);
        b->bin.op = TOK_XOR;
        b->bin.l = l;
        b->bin.r = parse_and();
        l = b;
    }
    return l;
}

static cypher_ast_t *parse_expression(void) {
    cypher_ast_t *l = parse_xor();
    while (match(TOK_OR)) {
        cypher_ast_t *b = new_ast(AST_BINARY);
        b->bin.op = TOK_OR;
        b->bin.l = l;
        b->bin.r = parse_xor();
        l = b;
    }
    return l;
}

static cypher_ast_t *parse_node_pattern(void) {
    if (!match(TOK_LPAREN)) return NULL;
    cypher_ast_t *n = new_ast(AST_NODE_PAT);
    /* variable — consume identifier if present (may be followed by colon/label) */
    if (cur() == TOK_IDENT) {
        n->node.name = new_ast(AST_IDENT);
        strcpy(n->node.name->str, toks[ti].str);
        advance();
    }
    /* labels */
    while (match(TOK_COLON)) {
        if (cur() == TOK_IDENT) {
            cypher_ast_t *lab = new_ast(AST_LABEL);
            strcpy(lab->str, toks[ti].str);
            lab->next = n->node.props;
            n->node.props = lab;
            advance();
        }
    }
    /* properties — append to existing props chain (may already have labels) */
    if (match(TOK_LBRACE)) {
        cypher_ast_t *map = NULL, *map_tail = NULL;
        while (cur() != TOK_RBRACE && cur() != TOK_EOF) {
            if (cur() == TOK_IDENT) {
                cypher_ast_t *k = new_ast(AST_STRING);
                strcpy(k->str, toks[ti].str);
                advance();
                if (!match(TOK_COLON)) break;
                cypher_ast_t *v = parse_atom();
                if (!v) break;
                cypher_ast_t *e = new_ast(AST_MAP_ENTRY);
                e->pair.l = k; e->pair.r = v;
                if (!map) map = map_tail = e;
                else { map_tail->next = e; map_tail = e; }
            } else if (cur() == TOK_COMMA) {
                advance();
            } else {
                break;
            }
        }
        expect(TOK_RBRACE);
        /* append map to end of props chain */
        if (n->node.props) {
            cypher_ast_t *p = n->node.props;
            while (p->next) p = p->next;
            p->next = map;
        } else {
            n->node.props = map;
        }
    }
    expect(TOK_RPAREN);
    return n;
}

static cypher_ast_t *parse_rel_detail(void) {
    if (!match(TOK_LBRACKET)) return NULL;
    cypher_ast_t *r = new_ast(AST_REL_PAT);
    r->rel.varlen_min = 1;
    r->rel.varlen_max = 1;
    /* variable */
    if (cur() == TOK_IDENT) {
        r->rel.name = new_ast(AST_IDENT);
        strcpy(r->rel.name->str, toks[ti].str);
        advance();
    }
    /* types */
    while (match(TOK_COLON)) {
        if (cur() == TOK_IDENT) {
            cypher_ast_t *lab = new_ast(AST_LABEL);
            strcpy(lab->str, toks[ti].str);
            lab->next = r->rel.labels;
            r->rel.labels = lab;
            advance();
        }
    }
    /* type union: :T1|T2 */
    while (match(TOK_PIPE)) {
        match(TOK_COLON);
        if (cur() == TOK_IDENT) {
            cypher_ast_t *lab = new_ast(AST_LABEL);
            strcpy(lab->str, toks[ti].str);
            lab->next = r->rel.labels;
            r->rel.labels = lab;
            advance();
        }
    }
    /* variable-length: *min..max */
    if (match(TOK_STAR)) {
        r->rel.varlen_min = 0;
        r->rel.varlen_max = -1;
        if (cur() == TOK_INTEGER) {
            r->rel.varlen_min = toks[ti].ival;
            r->rel.varlen_max = toks[ti].ival;
            advance();
            if (cur() == TOK_DOTDOT) {
                advance();
                r->rel.varlen_max = -1;
                if (cur() == TOK_INTEGER) {
                    r->rel.varlen_max = toks[ti].ival;
                    advance();
                }
            }
        } else if (cur() == TOK_DOTDOT) {
            advance();
            r->rel.varlen_min = 0;
            r->rel.varlen_max = -1;
            if (cur() == TOK_INTEGER) {
                r->rel.varlen_max = toks[ti].ival;
                advance();
            }
        }
    }
    expect(TOK_RBRACKET);
    return r;
}

static cypher_ast_t *parse_pattern(void) {
    cypher_ast_t *p = new_ast(AST_PATTERN);
    cypher_ast_t *tail = NULL;

    cypher_ast_t *first = parse_node_pattern();
    if (!first) return NULL;
    p->list.items = malloc(sizeof(cypher_ast_t *) * 64);
    p->list.items[p->list.n++] = first;
    tail = first;

    while (cur() == TOK_DASH || cur() == TOK_MINUS || cur() == TOK_LT || cur() == TOK_LBRACKET) {
        int dir = 0;
        if (match(TOK_LT)) dir = -1;  /* <-- */

        cypher_ast_t *rel = NULL;
        if (cur() == TOK_DASH || cur() == TOK_MINUS) {
            advance();
            if (cur() == TOK_LBRACKET) rel = parse_rel_detail();
            if (!(match(TOK_DASH) || match(TOK_MINUS))) break;
        } else if (cur() == TOK_LBRACKET) {
            rel = parse_rel_detail();
            if (!(match(TOK_DASH) || match(TOK_MINUS))) break;
        }

        if (match(TOK_GT)) dir = 1;  /* --> */

        if (rel) {
            rel->rel.dir = dir;
            p->list.items[p->list.n++] = rel;
        }

        cypher_ast_t *next = parse_node_pattern();
        if (!next) break;
        p->list.items[p->list.n++] = next;
        tail = next;
    }
    return p;
}

static cypher_ast_t *parse_match(void) {
    int opt = match(TOK_OPTIONAL);
    if (!match(TOK_MATCH)) return NULL;
    if (!opt) opt = match(TOK_OPTIONAL);

    cypher_ast_t *p = parse_pattern();
    if (!p) { if (opt) return NULL; else return NULL; }

    /* handle comma-separated patterns: store as linked list */
    cypher_ast_t *ptail = p;
    while (match(TOK_COMMA)) {
        cypher_ast_t *next = parse_pattern();
        if (!next) break;
        if (ptail->next) {
            /* find tail of next chain */
            cypher_ast_t *nt = ptail->next;
            while (nt->next) nt = nt->next;
            nt->next = next;
        } else {
            ptail->next = next;
        }
        ptail = next;
    }

    cypher_ast_t *w = NULL;
    if (match(TOK_WHERE)) w = parse_expression();

    cypher_ast_t *m = new_ast(AST_MATCH);
    m->bin.op = opt;
    m->bin.l = p;
    m->bin.r = w;
    return m;
}

static cypher_ast_t *parse_return(void) {
    if (!match(TOK_RETURN)) return NULL;
    int distinct = match(TOK_DISTINCT);

    cypher_ast_t *r = new_ast(AST_RETURN);

    int ncols = 0;
    cypher_ast_t *cols[64];

    if (match(TOK_STAR)) {
        cypher_ast_t *c = new_ast(AST_COLUMN);
        c->col.name = new_ast(AST_IDENT);
        strcpy(c->col.name->str, "*");
        cols[ncols++] = c;
    } else {
        do {
            if (match(TOK_COMMA)) continue;

            cypher_ast_t *expr = parse_expression();
            if (!expr) break;

            cypher_ast_t *c = new_ast(AST_COLUMN);
            c->col.name = expr;
            if (match(TOK_AS)) {
                cypher_ast_t *as = new_ast(AST_IDENT);
                if (cur() == TOK_IDENT) {
                    strcpy(as->str, toks[ti].str);
                    advance();
                }
                c->col.as = as;
            }
            cols[ncols++] = c;
        } while (cur() != TOK_ORDER && cur() != TOK_SKIP &&
                 cur() != TOK_LIMIT && cur() != TOK_SEMI &&
                 cur() != TOK_EOF && cur() != TOK_UNION);
    }

    /* ORDER BY */
    if (match(TOK_ORDER)) {
        expect(TOK_BY);
        do {
            cypher_ast_t *expr = parse_expression();
            int desc = 0;
            if (match(TOK_DESC)) desc = 1;
            else if (match(TOK_ASC)) desc = 0;
            cypher_ast_t *oi = new_ast(AST_ORDER_ITEM);
            oi->bin.l = expr;
            oi->bin.op = desc;
            oi->next = r->bin.r;
            r->bin.r = oi;
        } while (match(TOK_COMMA));
    }

    /* SKIP */
    if (match(TOK_SKIP)) {
        cypher_ast_t *amt = parse_atom();
        cypher_ast_t *s = new_ast(AST_SKIP);
        s->ival = 0;
        if (amt) {
            if (amt->type == AST_INTEGER) s->ival = amt->ival;
            else if (amt->type == AST_FLOAT) s->ival = (int)amt->fval;
        }
        cypher_ast_free(amt);
        s->next = r->next;
        r->next = s;
    }
    /* LIMIT */
    if (match(TOK_LIMIT)) {
        cypher_ast_t *amt = parse_atom();
        cypher_ast_t *l = new_ast(AST_LIMIT);
        l->ival = 100;
        if (amt) {
            if (amt->type == AST_INTEGER) l->ival = amt->ival;
            else if (amt->type == AST_FLOAT) l->ival = (int)amt->fval;
        }
        cypher_ast_free(amt);
        l->next = r->next;
        r->next = l;
    }

    r->list.n = ncols;
    r->list.items = malloc(sizeof(cypher_ast_t *) * (size_t)(ncols + 1));
    for (int i = 0; i < ncols; i++) r->list.items[i] = cols[i];
    r->rel.dir = distinct;
    return r;
}

static cypher_ast_t *parse_create(void) {
    if (!match(TOK_CREATE)) return NULL;
    cypher_ast_t *p = parse_pattern();
    cypher_ast_t *c = new_ast(AST_CREATE);
    c->bin.l = p;
    return c;
}

static cypher_ast_t *parse_set(void) {
    if (!match(TOK_SET)) return NULL;
    cypher_ast_t *s = new_ast(AST_SET);
    cypher_ast_t *items[64]; int n = 0;
    do {
        if (match(TOK_COMMA)) continue;
        cypher_ast_t *l = parse_atom();
        if (!l) break;
        l = parse_postfix(l);

        /* SET n:Label — label assignment */
        if (cur() == TOK_COLON) {
            cypher_ast_t *b = new_ast(AST_BINARY);
            b->bin.op = TOK_COLON;
            b->bin.l = l;
            /* parse labels */
            cypher_ast_t *labels = NULL, *ltail = NULL;
            while (match(TOK_COLON)) {
                if (cur() == TOK_IDENT) {
                    cypher_ast_t *lab = new_ast(AST_LABEL);
                    strcpy(lab->str, toks[ti].str);
                    advance();
                    if (!labels) labels = ltail = lab;
                    else { ltail->next = lab; ltail = lab; }
                }
            }
            b->bin.r = labels;
            items[n++] = b;
            continue;
        }

        if (!match(TOK_EQ)) { cypher_ast_free(l); break; }
        cypher_ast_t *r = parse_expression();
        if (!r) { cypher_ast_free(l); break; }
        cypher_ast_t *b = new_ast(AST_BINARY);
        b->bin.op = TOK_EQ; b->bin.l = l; b->bin.r = r;
        items[n++] = b;
    } while (cur() == TOK_COMMA);
    s->list.n = n;
    s->list.items = malloc(sizeof(cypher_ast_t *) * (size_t)(n + 1));
    for (int i = 0; i < n; i++) s->list.items[i] = items[i];
    return s;
}

static cypher_ast_t *parse_delete(void) {
    int detached = match(TOK_DETACH);
    if (!match(TOK_DELETE)) { if (detached) return NULL; else return NULL; }
    if (!detached) detached = match(TOK_DETACH);
    cypher_ast_t *d = new_ast(AST_DELETE);
    d->bin.op = detached ? 1 : 0;
    d->bin.l = parse_expression();
    while (match(TOK_COMMA)) {
        cypher_ast_t *e = parse_expression();
        e->next = d->bin.r;
        d->bin.r = e;
    }
    return d;
}

static cypher_ast_t *parse_merge(void) {
    if (!match(TOK_MERGE)) return NULL;
    cypher_ast_t *p = parse_pattern();
    cypher_ast_t *m = new_ast(AST_MERGE);
    m->bin.l = p;
    return m;
}

static cypher_ast_t *parse_unwind(void) {
    if (!match(TOK_UNWIND)) return NULL;
    cypher_ast_t *expr = parse_expression();
    if (!expect(TOK_AS)) return NULL;  // consume AS
    cypher_ast_t *var = new_ast(AST_IDENT);
    if (cur() == TOK_IDENT) { strcpy(var->str, toks[ti].str); advance(); }
    else return NULL;
    
    cypher_ast_t *u = new_ast(AST_UNWIND);
    u->bin.l = expr;
    u->bin.r = var;
    return u;
}

// CASE expression parser
static cypher_ast_t *parse_case(void) {
    if (!match(TOK_CASE)) return NULL;
    
    cypher_ast_t *case_node = new_ast(AST_CASE);
    cypher_ast_t *head = NULL, *tail = NULL;
    
    // Parse WHEN/THEN pairs
    while (cur() == TOK_WHEN) {
        advance(); // consume WHEN
        cypher_ast_t *cond = parse_expression();
        if (!expect(TOK_THEN)) { cypher_ast_free(cond); break; }
        cypher_ast_t *result = parse_expression();
        
        cypher_ast_t *when = new_ast(AST_BINARY);
        when->bin.op = TOK_WHEN;
        when->bin.l = cond;
        when->bin.r = result;
        
        when->next = NULL;
        if (!head) head = tail = when;
        else { tail = tail->next = when; }
    }
    
    // Parse optional ELSE
    cypher_ast_t *else_result = NULL;
    if (match(TOK_ELSE)) {
        else_result = parse_expression();
    }
    
    if (!expect(TOK_END)) {
        // cleanup
    }
    
    case_node->bin.l = head;  // linked list of WHEN/THEN
    case_node->bin.r = else_result;
    return case_node;
}

static cypher_ast_t *parse_with(void) {
    if (!match(TOK_WITH)) return NULL;
    match(TOK_DISTINCT);

    cypher_ast_t *w = new_ast(AST_WITH);

    int ncols = 0;
    cypher_ast_t *cols[64];

    if (match(TOK_STAR)) {
        cypher_ast_t *c = new_ast(AST_COLUMN);
        c->col.name = new_ast(AST_IDENT);
        strcpy(c->col.name->str, "*");
        cols[ncols++] = c;
    } else {
        do {
            if (match(TOK_COMMA)) continue;
            cypher_ast_t *expr = parse_expression();
            if (!expr) break;
            cypher_ast_t *c = new_ast(AST_COLUMN);
            c->col.name = expr;
            if (match(TOK_AS)) {
                cypher_ast_t *as = new_ast(AST_IDENT);
                if (cur() == TOK_IDENT) {
                    strcpy(as->str, toks[ti].str);
                    advance();
                }
                c->col.as = as;
            }
            cols[ncols++] = c;
        } while (cur() != TOK_ORDER && cur() != TOK_SKIP &&
                 cur() != TOK_LIMIT && cur() != TOK_SEMI &&
                 cur() != TOK_EOF && cur() != TOK_MATCH &&
                 cur() != TOK_RETURN && cur() != TOK_CREATE);
    }

    /* ORDER BY */
    if (match(TOK_ORDER)) {
        expect(TOK_BY);
        do {
            cypher_ast_t *expr = parse_expression();
            int desc = 0;
            if (match(TOK_DESC)) desc = 1;
            else if (match(TOK_ASC)) desc = 0;
            cypher_ast_t *oi = new_ast(AST_ORDER_ITEM);
            oi->bin.l = expr;
            oi->bin.op = desc;
            oi->next = w->bin.r;
            w->bin.r = oi;
        } while (match(TOK_COMMA));
    }

    /* SKIP */
    if (match(TOK_SKIP)) {
        cypher_ast_t *amt = parse_atom();
        cypher_ast_t *s = new_ast(AST_SKIP);
        s->ival = (amt && amt->type == AST_INTEGER) ? amt->ival : 0;
        cypher_ast_free(amt);
        s->next = w->next;
        w->next = s;
    }
    /* LIMIT */
    if (match(TOK_LIMIT)) {
        cypher_ast_t *amt = parse_atom();
        cypher_ast_t *l = new_ast(AST_LIMIT);
        l->ival = (amt && amt->type == AST_INTEGER) ? amt->ival : 100;
        cypher_ast_free(amt);
        l->next = w->next;
        w->next = l;
    }

    w->list.n = ncols;
    w->list.items = malloc(sizeof(cypher_ast_t *) * (size_t)(ncols + 1));
    for (int i = 0; i < ncols; i++) w->list.items[i] = cols[i];
    return w;
}

static cypher_ast_t *parse_remove(void) {
    if (!match(TOK_REMOVE)) return NULL;
    cypher_ast_t *s = new_ast(AST_REMOVE);
    cypher_ast_t *items[64]; int n = 0;
    do {
        if (match(TOK_COMMA)) continue;
        cypher_ast_t *expr = parse_atom();
        if (!expr) break;
        expr = parse_postfix(expr);
        items[n++] = expr;
    } while (cur() == TOK_COMMA);
    s->list.n = n;
    s->list.items = malloc(sizeof(cypher_ast_t *) * (size_t)(n + 1));
    for (int i = 0; i < n; i++) s->list.items[i] = items[i];
    return s;
}

static cypher_ast_t *parse_clause(void) {
    switch (cur()) {
    case TOK_MATCH:  return parse_match();
    case TOK_OPTIONAL: return parse_match();
    case TOK_RETURN: return parse_return();
    case TOK_CREATE: return parse_create();
    case TOK_SET:    return parse_set();
    case TOK_DELETE: return parse_delete();
    case TOK_DETACH: return parse_delete();
    case TOK_MERGE:  return parse_merge();
    case TOK_UNWIND: return parse_unwind();
    case TOK_WITH:   return parse_with();
    case TOK_REMOVE: return parse_remove();
    default: return NULL;
    }
}

static int is_clause_start(int tok) {
    switch (tok) {
    case TOK_MATCH: case TOK_OPTIONAL: case TOK_RETURN:
    case TOK_CREATE: case TOK_SET: case TOK_DELETE: case TOK_DETACH:
    case TOK_MERGE: case TOK_UNWIND: case TOK_WITH:
    case TOK_EOF: case TOK_SEMI:
        return 1;
    default: return 0;
    }
}

cypher_ast_t *cypher_parse(cypher_token_t *tokens, int n, const char **error) {
    toks = tokens; tn = n; ti = 0;
    err = error; *error = NULL;

    cypher_ast_t *head = NULL, *tail = NULL;

    while (cur() != TOK_EOF) {
        while (cur() == TOK_SEMI) advance();
        if (cur() == TOK_EOF) break;
        cypher_ast_t *cl = parse_clause();
        if (!cl) {
            if (!*error) *error = "unexpected token before clause";
            while (cur() != TOK_EOF && !is_clause_start(cur())) advance();
            continue;
        }
        if (!head) head = tail = cl;
        else { tail->next = cl; tail = cl; }
    }
    return head;
}

void cypher_ast_free(cypher_ast_t *a) {
    if (!a) return;
    cypher_ast_free(a->next);
    switch (a->type) {
    case AST_BINARY:
        cypher_ast_free(a->bin.l);
        cypher_ast_free(a->bin.r);
        break;
    case AST_NOT:
    case AST_UNARY:
        cypher_ast_free(a->una.a);
        break;
    case AST_PROP:
        cypher_ast_free(a->prop.e);
        cypher_ast_free(a->prop.n);
        break;
    case AST_NODE_PAT:
        cypher_ast_free(a->node.name);
        cypher_ast_free(a->node.props);
        break;
    case AST_REL_PAT:
        cypher_ast_free(a->rel.name);
        cypher_ast_free(a->rel.props);
        cypher_ast_free(a->rel.labels);
        break;
    case AST_PATTERN:
    case AST_SET:
        for (int i = 0; i < a->list.n; i++) cypher_ast_free(a->list.items[i]);
        free(a->list.items);
        break;
    case AST_CREATE:
    case AST_MERGE:
    case AST_UNWIND:
        cypher_ast_free(a->bin.l);
        cypher_ast_free(a->bin.r);
        break;
    case AST_CASE:
        cypher_ast_free(a->bin.l);
        cypher_ast_free(a->bin.r);
        break;
    case AST_RETURN:
        for (int i = 0; i < a->list.n; i++) cypher_ast_free(a->list.items[i]);
        free(a->list.items);
        cypher_ast_free(a->bin.r);  /* ORDER BY */
        break;
    case AST_WITH:
        for (int i = 0; i < a->list.n; i++) cypher_ast_free(a->list.items[i]);
        free(a->list.items);
        cypher_ast_free(a->bin.r);  /* ORDER BY */
        break;
    case AST_LIMIT:
    case AST_SKIP:
        break;
    case AST_MATCH:
        cypher_ast_free(a->bin.l);
        cypher_ast_free(a->bin.r);
        break;
    case AST_COLUMN:
        cypher_ast_free(a->col.name);
        cypher_ast_free(a->col.as);
        break;
    case AST_DELETE:
        cypher_ast_free(a->bin.l);
        cypher_ast_free(a->bin.r);
        break;
    case AST_MAP_ENTRY:
        cypher_ast_free(a->pair.l);
        cypher_ast_free(a->pair.r);
        break;
    case AST_ORDER_ITEM:
        cypher_ast_free(a->bin.l);
        break;
    case AST_LIST:
    case AST_REMOVE:
        for (int i = 0; i < a->list.n; i++) cypher_ast_free(a->list.items[i]);
        free(a->list.items);
        break;
    case AST_FUNCALL:
        cypher_ast_free(a->call.func);
        for (int i = 0; i < a->call.n; i++) cypher_ast_free(a->call.args[i]);
        free(a->call.args);
        break;
    default: break;
    }
    free(a);
}
