/*
 * graph_jit.c — Phase 3 JIT query compiler (Ragel edition).
 *
 * Generates a .rl file with the label and CONTAINS needle embedded as
 * Ragel scanner literals. Compiled via ragel→gcc→dlopen.
 * Zero comparison overhead — the FSM directly matches the bytes.
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

int g_jit_enabled = 0;
static char g_jit_cwd[1024] = "";

static void gen_esc(FILE *f, const char *s) {
    for (int i = 0; s[i]; i++) {
        if (s[i] == '\\') fputs("\\\\", f);
        else if (s[i] == '"') fputs("\\\"", f);
        else if (s[i] == '\n') fputs("\\n", f);
        else fputc(s[i], f);
    }
}

static uint32_t query_hash(cypher_ast_t *m, cypher_ast_t *r) {
    uint32_t h = 0;
    if (m && m->bin.l) {
        cypher_ast_t *p = m->bin.l;
        for (int i = 0; i < p->list.n; i++) {
            cypher_ast_t *it = p->list.items[i];
            if (it->type == AST_NODE_PAT && it->node.name)
                for (const char *s = it->node.name->str; *s; s++) h = h*31+*s;
            if (it->type == AST_REL_PAT && it->rel.labels)
                for (const char *s = it->rel.labels->str; *s; s++) h = h*31+*s;
        }
    }
    for (int i = 0; i < r->list.n; i++) {
        cypher_ast_t *c = r->list.items[i];
        const char *n = NULL;
        if (c->col.name->type == AST_PROP && c->col.name->prop.n) n = c->col.name->prop.n->str;
        else if (c->col.name->type == AST_IDENT) n = c->col.name->str;
        if (n) for (const char *s = n; *s; s++) h = h*31+*s;
    }
    return h;
}

int cypher_jit_init(void) {
    mkdir(JIT_DIR, 0755);
    if (!g_jit_cwd[0]) {
        if (!getcwd(g_jit_cwd, sizeof(g_jit_cwd)))
            strcpy(g_jit_cwd, ".");
    }
    return 0;
}

jit_query_func cypher_jit_compile(cypher_graph_t *g, cypher_ast_t *match_cl,
                                   cypher_ast_t *return_cl) {
    if (!return_cl) return NULL;

    uint32_t h = query_hash(match_cl, return_cl);
    char so_path[512];
    snprintf(so_path, sizeof(so_path), "%s/q_%u.so", JIT_DIR, h);

    /* check cache */
    if (access(so_path, R_OK) == 0) {
        void *hdl = dlopen(so_path, RTLD_NOW | RTLD_GLOBAL);
        if (hdl) {
            jit_query_func fn = (jit_query_func)dlsym(hdl, "query_exec");
            if (fn) return fn;
            dlclose(hdl);
        }
    }

    /* extract query parameters */
    const char *label = NULL, *contains = NULL;
    int limit_val = 200;

    if (match_cl && match_cl->bin.l && match_cl->bin.l->type == AST_PATTERN) {
        cypher_ast_t *fn = match_cl->bin.l->list.items[0];
        if (fn && fn->type == AST_NODE_PAT)
            for (cypher_ast_t *p = fn->node.props; p; p = p->next)
                if (p->type == AST_LABEL) { label = p->str; break; }
        if (match_cl->bin.r) {
            cypher_ast_t *w = match_cl->bin.r;
            if (w->type == AST_BINARY
                && (w->bin.op == TOK_CONTAINS || w->bin.op == TOK_STARTS)
                && w->bin.r && w->bin.r->type == AST_STRING)
                contains = w->bin.r->str;
        }
    }
    if (return_cl->next)
        for (cypher_ast_t *n = return_cl->next; n; n = n->next)
            if (n->type == AST_LIMIT) limit_val = n->ival;

    if (!label) return NULL;

    /* generate .rl file with label inlined as Ragel literal */
    char rl_path[512], c_path[512], err_path[512];
    snprintf(rl_path,  sizeof(rl_path),  "%s/q_%u.rl", JIT_DIR, h);
    snprintf(c_path,   sizeof(c_path),   "%s/q_%u.c",  JIT_DIR, h);
    snprintf(err_path, sizeof(err_path), "%s/err.txt",  JIT_DIR);

    FILE *f = fopen(rl_path, "w");
    if (!f) return NULL;

    fprintf(f, "/* JIT Ragel query: MATCH (n:%s)%s%s */\n",
            label, contains ? " WHERE ... CONTAINS ..." : "", limit_val < 200 ? " LIMIT" : "");
    fprintf(f, "#include <stdio.h>\n");
    fprintf(f, "#include <string.h>\n");
    fprintf(f, "#include \"cypher_parser.h\"\n\n");

    fprintf(f, "%%%%{\n");
    fprintf(f, "  machine jit_q;\n");
    fprintf(f, "  write data;\n");
    fprintf(f, "}%%%%\n\n");

    fprintf(f, "void query_exec(void *data_ptr, void *result_ptr) {\n");
    fprintf(f, "  cypher_result_t *res = (cypher_result_t *)result_ptr;\n");
    fprintf(f, "  const char *p = *(const char **)data_ptr;\n");
    fprintf(f, "  const char *pe = p + ((size_t *)data_ptr)[1];\n");
    fprintf(f, "  const char *eof = pe;\n");
    fprintf(f, "  int cs, act;\n");
    fprintf(f, "  const char *ts, *te;\n");
    fprintf(f, "  int count = 0, match = 0;\n");
    fprintf(f, "  char buf[2048];\n");

    fprintf(f, "  %%%%{\n");
    fprintf(f, "    action rec_start   { match = 0; }\n");
    fprintf(f, "    action found_label { match = 1; }\n");
    fprintf(f, "    action found_text  {\n");
    fprintf(f, "      if (match && count < %d) {\n", limit_val);
    fprintf(f, "        const char *vs = ts + 8;\n");
    fprintf(f, "        int vl = (int)(te - vs - 1);\n");
    fprintf(f, "        int ti = 0;\n");
    fprintf(f, "        for (int i = 0; i < vl && ti < 2047; i++) {\n");
    fprintf(f, "          if (vs[i] == '\\\\' && i+1 < vl) {\n");
    fprintf(f, "            i++;\n");
    fprintf(f, "            if (vs[i] == 'n') buf[ti++] = '\\n';\n");
    fprintf(f, "            else buf[ti++] = vs[i];\n");
    fprintf(f, "          } else buf[ti++] = vs[i];\n");
    fprintf(f, "        }\n");
    fprintf(f, "        buf[ti] = '\\0';\n");
    if (contains) {
        fprintf(f, "        if (strstr(buf, \"");
        gen_esc(f, contains);
        fprintf(f, "\")) {\n");
    }
    fprintf(f, "          cypher_result_add_row_empty(res);\n");
    fprintf(f, "          cypher_result_set_cell(res, count, 0, buf);\n");
    fprintf(f, "          count++;\n");
    if (contains) fprintf(f, "        }\n");
    fprintf(f, "      }\n");
    fprintf(f, "      match = 0;\n");
    fprintf(f, "    }\n");

    /* the key innovation: label name inlined as Ragel literal */
    fprintf(f, "    main := |*\n");
    fprintf(f, "      '{' => rec_start;\n");
    fprintf(f, "      '\"type\":\"");
    gen_esc(f, label);
    fprintf(f, "\"' => found_label;\n");
    fprintf(f, "      '\"text\":\"' ([^\"\\\\] | '\\\\' any)* '\"' => found_text;\n");
    fprintf(f, "      any;\n");
    fprintf(f, "    *|;\n");
    fprintf(f, "  }%%%%\n\n");

    fprintf(f, "  %%%% write init;\n");
    fprintf(f, "  %%%% write exec;\n");
    fprintf(f, "}\n");
    fclose(f);

    /* compile: ragel → .c */
    char cmd[2048];
    snprintf(cmd, sizeof(cmd), "ragel -m -G2 -o %s %s 2>%s",
             c_path, rl_path, err_path);
    if (system(cmd) != 0) { unlink(rl_path); return NULL; }

    /* compile: gcc → .so */
    snprintf(cmd, sizeof(cmd),
        "gcc -O2 -shared -fPIC -I%s -I%s/vendor/klib -o %s %s 2>%s",
        g_jit_cwd, g_jit_cwd, so_path, c_path, err_path);
    if (system(cmd) != 0) { unlink(rl_path); unlink(c_path); return NULL; }

    void *hdl = dlopen(so_path, RTLD_NOW | RTLD_GLOBAL);
    if (!hdl) { unlink(rl_path); unlink(c_path); return NULL; }

    jit_query_func fn = (jit_query_func)dlsym(hdl, "query_exec");
    if (!fn) { dlclose(hdl); unlink(rl_path); unlink(c_path); return NULL; }

    unlink(rl_path); unlink(c_path);
    return fn;
}

cypher_result_t *cypher_jit_exec(cypher_graph_t *g, cypher_ast_t *match_cl,
                                  cypher_ast_t *return_cl) {
    if (!return_cl) return cypher_result_new();
    jit_query_func fn = cypher_jit_compile(g, match_cl, return_cl);
    if (!fn) return NULL;
    cypher_result_t *result = cypher_result_new();
    fn(cypher_graph_get_store(g), result);
    return result;
}

void cypher_jit_free(jit_query_func f) { (void)f; }
void cypher_jit_cleanup(void) {}
