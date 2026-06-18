/*
 * parse_tree.h — S-expression parse tree representation.
 *
 * Stores a constituency parse tree as a flat array of nodes
 * with parent/child/sibling links. One allocation per tree.
 */
#ifndef PARSE_TREE_H
#define PARSE_TREE_H
#include <stdint.h>
#include <stdbool.h>

enum {
    L_NP=1, L_VP, L_S, L_PP, L_SBAR, L_ADJP, L_FRAG,
    L_CD, L_DT, L_JJ, L_NN, L_NNS, L_NNP, L_PRP, L_PRP_DOLLAR,
    L_VBZ, L_VBP, L_VBD, L_VBN, L_VBG, L_VB, L_MD,
    L_IN, L_TO, L_CC, L_RB, L_WRB, L_WDT,
    L_COLON, L_COMMA, L_PERIOD, L_LRB, L_RRB,
    L_QP, L_WHNP, L_WHADVP, L_SYM, L_NML,
    L_UNKNOWN
};

typedef struct {
    int16_t label;
    int16_t parent;
    int16_t first_child;
    int16_t next_sibling;
    int16_t text_off;
    int16_t text_len;
} node_t;

typedef struct {
    node_t *nodes;
    int     n_nodes;
    int     max_nodes;
    char   *text;
    int     text_len;
    int     max_text;
} tree_t;

tree_t *parse_tree(const char *s);
void    free_tree(tree_t *t);

/* tree walkers */
static inline node_t *n_child(tree_t *t, int idx) {
    if (idx < 0 || idx >= t->n_nodes) return NULL;
    int c = t->nodes[idx].first_child;
    return c >= 0 ? &t->nodes[c] : NULL;
}
static inline node_t *n_next(tree_t *t, int idx) {
    if (idx < 0 || idx >= t->n_nodes) return NULL;
    int s = t->nodes[idx].next_sibling;
    return s >= 0 ? &t->nodes[s] : NULL;
}
static inline node_t *n_parent(tree_t *t, int idx) {
    if (idx < 0 || idx >= t->n_nodes) return NULL;
    int p = t->nodes[idx].parent;
    return p >= 0 ? &t->nodes[p] : NULL;
}
static inline char *n_text(tree_t *t, int idx) {
    if (idx < 0 || idx >= t->n_nodes) return NULL;
    return t->text + t->nodes[idx].text_off;
}

#endif
