#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse_tree.h"

/* Use a global tree pointer during parsing (avoids parameter passing issues) */
static tree_t *g_tree;

static struct { const char *name; int id; } label_map[] = {
    {"NP",L_NP},{"VP",L_VP},{"S",L_S},{"PP",L_PP},{"SBAR",L_SBAR},
    {"ADJP",L_ADJP},{"FRAG",L_FRAG},{"CD",L_CD},{"DT",L_DT},{"JJ",L_JJ},
    {"NN",L_NN},{"NNS",L_NNS},{"NNP",L_NNP},{"PRP",L_PRP},{"PRP$",L_PRP_DOLLAR},
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

static int add_node(int label, int parent) {
    tree_t *t = g_tree;
    if (t->n_nodes >= t->max_nodes) {
        t->max_nodes += 2048;
        t->nodes = realloc(t->nodes, (size_t)t->max_nodes * sizeof(node_t));
    }
    int me = t->n_nodes++;
    t->nodes[me].label = (int16_t)label;
    t->nodes[me].parent = (int16_t)parent;
    t->nodes[me].first_child = -1;
    t->nodes[me].next_sibling = -1;
    if (parent >= 0) {
        if (t->nodes[parent].first_child < 0) t->nodes[parent].first_child = (int16_t)me;
        else { int s = t->nodes[parent].first_child;
               while (t->nodes[s].next_sibling >= 0) s = t->nodes[s].next_sibling;
               t->nodes[s].next_sibling = (int16_t)me; }
    }
    return me;
}

static int add_leaf(int parent, const char *word, int wlen, int plabel) {
    tree_t *t = g_tree;
    if (t->n_nodes >= t->max_nodes) {
        t->max_nodes += 2048;
        t->nodes = realloc(t->nodes, (size_t)t->max_nodes * sizeof(node_t));
    }
    int leaf = t->n_nodes++;
    t->nodes[leaf].label = (int16_t)plabel;
    t->nodes[leaf].parent = (int16_t)parent;
    t->nodes[leaf].first_child = -1;
    t->nodes[leaf].next_sibling = -1;
    t->nodes[leaf].text_off = (int16_t)t->text_len;
    t->nodes[leaf].text_len = (int16_t)wlen;
    if (!t->text) { t->max_text = 4096; t->text = malloc(4096); t->text_len = 0; }
    if (t->text_len + wlen + 2 > t->max_text) {
        t->max_text += 4096;
        t->text = realloc(t->text, (size_t)t->max_text);
    }
    memcpy(t->text + t->text_len, word, (size_t)wlen);
    t->text_len += wlen;
    t->text[t->text_len++] = '\0';
    if (parent >= 0) {
        if (t->nodes[parent].first_child < 0) t->nodes[parent].first_child = (int16_t)leaf;
        else { int s = t->nodes[parent].first_child;
               while (t->nodes[s].next_sibling >= 0) s = t->nodes[s].next_sibling;
               t->nodes[s].next_sibling = (int16_t)leaf; }
    }
    return leaf;
}

static int parse_node(const char *s, int *i, int parent) {
    while (s[*i] == ' ' || s[*i] == '\n' || s[*i] == '\r') (*i)++;
    if (s[*i] != '(') return -1;
    (*i)++;
    int ls = *i;
    while (s[*i] && s[*i] != ' ' && s[*i] != '\n' && s[*i] != '(' && s[*i] != ')') (*i)++;
    int label = intern(s + ls, *i - ls);
    int me = add_node(label, parent);
    while (1) {
        while (s[*i] == ' ' || s[*i] == '\n' || s[*i] == '\r') (*i)++;
        if (s[*i] == '\0') break;
        if (s[*i] == ')') { (*i)++; break; }
        if (s[*i] == '(') { parse_node(s, i, me); }
        else {
            int ws = *i;
            while (s[*i] && s[*i] != ' ' && s[*i] != '\n' && s[*i] != ')') (*i)++;
            add_leaf(me, s + ws, *i - ws, label);
        }
    }
    return me;
}

tree_t *parse_tree(const char *s) {
    tree_t *t = calloc(1, sizeof(*t));
    if (!t) return NULL;
    g_tree = t;
    t->max_nodes = 2048;
    t->nodes = malloc((size_t)t->max_nodes * sizeof(node_t));
    t->text = NULL; t->text_len = 0; t->max_text = 0;
    if (!t->nodes) { free_tree(t); return NULL; }
    int i = 0;
    parse_node(s, &i, -1);
    return t;
}

void free_tree(tree_t *t) {
    if (!t) return;
    free(t->nodes);
    free(t->text);
    free(t);
}
