/*
 * parse_tree.c — Recursive-descent S-expression parser.
 *
 * Converts Penn Treebank parse trees into a flat node array
 * with parent/child/sibling links. One allocation per tree.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parse_tree.h"

static struct { const char *name; int id; } label_map[] = {
    {"NP",L_NP},{"VP",L_VP},{"S",L_S},{"PP",L_PP},{"SBAR",L_SBAR},
    {"ADJP",L_ADJP},{"FRAG",L_FRAG},
    {"CD",L_CD},{"DT",L_DT},{"JJ",L_JJ},{"NN",L_NN},{"NNS",L_NNS},
    {"NNP",L_NNP},{"PRP",L_PRP},{"PRP$",L_PRP_DOLLAR},
    {"VBZ",L_VBZ},{"VBP",L_VBP},{"VBD",L_VBD},{"VBN",L_VBN},
    {"VBG",L_VBG},{"VB",L_VB},{"MD",L_MD},
    {"IN",L_IN},{"TO",L_TO},{"CC",L_CC},{"RB",L_RB},
    {"WRB",L_WRB},{"WDT",L_WDT},
    {":",L_COLON},{",",L_COMMA},{".",L_PERIOD},
    {"-LRB-",L_LRB},{"-RRB-",L_RRB},
    {"QP",L_QP},{"WHNP",L_WHNP},{"WHADVP",L_WHADVP},
    {"SYM",L_SYM},{"NML",L_NML},
};
#define N_LABELS (int)(sizeof(label_map)/sizeof(label_map[0]))

static int intern(const char *s, int len) {
    for (int i = 0; i < N_LABELS; i++)
        if ((int)strlen(label_map[i].name) == len &&
            !strncmp(label_map[i].name, s, len))
            return label_map[i].id;
    return L_UNKNOWN;
}

static int add_node(tree_t *t, int label, int parent) {
    if (t->n_nodes >= t->max_nodes) {
        t->max_nodes *= 2;
        t->nodes = realloc(t->nodes, (size_t)t->max_nodes * sizeof(node_t));
    }
    int me = t->n_nodes++;
    t->nodes[me] = (node_t){
        .label = (int16_t)label, .parent = (int16_t)parent,
        .first_child = -1, .next_sibling = -1,
        .text_off = 0, .text_len = 0,
    };
    return me;
}

static void add_leaf(tree_t *t, int label, int parent,
                     const char *word, int wlen) {
    int me = add_node(t, label, parent);
    t->nodes[me].text_off = (int16_t)t->text_len;
    t->nodes[me].text_len = (int16_t)wlen;
    if (t->text_len + wlen + 2 > t->max_text) {
        t->max_text *= 2;
        t->text = realloc(t->text, (size_t)t->max_text);
    }
    memcpy(t->text + t->text_len, word, (size_t)wlen);
    t->text_len += wlen;
    t->text[t->text_len++] = '\0';
}

static void link_child(tree_t *t, int parent, int child) {
    if (parent < 0) return;
    int prev = t->nodes[parent].first_child;
    if (prev < 0) {
        t->nodes[parent].first_child = (int16_t)child;
        return;
    }
    while (t->nodes[prev].next_sibling >= 0)
        prev = t->nodes[prev].next_sibling;
    t->nodes[prev].next_sibling = (int16_t)child;
}

static int parse_node(const char *s, int *i, int parent, tree_t *t) {
    while (s[*i] && (s[*i] == ' ' || s[*i] == '\n' || s[*i] == '\r')) (*i)++;
    if (!s[*i] || s[*i] != '(') return -1;
    (*i)++;
    while (s[*i] == ' ' || s[*i] == '\n') (*i)++;

    int ls = *i;
    while (s[*i] && s[*i] != ' ' && s[*i] != '\n' &&
           s[*i] != '(' && s[*i] != ')') (*i)++;
    int label = intern(s + ls, *i - ls);

    int me = add_node(t, label, parent);
    if (parent >= 0) link_child(t, parent, me);

    for (;;) {
        while (s[*i] && (s[*i] == ' ' || s[*i] == '\n' || s[*i] == '\r')) (*i)++;
        if (!s[*i] || s[*i] == ')') { if (s[*i] == ')') (*i)++; break; }
        if (s[*i] == '(') {
            int child = parse_node(s, i, me, t);
            if (child >= 0) link_child(t, me, child);
        } else {
            int ws = *i;
            while (s[*i] && s[*i] != ' ' && s[*i] != '\n' && s[*i] != ')') (*i)++;
            int wlen = *i - ws;
            int leaf = add_node(t, label, parent < 0 ? -1 : me);
            t->nodes[leaf].text_off = (int16_t)t->text_len;
            t->nodes[leaf].text_len = (int16_t)wlen;
            if (t->text_len + wlen + 2 > t->max_text) {
                t->max_text *= 2;
                t->text = realloc(t->text, (size_t)t->max_text);
            }
            memcpy(t->text + t->text_len, s + ws, (size_t)wlen);
            t->text_len += wlen;
            t->text[t->text_len++] = '\0';
            link_child(t, parent < 0 ? -1 : me, leaf);
        }
    }
    return me;
}

tree_t *parse_tree(const char *s) {
    tree_t *t = calloc(1, sizeof(*t));
    if (!t) return NULL;
    t->max_nodes = 2048;
    t->nodes = malloc((size_t)t->max_nodes * sizeof(node_t));
    t->max_text = 8192;
    t->text = malloc((size_t)t->max_text);
    if (!t->nodes || !t->text) { free_tree(t); return NULL; }
    int i = 0;
    parse_node(s, &i, -1, t);
    return t;
}

void free_tree(tree_t *t) {
    if (!t) return;
    free(t->nodes);
    free(t->text);
    free(t);
}
