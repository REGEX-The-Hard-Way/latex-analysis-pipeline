/*
 * bench_exec.c — Compare FSM vs interpreter query execution performance.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cypher_parser.h"
#include "graph_store.h"
#include "graph_exec.h"

#define MS(start, end) (((double)(end) - (double)(start)) / CLOCKS_PER_SEC * 1000.0)

static void bench_query(cypher_graph_t *g, const char *name, const char *query, int rounds) {
    static cypher_token_t tokens[MAX_TOKENS];
    int n = cypher_lex(query, (int)strlen(query), tokens, MAX_TOKENS);
    if (n == 0) { printf("  %-30s lex failed\n", name); return; }

    const char *error = NULL;
    cypher_ast_t *ast = cypher_parse(tokens, n, &error);
    if (!ast) { printf("  %-30s parse failed: %s\n", name, error); return; }

    /* find MATCH and RETURN */
    cypher_ast_t *match_cl = NULL, *return_cl = NULL;
    for (cypher_ast_t *cur = ast; cur; cur = cur->next) {
        if (!match_cl && cur->type == AST_MATCH) match_cl = cur;
        if (!return_cl && cur->type == AST_RETURN) return_cl = cur;
    }

    if (!return_cl) { cypher_ast_free(ast); return; }

    /* warm up */
    cypher_result_t *warm = cypher_fsm_exec(g, match_cl, return_cl);
    if (warm) cypher_result_free(warm);

    /* benchmark */
    clock_t t0 = clock();
    long total_rows = 0;
    for (int i = 0; i < rounds; i++) {
        cypher_result_t *r = cypher_fsm_exec(g, match_cl, return_cl);
        if (r) { total_rows += r->nrows; cypher_result_free(r); }
    }
    clock_t t1 = clock();

    double ms = MS(t0, t1);
    double per_query = ms / (double)rounds;
    printf("  %-30s %4d rounds  %8.3f ms  (%7.1f us/q, %ld rows)\n",
           name, rounds, ms, per_query * 1000.0, total_rows / rounds);

    cypher_ast_free(ast);
}

int main(int argc, char *argv[]) {
    cypher_graph_t *g = cypher_graph_new();
    cypher_fsm_init();

    const char *sidecar = "../sidecar.json";
    if (argc > 1) sidecar = argv[1];

    printf("Loading %s ...\n", sidecar);
    cypher_graph_load_sidecar(g, sidecar);
    printf("\nBenchmark (FSM executor, goto-based dispatch):\n");
    printf("  %-30s %4s  %10s  %s\n", "Query", "Iters", "Total ms", "Per query");

    /* benchmark queries */
    int rounds = 100;

    bench_query(g, "label scan (label)",
        "MATCH (l:label) RETURN l.text LIMIT 200;", rounds);

    bench_query(g, "label scan (section)",
        "MATCH (s:section) RETURN s.text LIMIT 200;", rounds);

    bench_query(g, "WHERE prop == string",
        "MATCH (s:section) WHERE s.text = '\\section{Introduction}' RETURN s.text;", rounds);

    bench_query(g, "edge expand (eq->label)",
        "MATCH (eq:equation)-[:PARENT_OF]->(l:label) RETURN l.text LIMIT 200;", rounds);

    bench_query(g, "WHERE numeric >",
        "MATCH (t:Token) WHERE t.offset > 10000 RETURN t.text LIMIT 200;", rounds);

    bench_query(g, "full scan + WHERE",
        "MATCH (t:Token) WHERE t.length > 100 RETURN t.text LIMIT 200;", rounds);

    bench_query(g, "ORDER BY + LIMIT",
        "MATCH (l:label) RETURN l.text ORDER BY l.text LIMIT 50;", rounds/2);

    printf("\nDone.\n");
    cypher_graph_free(g);
    cypher_fsm_cleanup();
    return 0;
}
