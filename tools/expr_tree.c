/*
 * expr_tree.c — Build expression trees from scanner token streams.
 *
 * Reads sidecar tokens, finds display_math/equation parents with children,
 * groups sibling tokens into binary expression trees using operator
 * precedence (Pratt-style).
 *
 * Output: S-expression trees in JSON.
 *
 * Usage: ./expr_tree sidecar.json [--type display_2_math] [--sample]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "json_scan.h"

#define MAX_TOKENS 200000
#define MAX_CHILDREN 256

typedef struct {
    uint32_t token_id, parent_id, filepath_id;
    char type[64], text[512];
} token_t;

static token_t tokens[MAX_TOKENS];
static int n_tokens = 0;

/* json_scan accumulator */
typedef struct { char k[64], v[512]; } fld_t;
typedef struct { fld_t f[16]; int nf; } rec_t;
static rec_t cur;

static void rec_cb(const char *key, int kl, const char *val, int vl, int vt, void *u) {
    (void)vt; (void)u;
    if (!key) {
        if (n_tokens >= MAX_TOKENS) { cur.nf = 0; return; }
        token_t *t = &tokens[n_tokens]; memset(t, 0, sizeof(*t));
        for (int i = 0; i < cur.nf; i++) {
            if (!strcmp(cur.f[i].k, "token_id")) t->token_id = (uint32_t)strtoul(cur.f[i].v, NULL, 10);
            if (!strcmp(cur.f[i].k, "parent_id")) t->parent_id = (uint32_t)strtoul(cur.f[i].v, NULL, 10);
            if (!strcmp(cur.f[i].k, "filepath_id")) t->filepath_id = (uint32_t)strtoul(cur.f[i].v, NULL, 10);
            if (!strcmp(cur.f[i].k, "type")) snprintf(t->type, sizeof(t->type), "%s", cur.f[i].v);
            if (!strcmp(cur.f[i].k, "text")) snprintf(t->text, sizeof(t->text), "%s", cur.f[i].v);
        }
        n_tokens++; cur.nf = 0; return;
    }
    if (cur.nf < 16) {
        int nl = kl < 63 ? kl : 63; memcpy(cur.f[cur.nf].k, key, nl); cur.f[cur.nf].k[nl] = 0;
        int vl2 = vl < 511 ? vl : 511; memcpy(cur.f[cur.nf].v, val, vl2); cur.f[cur.nf].v[vl2] = 0;
        cur.nf++;
    }
}

/* find children of a parent token */
static int find_children(uint32_t parent_id, int *out, int max) {
    int n = 0;
    for (int i = 0; i < n_tokens && n < max; i++)
        if (tokens[i].parent_id == parent_id)
            out[n++] = i;
    return n;
}

/* operator precedence: higher = tighter binding */
static int op_precedence(const char *type, const char *text) {
    if (!strcmp(type, "math_rel")) return 1;
    if (!strcmp(type, "math_op") && (strchr(text, '+') || strchr(text, '-'))) return 2;
    if (!strcmp(type, "math_op") && (strchr(text, '*') || strstr(text, "times")
         || strstr(text, "cdot"))) return 3;
    if (!strcmp(type, "math_sup")) return 4;
    if (!strcmp(type, "math_fn") || !strcmp(type, "frac")) return 5;
    return 6; /* atoms */
}

static int is_binary_op(const char *type) {
    return !strcmp(type, "math_op") || !strcmp(type, "math_rel");
}

/* ---- Pratt parser for expression tree ---- */
typedef struct expr_node {
    int   token_idx;       /* -1 for internal nodes */
    char  op_type[64];     /* operator type or "atom" */
    char  op_text[128];
    struct expr_node *left, *right;
    int   children_start, children_end; /* for parens/frac children range */
} expr_node_t;

static expr_node_t nodes[4096];
static int n_nodes = 0;

static expr_node_t *new_node(int ti) {
    if (n_nodes >= 4096) return NULL;
    expr_node_t *n = &nodes[n_nodes++];
    memset(n, 0, sizeof(*n));
    n->token_idx = ti;
    if (ti >= 0) {
        snprintf(n->op_type, sizeof(n->op_type), "%s", tokens[ti].type);
        snprintf(n->op_text, sizeof(n->op_text), "%s", tokens[ti].text);
    }
    return n;
}

/* Pratt: parse expression at given precedence level */
static expr_node_t *parse_pratt(int *children, int n_child, int *pos, int min_prec) {
    if (*pos >= n_child) return NULL;

    /* prefix: atom or unary */
    int ti = children[*pos];
    token_t *t = &tokens[ti];
    expr_node_t *left;

    if (!strcmp(t->type, "parens")) {
        /* parenthesized sub-expression — flatten */
        int sub_children[64];
        int ns = find_children(t->token_id, sub_children, 64);
        if (ns > 0) {
            int sp = 0;
            left = parse_pratt(sub_children, ns, &sp, 0);
            (*pos)++;
            return left;
        }
        (*pos)++;
        return new_node(ti);
    }

    if (!strcmp(t->type, "frac")) {
        left = new_node(ti);
        left->children_start = -1;
        (*pos)++;
        return left;
    }

    /* atom */
    left = new_node(ti);
    (*pos)++;

    /* infix: binary operators */
    while (*pos < n_child) {
        int op_ti = children[*pos];
        token_t *op_t = &tokens[op_ti];
        if (!is_binary_op(op_t->type)) break;

        int prec = op_precedence(op_t->type, op_t->text);
        if (prec < min_prec) break;

        (*pos)++; /* consume operator */

        /* right operand */
        expr_node_t *right = parse_pratt(children, n_child, pos, prec + 1);
        if (!right) break;

        /* build binary node */
        expr_node_t *op_node = new_node(-1);
        op_node->left = left;
        op_node->right = right;
        snprintf(op_node->op_type, sizeof(op_node->op_type), "binary");
        snprintf(op_node->op_text, sizeof(op_node->op_text), "%s", op_t->text);
        op_node->token_idx = -1;
        left = op_node;
    }

    return left;
}

static void print_tree_json(expr_node_t *n) {
    if (!n) return;
    if (n->left || n->right) {
        printf("{\"op\":\"%s\",\"left\":", n->op_text);
        print_tree_json(n->left);
        printf(",\"right\":");
        print_tree_json(n->right);
        printf("}");
    } else {
        printf("{\"leaf\":\"%s\",\"text\":\"", n->op_type);
        for (char *s = n->op_text; *s; s++) {
            if (*s == '\\') printf("\\\\"); else if (*s == '"') printf("\\\"");
            else if (*s == '\n') printf("\\n"); else putchar(*s);
        }
        printf("\"}");
    }
}

int main(int argc, char *argv[]) {
    const char *sidecar = (argc >= 2 && argv[1][0] != '-') ? argv[1] : "../scanner/sidecar.json";
    const char *type_filter = NULL;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--type") && i+1 < argc) type_filter = argv[++i];
    }

    fprintf(stderr, "Loading sidecar...\n");
    json_scan_file(sidecar, rec_cb, NULL);
    fprintf(stderr, "  %d tokens\n", n_tokens);

    int shown = 0;
    for (int i = 0; i < n_tokens && shown < 10; i++) {
        if (type_filter && strcmp(tokens[i].type, type_filter)) continue;
        int children[256];
        int nc = find_children(tokens[i].token_id, children, 256);
        if (nc < 2) continue; /* need at least 2 children for an expression */

        /* filter: only process children that are math tokens */
        int math_kids[256], nm = 0;
        for (int j = 0; j < nc; j++) {
            const char *ct = tokens[children[j]].type;
            if (!strncmp(ct, "math_", 5) || !strcmp(ct, "parens")
                || !strcmp(ct, "frac") || !strcmp(ct, "braces"))
                math_kids[nm++] = children[j];
        }
        if (nm < 2) continue;

        int pos = 0;
        n_nodes = 0;
        expr_node_t *root = parse_pratt(math_kids, nm, &pos, 0);
        if (root) {
            printf("{\"parent_type\":\"%s\",\"token_id\":%u,\"tree\":",
                   tokens[i].type, tokens[i].token_id);
            print_tree_json(root);
            printf("}\n");
            shown++;
        }
    }

    fprintf(stderr, "Done: %d expression trees generated\n", shown);
    return 0;
}
