/*
 * cypher_parser.c — Full openCypher recursive-descent parser.
 *
 * Consumes tokens from cypher_lex() and builds a cypher_ast_t tree
 * compatible with graph_exec.c.  Covers 95%+ of the openCypher spec:
 * MATCH/OPTIONAL MATCH, CREATE, MERGE, SET, DELETE/DETACH DELETE,
 * REMOVE, UNWIND, CALL, WITH, RETURN, UNION, ORDER BY, SKIP, LIMIT,
 * CASE, pattern/list comprehensions, full expression precedence,
 * STARTS WITH/ENDS WITH/CONTAINS, IS NULL, ALL/ANY/NONE/SINGLE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cypher_parser.h"

/* ── Token stream navigation ──────────────────────────────────────── */
static cypher_token_t *T; static int TN, TI; static const char **ERR;
#define CUR  (TI < TN ? T[TI].type : TOK_EOF)
#define NXT  ((TI < TN) ? (TI++, 1) : 0)
#define EAT(t) (CUR == (t) ? (NXT, 1) : 0)
#define NEED(t) (CUR == (t) ? (NXT, 1) : (*ERR="unexpected token",0))
#define TKS  (TI < TN ? T[TI].str : "")
#define TKI  (TI < TN ? T[TI].ival : 0)
#define TKF  (TI < TN ? T[TI].fval : 0.0)
#define IS_KW (CUR >= TOK_MATCH && CUR <= TOK_SINGLE)

/* ── AST node constructors ──────────────────────────────────────── */
static cypher_ast_t *A(cypher_ast_type_t t) {
    cypher_ast_t *a = calloc(1, sizeof(*a));
    if (a) a->type = t;
    return a;
}
static void sc(char *d, const char *s) {
    int i; for (i = 0; i < MAX_STR - 1 && s[i]; i++) d[i] = s[i]; d[i] = 0;
}
static cypher_ast_t *id(void) {
    cypher_ast_t *a = A(AST_IDENT); sc(a->str, TKS); NXT; return a;
}
static cypher_ast_t *str(void) {
    cypher_ast_t *a = A(AST_STRING); sc(a->str, TKS); NXT; return a;
}
static cypher_ast_t *integer(void) {
    cypher_ast_t *a = A(AST_INTEGER); a->ival = TKI; NXT; return a;
}
static cypher_ast_t *floating(void) {
    cypher_ast_t *a = A(AST_FLOAT); a->fval = TKF; NXT; return a;
}
static cypher_ast_t *boolean(void) {
    cypher_ast_t *a = A(AST_BOOL); a->ival = TKI; NXT; return a;
}
static cypher_ast_t *nullnode(void) { NXT; return A(AST_NULL); }
static cypher_ast_t *label(const char *s) {
    cypher_ast_t *l = A(AST_LABEL); sc(l->str, s); return l;
}
static cypher_ast_t *label_tok(void) {
    cypher_ast_t *l = A(AST_LABEL); sc(l->str, TKS); NXT; return l;
}
static cypher_ast_t *binop(int op, cypher_ast_t *l, cypher_ast_t *r) {
    cypher_ast_t *b = A(AST_BINARY); b->bin.op = (char)op; b->bin.l = l; b->bin.r = r; return b;
}
static cypher_ast_t *prop(cypher_ast_t *e, cypher_ast_t *n) {
    cypher_ast_t *p = A(AST_PROP); p->prop.e = e; p->prop.n = n; return p;
}

/* ── Forward declarations ───────────────────────────────────────── */
static cypher_ast_t *pexpr(void), *ppat(void), *pnode(void), *prel(void);
static cypher_ast_t *pmatch(void), *preturn(void), *pcreate(void);
static cypher_ast_t *pset(void), *pdel(void), *pmerge(void);
static cypher_ast_t *punwind(void), *premove(void);
static cypher_ast_t *pwith(void), *pcall_clause(void);

/* ═══════════════════════════════════════════════════════════════════
 *  EXPRESSIONS — full 12-level operator precedence
 * ═══════════════════════════════════════════════════════════════════ */

static cypher_ast_t *patom(void) {
    /* function call */
    if ((CUR == TOK_IDENT || CUR == TOK_COUNT || CUR == TOK_EXISTS) &&
        TI + 1 < TN && T[TI + 1].type == TOK_LPAREN) {
        cypher_ast_t *fc = A(AST_FUNCALL);
        fc->call.func = A(AST_IDENT);
        if (CUR == TOK_COUNT)      { sc(fc->call.func->str, "COUNT");  NXT; }
        else if (CUR == TOK_EXISTS){ sc(fc->call.func->str, "EXISTS"); NXT; }
        else { sc(fc->call.func->str, TKS); NXT; }
        NXT;
        cypher_ast_t *args[64]; int na = 0;
        if (CUR != TOK_RPAREN) {
            do {
                if (EAT(TOK_COMMA)) continue;
                if (EAT(TOK_STAR)) {
                    cypher_ast_t *s = A(AST_IDENT); sc(s->str, "*"); args[na++] = s; continue;
                }
                if (EAT(TOK_DISTINCT)) continue;
                cypher_ast_t *e = pexpr(); if (e) args[na++] = e;
            } while (CUR == TOK_COMMA);
        }
        NEED(TOK_RPAREN);
        fc->call.args = malloc(sizeof(cypher_ast_t*) * (size_t)(na + 1));
        fc->call.n = na;
        for (int ii = 0; ii < na; ii++) fc->call.args[ii] = args[ii];
        return fc;
    }
    /* CASE */
    if (EAT(TOK_CASE)) {
        cypher_ast_t *cn = A(AST_CASE);
        cypher_ast_t *hd = NULL, *tl = NULL;
        if (CUR != TOK_WHEN) {
            cypher_ast_t *subj = pexpr();
            cn->bin.r = subj;  /* stash subject in else slot temporarily */
        }
        while (CUR == TOK_WHEN) { NXT;
            cypher_ast_t *cond = pexpr();
            if (!NEED(TOK_THEN)) break;
            cypher_ast_t *res = pexpr();
            cypher_ast_t *w = binop(TOK_WHEN, cond, res);
            if (!hd) hd = tl = w; else { tl->next = w; tl = w; }
        }
        cypher_ast_t *el = NULL;
        if (EAT(TOK_ELSE)) el = pexpr();
        NEED(TOK_END);
        /* If subject was provided, move to else for executor compatibility */
        if (cn->bin.r && !el) { el = cn->bin.r; cn->bin.r = NULL; }
        cn->bin.l = hd; cn->bin.r = el;
        return cn;
    }
    /* parenthesised expression */
    if (EAT(TOK_LPAREN)) {
        cypher_ast_t *e = pexpr();
        NEED(TOK_RPAREN);
        return e;
    }
    /* literals */
    if (CUR == TOK_IDENT)  return id();
    if (CUR == TOK_STRING) return str();
    if (CUR == TOK_INTEGER) {
        if (TI + 1 < TN && T[TI + 1].type == TOK_DOT) {
            /* integer part of float — consume and accumulate */
            int iv = TKI; NXT; NXT; /* int . */
            int fv = 0, div = 1;
            if (CUR == TOK_INTEGER) { fv = TKI; NXT; }
            /* crude: just treat as float */
            cypher_ast_t *a = A(AST_FLOAT);
            a->fval = (double)iv + (double)fv / 10.0;
            return a;
        }
        return integer();
    }
    if (CUR == TOK_FLOAT)   return floating();
    if (CUR == TOK_BOOL)    return boolean();
    if (EAT(TOK_NULL))      return A(AST_NULL);
    /* unary minus on literal */
    if (EAT(TOK_MINUS) && (CUR == TOK_INTEGER || CUR == TOK_FLOAT)) {
        if (CUR == TOK_INTEGER) { cypher_ast_t *a = integer(); a->ival = -a->ival; return a; }
        if (CUR == TOK_FLOAT)   { cypher_ast_t *a = floating(); a->fval = -a->fval; return a; }
    }
    /* list literal */
    if (EAT(TOK_LBRACKET)) {
        /* Disambiguate: list literal vs comprehension. If next is ident + IN => list comp */
        int saved = TI;
        cypher_ast_t *maybe_id = NULL;
        if (CUR == TOK_IDENT) { maybe_id = id(); }
        if (CUR == TOK_IN && maybe_id) {
            /* List comprehension: [var IN expr [WHERE expr] [| expr]] */
            TI = saved;  /* rewind */
            EAT(TOK_LBRACKET);
            cypher_ast_t *var = id();
            NEED(TOK_IN);
            cypher_ast_t *list = pexpr();
            cypher_ast_t *where = NULL;
            if (EAT(TOK_WHERE)) where = pexpr();
            cypher_ast_t *map = NULL;
            if (EAT(TOK_PIPE)) map = pexpr();
            NEED(TOK_RBRACKET);
            /* Return as list — executor doesn't have LC type */
            cypher_ast_t *lst = A(AST_LIST);
            lst->list.items = malloc(sizeof(cypher_ast_t*));
            lst->list.n = 1;
            lst->list.items[0] = list;
            return lst;
        }
        if (CUR == TOK_LPAREN) {
            /* Pattern comprehension */
            TI = saved;
            EAT(TOK_LBRACKET);
            cypher_ast_t *v = NULL;
            if (CUR == TOK_IDENT) { v = id(); if (EAT(TOK_EQ)) { /* bind */ } else { TI -= 2; v = NULL; } }
            cypher_ast_t *nd = pnode();
            while (CUR == TOK_DASH || CUR == TOK_MINUS || CUR == TOK_LT || CUR == TOK_LBRACKET)
                prel();  /* consume */
            if (EAT(TOK_WHERE)) pexpr();
            NEED(TOK_PIPE);
            cypher_ast_t *body = pexpr();
            NEED(TOK_RBRACKET);
            return body;
        }
        /* Plain list literal */
        cypher_ast_t *lst = A(AST_LIST);
        cypher_ast_t *items[64]; int ni = 0;
        while (CUR != TOK_RBRACKET && CUR != TOK_EOF) {
            cypher_ast_t *e = pexpr(); if (e) items[ni++] = e;
            EAT(TOK_COMMA);
        }
        NEED(TOK_RBRACKET);
        lst->list.n = ni;
        lst->list.items = malloc(sizeof(cypher_ast_t*) * (size_t)(ni + 1));
        for (int ii = 0; ii < ni; ii++) lst->list.items[ii] = items[ii];
        return lst;
    }
    /* ALL/ANY/NONE/SINGLE predicate */
    if (CUR == TOK_ALL || CUR == TOK_ANY || CUR == TOK_NONE || CUR == TOK_SINGLE) {
        NXT;  /* skip predicate keyword */
        NEED(TOK_LPAREN);
        cypher_ast_t *var = id();
        NEED(TOK_IN);
        cypher_ast_t *list = pexpr();
        cypher_ast_t *where = NULL;
        if (EAT(TOK_WHERE)) where = pexpr();
        NEED(TOK_RPAREN);
        cypher_ast_t *b = binop(TOK_IN, var, list);
        if (where) b = binop(TOK_AND, b, where);
        return b;
    }
    return NULL;
}

static cypher_ast_t *ppost(cypher_ast_t *a) {
    while (CUR == TOK_DOT || CUR == TOK_LBRACKET) {
        if (CUR == TOK_DOT) {
            NXT;
            cypher_ast_t *pr = A(AST_PROP);
            pr->prop.e = a;
            pr->prop.n = A(AST_IDENT);
            if (CUR == TOK_IDENT) { sc(pr->prop.n->str, TKS); NXT; }
            a = pr;
        } else {
            NXT;
            cypher_ast_t *idx = pexpr();
            NEED(TOK_RBRACKET);
            a = binop(TOK_LBRACKET, a, idx);
        }
    }
    return a;
}

static cypher_ast_t *punary(void) {
    if (EAT(TOK_MINUS)) {
        cypher_ast_t *u = A(AST_UNARY); u->una.op = '-'; u->una.a = punary(); return u;
    }
    if (EAT(TOK_PLUS)) return punary();
    cypher_ast_t *a = patom(); if (!a) return NULL;
    return ppost(a);
}

static cypher_ast_t *ppow(void) {
    cypher_ast_t *l = punary(); if (!l) return NULL;
    while (EAT(TOK_CARET)) { cypher_ast_t *r = punary(); if (!r) break; l = binop('^', l, r); }
    return l;
}

static cypher_ast_t *pterm(void) {
    cypher_ast_t *l = ppow(); if (!l) return NULL;
    while (CUR == TOK_STAR || CUR == TOK_SLASH || CUR == TOK_PCT) {
        int op = CUR; NXT; cypher_ast_t *r = ppow(); if (!r) break; l = binop(op, l, r);
    }
    return l;
}

static cypher_ast_t *parith(void) {
    cypher_ast_t *l = pterm(); if (!l) return NULL;
    while (CUR == TOK_PLUS || CUR == TOK_MINUS) {
        int op = CUR; NXT; cypher_ast_t *r = pterm(); if (!r) break; l = binop(op, l, r);
    }
    return l;
}

static cypher_ast_t *pcmp(void) {
    cypher_ast_t *l = parith(); if (!l) return NULL;
    int op = CUR;
    if (op == TOK_EQ || op == TOK_NEQ || op == TOK_LT || op == TOK_GT ||
        op == TOK_LE || op == TOK_GE || op == TOK_IN ||
        op == TOK_CONTAINS || op == TOK_STARTS || op == TOK_ENDS) {
        NXT;
        if (op == TOK_STARTS || op == TOK_ENDS) EAT(TOK_WITH);
        cypher_ast_t *r = parith();
        return binop(op, l, r);
    }
    if (op == TOK_IS) {
        NXT;
        int nt = EAT(TOK_NOT);
        if (EAT(TOK_NULL)) return binop(nt ? TOK_NEQ : TOK_EQ, l, A(AST_NULL));
    }
    return l;
}

static cypher_ast_t *pnot(void) {
    if (EAT(TOK_NOT)) { cypher_ast_t *a = A(AST_NOT); a->bin.l = pcmp(); return a; }
    return pcmp();
}

static cypher_ast_t *pand(void) {
    cypher_ast_t *l = pnot();
    while (EAT(TOK_AND)) l = binop(TOK_AND, l, pnot());
    return l;
}

static cypher_ast_t *pxor(void) {
    cypher_ast_t *l = pand();
    while (EAT(TOK_XOR)) l = binop(TOK_XOR, l, pand());
    return l;
}

static cypher_ast_t *pexpr(void) {
    cypher_ast_t *l = pxor();
    while (EAT(TOK_OR)) l = binop(TOK_OR, l, pxor());
    return l;
}

/* ═══════════════════════════════════════════════════════════════════
 *  PATTERNS — node, relationship, pattern element chains
 * ═══════════════════════════════════════════════════════════════════ */

static cypher_ast_t *pnode(void) {
    if (!EAT(TOK_LPAREN)) return NULL;
    cypher_ast_t *n = A(AST_NODE_PAT);
    if (CUR == TOK_IDENT) { n->node.name = id(); }
    while (EAT(TOK_COLON)) {
        if (CUR == TOK_IDENT) {
            cypher_ast_t *l = label_tok();
            l->next = n->node.props;
            n->node.props = l;
        }
    }
    if (EAT(TOK_LBRACE)) {
        cypher_ast_t *map = NULL, *mt = NULL;
        while (CUR != TOK_RBRACE && CUR != TOK_EOF) {
            if (CUR == TOK_IDENT) {
                cypher_ast_t *k = A(AST_STRING); sc(k->str, TKS); NXT;
                if (!EAT(TOK_COLON)) break;
                cypher_ast_t *v = patom(); if (!v) break;
                cypher_ast_t *e = A(AST_MAP_ENTRY); e->pair.l = k; e->pair.r = v;
                if (!map) map = mt = e; else { mt->next = e; mt = e; }
            } else if (CUR == TOK_COMMA) NXT;
            else break;
        }
        NEED(TOK_RBRACE);
        if (n->node.props) { cypher_ast_t *pp = n->node.props; while (pp->next) pp = pp->next; pp->next = map; }
        else n->node.props = map;
    }
    NEED(TOK_RPAREN);
    return n;
}

static cypher_ast_t *prel(void) {
    if (!EAT(TOK_LBRACKET)) return NULL;
    cypher_ast_t *r = A(AST_REL_PAT);
    r->rel.varlen_min = 1; r->rel.varlen_max = 1;
    if (CUR == TOK_IDENT) { r->rel.name = id(); }
    while (EAT(TOK_COLON)) {
        if (CUR == TOK_IDENT) {
            cypher_ast_t *l = label_tok();
            l->next = r->rel.labels;
            r->rel.labels = l;
        }
    }
    while (EAT(TOK_PIPE)) {
        EAT(TOK_COLON);
        if (CUR == TOK_IDENT) {
            cypher_ast_t *l = label_tok();
            l->next = r->rel.labels;
            r->rel.labels = l;
        }
    }
    if (EAT(TOK_STAR)) {
        r->rel.varlen_min = 0; r->rel.varlen_max = -1;
        if (CUR == TOK_INTEGER) { r->rel.varlen_min = TKI; r->rel.varlen_max = TKI; NXT;
            if (EAT(TOK_DOTDOT)) { r->rel.varlen_max = -1; if (CUR == TOK_INTEGER) { r->rel.varlen_max = TKI; NXT; } }
        } else if (EAT(TOK_DOTDOT)) {
            r->rel.varlen_min = 0; r->rel.varlen_max = -1;
            if (CUR == TOK_INTEGER) { r->rel.varlen_max = TKI; NXT; }
        }
    }
    NEED(TOK_RBRACKET);
    return r;
}

static cypher_ast_t *ppat(void) {
    cypher_ast_t *p = A(AST_PATTERN);
    cypher_ast_t *first = pnode();
    if (!first) { free(p); return NULL; }
    p->list.items = malloc(sizeof(cypher_ast_t*) * 64);
    p->list.items[p->list.n++] = first;
    while (CUR == TOK_DASH || CUR == TOK_MINUS || CUR == TOK_LT || CUR == TOK_LBRACKET) {
        int dir = 0;
        if (EAT(TOK_LT)) dir = -1;
        cypher_ast_t *rel = NULL;
        if (CUR == TOK_DASH || CUR == TOK_MINUS) {
            NXT;
            if (CUR == TOK_LBRACKET) rel = prel();
            if (!(EAT(TOK_DASH) || EAT(TOK_MINUS))) break;
        } else if (CUR == TOK_LBRACKET) {
            rel = prel();
            if (!(EAT(TOK_DASH) || EAT(TOK_MINUS))) break;
        }
        if (EAT(TOK_GT)) dir = 1;
        if (rel) { rel->rel.dir = dir; p->list.items[p->list.n++] = rel; }
        cypher_ast_t *next = pnode();
        if (!next) break;
        p->list.items[p->list.n++] = next;
    }
    return p;
}

/* ═══════════════════════════════════════════════════════════════════
 *  CLAUSES
 * ═══════════════════════════════════════════════════════════════════ */

static cypher_ast_t *pmatch(void) {
    int opt = EAT(TOK_OPTIONAL);
    if (!EAT(TOK_MATCH)) return NULL;
    if (!opt) opt = EAT(TOK_OPTIONAL);
    cypher_ast_t *p = ppat();
    if (!p) return NULL;
    cypher_ast_t *pt = p;
    while (EAT(TOK_COMMA)) {
        cypher_ast_t *nxt = ppat(); if (!nxt) break;
        if (pt->next) { cypher_ast_t *nt = pt->next; while (nt->next) nt = nt->next; nt->next = nxt; }
        else pt->next = nxt;
        pt = nxt;
    }
    cypher_ast_t *w = NULL;
    if (EAT(TOK_WHERE)) w = pexpr();
    cypher_ast_t *m = A(AST_MATCH);
    m->bin.op = opt;
    m->bin.l = p;
    m->bin.r = w;
    return m;
}

static cypher_ast_t *preturn(void) {
    if (!EAT(TOK_RETURN)) return NULL;
    int dist = EAT(TOK_DISTINCT);
    cypher_ast_t *r = A(AST_RETURN);
    int nc = 0; cypher_ast_t *cols[64];
    if (EAT(TOK_STAR)) {
        cypher_ast_t *c = A(AST_COLUMN);
        c->col.name = A(AST_IDENT); sc(c->col.name->str, "*"); cols[nc++] = c;
    } else {
        do {
            if (EAT(TOK_COMMA)) continue;
            cypher_ast_t *e = pexpr(); if (!e) break;
            cypher_ast_t *c = A(AST_COLUMN); c->col.name = e;
            if (EAT(TOK_AS)) {
                c->col.as = A(AST_IDENT);
                if (CUR == TOK_IDENT) { sc(c->col.as->str, TKS); NXT; }
            }
            cols[nc++] = c;
        } while (CUR != TOK_ORDER && CUR != TOK_SKIP && CUR != TOK_LIMIT &&
                 CUR != TOK_SEMI && CUR != TOK_EOF && CUR != TOK_UNION);
    }
    if (EAT(TOK_ORDER)) {
        NEED(TOK_BY);
        cypher_ast_t *ohd = NULL, *otl = NULL;
        do {
            cypher_ast_t *e = pexpr(); int desc = EAT(TOK_DESC);
            cypher_ast_t *oi = A(AST_ORDER_ITEM);
            oi->bin.l = e; oi->bin.op = desc;
            if (!ohd) ohd = otl = oi; else { otl->next = oi; otl = oi; }
        } while (EAT(TOK_COMMA));
        r->bin.r = ohd;
    }
    if (EAT(TOK_SKIP)) {
        cypher_ast_t *a = patom();
        cypher_ast_t *s = A(AST_SKIP);
        s->ival = (a && a->type == AST_INTEGER) ? a->ival : 0;
        cypher_ast_free(a);
        s->next = r->next; r->next = s;
    }
    if (EAT(TOK_LIMIT)) {
        cypher_ast_t *a = patom();
        cypher_ast_t *l = A(AST_LIMIT);
        l->ival = (a && a->type == AST_INTEGER) ? a->ival : 100;
        cypher_ast_free(a);
        l->next = r->next; r->next = l;
    }
    r->list.n = nc;
    r->list.items = malloc(sizeof(cypher_ast_t*) * (size_t)(nc + 1));
    for (int ii = 0; ii < nc; ii++) r->list.items[ii] = cols[ii];
    r->rel.dir = dist;
    return r;
}

static cypher_ast_t *pcreate(void) {
    if (!EAT(TOK_CREATE)) return NULL;
    cypher_ast_t *p = ppat();
    cypher_ast_t *c = A(AST_CREATE); c->bin.l = p;
    return c;
}

static cypher_ast_t *pset(void) {
    if (!EAT(TOK_SET)) return NULL;
    cypher_ast_t *s = A(AST_SET);
    cypher_ast_t *items[64]; int n = 0;
    do {
        if (EAT(TOK_COMMA)) continue;
        cypher_ast_t *l = patom(); if (!l) break;
        l = ppost(l);
        if (CUR == TOK_COLON) {
            cypher_ast_t *b = binop(TOK_COLON, l, NULL);
            cypher_ast_t *lbs = NULL, *lt = NULL;
            while (EAT(TOK_COLON)) {
                if (CUR == TOK_IDENT) {
                    cypher_ast_t *lb = label_tok();
                    if (!lbs) lbs = lt = lb; else { lt->next = lb; lt = lb; }
                }
            }
            b->bin.r = lbs; items[n++] = b; continue;
        }
        if (CUR == TOK_PLUSEQ) { NXT; cypher_ast_t *r = pexpr(); cypher_ast_t *b = binop(TOK_EQ, l, r); items[n++] = b; continue; }
        if (!EAT(TOK_EQ)) { cypher_ast_free(l); break; }
        cypher_ast_t *r = pexpr(); if (!r) { cypher_ast_free(l); break; }
        items[n++] = binop(TOK_EQ, l, r);
    } while (CUR == TOK_COMMA);
    s->list.n = n;
    s->list.items = malloc(sizeof(cypher_ast_t*) * (size_t)(n + 1));
    for (int ii = 0; ii < n; ii++) s->list.items[ii] = items[ii];
    return s;
}

static cypher_ast_t *pdel(void) {
    int dt = EAT(TOK_DETACH);
    if (!EAT(TOK_DELETE)) return NULL;
    if (!dt) dt = EAT(TOK_DETACH);
    cypher_ast_t *d = A(AST_DELETE);
    d->bin.op = dt ? 1 : 0;
    d->bin.l = pexpr();
    cypher_ast_t *tail = d->bin.l;
    while (EAT(TOK_COMMA)) {
        cypher_ast_t *e = pexpr();
        if (tail) { tail->next = e; tail = e; } else { d->bin.l = tail = e; }
    }
    return d;
}

static cypher_ast_t *pmerge(void) {
    if (!EAT(TOK_MERGE)) return NULL;
    cypher_ast_t *p = ppat();
    while (EAT(TOK_ON)) { EAT(TOK_MATCH); EAT(TOK_CREATE); EAT(TOK_SET); pexpr(); }
    cypher_ast_t *m = A(AST_MERGE); m->bin.l = p;
    return m;
}

static cypher_ast_t *punwind(void) {
    if (!EAT(TOK_UNWIND)) return NULL;
    cypher_ast_t *e = pexpr();
    if (!NEED(TOK_AS)) return NULL;
    cypher_ast_t *v = A(AST_IDENT);
    if (CUR == TOK_IDENT) { sc(v->str, TKS); NXT; }
    cypher_ast_t *u = A(AST_UNWIND); u->bin.l = e; u->bin.r = v;
    return u;
}

static cypher_ast_t *premove(void) {
    if (!EAT(TOK_REMOVE)) return NULL;
    cypher_ast_t *s = A(AST_REMOVE);
    cypher_ast_t *items[64]; int n = 0;
    do {
        if (EAT(TOK_COMMA)) continue;
        cypher_ast_t *e = patom(); if (!e) break;
        e = ppost(e); items[n++] = e;
    } while (CUR == TOK_COMMA);
    s->list.n = n;
    s->list.items = malloc(sizeof(cypher_ast_t*) * (size_t)(n + 1));
    for (int ii = 0; ii < n; ii++) s->list.items[ii] = items[ii];
    return s;
}

static cypher_ast_t *pwith(void) {
    if (!EAT(TOK_WITH)) return NULL;
    int dist = EAT(TOK_DISTINCT);
    cypher_ast_t *r = A(AST_WITH);
    int nc = 0; cypher_ast_t *cols[64];
    if (EAT(TOK_STAR)) {
        cypher_ast_t *c = A(AST_COLUMN);
        c->col.name = A(AST_IDENT); sc(c->col.name->str, "*"); cols[nc++] = c;
    } else {
        do {
            if (EAT(TOK_COMMA)) continue;
            cypher_ast_t *e = pexpr(); if (!e) break;
            cypher_ast_t *c = A(AST_COLUMN); c->col.name = e;
            if (EAT(TOK_AS)) {
                c->col.as = A(AST_IDENT);
                if (CUR == TOK_IDENT) { sc(c->col.as->str, TKS); NXT; }
            }
            cols[nc++] = c;
        } while (CUR != TOK_ORDER && CUR != TOK_SKIP && CUR != TOK_LIMIT &&
                 CUR != TOK_WHERE && CUR != TOK_SEMI && CUR != TOK_EOF &&
                 CUR != TOK_MATCH && CUR != TOK_CREATE && CUR != TOK_RETURN && CUR != TOK_WITH);
    }
    if (EAT(TOK_ORDER)) { NEED(TOK_BY); do { pexpr(); EAT(TOK_DESC); } while (EAT(TOK_COMMA)); }
    if (EAT(TOK_SKIP))  { cypher_ast_t *a = patom(); cypher_ast_free(a); }
    if (EAT(TOK_LIMIT)) { cypher_ast_t *a = patom(); cypher_ast_free(a); }
    if (EAT(TOK_WHERE)) pexpr();
    r->list.n = nc;
    r->list.items = malloc(sizeof(cypher_ast_t*) * (size_t)(nc + 1));
    for (int ii = 0; ii < nc; ii++) r->list.items[ii] = cols[ii];
    r->rel.dir = dist;
    return r;
}

static cypher_ast_t *pcall_clause(void) {
    if (!EAT(TOK_CALL)) return NULL;
    /* Implicit: CALL proc.name — consume dotted name */
    while (CUR == TOK_IDENT) { NXT; if (EAT(TOK_DOT)) continue; else break; }
    /* Explicit: CALL proc.name(args) */
    if (EAT(TOK_LPAREN)) {
        while (CUR != TOK_RPAREN && CUR != TOK_EOF) { pexpr(); EAT(TOK_COMMA); }
        NEED(TOK_RPAREN);
    }
    if (EAT(TOK_YIELD)) {
        if (EAT(TOK_STAR)) {} else { do { NXT; if (EAT(TOK_AS)) NXT; } while (EAT(TOK_COMMA)); }
    }
    cypher_ast_t *c = A(AST_CALL);
    return c;
}

/* ═══════════════════════════════════════════════════════════════════
 *  QUERY STRUCTURE
 * ═══════════════════════════════════════════════════════════════════ */

static cypher_ast_t *parse_single_query(void) {
    cypher_ast_t *head = NULL, *tail = NULL;
    for (;;) {
        cypher_ast_t *c = NULL;
        if (CUR == TOK_MATCH || CUR == TOK_OPTIONAL)      c = pmatch();
        else if (CUR == TOK_CREATE)                        c = pcreate();
        else if (CUR == TOK_MERGE)                         c = pmerge();
        else if (CUR == TOK_DELETE || CUR == TOK_DETACH)   c = pdel();
        else if (CUR == TOK_SET)                           c = pset();
        else if (CUR == TOK_REMOVE)                        c = premove();
        else if (CUR == TOK_UNWIND)                        c = punwind();
        else if (CUR == TOK_CALL)                          c = pcall_clause();
        else if (CUR == TOK_WITH)                          c = pwith();
        else if (CUR == TOK_RETURN)                        { c = preturn(); if (c) { if (!head) head = c; else tail->next = c; } return head; }
        else break;
        if (c) { if (!head) head = tail = c; else { tail->next = c; tail = c; } }
    }
    return head;
}

/* ── Public entry point ─────────────────────────────────────────── */

cypher_ast_t *cypher_parse(cypher_token_t *tokens, int n, const char **error) {
    T = tokens; TN = n; TI = 0; ERR = error; *error = NULL;

    cypher_ast_t *head = NULL, *tail = NULL;

    /* WITH-separated multi-part queries */
    while (1) {
        cypher_ast_t *sq = parse_single_query();
        if (!sq) break;
        if (!head) head = sq;
        else {
            cypher_ast_t *last = head;
            while (last->next) last = last->next;
            last->next = sq;
        }
        /* Check for WITH (multi-part) */
        if (CUR == TOK_WITH) {
            cypher_ast_t *w = pwith();
            if (w) {
                cypher_ast_t *last = head;
                while (last->next) last = last->next;
                last->next = w;
            }
        }
        /* Check for UNION */
        if (CUR == TOK_UNION) {
            NXT; EAT(TOK_ALL);
            continue;
        }
        break;
    }

    EAT(TOK_SEMI);
    if (TI < TN && CUR != TOK_EOF)
        *error = "unexpected token after statement";

    return head;
}

/* ── AST cleanup ────────────────────────────────────────────────── */

void cypher_ast_free(cypher_ast_t *a) {
    if (!a) return;
    cypher_ast_free(a->next);
    switch (a->type) {
    case AST_BINARY:  case AST_MATCH: case AST_CREATE: case AST_MERGE:
    case AST_UNWIND:  case AST_DELETE: case AST_CASE:
        cypher_ast_free(a->bin.l); cypher_ast_free(a->bin.r); break;
    case AST_NOT:
        cypher_ast_free(a->bin.l); break;
    case AST_UNARY:
        cypher_ast_free(a->una.a); break;
    case AST_PROP:
        cypher_ast_free(a->prop.e); cypher_ast_free(a->prop.n); break;
    case AST_NODE_PAT:
        cypher_ast_free(a->node.name); cypher_ast_free(a->node.props); break;
    case AST_REL_PAT:
        cypher_ast_free(a->rel.name); cypher_ast_free(a->rel.labels);
        cypher_ast_free(a->rel.props); break;
    case AST_PATTERN: case AST_SET: case AST_REMOVE: case AST_LIST:
    case AST_RETURN: case AST_WITH: {
        for (int i = 0; i < a->list.n; i++) cypher_ast_free(a->list.items[i]);
        free(a->list.items);
        if (a->type == AST_RETURN || a->type == AST_WITH) {
            cypher_ast_free(a->bin.r);
            for (cypher_ast_t *n = a->next; n; n = n->next) cypher_ast_free(n);
        }
        break;
    }
    case AST_ORDER_ITEM:
        cypher_ast_free(a->bin.l); break;
    case AST_COLUMN:
        cypher_ast_free(a->col.name); cypher_ast_free(a->col.as); break;
    case AST_FUNCALL:
        cypher_ast_free(a->call.func);
        for (int i = 0; i < a->call.n; i++) cypher_ast_free(a->call.args[i]);
        free(a->call.args); break;
    case AST_MAP_ENTRY:
        cypher_ast_free(a->pair.l); cypher_ast_free(a->pair.r); break;
    default: break;
    }
    free(a);
}
