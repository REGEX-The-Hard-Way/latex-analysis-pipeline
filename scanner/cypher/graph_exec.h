/*
 * graph_exec.h — Ragel FSM query execution engine (Phases 3+4).
 *
 * Two modes:
 *   1. Pre-compiled FSM (always available): use_fsm=0, callback-driven.
 *      Ragel state machine compiled into binary at build time.
 *   2. JIT-compiled FSM (requires ragel+gcc at runtime): use_fsm=1.
 *      Each query compiles to a machine-specific .so, then dlopen'd.
 *
 * The FSM eliminates interpreter overhead: goto-based dispatch,
 * no virtual calls, no AST traversal on the hot path.
 */
#ifndef GRAPH_EXEC_H
#define GRAPH_EXEC_H

#include "cypher_parser.h"
#include "graph_store.h"

/* result from a compiled FSM execution */
typedef struct {
    graph_store_t  *gs;
    cypher_result_t *result;
    int              use_jit;
    int              ok;
} cypher_fsm_ctx_t;

int cypher_fsm_init(void);
cypher_result_t *cypher_fsm_exec(cypher_graph_t *g, cypher_ast_t *match_cl,
                                  cypher_ast_t *return_cl);
void cypher_fsm_cleanup(void);

#endif /* GRAPH_EXEC_H */
