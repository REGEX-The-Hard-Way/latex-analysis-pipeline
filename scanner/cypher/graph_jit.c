/*
 * graph_jit.c — Phase 3 JIT query compiler.
 *
 * Generates a C source file from a Cypher AST, compiles it with -O2,
 * dlopen's the .so, and invokes the inlined query function.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <unistd.h>
#include "graph_jit.h"
#include "graph_store.h"

#define JIT_DIR "/tmp/cypher_jit"
#define MAX_SRC  (256 * 1024)

int g_jit_enabled = 1;

/* ---- C code generation helpers ---- */

static void gen_escape(char *out, const char *in, int max) {
    int j = 0;
    for (int i = 0; in[i] && j < max - 2; i++) {
        if (in[i] == '\\') { out[j++] = '\\'; out[j++] = '\\'; }
        else if (in[i] == '"') { out[j++] = '\\'; out[j++] = '"'; }
        else if (in[i] == '\n') { out[j++] = '\\'; out[j++] = 'n'; }
        else out[j++] = in[i];
    }
    out[j] = '\0';
}

static void gen_expr(FILE *f, cypher_ast_t *e, const char *nvar) {
    if (!e) { fprintf(f, "1"); return; }
    if (e->type == AST_NOT) {
        fprintf(f, "!("); gen_expr(f, e->bin.l, nvar); fprintf(f, ")");
        return;
    }
    if (e->type == AST_BINARY) {
        int op = e->bin.op;
        if (op == TOK_AND) {
            fprintf(f, "("); gen_expr(f, e->bin.l, nvar);
            fprintf(f, "&&"); gen_expr(f, e->bin.r, nvar); fprintf(f, ")");
            return;
        }
        if (op == TOK_OR) {
            fprintf(f, "("); gen_expr(f, e->bin.l, nvar);
            fprintf(f, "||"); gen_expr(f, e->bin.r, nvar); fprintf(f, ")");
            return;
        }
        if (op == TOK_CONTAINS || op == TOK_STARTS || op == TOK_ENDS) {
            if (e->bin.l->type == AST_PROP && e->bin.r->type == AST_STRING) {
                fprintf(f, "({const char*v=gs_prop_str(gs,%s,\"%s\");",
                        nvar, e->bin.l->prop.n->str);
                if (op == TOK_CONTAINS)
                    fprintf(f, "v&&strstr(v,\"%s\")", e->bin.r->str);
                else if (op == TOK_STARTS)
                    fprintf(f, "v&&!strncmp(v,\"%s\",%zu)", e->bin.r->str, strlen(e->bin.r->str));
                else
                    fprintf(f, "v&&({size_t vl=strlen(v),sl=strlen(\"%s\");vl>=sl&&!strcmp(v+vl-sl,\"%s\");})",
                            e->bin.r->str, e->bin.r->str);
                fprintf(f, "?1:0;})");
            } else fprintf(f, "1");
            return;
        }
        if (e->bin.l->type == AST_PROP && e->bin.r) {
            const char *cmp = "==";
            if (op == TOK_NEQ) cmp = "!=";
            else if (op == TOK_LT) cmp = "<";
            else if (op == TOK_GT) cmp = ">";
            else if (op == TOK_LE) cmp = "<=";
            else if (op == TOK_GE) cmp = ">=";
            if (e->bin.r->type == AST_STRING) {
                fprintf(f, "({const char*v=gs_prop_str(gs,%s,\"%s\");v&&!strcmp(v,\"%s\");})",
                        nvar, e->bin.l->prop.n->str, e->bin.r->str);
                if (op == TOK_NEQ)
                    fprintf(f, " = ({const char*v=gs_prop_str(gs,%s,\"%s\");!v||strcmp(v,\"%s\");})",
                            nvar, e->bin.l->prop.n->str, e->bin.r->str);
            } else {
                double val = e->bin.r->type == AST_INTEGER ? (double)e->bin.r->ival : e->bin.r->fval;
                fprintf(f, "(gs_prop_num(gs,%s,\"%s\")%s%.15g)",
                        nvar, e->bin.l->prop.n->str, cmp, val);
            }
            return;
        }
    }
    fprintf(f, "1");
}

static void gen_query_prop(FILE *f, cypher_ast_t *col, int ci, const char *src_var,
                            const char *tgt_var, const char *tgt_name) {
    cypher_ast_t *e = col->col.name;
    const char *vname = NULL;
    if (e->type == AST_PROP && e->prop.e->type == AST_IDENT)
        vname = e->prop.e->str;

    const char *nvar = src_var;
    if (tgt_name && vname && !strcmp(vname, tgt_name)) nvar = tgt_var;

    if (e->type == AST_PROP && e->prop.n) {
        fprintf(f,
            "  {const char*v=gs_prop_str(gs,%s,\"%s\");"
            "char b[512];"
            "if(v)snprintf(b,sizeof(b),\"%%s\",v);"
            "else{double d=gs_prop_num(gs,%s,\"%s\");snprintf(b,sizeof(b),\"%%g\",d);}"
            "cypher_result_set_cell(result,result->nrows-1,%d,b);}\n",
            nvar, e->prop.n->str, nvar, e->prop.n->str, ci);
    } else if (e->type == AST_IDENT) {
        fprintf(f,
            "  {const char*v=gs_prop_str(gs,%s,\"%s\");"
            "char b[512];"
            "if(v)snprintf(b,sizeof(b),\"%%s\",v);"
            "else snprintf(b,sizeof(b),\"(node %%u)\",%s);"
            "cypher_result_set_cell(result,result->nrows-1,%d,b);}\n",
            nvar, e->str, nvar, ci);
    }
}

/* ---- main JIT compiler ---- */

int cypher_jit_init(void) {
    mkdir(JIT_DIR, 0755);
    return 0;
}

static char g_jit_cwd[1024] = "";

jit_query_func cypher_jit_compile(cypher_graph_t *g, cypher_ast_t *match_cl,
                                   cypher_ast_t *return_cl) {
    if (!return_cl) return NULL;

    if (!g_jit_cwd[0]) {
        if (!getcwd(g_jit_cwd, sizeof(g_jit_cwd)))
            strcpy(g_jit_cwd, ".");
    }

    /* hash the query for cache filename */
    uint32_t h = 0;
    for (int i = 0; i < return_cl->list.n; i++) {
        cypher_ast_t *col = return_cl->list.items[i];
        const char *n = NULL;
        if (col->col.name->type == AST_PROP && col->col.name->prop.n)
            n = col->col.name->prop.n->str;
        else if (col->col.name->type == AST_IDENT)
            n = col->col.name->str;
        if (n) { const char *p = n; while (*p) h = h * 31 + (unsigned char)*p++; }
    }
    if (match_cl && match_cl->bin.l && match_cl->bin.l->type == AST_PATTERN) {
        cypher_ast_t *pat = match_cl->bin.l;
        for (int i = 0; i < pat->list.n; i++) {
            cypher_ast_t *it = pat->list.items[i];
            if (it->type == AST_NODE_PAT && it->node.name) {
                const char *p = it->node.name->str;
                while (*p) h = h * 31 + (unsigned char)*p++;
            }
            if (it->type == AST_REL_PAT && it->rel.labels) {
                const char *p = it->rel.labels->str;
                while (*p) h = h * 31 + (unsigned char)*p++;
            }
        }
    }

    char so_path[512];
    snprintf(so_path, sizeof(so_path), "%s/q_%u.so", JIT_DIR, h);

    /* check cache */
    if (access(so_path, R_OK) == 0) {
        void *hnd = dlopen(so_path, RTLD_NOW | RTLD_GLOBAL);
        if (hnd) {
            jit_query_func f = (jit_query_func)dlsym(hnd, "query_exec");
            if (f) return f;
            dlclose(hnd);
        }
    }

    /* generate C source */
    char src_path[512];
    snprintf(src_path, sizeof(src_path), "%s/q_%u.c", JIT_DIR, h);
    FILE *f = fopen(src_path, "w");
    if (!f) return NULL;

    fprintf(f, "/* JIT-compiled Cypher query */\n");
    fprintf(f, "#include <stdio.h>\n");
    fprintf(f, "#include <string.h>\n");
    fprintf(f, "#include \"graph_store.h\"\n");
    fprintf(f, "#include \"cypher_parser.h\"\n\n");
    fprintf(f, "void query_exec(void *gs_ptr, void *result_ptr) {\n");
    fprintf(f, "  graph_store_t *gs = (graph_store_t *)gs_ptr;\n");
    fprintf(f, "  cypher_result_t *result = (cypher_result_t *)result_ptr;\n");
    fprintf(f, "  uint32_t nid = 0, ci = 0, ncan = 0;\n");
    fprintf(f, "  uint32_t candidates[65536];\n");

    /* extract pattern info */
    const char *src_label = NULL;
    const char *edge_type = NULL;
    const char *tgt_label = NULL;
    const char *tgt_name = NULL;
    int has_edge = 0;
    cypher_ast_t *where = NULL;
    cypher_ast_t *order = NULL;
    int distinct = 0;

    if (match_cl && match_cl->bin.l && match_cl->bin.l->type == AST_PATTERN) {
        cypher_ast_t *pat = match_cl->bin.l;
        where = match_cl->bin.r;
        if (pat->list.n >= 1 && pat->list.items[0]->type == AST_NODE_PAT) {
            cypher_ast_t *sn = pat->list.items[0];
            for (cypher_ast_t *p = sn->node.props; p; p = p->next)
                if (p->type == AST_LABEL) { src_label = p->str; break; }
        }
        if (pat->list.n >= 3 && pat->list.items[1]
            && pat->list.items[1]->type == AST_REL_PAT) {
            has_edge = 1;
            cypher_ast_t *rel = pat->list.items[1];
            if (rel->rel.labels && rel->rel.labels->str[0])
                edge_type = rel->rel.labels->str;
            cypher_ast_t *tn = pat->list.items[2];
            if (tn && tn->type == AST_NODE_PAT) {
                if (tn->node.name) tgt_name = tn->node.name->str;
                for (cypher_ast_t *p = tn->node.props; p; p = p->next)
                    if (p->type == AST_LABEL) { tgt_label = p->str; break; }
            }
        }
    }
    order = return_cl->bin.r;
    distinct = (return_cl->bin.op == 1);

    /* set columns */
    for (int i = 0; i < return_cl->list.n; i++) {
        cypher_ast_t *col = return_cl->list.items[i];
        const char *name = "expr";
        if (col->col.as) name = col->col.as->str;
        else if (col->col.name->type == AST_IDENT) name = col->col.name->str;
        else if (col->col.name->type == AST_PROP && col->col.name->prop.n)
            name = col->col.name->prop.n->str;
        char esc[256]; gen_escape(esc, name, sizeof(esc));
        fprintf(f, "  cypher_result_add_col(result, \"%s\");\n", esc);
    }

    /* candidate scan */
    if (src_label) {
        fprintf(f, "  ncan = gs_label_nodes(gs, \"%s\", candidates, 65536);\n", src_label);
    } else {
        fprintf(f, "  ncan = gs_node_count(gs);\n");
        fprintf(f, "  for (uint32_t ni = 0; ni < ncan; ni++) candidates[ni] = ni;\n");
    }

    fprintf(f, "\n  goto state_scan;\n\n");

    /* state_scan */
    fprintf(f, "state_scan:\n");
    fprintf(f, "  if (ci >= ncan || result->nrows >= 200) goto state_done;\n");
    fprintf(f, "  nid = candidates[ci++];\n");

    if (src_label) {
        fprintf(f, "  {\n    int ok = 0;\n");
        fprintf(f, "    uint64_t mask = 0;\n");
        fprintf(f, "    for (uint32_t li = 0; li < gs->label_count; li++) {\n");
        fprintf(f, "      if (!strcmp(gs->labels[li].name, \"%s\")) { mask = 1ULL << li; if (gs->nodes[nid].label_mask & mask) ok = 1; break; }\n", src_label);
        fprintf(f, "    }\n    if (!ok) goto state_scan;\n  }\n");
    }

    /* WHERE filter */
    if (where) {
        fprintf(f, "  ");
        gen_expr(f, where, "nid");
        fprintf(f, " || ({goto state_scan; 0;});\n");
    }

    /* edge expand */
    if (has_edge) {
        fprintf(f, "  {\n");
        fprintf(f, "    uint32_t ec = gs_edge_count(gs, nid);\n");
        fprintf(f, "    for (uint32_t ej = 0; ej < ec && result->nrows < 200; ej++) {\n");
        fprintf(f, "      uint32_t dst = gs_edge_dst(gs, nid, ej);\n");
        fprintf(f, "      if (dst == 0xFFFFFFFF) continue;\n");
        if (edge_type) {
            fprintf(f, "      { uint32_t et = gs_edge_type(gs, nid, ej);\n");
            fprintf(f, "        if (gs_hash_str(\"%s\") != et) continue; }\n", edge_type);
        }
        if (tgt_label) {
            fprintf(f, "      { int ok = 0; uint64_t tm = 0;\n");
            fprintf(f, "        for (uint32_t li = 0; li < gs->label_count; li++) {\n");
            fprintf(f, "          if (!strcmp(gs->labels[li].name, \"%s\")) { tm = 1ULL<<li; if (gs->nodes[dst].label_mask & tm) ok = 1; break; }\n", tgt_label);
            fprintf(f, "        }\n        if (!ok) continue; }\n");
        }
        fprintf(f, "      cypher_result_add_row_empty(result);\n");
        for (int i = 0; i < return_cl->list.n; i++)
            gen_query_prop(f, return_cl->list.items[i], i, "nid", "dst", tgt_name);
        fprintf(f, "    }\n  }\n");
    } else {
        fprintf(f, "  cypher_result_add_row_empty(result);\n");
        for (int i = 0; i < return_cl->list.n; i++)
            gen_query_prop(f, return_cl->list.items[i], i, "nid", "dst", NULL);
    }
    fprintf(f, "  goto state_scan;\n\n");

    /* state_done */
    fprintf(f, "state_done: (void)0;\n");

    /* post-processing */
    if (order) {
        fprintf(f, "  {\n  int nc = result->ncols;\n");
        fprintf(f, "  for (int i = 0; i < (int)result->nrows - 1; i++) {\n");
        fprintf(f, "    for (int j = 0; j < (int)result->nrows - i - 1; j++) {\n");
        fprintf(f, "      int cmp = 0;\n");
        for (cypher_ast_t *o = order; o; o = o->next) {
            if (o->type != AST_ORDER_ITEM) continue;
            const char *key = NULL;
            if (o->bin.l && o->bin.l->type == AST_IDENT) key = o->bin.l->str;
            else if (o->bin.l && o->bin.l->type == AST_PROP && o->bin.l->prop.n)
                key = o->bin.l->prop.n->str;
            if (!key) continue;
            fprintf(f, "      { int c = -1; for (int ci = 0; ci < nc; ci++) if (!strcmp(result->columns[ci], \"%s\")) c = ci;\n", key);
            fprintf(f, "        if (c >= 0) { const char *a = result->rows[j][c]; const char *b = result->rows[j+1][c];\n");
            fprintf(f, "          if (!a && !b) ; else if (!a) cmp = -1; else if (!b) cmp = 1; else cmp = strcmp(a,b); }\n");
            fprintf(f, "        if (cmp) break; }\n");
        }
        fprintf(f, "      if (cmp > 0) { char **t = result->rows[j]; result->rows[j] = result->rows[j+1]; result->rows[j+1] = t; }\n");
        fprintf(f, "    }\n  }\n  }\n");
    }
    if (distinct) {
        fprintf(f, "  { int wr = 0;\n");
        fprintf(f, "  for (int i = 0; i < (int)result->nrows; i++) {\n");
        fprintf(f, "    int dup = 0;\n");
        fprintf(f, "    for (int k = 0; k < wr; k++) { int same = 1;\n");
        fprintf(f, "      for (int c = 0; c < result->ncols; c++) {\n");
        fprintf(f, "        const char *a = result->rows[i][c], *b = result->rows[k][c];\n");
        fprintf(f, "        if (!a && !b); else if (!a || !b || strcmp(a,b)) same = 0;\n");
        fprintf(f, "      }\n      if (same) { dup = 1; break; }\n");
        fprintf(f, "    }\n    if (!dup) { if (wr != i) { char **t = result->rows[i]; result->rows[i] = result->rows[wr]; result->rows[wr] = t; } wr++; }\n");
        fprintf(f, "  }\n  result->nrows = wr; }\n");
    }
    /* LIMIT / SKIP */
    if (return_cl->next) {
        for (cypher_ast_t *n = return_cl->next; n; n = n->next) {
            if (n->type == AST_LIMIT)
                fprintf(f, "  if (result->nrows > %d) result->nrows = %d;\n", n->ival, n->ival);
            else if (n->type == AST_SKIP)
                fprintf(f, "  { int sk = %d; if (sk < (int)result->nrows) { for (int ii = sk; ii < (int)result->nrows; ii++) result->rows[ii-sk] = result->rows[ii]; result->nrows -= sk; } else result->nrows = 0; }\n", n->ival);
        }
    }

    fprintf(f, "}\n");
    fclose(f);

    /* compile */
    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
        "gcc -O2 -shared -fPIC -I%s -I%s/vendor/klib -o %s %s 2>/tmp/cypher_jit_err.txt",
        g_jit_cwd, g_jit_cwd, so_path, src_path);
    int rc = system(cmd);
    if (rc != 0) {
        /* log error */
        FILE *elog = fopen("/tmp/cypher_jit_err.txt", "r");
        if (elog) {
            char ebuf[1024]; size_t er = fread(ebuf, 1, sizeof(ebuf)-1, elog);
            if (er > 0) { ebuf[er] = '\0'; fprintf(stderr, "JIT gcc failed: %s\n", ebuf); }
            fclose(elog);
        }
        unlink(src_path); return NULL;
    }

    void *dlh = dlopen(so_path, RTLD_NOW | RTLD_GLOBAL);
    if (!dlh) return NULL;

    jit_query_func fn = (jit_query_func)dlsym(dlh, "query_exec");
    if (!fn) { dlclose(dlh); return NULL; }

    return fn;
}

cypher_result_t *cypher_jit_exec(cypher_graph_t *g, cypher_ast_t *match_cl,
                                  cypher_ast_t *return_cl) {
    if (!return_cl) return cypher_result_new();

    jit_query_func fn = cypher_jit_compile(g, match_cl, return_cl);
    if (!fn) return NULL; /* caller falls back to interpreter */

    cypher_result_t *result = cypher_result_new();
    fn((void *)cypher_graph_get_store(g), (void *)result);
    return result;
}

void cypher_jit_free(jit_query_func f) { (void)f; }

void cypher_jit_cleanup(void) {}
