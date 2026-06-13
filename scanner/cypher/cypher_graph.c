/*
 * cypher_graph.c — Cypher graph API backed by the high-performance graph_store.c.
 *
 * Thin wrapper: exposes the same public API, delegates storage to graph_store,
 * keeps variable binding for multi-clause CREATE queries.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cypher_parser.h"
#include "graph_store.h"
#include "graph_exec.h"
#include "khashl.h"
#include "kvec.h"

struct cypher_graph {
    graph_store_t *gs;
    struct { char name[MAX_STR]; cypher_node_id_t id; } vars[256];
    int nvars;
};

static cypher_node_id_t var_get(cypher_graph_t *g, const char *n) {
    for (int i = 0; i < g->nvars; i++)
        if (!strcmp(g->vars[i].name, n)) return g->vars[i].id;
    return (cypher_node_id_t)-1;
}
static void var_set(cypher_graph_t *g, const char *n, cypher_node_id_t id) {
    for (int i = 0; i < g->nvars; i++)
        if (!strcmp(g->vars[i].name, n)) { g->vars[i].id = id; return; }
    if (g->nvars < 256) {
        strncpy(g->vars[g->nvars].name, n, MAX_STR - 1);
        g->vars[g->nvars].id = id;
        g->nvars++;
    }
}

cypher_graph_t *cypher_graph_new(void) {
    cypher_graph_t *g = calloc(1, sizeof(*g));
    g->gs = gs_create();
    return g;
}
void *cypher_graph_get_store(cypher_graph_t *g) {
    return g ? g->gs : NULL;
}
void cypher_graph_free(cypher_graph_t *g) {
    if (!g) return;
    gs_destroy(g->gs);
    free(g);
}

cypher_node_id_t cypher_graph_add_node(cypher_graph_t *g) { return gs_add_node(g->gs); }
void cypher_graph_set_label(cypher_graph_t *g, cypher_node_id_t n, const char *l) { gs_set_label(g->gs, (uint32_t)n, l); }
void cypher_graph_set_prop_str(cypher_graph_t *g, cypher_node_id_t n, const char *k, const char *v) { gs_add_prop_str(g->gs, (uint32_t)n, k, v); }
void cypher_graph_set_prop_num(cypher_graph_t *g, cypher_node_id_t n, const char *k, double v) { gs_add_prop_num(g->gs, (uint32_t)n, k, v); }
cypher_node_id_t cypher_graph_add_rel(cypher_graph_t *g, cypher_node_id_t s, cypher_node_id_t d, const char *t) { return gs_add_edge(g->gs, (uint32_t)s, (uint32_t)d, t); }
const char *cypher_graph_get_label(cypher_graph_t *g, cypher_node_id_t n) {
    uint32_t nn = (uint32_t)n;
    for (uint32_t li = 0; li < g->gs->label_count; li++)
        if (g->gs->nodes[nn].label_mask & (1ULL << li))
            return g->gs->labels[li].name;
    return NULL;
}
const char *cypher_graph_get_prop(cypher_graph_t *g, cypher_node_id_t n, const char *k) { return gs_prop_str(g->gs, (uint32_t)n, k); }

/* ---- result API (unchanged) ---- */
cypher_result_t *cypher_result_new(void) {
    cypher_result_t *r = calloc(1, sizeof(*r)); return r;
}
void cypher_result_add_col(cypher_result_t *r, const char *col) {
    r->columns = realloc(r->columns, sizeof(char*) * (size_t)(r->ncols+1));
    r->columns[r->ncols++] = strdup(col);
}
void cypher_result_add_row_empty(cypher_result_t *r) {
    if (r->nrows >= r->cap) {
        r->cap = r->cap < 16 ? 16 : r->cap * 2;
        r->rows = realloc(r->rows, sizeof(char**) * (size_t)r->cap);
        for (int i = r->nrows; i < r->cap; i++) r->rows[i] = NULL;
    }
    r->rows[r->nrows] = calloc((size_t)r->ncols + 1, sizeof(char*));
    r->nrows++;
}
void cypher_result_set_cell(cypher_result_t *r, int row, int col, const char *val) {
    if (row >= r->nrows || col >= r->ncols) return;
    free(r->rows[row][col]);
    r->rows[row][col] = val ? strdup(val) : strdup("NULL");
}
void cypher_result_free(cypher_result_t *r) {
    if (!r) return;
    for (int i = 0; i < r->ncols; i++) free(r->columns[i]);
    free(r->columns);
    for (int i = 0; i < r->nrows; i++) {
        if (r->rows[i]) {
            for (int j = 0; j < r->ncols; j++) free(r->rows[i][j]);
            free(r->rows[i]);
        }
    }
    free(r->rows); free(r);
}

/* ---- emit cell value from a node ---- */
static void emit_cell(cypher_result_t *r, int row, int col,
                       cypher_graph_t *g, uint32_t nid, cypher_ast_t *expr) {
    graph_store_t *gs = g->gs;
    char buf[MAX_STR * 2]; buf[0] = 0;
    if (expr->type == AST_IDENT) {
        const char *lab = cypher_graph_get_label(g, nid);
        /* check props first */
        const char *pv = gs_prop_str(gs, nid, expr->str);
        if (pv) snprintf(buf, sizeof(buf), "%s", pv);
        else if (lab && lab[0]) snprintf(buf, sizeof(buf), "(:%s)", lab);
        else snprintf(buf, sizeof(buf), "(node %u)", nid);
    } else if (expr->type == AST_PROP) {
        const char *pkey = expr->prop.n->str;
        const char *pv = gs_prop_str(gs, nid, pkey);
        if (pv) snprintf(buf, sizeof(buf), "%s", pv);
        else { double dv = gs_prop_num(gs, nid, pkey); snprintf(buf, sizeof(buf), "%g", dv); }
    } else if (expr->type == AST_STRING) {
        snprintf(buf, sizeof(buf), "%s", expr->str);
    } else if (expr->type == AST_INTEGER) {
        snprintf(buf, sizeof(buf), "%d", expr->ival);
    } else if (expr->type == AST_FLOAT) {
        snprintf(buf, sizeof(buf), "%g", expr->fval);
    } else {
        snprintf(buf, sizeof(buf), "?");
    }
    cypher_result_set_cell(r, row, col, buf);
}

/* ---- label/prop matching helpers ---- */
static int match_label2(graph_store_t *gs, uint32_t nid, cypher_ast_t *label_ast) {
    if (!label_ast || !label_ast->str[0]) return 1;
    for (uint32_t li = 0; li < gs->label_count; li++) {
        if (gs->nodes[nid].label_mask & (1ULL << li)) {
            cypher_ast_t *l = label_ast;
            while (l) {
                if (!strcmp(gs->labels[li].name, l->str)) return 1;
                l = l->next;
            }
        }
    }
    return 0;
}
static int match_props2(graph_store_t *gs, uint32_t nid, cypher_ast_t *props) {
    if (!props) return 1;
    for (cypher_ast_t *e = props; e; e = e->next) {
        if (e->type != AST_MAP_ENTRY || !e->pair.l || !e->pair.r) continue;
        if (e->pair.l->type != AST_STRING) continue;
        const char *key = e->pair.l->str;
        if (e->pair.r->type == AST_STRING) {
            const char *v = gs_prop_str(gs, nid, key);
            if (!v || strcmp(v, e->pair.r->str)) return 0;
        } else if (e->pair.r->type == AST_INTEGER) {
            double v = gs_prop_num(gs, nid, key);
            if (v != (double)e->pair.r->ival) return 0;
        } else if (e->pair.r->type == AST_FLOAT) {
            double v = gs_prop_num(gs, nid, key);
            if (v != e->pair.r->fval) return 0;
        }
    }
    return 1;
}
static int eval_bool(graph_store_t *gs, uint32_t nid, cypher_ast_t *e) {
    if (!e) return 1;
    if (e->type == AST_BINARY) {
        if (e->bin.op == TOK_AND)
            return eval_bool(gs, nid, e->bin.l) && eval_bool(gs, nid, e->bin.r);
        if (e->bin.op == TOK_OR)
            return eval_bool(gs, nid, e->bin.l) || eval_bool(gs, nid, e->bin.r);
        if (e->bin.l && e->bin.r) {
            int cmp_op = e->bin.op;
            if (cmp_op == TOK_EQ || cmp_op == TOK_NEQ || cmp_op == TOK_LT ||
                cmp_op == TOK_GT || cmp_op == TOK_LE || cmp_op == TOK_GE) {
                if (e->bin.l->type == AST_PROP) {
                    if (e->bin.r->type == AST_STRING) {
                        const char *v = gs_prop_str(gs, nid, e->bin.l->prop.n->str);
                        if (cmp_op == TOK_EQ) return v && !strcmp(v, e->bin.r->str);
                        if (cmp_op == TOK_NEQ) return !v || !!strcmp(v, e->bin.r->str);
                        return 1;
                    }
                    double lv = 0, rv = 0;
                    if (e->bin.r->type == AST_INTEGER) {
                        lv = gs_prop_num(gs, nid, e->bin.l->prop.n->str);
                        rv = (double)e->bin.r->ival;
                    } else if (e->bin.r->type == AST_FLOAT) {
                        lv = gs_prop_num(gs, nid, e->bin.l->prop.n->str);
                        rv = e->bin.r->fval;
                    } else {
                        return 1;
                    }
                    if (cmp_op == TOK_EQ) return lv == rv;
                    if (cmp_op == TOK_NEQ) return lv != rv;
                    if (cmp_op == TOK_LT) return lv < rv;
                    if (cmp_op == TOK_GT) return lv > rv;
                    if (cmp_op == TOK_LE) return lv <= rv;
                    if (cmp_op == TOK_GE) return lv >= rv;
                }
            }
        }
    }
    if (e->type == AST_NOT) return !eval_bool(gs, nid, e->bin.l);
    return 1;
}

/* ---- extract labels from node pattern AST ---- */
static cypher_ast_t *extract_labels(cypher_ast_t *node_pat) {
    cypher_ast_t *labels = NULL;
    for (cypher_ast_t *p = node_pat->node.props; p; ) {
        cypher_ast_t *nxt = p->next;
        if (p->type == AST_LABEL) { p->next = labels; labels = p; }
        p = nxt;
    }
    return labels;
}
static cypher_ast_t *extract_props(cypher_ast_t *node_pat) {
    cypher_ast_t *props = NULL, *prev = NULL;
    for (cypher_ast_t *p = node_pat->node.props; p;) {
        cypher_ast_t *nxt = p->next;
        if (p->type != AST_LABEL) {
            p->next = NULL;
            if (prev) prev->next = p; else props = p;
            prev = p;
        }
        p = nxt;
    }
    return props;
}

/* ---- main executor ---- */

static void apply_set(cypher_graph_t *g, cypher_ast_t *set) {
    graph_store_t *gs = g->gs;
    for (int i = 0; i < set->list.n; i++) {
        cypher_ast_t *assign = set->list.items[i];
        if (assign->type != AST_BINARY || assign->bin.op != TOK_EQ) continue;
        cypher_ast_t *lhs = assign->bin.l;
        cypher_ast_t *rhs = assign->bin.r;
        if (!lhs || !rhs) continue;
        if (lhs->type == AST_PROP && lhs->prop.e->type == AST_IDENT) {
            const char *vname = lhs->prop.e->str;
            uint32_t nid = (uint32_t)var_get(g, vname);
            if (nid == 0xFFFFFFFF) continue;
            const char *key = lhs->prop.n->str;
            if (rhs->type == AST_STRING)
                gs_set_prop_str(gs, nid, key, rhs->str);
            else if (rhs->type == AST_INTEGER)
                gs_set_prop_num(gs, nid, key, (double)rhs->ival);
            else if (rhs->type == AST_FLOAT)
                gs_set_prop_num(gs, nid, key, rhs->fval);
        }
    }
}

static void apply_delete(cypher_graph_t *g, cypher_ast_t *del) {
    graph_store_t *gs = g->gs;
    int detach = (del->bin.op != 0);
    cypher_ast_t *exprs = del->bin.l;
    while (exprs) {
        if (exprs->type == AST_IDENT) {
            uint32_t nid = (uint32_t)var_get(g, exprs->str);
            if (nid == 0xFFFFFFFF || nid >= gs->node_count) { exprs = exprs->next; continue; }
            if (detach) {
                for (uint32_t e = 0; e < gs->edge_count; e++) {
                    if (gs->edges[e].dst == nid) {
                        gs->edges[e].dst = 0xFFFFFFFF;
                    }
                }
                while (gs->edge_heads[nid] != 0xFFFFFFFF) {
                    gs->edge_heads[nid] = gs->edges[gs->edge_heads[nid]].next;
                }
                gs->nodes[nid].edge_count = 0;
            }
            gs->nodes[nid].label_mask = 0;
            gs->nodes[nid].props_off = 0xFFFFFFFF;
        }
        exprs = exprs->next;
    }
}

static void result_sort(cypher_result_t *r, cypher_ast_t *order) {
    if (!r->nrows || !order) return;
    int ncols = r->ncols;
    for (int i = 0; i < r->nrows - 1; i++) {
        for (int j = 0; j < r->nrows - i - 1; j++) {
            int cmp = 0;
            for (cypher_ast_t *o = order; o; o = o->next) {
                if (o->type != AST_ORDER_ITEM) continue;
                const char *key = (o->bin.l && o->bin.l->type == AST_IDENT)
                    ? o->bin.l->str : NULL;
                if (!key) {
                    if (o->bin.l && o->bin.l->type == AST_PROP && o->bin.l->prop.n)
                        key = o->bin.l->prop.n->str;
                }
                if (!key) continue;
                int col = -1;
                for (int c = 0; c < ncols; c++)
                    if (!strcmp(r->columns[c], key)) { col = c; break; }
                if (col < 0) continue;
                const char *a = r->rows[j][col];
                const char *b = r->rows[j+1][col];
                if (!a && !b) continue;
                if (!a) { cmp = -1; break; }
                if (!b) { cmp = 1; break; }
                cmp = strcasecmp(a, b);
                if (o->bin.op) cmp = -cmp;
                if (cmp != 0) break;
            }
            if (cmp > 0) {
                char **tmp = r->rows[j];
                r->rows[j] = r->rows[j+1];
                r->rows[j+1] = tmp;
            }
        }
    }
}

static void result_distinct(cypher_result_t *r) {
    if (r->nrows < 2) return;
    int write = 0;
    for (int i = 0; i < r->nrows; i++) {
        int dup = 0;
        for (int k = 0; k < write; k++) {
            int same = 1;
            for (int c = 0; c < r->ncols; c++) {
                const char *a = r->rows[i][c];
                const char *b = r->rows[k][c];
                if (!a && !b) continue;
                if (!a || !b || strcmp(a, b)) { same = 0; break; }
            }
            if (same) { dup = 1; break; }
        }
        if (!dup) {
            if (write != i) {
                char **tmp = r->rows[i];
                r->rows[i] = r->rows[write];
                r->rows[write] = tmp;
            }
            write++;
        }
    }
    r->nrows = write;
}

static void result_limit_skip(cypher_result_t *r, cypher_ast_t *clauses) {
    int limit_val = r->nrows;
    int skip_val = 0;
    for (cypher_ast_t *n = clauses; n; n = n->next) {
        if (n->type == AST_LIMIT) limit_val = n->ival;
        else if (n->type == AST_SKIP) skip_val = n->ival;
    }
    if (skip_val > 0) {
        /* free skipped rows */
        for (int i = 0; i < skip_val && i < r->nrows; i++) {
            if (r->rows[i]) {
                for (int c = 0; c < r->ncols; c++) free(r->rows[i][c]);
                free(r->rows[i]);
                r->rows[i] = NULL;
            }
        }
        int dst = 0;
        for (int i = skip_val; i < r->nrows; i++) {
            r->rows[dst] = r->rows[i];
            r->rows[i] = NULL;
            dst++;
        }
        r->nrows = dst;
    }
    /* free rows beyond limit */
    for (int i = limit_val; i < r->nrows; i++) {
        if (r->rows[i]) {
            for (int c = 0; c < r->ncols; c++) free(r->rows[i][c]);
            free(r->rows[i]);
            r->rows[i] = NULL;
        }
    }
    if (limit_val < r->nrows) r->nrows = limit_val;
}

static int exec_match_return(cypher_graph_t *g, cypher_ast_t *match_cl,
                              cypher_ast_t *return_cl, cypher_result_t *result) {
    graph_store_t *gs = g->gs;
    int distinct = (return_cl->bin.op == 1);
    uint32_t *candidates = NULL;
    uint32_t ncan = 0, cap = 16384;

    if (match_cl && match_cl->bin.l && match_cl->bin.l->type == AST_PATTERN) {
        cypher_ast_t *pat = match_cl->bin.l;
        cypher_ast_t *fn = pat->list.items[0];

        if (fn->type == AST_NODE_PAT) {
            cypher_ast_t *labels = extract_labels(fn);
            cypher_ast_t *props  = extract_props(fn);
            cypher_ast_t *where  = match_cl->bin.r;

            candidates = malloc(cap * sizeof(uint32_t));

            if (labels && labels->str[0]) {
                ncan = gs_label_nodes(gs, labels->str, candidates, cap);
                if (ncan <= 0 && cap < gs->node_count) {
                    cap = gs->node_count;
                    candidates = realloc(candidates, cap * sizeof(uint32_t));
                    ncan = gs_label_nodes(gs, labels->str, candidates, cap);
                }
            } else {
                ncan = gs_node_count(gs);
                if (ncan > cap) {
                    cap = ncan;
                    candidates = realloc(candidates, cap * sizeof(uint32_t));
                }
                for (uint32_t ni = 0; ni < ncan; ni++) candidates[ni] = ni;
            }

            cypher_ast_t *rel_pat = (pat->list.n >= 2 && pat->list.items[1]->type == AST_REL_PAT)
                ? pat->list.items[1] : NULL;
            cypher_ast_t *tgt_node = (pat->list.n >= 3) ? pat->list.items[2] : NULL;
            const char *ov = (tgt_node && tgt_node->type == AST_NODE_PAT && tgt_node->node.name)
                ? tgt_node->node.name->str : NULL;

            const char *edge_type = NULL;
            if (rel_pat && rel_pat->rel.labels && rel_pat->rel.labels->str[0])
                edge_type = rel_pat->rel.labels->str;

            cypher_ast_t *tgt_labels = NULL;
            cypher_ast_t *tgt_props  = NULL;
            if (tgt_node && tgt_node->type == AST_NODE_PAT) {
                tgt_labels = extract_labels(tgt_node);
                tgt_props  = extract_props(tgt_node);
            }

            for (uint32_t ci = 0; ci < ncan && result->nrows < 200; ci++) {
                uint32_t ni = candidates[ci];

                if (!match_label2(gs, ni, labels)) continue;
                if (!match_props2(gs, ni, props)) continue;
                if (!eval_bool(gs, ni, where)) continue;

                if (rel_pat) {
                    uint32_t ec = gs_edge_count(gs, ni);
                    for (uint32_t ej = 0; ej < ec && result->nrows < 200; ej++) {
                        uint32_t dst = gs_edge_dst(gs, ni, ej);
                        if (dst == 0xFFFFFFFF) continue;
                        if (edge_type) {
                            uint32_t et = gs_edge_type(gs, ni, ej);
                            if (gs_hash_str(edge_type) != et) continue;
                        }
                        if (!match_label2(gs, dst, tgt_labels)) continue;
                        if (!match_props2(gs, dst, tgt_props)) continue;

                        cypher_result_add_row_empty(result);
                        for (int ci2 = 0; ci2 < return_cl->list.n; ci2++) {
                            cypher_ast_t *col = return_cl->list.items[ci2];
                            uint32_t un = ni;
                            if (col->col.name->type == AST_PROP
                                && col->col.name->prop.e->type == AST_IDENT) {
                                if (tgt_node && tgt_node->node.name
                                    && !strcmp(col->col.name->prop.e->str,
                                               tgt_node->node.name->str))
                                    un = dst;
                            }
                            emit_cell(result, result->nrows-1, ci2, g, un, col->col.name);
                        }
                    }
                } else {
                    cypher_result_add_row_empty(result);
                    for (int ci2 = 0; ci2 < return_cl->list.n; ci2++)
                        emit_cell(result, result->nrows-1, ci2, g, ni,
                                  return_cl->list.items[ci2]->col.name);
                }
            }
        }
    } else {
        uint32_t nc = gs_node_count(gs);
        for (uint32_t ni = 0; ni < nc && result->nrows < 200; ni++) {
            cypher_result_add_row_empty(result);
            for (int ci = 0; ci < return_cl->list.n; ci++)
                emit_cell(result, result->nrows-1, ci, g, ni,
                          return_cl->list.items[ci]->col.name);
        }
    }

    free(candidates);

    cypher_ast_t *order = return_cl->bin.r;
    if (order) result_sort(result, order);
    if (distinct) result_distinct(result);
    result_limit_skip(result, return_cl->next);

    return 0;
}

cypher_result_t *cypher_execute(cypher_graph_t *g, cypher_ast_t *ast, const char **error) {
    if (!g || !ast) return NULL;
    graph_store_t *gs = g->gs;
    if (error) *error = NULL;

    for (cypher_ast_t *m = ast; m; m = m->next) {
        if (m->type == AST_SET) {
            apply_set(g, m);
            continue;
        }
        if (m->type == AST_DELETE) {
            apply_delete(g, m);
            continue;
        }
        if (m->type != AST_CREATE) continue;
        cypher_ast_t *cpat = m->bin.l;
        if (!cpat || cpat->type != AST_PATTERN || cpat->list.n < 1) continue;

        cypher_ast_t *fn = cpat->list.items[0];
        if (fn->type != AST_NODE_PAT) continue;

        const char *vname = fn->node.name ? fn->node.name->str : NULL;
        uint32_t nn = (vname && var_get(g, vname) != (cypher_node_id_t)-1)
            ? (uint32_t)var_get(g, vname) : gs_add_node(gs);
        if (vname) var_set(g, vname, nn);

        for (cypher_ast_t *p = fn->node.props; p; p = p->next) {
            if (p->type == AST_LABEL) gs_set_label(gs, nn, p->str);
            else if (p->type == AST_MAP_ENTRY && p->pair.l && p->pair.r) {
                if (p->pair.r->type == AST_STRING)
                    gs_add_prop_str(gs, nn, p->pair.l->str, p->pair.r->str);
                else if (p->pair.r->type == AST_INTEGER)
                    gs_add_prop_num(gs, nn, p->pair.l->str, (double)p->pair.r->ival);
                else if (p->pair.r->type == AST_FLOAT)
                    gs_add_prop_num(gs, nn, p->pair.l->str, p->pair.r->fval);
            }
        }

        if (cpat->list.n >= 3 && cpat->list.items[1]->type == AST_REL_PAT) {
            cypher_ast_t *rpat = cpat->list.items[1];
            cypher_ast_t *tgt = cpat->list.items[2];
            const char *tvname = (tgt->type == AST_NODE_PAT && tgt->node.name)
                ? tgt->node.name->str : NULL;
            uint32_t tn = (tvname && var_get(g, tvname) != (cypher_node_id_t)-1)
                ? (uint32_t)var_get(g, tvname) : gs_add_node(gs);
            if (tvname) var_set(g, tvname, tn);
            const char *rtype = (rpat->rel.labels && rpat->rel.labels->str[0])
                ? rpat->rel.labels->str : "REL";
            gs_add_edge(gs, nn, tn, rtype);
        }
    }

    gs_build_indexes(gs);

    cypher_ast_t *match_cl = NULL, *return_cl = NULL;
    for (cypher_ast_t *cur = ast; cur; cur = cur->next) {
        if (!match_cl && cur->type == AST_MATCH) match_cl = cur;
        if (!return_cl && cur->type == AST_RETURN) return_cl = cur;
    }

    /* Phase 4: execute query via FSM */
    cypher_result_t *result = cypher_fsm_exec(g, match_cl, return_cl);
    return result;
}

/* ---- sidecar.json bulk import ---- */

KHASHL_MAP_INIT(static, smap_t, smap, uint32_t, uint32_t, kh_hash_dummy, kh_eq_generic)

static int json_extract_str(const char *json, const char *key, char *out, int max) {
    char pat[128];
    snprintf(pat, sizeof(pat), "\"%s\":\"", key);
    const char *s = strstr(json, pat);
    if (!s) return 0;
    s += strlen(pat);
    int i = 0;
    while (*s && *s != '"' && i < max - 1) {
        if (*s == '\\' && s[1]) { s++; out[i++] = *s++; }
        else out[i++] = *s++;
    }
    out[i] = '\0';
    return 1;
}

static int json_extract_uint(const char *json, const char *key, uint32_t *out) {
    char pat[128];
    snprintf(pat, sizeof(pat), "\"%s\":", key);
    const char *s = strstr(json, pat);
    if (!s) return 0;
    s += strlen(pat);
    *out = (uint32_t)strtoul(s, NULL, 10);
    return 1;
}

static int json_extract_int(const char *json, const char *key, int *out) {
    uint32_t u;
    if (!json_extract_uint(json, key, &u)) return 0;
    *out = (int)u;
    return 1;
}

int cypher_graph_load_sidecar(cypher_graph_t *g, const char *filename) {
    graph_store_t *gs = g->gs;
    FILE *fp = fopen(filename, "rb");
    if (!fp) return -1;

    smap_t *sm = smap_init();
    kvec_t(uint32_t) ep;
    kvec_t(uint32_t) ec;
    kv_init(ep);
    kv_init(ec);

    char line[16384];
    int lineno = 0;

    while (fgets(line, sizeof(line), fp)) {
        lineno++;
        if (!line[0] || line[0] != '{') continue;

        uint32_t token_id = 0, parent_id = 0, filepath_id = 0;
        int offset = 0, length = 0;
        char type[128] = "";
        char text[300] = "";
        char filepath[512] = "";

        json_extract_uint(line, "token_id", &token_id);
        json_extract_uint(line, "parent_id", &parent_id);
        json_extract_uint(line, "filepath_id", &filepath_id);
        json_extract_int(line, "offset", &offset);
        json_extract_int(line, "length", &length);
        json_extract_str(line, "type", type, sizeof(type));
        json_extract_str(line, "text", text, sizeof(text));
        json_extract_str(line, "filepath", filepath, sizeof(filepath));

        if (!token_id || !type[0]) continue;

        uint32_t nid = gs_add_node(gs);

        gs_set_label(gs, nid, "Token");
        if (type[0]) gs_set_label(gs, nid, type);

        gs_add_prop_num(gs, nid, "token_id", (double)token_id);
        gs_add_prop_num(gs, nid, "parent_id", (double)parent_id);
        gs_add_prop_num(gs, nid, "filepath_id", (double)filepath_id);
        gs_add_prop_num(gs, nid, "offset", (double)offset);
        gs_add_prop_num(gs, nid, "length", (double)length);
        if (text[0]) gs_add_prop_str(gs, nid, "text", text);
        if (filepath[0]) gs_add_prop_str(gs, nid, "filepath", filepath);

        int absent;
        khint_t slot = smap_put(sm, (uint32_t)token_id, &absent);
        kh_val(sm, slot) = nid;

        if (parent_id != filepath_id) {
            kv_push(uint32_t, ep, (uint32_t)parent_id);
            kv_push(uint32_t, ec, nid);
        }
    }
    fclose(fp);

    for (size_t i = 0; i < kv_size(ep); i++) {
        khint_t slot = smap_get(sm, kv_A(ep, i));
        if (slot != kh_end(sm)) {
            uint32_t pnid = kh_val(sm, slot);
            gs_add_edge(gs, pnid, kv_A(ec, i), "PARENT_OF");
        }
    }

    kv_destroy(ep);
    kv_destroy(ec);
    smap_destroy(sm);

    gs_build_indexes(gs);

    printf("Loaded %d nodes, %zu edges from %s\n",
           lineno, kv_size(ep), filename);
    return 0;
}
