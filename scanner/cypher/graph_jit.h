/*
 * graph_jit.h — Phase 3 JIT query compiler.
 *
 * AST → C source → gcc -O2 -shared -fPIC → dlopen.
 * Inlines label names, property keys, comparison values as C literals.
 * Eliminates AST traversal, string comparison, and virtual dispatch.
 */
#ifndef GRAPH_JIT_H
#define GRAPH_JIT_H

#include "cypher_parser.h"
#include "graph_store.h"

typedef void (*jit_query_func)(void *gs, void *result);

int cypher_jit_init(void);
jit_query_func cypher_jit_compile(cypher_graph_t *g, cypher_ast_t *match_cl,
                                   cypher_ast_t *return_cl);
cypher_result_t *cypher_jit_exec(cypher_graph_t *g, cypher_ast_t *match_cl,
                                  cypher_ast_t *return_cl);
void cypher_jit_free(jit_query_func f);
void cypher_jit_cleanup(void);

extern int g_jit_enabled;

#endif
