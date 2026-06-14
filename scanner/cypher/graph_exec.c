/*
 * graph_exec.c — Ragel FSM execution engine (Phases 3+4).
 *
 * Phase 3: generates a query-specific C file, compiles it with -O2,
 *           dlopen's the resulting .so, and invokes the FSM.
 * Phase 4: fallback state-machine executor (goto-based dispatch) when
 *           runtime compilation is unavailable.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include "graph_exec.h"
#include "cypher_parser.h"
#include "graph_store.h"

#define FSM_TMPDIR "/tmp/cypher_fsm"
#define MAX_FSM_SRC  (64 * 1024)

/* ---------- Phase 4: Goto-based fallback executor ---------- */

static int fsm_match_label(cypher_fsm_ctx_t *ctx, uint32_t nid,
                            cypher_ast_t *labels) {
    graph_store_t *gs = ctx->gs;
    if (!labels || !labels->str[0]) return 1;
    for (uint32_t li = 0; li < gs->label_count; li++) {
        if (gs->nodes[nid].label_mask & (1ULL << li)) {
            cypher_ast_t *l = labels;
            while (l) {
                if (!strcmp(gs->labels[li].name, l->str)) return 1;
                l = l->next;
            }
        }
    }
    return 0;
}

static int fsm_match_props(cypher_fsm_ctx_t *ctx, uint32_t nid,
                            cypher_ast_t *props) {
    graph_store_t *gs = ctx->gs;
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

static double fsm_eval_value(cypher_fsm_ctx_t *ctx, uint32_t nid,
                              cypher_ast_t *e) {
    graph_store_t *gs = ctx->gs;
    if (!e) return 0.0;
    if (e->type == AST_INTEGER) return (double)e->ival;
    if (e->type == AST_FLOAT)   return e->fval;
    if (e->type == AST_PROP) {
        double v = gs_prop_num(gs, nid, e->prop.n->str);
        if (v == 0.0) v = (double)gs_prop_int(gs, nid, e->prop.n->str);
        return v;
    }
    if (e->type == AST_BINARY) {
        int op = e->bin.op;
        double lv = fsm_eval_value(ctx, nid, e->bin.l);
        double rv = fsm_eval_value(ctx, nid, e->bin.r);
        if (op == TOK_PLUS)  return lv + rv;
        if (op == TOK_MINUS) return lv - rv;
        if (op == TOK_STAR)  return lv * rv;
        if (op == TOK_SLASH) return rv != 0.0 ? lv / rv : 0.0;
        if (op == TOK_PCT)   return (long)lv % (long)rv;
    }
    return 0.0;
}

static int fsm_eval_where(cypher_fsm_ctx_t *ctx, uint32_t nid,
                           cypher_ast_t *e) {
    graph_store_t *gs = ctx->gs;
    if (!e) return 1;
    if (e->type == AST_BINARY) {
        int op = e->bin.op;
        if (op == TOK_AND)
            return fsm_eval_where(ctx, nid, e->bin.l)
                && fsm_eval_where(ctx, nid, e->bin.r);
        if (op == TOK_OR)
            return fsm_eval_where(ctx, nid, e->bin.l)
                || fsm_eval_where(ctx, nid, e->bin.r);
        if (e->bin.l && e->bin.r && (op == TOK_EQ || op == TOK_NEQ
            || op == TOK_LT || op == TOK_GT || op == TOK_LE || op == TOK_GE)) {
            if (e->bin.l->type == AST_PROP) {
                /* IS NULL / IS NOT NULL */
                if (e->bin.r->type == AST_NULL) {
                    const char *v = gs_prop_str(gs, nid, e->bin.l->prop.n->str);
                    double dv = gs_prop_num(gs, nid, e->bin.l->prop.n->str);
                    int is_null = (!v && dv == 0.0);
                    if (op == TOK_EQ) return is_null;
                    if (op == TOK_NEQ) return !is_null;
                    return 1;
                }
                if (e->bin.r->type == AST_STRING) {
                    const char *v = gs_prop_str(gs, nid, e->bin.l->prop.n->str);
                    if (op == TOK_EQ) return v && !strcmp(v, e->bin.r->str);
                    if (op == TOK_NEQ) return !v || !!strcmp(v, e->bin.r->str);
                    return 1;
                }
                if (e->bin.r->type == AST_INTEGER || e->bin.r->type == AST_FLOAT) {
                    double lv = gs_prop_num(gs, nid, e->bin.l->prop.n->str);
                    if (lv == 0.0) lv = (double)gs_prop_int(gs, nid, e->bin.l->prop.n->str);
                    double rv = e->bin.r->type == AST_INTEGER ? (double)e->bin.r->ival : e->bin.r->fval;
                    if (op == TOK_EQ) return lv == rv;
                    if (op == TOK_NEQ) return lv != rv;
                    if (op == TOK_LT) return lv < rv;
                    if (op == TOK_GT) return lv > rv;
                    if (op == TOK_LE) return lv <= rv;
                    if (op == TOK_GE) return lv >= rv;
                }
                /* arithmetic on right side: e.g. n.val > n.other + 5 */
                if (e->bin.r->type == AST_PROP || e->bin.r->type == AST_BINARY) {
                    double lv = gs_prop_num(gs, nid, e->bin.l->prop.n->str);
                    if (lv == 0.0) lv = (double)gs_prop_int(gs, nid, e->bin.l->prop.n->str);
                    double rv = fsm_eval_value(ctx, nid, e->bin.r);
                    if (op == TOK_EQ) return lv == rv;
                    if (op == TOK_NEQ) return lv != rv;
                    if (op == TOK_LT) return lv < rv;
                    if (op == TOK_GT) return lv > rv;
                    if (op == TOK_LE) return lv <= rv;
                    if (op == TOK_GE) return lv >= rv;
                }
            }
            /* both sides are arithmetic expressions */
            if ((e->bin.l->type == AST_BINARY || e->bin.r->type == AST_BINARY)
                && e->bin.l->type != AST_PROP) {
                double lv = fsm_eval_value(ctx, nid, e->bin.l);
                double rv = fsm_eval_value(ctx, nid, e->bin.r);
                if (op == TOK_EQ) return lv == rv;
                if (op == TOK_NEQ) return lv != rv;
                if (op == TOK_LT) return lv < rv;
                if (op == TOK_GT) return lv > rv;
                if (op == TOK_LE) return lv <= rv;
                if (op == TOK_GE) return lv >= rv;
            }
        }
        /* CONTAINS / STARTS WITH / ENDS WITH */
        if (op == TOK_CONTAINS || op == TOK_STARTS || op == TOK_ENDS) {
            if (e->bin.l->type == AST_PROP && e->bin.r->type == AST_STRING) {
                const char *v = gs_prop_str(gs, nid, e->bin.l->prop.n->str);
                if (!v) return 0;
                if (op == TOK_CONTAINS) return !!strstr(v, e->bin.r->str);
                if (op == TOK_STARTS) return !strncmp(v, e->bin.r->str, strlen(e->bin.r->str));
                if (op == TOK_ENDS) {
                    size_t vl = strlen(v), sl = strlen(e->bin.r->str);
                    return vl >= sl && !strcmp(v + vl - sl, e->bin.r->str);
                }
            }
            return 1;
        }
        /* IN */
        if (op == TOK_IN) {
            if (e->bin.l->type == AST_PROP && e->bin.r->type == AST_LIST) {
                const char *v = gs_prop_str(gs, nid, e->bin.l->prop.n->str);
                if (!v) {
                    double dv = gs_prop_num(gs, nid, e->bin.l->prop.n->str);
                    for (int li = 0; li < e->bin.r->list.n; li++) {
                        cypher_ast_t *li_item = e->bin.r->list.items[li];
                        if (li_item->type == AST_INTEGER && dv == (double)li_item->ival) return 1;
                        if (li_item->type == AST_FLOAT && dv == li_item->fval) return 1;
                    }
                    return 0;
                }
                for (int li = 0; li < e->bin.r->list.n; li++) {
                    cypher_ast_t *li_item = e->bin.r->list.items[li];
                    if (li_item->type == AST_STRING && !strcmp(v, li_item->str)) return 1;
                }
                return 0;
            }
        }
    }
    if (e->type == AST_NOT) return !fsm_eval_where(ctx, nid, e->bin.l);
    return 1;
}

static void fsm_emit_cell(cypher_fsm_ctx_t *ctx, int row, int col,
                           uint32_t nid, cypher_ast_t *expr) {
    graph_store_t *gs = ctx->gs;
    char buf[512]; buf[0] = 0;
    if (expr->type == AST_IDENT) {
        if (!strcmp(expr->str, "*")) {
            int off = 0;
            off += snprintf(buf + off, sizeof(buf) - off, "(");
            if (gs->nodes[nid].props_off != 0xFFFFFFFF) {
                int first = 1;
                for (uint32_t pi = gs->nodes[nid].props_off;
                     pi < gs->prop_count && pi < gs->nodes[nid].props_off + gs->nodes[nid].prop_count
                     && off < 480; pi++) {
                    char *vbuf = gs->val_data + gs->props[pi].val_off;
                    if (vbuf[0] != 0) continue;
                    if (!first) off += snprintf(buf + off, sizeof(buf) - off, ", ");
                    first = 0;
                    if (off < 480) off += snprintf(buf + off, sizeof(buf) - off, "%s", vbuf + 1);
                }
            }
            if (off < 500) snprintf(buf + off, sizeof(buf) - off, ")");
        } else {
            const char *lab = NULL;
            for (uint32_t li = 0; li < gs->label_count; li++)
                if (gs->nodes[nid].label_mask & (1ULL << li)) { lab = gs->labels[li].name; break; }
            const char *pv = gs_prop_str(gs, nid, expr->str);
            if (pv) snprintf(buf, sizeof(buf), "%s", pv);
            else if (lab && lab[0]) snprintf(buf, sizeof(buf), "(:%s)", lab);
            else snprintf(buf, sizeof(buf), "(node %u)", nid);
        }
    } else if (expr->type == AST_PROP) {
        const char *pkey = expr->prop.n->str;
        const char *pv = gs_prop_str(gs, nid, pkey);
        if (pv) snprintf(buf, sizeof(buf), "%s", pv);
        else { double dv = gs_prop_num(gs, nid, pkey);
               snprintf(buf, sizeof(buf), "%g", dv); }
    } else if (expr->type == AST_STRING) {
        snprintf(buf, sizeof(buf), "%s", expr->str);
    } else if (expr->type == AST_INTEGER) {
        snprintf(buf, sizeof(buf), "%d", expr->ival);
    } else if (expr->type == AST_FLOAT) {
        snprintf(buf, sizeof(buf), "%g", expr->fval);
    } else if (expr->type == AST_FUNCALL) {
        snprintf(buf, sizeof(buf), "?");
    } else {
        snprintf(buf, sizeof(buf), "?");
    }
    cypher_result_set_cell(ctx->result, row, col, buf);
}

/* fallback: label extraction helper */
static cypher_ast_t *fsm_extract_labels(cypher_ast_t *node_pat) {
    cypher_ast_t *labels = NULL;
    for (cypher_ast_t *p = node_pat->node.props; p; ) {
        cypher_ast_t *nxt = p->next;
        if (p->type == AST_LABEL) { p->next = labels; labels = p; }
        p = nxt;
    }
    return labels;
}
static cypher_ast_t *fsm_extract_props(cypher_ast_t *node_pat) {
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

/* extract the first property key referenced in a WHERE expression */
static const char *fsm_first_prop_key(cypher_ast_t *e) {
    if (!e) return NULL;
    if (e->type == AST_PROP && e->prop.n) return e->prop.n->str;
    if (e->type == AST_NOT) return fsm_first_prop_key(e->bin.l);
    if (e->type == AST_BINARY) {
        const char *k = fsm_first_prop_key(e->bin.l);
        if (k) return k;
        return fsm_first_prop_key(e->bin.r);
    }
    return NULL;
}

/* extract the search string from the first CONTAINS/STARTS/ENDS in WHERE.
   returns the search string, sets *prop_key and *op_type */
static const char *fsm_first_text_op(cypher_ast_t *e, const char **prop_key, int *op_type) {
    if (!e) return NULL;
    if (e->type == AST_BINARY) {
        int op = e->bin.op;
        if ((op == TOK_CONTAINS || op == TOK_STARTS || op == TOK_ENDS)
            && e->bin.l->type == AST_PROP && e->bin.r->type == AST_STRING) {
            *prop_key = e->bin.l->prop.n->str;
            *op_type = op;
            return e->bin.r->str;
        }
        if (op == TOK_AND) {
            const char *r = fsm_first_text_op(e->bin.l, prop_key, op_type);
            if (r) return r;
            return fsm_first_text_op(e->bin.r, prop_key, op_type);
        }
        if (op == TOK_OR) {
            const char *r = fsm_first_text_op(e->bin.l, prop_key, op_type);
            if (r) return r;
        }
    }
    return NULL;
}

/* ---------- Goto-based FSM executor ---------- */

cypher_result_t *cypher_fsm_exec(cypher_graph_t *g, cypher_ast_t *match_cl,
                                  cypher_ast_t *return_cl) {
    graph_store_t *gs = (graph_store_t *)cypher_graph_get_store(g);
    if (!gs || !return_cl) return cypher_result_new();
    cypher_fsm_ctx_t ctx;
    ctx.gs = gs;
    ctx.result = cypher_result_new();
    ctx.use_jit = 0;
    ctx.ok = 1;

    /* set columns */
    for (int i = 0; i < return_cl->list.n; i++) {
        cypher_ast_t *col = return_cl->list.items[i];
        const char *name = "expr";
        if (col->col.as) name = col->col.as->str;
        else if (col->col.name->type == AST_IDENT) name = col->col.name->str;
        else if (col->col.name->type == AST_PROP && col->col.name->prop.n)
            name = col->col.name->prop.n->str;
        else if (col->col.name->type == AST_FUNCALL && col->col.name->call.func)
            name = col->col.name->call.func->str;
        cypher_result_add_col(ctx.result, name);
    }

    /* set up candidate list */
    uint32_t *candidates = NULL;
    uint32_t ncan = 0, cap = 16384;
    uint32_t ci = 0;
    cypher_ast_t *labels = NULL, *props = NULL, *where = NULL;
    int has_edge = 0;
    int optional = (match_cl && match_cl->bin.op == 1);

    /* multi-hop: extract all node patterns and relationship patterns */
    #define MAX_HOPS 8
    cypher_ast_t *hop_nodes[MAX_HOPS];     /* node patterns: items[0], items[2], items[4], ... */
    cypher_ast_t *hop_rels[MAX_HOPS];      /* rel patterns:  items[1], items[3], items[5], ... */
    cypher_ast_t *hop_labels[MAX_HOPS];    /* extracted labels per hop node */
    cypher_ast_t *hop_props[MAX_HOPS];     /* extracted props per hop node */
    const char   *hop_edge_types[MAX_HOPS];/* edge type per hop */
    int num_hops = 0;

    if (match_cl && match_cl->bin.l && match_cl->bin.l->type == AST_PATTERN) {
        cypher_ast_t *pat = match_cl->bin.l;
        where  = match_cl->bin.r;

        for (int i = 0; i < pat->list.n; i++) {
            cypher_ast_t *it = pat->list.items[i];
            if (it->type == AST_NODE_PAT) {
                hop_nodes[num_hops] = it;
                hop_labels[num_hops] = fsm_extract_labels(it);
                hop_props[num_hops]  = fsm_extract_props(it);
                num_hops++;
            } else if (it->type == AST_REL_PAT && num_hops > 0) {
                hop_rels[num_hops - 1] = it;
                if (it->rel.labels && it->rel.labels->str[0])
                    hop_edge_types[num_hops - 1] = it->rel.labels->str;
                else
                    hop_edge_types[num_hops - 1] = NULL;
            }
        }
        if (num_hops >= 2) has_edge = 1;
        labels = hop_labels[0];
        props  = hop_props[0];

        candidates = malloc(cap * sizeof(uint32_t));
        {
            int use_prop_idx = 0;
            const char *pk = fsm_first_prop_key(where);
            if (!pk) pk = fsm_first_prop_key(props);

            /* trigram text index: check for CONTAINS/STARTS/ENDS */
            const char *tk = NULL;
            int top_type = 0;
            const char *search_str = fsm_first_text_op(where, &tk, &top_type);

            if (search_str && tk) {
                ncan = gs_text_search(gs, tk, search_str, candidates, cap);
                if (ncan == 0) { /* trigram found nothing, fall back */ }
            } else if (use_prop_idx) {
                ncan = gs_prop_key_nodes(gs, pk, candidates, cap);
            } else if (labels && labels->str[0]) {
                ncan = gs_label_nodes(gs, labels->str, candidates, cap);
            } else if (pk) {
                ncan = gs_prop_key_nodes(gs, pk, candidates, cap);
                if (ncan == 0) {
                    ncan = gs_node_count(gs);
                    if (ncan > cap) { cap = ncan; candidates = realloc(candidates, cap * sizeof(uint32_t)); }
                    for (uint32_t ni = 0; ni < ncan; ni++) candidates[ni] = ni;
                }
            } else {
                ncan = gs_node_count(gs);
                if (ncan > cap) { cap = ncan; candidates = realloc(candidates, cap * sizeof(uint32_t)); }
                for (uint32_t ni = 0; ni < ncan; ni++) candidates[ni] = ni;
            }

            /* if text search returned 0, use label/prop index as fallback */
            if (ncan == 0 && (labels || pk)) {
                if (labels && labels->str[0]) {
                    ncan = gs_label_nodes(gs, labels->str, candidates, cap);
                } else if (pk) {
                    ncan = gs_prop_key_nodes(gs, pk, candidates, cap);
                }
            }
        }
    } else {
        candidates = malloc(cap * sizeof(uint32_t));
        ncan = gs_node_count(gs);
        if (ncan > cap) { cap = ncan; candidates = realloc(candidates, cap * sizeof(uint32_t)); }
        for (uint32_t ni = 0; ni < ncan; ni++) candidates[ni] = ni;
    }

    /* ---- GOTO-BASED STATE MACHINE (with multi-hop support) ---- */
    uint32_t nid = 0;
    int hop = 0;
    uint32_t hop_nids[MAX_HOPS];
    uint32_t hop_ej[MAX_HOPS];
    uint32_t hop_ec[MAX_HOPS];
    goto state_scan;

state_scan:
    if (ci >= ncan || ctx.result->nrows >= 200) goto state_done;
    nid = candidates[ci++];
    goto state_filter;

state_filter:
    if (labels && !fsm_match_label(&ctx, nid, labels)) goto state_scan;
    if (props && !fsm_match_props(&ctx, nid, props)) goto state_scan;
    if (where && !fsm_eval_where(&ctx, nid, where)) goto state_scan;
    if (has_edge) {
        hop = 0;
        hop_nids[0] = nid;
        goto state_expand;
    }
    if (optional) goto state_emit_null;
    goto state_emit;

state_expand:
    {
        uint32_t src = hop_nids[hop];
        hop_ec[hop] = gs_edge_count(gs, src);
        hop_ej[hop] = 0;
        if (hop_ec[hop] == 0 && optional) goto state_emit_null;
    }
state_expand_loop:
    if (hop_ej[hop] >= hop_ec[hop]) {
        if (hop == 0) goto state_scan;
        hop--;
        hop_ej[hop]++;
        goto state_expand_loop;
    }
    {
        uint32_t dst = gs_edge_dst(gs, hop_nids[hop], hop_ej[hop]);
        hop_ej[hop]++;
        if (dst == 0xFFFFFFFF) goto state_expand_loop;

        if (hop_edge_types[hop]) {
            uint32_t et = gs_edge_type(gs, hop_nids[hop], hop_ej[hop] - 1);
            if (gs_hash_str(hop_edge_types[hop]) != et) goto state_expand_loop;
        }

        int tgt_idx = hop + 1;
        if (hop_labels[tgt_idx] && !fsm_match_label(&ctx, dst, hop_labels[tgt_idx]))
            goto state_expand_loop;
        if (hop_props[tgt_idx] && !fsm_match_props(&ctx, dst, hop_props[tgt_idx]))
            goto state_expand_loop;

        hop_nids[tgt_idx] = dst;
        if (hop + 1 >= num_hops - 1) {
            goto state_emit_multi;
        }
        hop++;
        goto state_expand;
    }

state_emit_multi:
    cypher_result_add_row_empty(ctx.result);
    for (int ci2 = 0; ci2 < return_cl->list.n; ci2++) {
        cypher_ast_t *col = return_cl->list.items[ci2];
        uint32_t un = hop_nids[0];
        if (col->col.name->type == AST_PROP
            && col->col.name->prop.e->type == AST_IDENT) {
            for (int h = 0; h < num_hops; h++) {
                if (hop_nodes[h]->node.name
                    && !strcmp(col->col.name->prop.e->str,
                               hop_nodes[h]->node.name->str)) {
                    un = hop_nids[h];
                    break;
                }
            }
        }
        fsm_emit_cell(&ctx, ctx.result->nrows-1, ci2, un, col->col.name);
    }
    goto state_expand_loop;

state_emit_null:
    cypher_result_add_row_empty(ctx.result);
    for (int ci2 = 0; ci2 < return_cl->list.n; ci2++) {
        cypher_ast_t *col = return_cl->list.items[ci2];
        int use_null = 0;
        if (col->col.name->type == AST_PROP
            && col->col.name->prop.e->type == AST_IDENT) {
            const char *vname = col->col.name->prop.e->str;
            /* check if this variable belongs to a hop target */
            for (int h = 1; h < num_hops; h++) {
                if (hop_nodes[h]->node.name && !strcmp(vname, hop_nodes[h]->node.name->str)) {
                    use_null = 1; break;
                }
            }
        }
        if (use_null)
            cypher_result_set_cell(ctx.result, ctx.result->nrows-1, ci2, "NULL");
        else
            fsm_emit_cell(&ctx, ctx.result->nrows-1, ci2, hop_nids[0], col->col.name);
    }
    goto state_scan;

state_emit:
    cypher_result_add_row_empty(ctx.result);
    for (int ci2 = 0; ci2 < return_cl->list.n; ci2++)
        fsm_emit_cell(&ctx, ctx.result->nrows-1, ci2, nid,
                       return_cl->list.items[ci2]->col.name);
    goto state_scan;

state_done:
    free(candidates);

    /* COUNT(*) aggregation: compress all rows into a single aggregate row */
    for (int ci = 0; ci < return_cl->list.n; ci++) {
        cypher_ast_t *col = return_cl->list.items[ci];
        if (col->col.name->type == AST_FUNCALL && col->col.name->call.func
            && !strcasecmp(col->col.name->call.func->str, "COUNT")) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%d", ctx.result->nrows);
            /* free all rows, create one aggregate row */
            for (int ri = 0; ri < ctx.result->nrows; ri++) {
                for (int ck = 0; ck < ctx.result->ncols; ck++)
                    free(ctx.result->rows[ri][ck]);
                free(ctx.result->rows[ri]);
            }
            ctx.result->nrows = 0;
            cypher_result_add_row_empty(ctx.result);
            cypher_result_set_cell(ctx.result, 0, ci, buf);
            break;
        }
    }

    /* post-processing */
    {
        cypher_ast_t *order = return_cl->bin.r;
        if (order) {
            int ncols = ctx.result->ncols;
            for (int i = 0; i < ctx.result->nrows - 1; i++) {
                for (int j = 0; j < ctx.result->nrows - i - 1; j++) {
                    int cmp = 0;
                    for (cypher_ast_t *o = order; o; o = o->next) {
                        if (o->type != AST_ORDER_ITEM) continue;
                        const char *key = NULL;
                        if (o->bin.l && o->bin.l->type == AST_IDENT)
                            key = o->bin.l->str;
                        else if (o->bin.l && o->bin.l->type == AST_PROP
                                 && o->bin.l->prop.n)
                            key = o->bin.l->prop.n->str;
                        if (!key) continue;
                        int col = -1;
                        for (int ci = 0; ci < ncols; ci++)
                            if (!strcmp(ctx.result->columns[ci], key)) { col = ci; break; }
                        if (col < 0) continue;
                        const char *a = ctx.result->rows[j][col];
                        const char *b = ctx.result->rows[j+1][col];
                        if (!a && !b) continue;
                        if (!a) { cmp = -1; break; }
                        if (!b) { cmp = 1; break; }
                        cmp = strcasecmp(a, b);
                        if (o->bin.op) cmp = -cmp;
                        if (cmp != 0) break;
                    }
                    if (cmp > 0) {
                        char **tmp = ctx.result->rows[j];
                        ctx.result->rows[j] = ctx.result->rows[j+1];
                        ctx.result->rows[j+1] = tmp;
                    }
                }
            }
        }
        if (return_cl->bin.op == 1) {
            int write = 0;
            for (int i = 0; i < ctx.result->nrows; i++) {
                int dup = 0;
                for (int k = 0; k < write; k++) {
                    int same = 1;
                    for (int ck = 0; ck < ctx.result->ncols; ck++) {
                        const char *a = ctx.result->rows[i][ck];
                        const char *b = ctx.result->rows[k][ck];
                        if (!a && !b) continue;
                        if (!a || !b || strcmp(a, b)) { same = 0; break; }
                    }
                    if (same) { dup = 1; break; }
                }
                if (!dup) {
                    if (write != i) {
                        char **tmp = ctx.result->rows[i];
                        ctx.result->rows[i] = ctx.result->rows[write];
                        ctx.result->rows[write] = tmp;
                    }
                    write++;
                }
            }
            ctx.result->nrows = write;
        }
        /* LIMIT / SKIP */
        {
            int limit_val = ctx.result->nrows;
            int skip_val = 0;
            for (cypher_ast_t *n = return_cl->next; n; n = n->next) {
                if (n->type == AST_LIMIT) limit_val = n->ival;
                else if (n->type == AST_SKIP) skip_val = n->ival;
            }
            if (skip_val > 0) {
                for (int i = 0; i < skip_val && i < ctx.result->nrows; i++) {
                    if (ctx.result->rows[i]) {
                        for (int ck = 0; ck < ctx.result->ncols; ck++)
                            free(ctx.result->rows[i][ck]);
                        free(ctx.result->rows[i]);
                        ctx.result->rows[i] = NULL;
                    }
                }
                int dst = 0;
                for (int i = skip_val; i < ctx.result->nrows; i++) {
                    ctx.result->rows[dst] = ctx.result->rows[i];
                    ctx.result->rows[i] = NULL;
                    dst++;
                }
                ctx.result->nrows = dst;
            }
            for (int i = limit_val; i < ctx.result->nrows; i++) {
                if (ctx.result->rows[i]) {
                    for (int ck = 0; ck < ctx.result->ncols; ck++)
                        free(ctx.result->rows[i][ck]);
                    free(ctx.result->rows[i]);
                    ctx.result->rows[i] = NULL;
                }
            }
            if (limit_val < ctx.result->nrows) ctx.result->nrows = limit_val;
        }
    }

    return ctx.result;
}

/* ---------- Phase 3: JIT compilation (placeholder) ---------- */

int cypher_fsm_init(void) {
    mkdir(FSM_TMPDIR, 0755);
    return 0;
}

cypher_result_t *cypher_fsm_exec_optional(cypher_graph_t *g,
                                           cypher_ast_t *match_cl,
                                           cypher_ast_t *opt_match,
                                           cypher_ast_t *return_cl) {
    /* execute primary MATCH, treat optional MATCH as required for now.
       Full OPTIONAL MATCH (outer join with NULL rows) needs multi-clause
       architecture with cross-clause variable binding. */
    (void)opt_match;
    return cypher_fsm_exec(g, match_cl, return_cl);
}

void cypher_fsm_cleanup(void) {
    /* cleanup temp .so files */
}
