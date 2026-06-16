/*
 * cypher_graph.c — Cypher graph API backed by the high-performance graph_store.c.
 *
 * Thin wrapper: exposes the same public API, delegates storage to graph_store,
 * keeps variable binding for multi-clause CREATE queries.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "cypher_parser.h"
#include "graph_store.h"
#include "graph_exec.h"
#include "graph_jit.h"
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
/* ---- mutations ---- */

static double eval_const_expr(cypher_ast_t *e) {
    if (!e) return 0.0;
    if (e->type == AST_INTEGER) return (double)e->ival;
    if (e->type == AST_FLOAT)   return e->fval;
    if (e->type == AST_UNARY) {
        double v = eval_const_expr(e->una.a);
        return e->una.op == '-' ? -v : v;
    }
    if (e->type == AST_BINARY) {
        double l = eval_const_expr(e->bin.l);
        double r = eval_const_expr(e->bin.r);
        switch (e->bin.op) {
            case '+': case TOK_PLUS:  return l + r;
            case '-': case TOK_MINUS: return l - r;
            case '*': case TOK_STAR:  return l * r;
            case '/': case TOK_SLASH: return r != 0 ? l / r : 0;
            case '%': case TOK_PCT:   return (long)l % (long)r;
            default: return 0.0;
        }
    }
    return 0.0;
}

static void apply_set(cypher_graph_t *g, cypher_ast_t *set) {
    graph_store_t *gs = g->gs;
    for (int i = 0; i < set->list.n; i++) {
        cypher_ast_t *assign = set->list.items[i];
        if (assign->type != AST_BINARY) continue;

        /* SET n:Label — label assignment */
        if (assign->bin.op == TOK_COLON && assign->bin.l
            && assign->bin.l->type == AST_IDENT) {
            const char *vname = assign->bin.l->str;
            uint32_t nid = (uint32_t)var_get(g, vname);
            if (nid == 0xFFFFFFFF) continue;
            for (cypher_ast_t *lab = assign->bin.r; lab; lab = lab->next)
                if (lab->type == AST_LABEL) gs_set_label(gs, nid, lab->str);
            continue;
        }

        /* SET n.key = value — property assignment */
        if (assign->bin.op != TOK_EQ) continue;
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
            else if (rhs->type == AST_UNARY && rhs->una.a->type == AST_INTEGER) {
                double v = rhs->una.op == '-' ? -(double)rhs->una.a->ival : (double)rhs->una.a->ival;
                gs_set_prop_num(gs, nid, key, v);
            }
            else if (rhs->type == AST_UNARY || rhs->type == AST_BINARY) {
                double v = eval_const_expr(rhs);
                gs_set_prop_num(gs, nid, key, v);
            }
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
            if (nid != 0xFFFFFFFF && nid < gs->node_count) {
                if (detach) {
                    for (uint32_t e = 0; e < gs->edge_count; e++) {
                        if (gs->edges[e].dst == nid) gs->edges[e].dst = 0xFFFFFFFF;
                    }
                    while (gs->edge_heads[nid] != 0xFFFFFFFF)
                        gs->edge_heads[nid] = gs->edges[gs->edge_heads[nid]].next;
                    gs->nodes[nid].edge_count = 0;
                }
                gs->nodes[nid].label_mask = 0;
                gs->nodes[nid].props_off = 0xFFFFFFFF;
            }
        }
        exprs = exprs->next;
    }
    /* also process comma-separated expressions stored in bin.r */
    for (cypher_ast_t *e = del->bin.r; e; e = e->next) {
        if (e->type == AST_IDENT) {
            uint32_t nid = (uint32_t)var_get(g, e->str);
            if (nid != 0xFFFFFFFF && nid < gs->node_count) {
                if (detach) {
                    for (uint32_t ei = 0; ei < gs->edge_count; ei++) {
                        if (gs->edges[ei].dst == nid) gs->edges[ei].dst = 0xFFFFFFFF;
                    }
                    while (gs->edge_heads[nid] != 0xFFFFFFFF)
                        gs->edge_heads[nid] = gs->edges[gs->edge_heads[nid]].next;
                    gs->nodes[nid].edge_count = 0;
                }
                gs->nodes[nid].label_mask = 0;
                gs->nodes[nid].props_off = 0xFFFFFFFF;
            }
        }
    }
}

// MERGE: Create pattern if it doesn't exist, match if it does
static void apply_merge(cypher_graph_t *g, cypher_ast_t *m) {
    graph_store_t *gs = g->gs;
    
    // m->bin.l is the pattern to merge
    cypher_ast_t *pattern = m->bin.l;
    if (!pattern || pattern->type != AST_PATTERN || pattern->list.n < 1) return;
    
    // Get the first node pattern
    cypher_ast_t *first = pattern->list.items[0];
    if (first->type != AST_NODE_PAT) return;
    
    // Get node variable name
    const char *vname = first->node.name ? first->node.name->str : NULL;
    cypher_node_id_t nid;
    int created = 0;
    
    // Check if variable already bound (from previous MERGE or CREATE)
    if (vname && var_get(g, vname) != (cypher_node_id_t)-1) {
        nid = (cypher_node_id_t)var_get(g, vname);
    } else {
        // Create new node
        nid = gs_add_node(gs);
        created = 1;
    }
    
    // Bind variable if present
    if (vname) var_set(g, vname, nid);
    
    // Set labels on the node
    for (cypher_ast_t *p = first->node.props; p; p = p->next) {
        if (p->type == AST_LABEL) {
            gs_set_label(gs, nid, p->str);
        } else if (p->type == AST_MAP_ENTRY && p->pair.l && p->pair.r) {
            if (p->pair.r->type == AST_STRING)
                gs_add_prop_str(gs, nid, p->pair.l->str, p->pair.r->str);
            else if (p->pair.r->type == AST_INTEGER)
                gs_add_prop_num(gs, nid, p->pair.l->str, (double)p->pair.r->ival);
            else if (p->pair.r->type == AST_FLOAT)
                gs_add_prop_num(gs, nid, p->pair.l->str, p->pair.r->fval);
        }
    }
    
    // Handle relationships in pattern
    if (pattern->list.n >= 3 && pattern->list.items[1]->type == AST_REL_PAT) {
        cypher_ast_t *rel_pat = pattern->list.items[1];
        cypher_ast_t *target = pattern->list.items[2];
        
        const char *tvname = (target->type == AST_NODE_PAT && target->node.name)
            ? target->node.name->str : NULL;
        cypher_node_id_t tnid;
        int tcreated = 0;
        
        // Get or create target node
        if (tvname && var_get(g, tvname) != (cypher_node_id_t)-1) {
            tnid = (cypher_node_id_t)var_get(g, tvname);
        } else {
            tnid = gs_add_node(gs);
            tcreated = 1;
        }
        
        // Bind target variable
        if (tvname) var_set(g, tvname, tnid);
        
        // Set target properties
        cypher_ast_t *tgt_node = (target->type == AST_NODE_PAT) ? target : NULL;
        if (tgt_node) {
            for (cypher_ast_t *p = tgt_node->node.props; p; p = p->next) {
                if (p->type == AST_LABEL) gs_set_label(gs, tnid, p->str);
                else if (p->type == AST_MAP_ENTRY && p->pair.l && p->pair.r) {
                    if (p->pair.r->type == AST_STRING)
                        gs_add_prop_str(gs, tnid, p->pair.l->str, p->pair.r->str);
                    else if (p->pair.r->type == AST_INTEGER)
                        gs_add_prop_num(gs, tnid, p->pair.l->str, (double)p->pair.r->ival);
                    else if (p->pair.r->type == AST_FLOAT)
                        gs_add_prop_num(gs, tnid, p->pair.l->str, p->pair.r->fval);
                }
            }
        }
        
        // Create relationship if either node was created
        if (created || tcreated) {
            const char *rtype = (rel_pat->rel.labels && rel_pat->rel.labels->str[0])
                ? rel_pat->rel.labels->str : "REL";
            gs_add_edge(gs, nid, tnid, rtype);
        }
    }
}

static cypher_result_t *exec_unwind_return(cypher_ast_t *unwind_cl,
                                           cypher_ast_t *return_cl) {
    cypher_result_t *result = cypher_result_new();

    cypher_ast_t *list_expr = unwind_cl->bin.l;
    cypher_ast_t *var_node = unwind_cl->bin.r;
    if (!var_node || !var_node->str[0]) return result;
    const char *vname = var_node->str;

    for (int ci = 0; ci < return_cl->list.n; ci++) {
        cypher_ast_t *col = return_cl->list.items[ci];
        const char *name = "expr";
        if (col->col.as) name = col->col.as->str;
        else if (col->col.name->type == AST_IDENT) name = col->col.name->str;
        cypher_result_add_col(result, name);
    }

    if (list_expr->type != AST_LIST) return result;

    for (int i = 0; i < list_expr->list.n; i++) {
        cypher_ast_t *item = list_expr->list.items[i];
        cypher_result_add_row_empty(result);
        for (int ci = 0; ci < return_cl->list.n; ci++) {
            cypher_ast_t *col = return_cl->list.items[ci];
            char buf[64];
            int matches = 0;
            if (col->col.name->type == AST_IDENT
                && !strcmp(col->col.name->str, vname)) matches = 1;
            else if (col->col.name->type == AST_PROP
                     && col->col.name->prop.e->type == AST_IDENT
                     && !strcmp(col->col.name->prop.e->str, vname)) matches = 1;

            if (matches) {
                if (item->type == AST_INTEGER)
                    snprintf(buf, sizeof(buf), "%d", item->ival);
                else if (item->type == AST_STRING)
                    snprintf(buf, sizeof(buf), "%s", item->str);
                else if (item->type == AST_FLOAT)
                    snprintf(buf, sizeof(buf), "%g", item->fval);
                else
                    snprintf(buf, sizeof(buf), "?");
            } else {
                snprintf(buf, sizeof(buf), "?");
            }
            cypher_result_set_cell(result, i, ci, buf);
        }
    }

    /* LIMIT / SKIP */
    {
        int limit_val = result->nrows;
        int skip_val = 0;
        for (cypher_ast_t *n = return_cl->next; n; n = n->next) {
            if (n->type == AST_LIMIT) limit_val = n->ival;
            else if (n->type == AST_SKIP) skip_val = n->ival;
        }
        if (skip_val > 0) {
            int dst = 0;
            for (int i = skip_val; i < result->nrows; i++)
                result->rows[dst++] = result->rows[i];
            result->nrows = dst;
        }
        if (limit_val < result->nrows)
            result->nrows = limit_val;
    }

    return result;
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
        // MERGE implementation: check if pattern exists, create if not
        if (m->type == AST_MERGE) {
            apply_merge(g, m);
            continue;
        }
        // UNWIND: expand list into rows
        // For now, store UNWIND in a way that RETURN can consume
        if (m->type == AST_UNWIND) {
            // UNWIND will be processed before RETURN
            // For simple queries, we'll handle it in exec_match_return
            continue;
        }
        // REMOVE: remove properties from nodes
        if (m->type == AST_REMOVE) {
            for (int i = 0; i < m->list.n; i++) {
                cypher_ast_t *expr = m->list.items[i];
                if (expr->type == AST_PROP && expr->prop.e->type == AST_IDENT) {
                    const char *vname = expr->prop.e->str;
                    uint32_t nid = (uint32_t)var_get(g, vname);
                    if (nid == 0xFFFFFFFF) continue;
                    const char *key = expr->prop.n->str;
                    /* clear property by setting it to empty string */
                    gs_set_prop_str(gs, nid, key, "");
                }
            }
            continue;
        }
        if (m->type == AST_CASE) {
            // CASE expressions are handled in evaluate_expression
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
            /* set target node labels and properties */
            if (tgt->type == AST_NODE_PAT) {
                for (cypher_ast_t *tp = tgt->node.props; tp; tp = tp->next) {
                    if (tp->type == AST_LABEL) gs_set_label(gs, tn, tp->str);
                    else if (tp->type == AST_MAP_ENTRY && tp->pair.l && tp->pair.r) {
                        if (tp->pair.r->type == AST_STRING)
                            gs_add_prop_str(gs, tn, tp->pair.l->str, tp->pair.r->str);
                        else if (tp->pair.r->type == AST_INTEGER)
                            gs_add_prop_num(gs, tn, tp->pair.l->str, (double)tp->pair.r->ival);
                        else if (tp->pair.r->type == AST_FLOAT)
                            gs_add_prop_num(gs, tn, tp->pair.l->str, tp->pair.r->fval);
                    }
                }
            }
            const char *rtype = (rpat->rel.labels && rpat->rel.labels->str[0])
                ? rpat->rel.labels->str : "REL";
            gs_add_edge(gs, nn, tn, rtype);
        }
    }

    gs_build_indexes(gs);

    /* UNION: multiple RETURN clauses → execute each sub-query independently */
    {
        int n_returns = 0;
        for (cypher_ast_t *cur = ast; cur; cur = cur->next)
            if (cur->type == AST_RETURN) n_returns++;
        if (n_returns > 1) {
            /* Split at RETURN boundaries, execute each sub-query, concatenate */
            cypher_result_t *result = cypher_result_new();
            cypher_ast_t *sub_start = ast;
            cypher_ast_t *prev = NULL;
            for (cypher_ast_t *cur = ast; cur; ) {
                if (cur->type == AST_RETURN && prev && prev->type == AST_RETURN) {
                    /* Found a new sub-query boundary */
                    cypher_ast_t *sub_end = prev;
                    cypher_ast_t *next_sub = cur;
                    /* Detach sub_start..sub_end */
                    if (sub_end) sub_end->next = NULL;
                    /* Execute this sub-query */
                    cypher_result_t *sub_r = cypher_execute(g, sub_start, error);
                    if (sub_r) {
                        if (result->ncols == 0)
                            for (int ci = 0; ci < sub_r->ncols; ci++)
                                cypher_result_add_col(result, sub_r->columns[ci]);
                        for (int ri = 0; ri < sub_r->nrows && result->nrows < MAX_ROWS; ri++) {
                            cypher_result_add_row_empty(result);
                            for (int ci = 0; ci < sub_r->ncols && ci < result->ncols; ci++)
                                cypher_result_set_cell(result, result->nrows-1, ci,
                                    sub_r->rows[ri][ci]);
                        }
                        cypher_result_free(sub_r);
                    }
                    sub_start = next_sub;
                    prev = NULL;
                    cur = next_sub;
                    continue;
                }
                prev = cur;
                cur = cur->next;
            }
            /* Execute last sub-query */
            cypher_result_t *sub_r = cypher_execute(g, sub_start, error);
            if (sub_r) {
                if (result->ncols == 0)
                    for (int ci = 0; ci < sub_r->ncols; ci++)
                        cypher_result_add_col(result, sub_r->columns[ci]);
                for (int ri = 0; ri < sub_r->nrows && result->nrows < MAX_ROWS; ri++) {
                    cypher_result_add_row_empty(result);
                    for (int ci = 0; ci < sub_r->ncols && ci < result->ncols; ci++)
                        cypher_result_set_cell(result, result->nrows-1, ci,
                            sub_r->rows[ri][ci]);
                }
                cypher_result_free(sub_r);
            }
            return result;
        }
    }

    cypher_ast_t *match_cl = NULL, *return_cl = NULL, *opt_match = NULL, *with_cl = NULL;
    cypher_ast_t *unwind_cl = NULL;
    for (cypher_ast_t *cur = ast; cur; cur = cur->next) {
        if (!match_cl && cur->type == AST_MATCH) match_cl = cur;
        else if (match_cl && !opt_match && cur->type == AST_MATCH && cur->bin.op == 1)
            opt_match = cur;
        if (!with_cl && cur->type == AST_WITH) with_cl = cur;
        if (!return_cl && cur->type == AST_RETURN) return_cl = cur;
        if (!unwind_cl && cur->type == AST_UNWIND) unwind_cl = cur;
    }

    /* UNWIND without MATCH: generate rows directly from list */
    if (unwind_cl && !match_cl && return_cl)
        return exec_unwind_return(unwind_cl, return_cl);

    /* WITH pipelining: forward column names through aliases */
    if (with_cl && return_cl) {
        for (int i = 0; i < with_cl->list.n && i < return_cl->list.n; i++) {
            cypher_ast_t *wc = with_cl->list.items[i];
            cypher_ast_t *rc = return_cl->list.items[i];
            if (wc->col.as && !rc->col.as)
                rc->col.as = wc->col.as;
        }
    }

    /* Phase 3+4: try JIT first, fall back to FSM interpreter */
    cypher_result_t *result = NULL;
    if (g_jit_enabled && match_cl && return_cl && !opt_match) {
        result = cypher_jit_exec(g, match_cl, return_cl);
    }
    if (!result) {
        if (opt_match)
            result = cypher_fsm_exec_optional(g, match_cl, opt_match, return_cl);
        else
            result = cypher_fsm_exec(g, match_cl, return_cl);
    }
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

    int fd = open(filename, O_RDONLY);
    if (fd < 0) return -1;

    struct stat st;
    if (fstat(fd, &st) < 0) { close(fd); return -1; }

    char *data = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (data == MAP_FAILED) return -1;

    smap_t *sm = smap_init();
    kvec_t(uint32_t) ep;
    kvec_t(uint32_t) ec;
    kv_init(ep);
    kv_init(ec);

    int lineno = 0;
    char *p = data;
    char *end = data + st.st_size;

    while (p < end) {
        /* find start of line */
        while (p < end && *p != '{') p++;
        if (p >= end) break;

        /* find end of line */
        char *line_start = p;
        char *eol = p;
        while (eol < end && *eol != '\n') eol++;
        ptrdiff_t line_len = eol - line_start;
        p = eol + 1;
        lineno++;

        if (line_len <= 2 || line_len > 16384) continue;

        /* copy line to stack buffer for null-terminated parsing */
        char line[16384];
        memcpy(line, line_start, (size_t)line_len);
        line[line_len] = '\0';

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
        if (type[0]) gs_add_prop_str(gs, nid, "type", type);

        gs_add_prop_num(gs, nid, "token_id", (double)token_id);
        gs_add_prop_num(gs, nid, "parent_id", (double)parent_id);
        gs_add_prop_num(gs, nid, "filepath_id", (double)filepath_id);
        gs_add_prop_num(gs, nid, "offset", (double)offset);
        gs_add_prop_num(gs, nid, "length", (double)length);
        if (text[0]) gs_add_prop_str(gs, nid, "text", text);
        if (filepath[0]) gs_fp_put(gs, filepath_id, filepath);

        int absent;
        khint_t slot = smap_put(sm, (uint32_t)token_id, &absent);
        kh_val(sm, slot) = nid;

        if (parent_id != filepath_id) {
            kv_push(uint32_t, ep, (uint32_t)parent_id);
            kv_push(uint32_t, ec, nid);
        }
    }

    munmap(data, (size_t)st.st_size);

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
