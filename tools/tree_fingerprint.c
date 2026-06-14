/*
 * tree_fingerprint.c — Structural fingerprint from scanner's parent-child tree.
 *
 * The scanner's EMIT_BLOCK already produces parse trees. We walk the
 * tree for a structural token (equation, display_math, etc.), collect
 * child type signatures, and abstract leaf values to produce a
 * structural hash.
 *
 * Example:
 *   display_2_math → children: [math_sub, braces, braces, math_sub]
 *   structural sig: "display_2_math:braces.2|math_sub.2"
 *
 * For equation equivalence, two equations with the same child-type
 * signature (same operators, same structure) get the same hash.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "json_scan.h"

#define MAX_TOKENS  200000
#define MAX_SIG     512

typedef struct {
    uint32_t token_id;
    uint32_t parent_id;
    uint32_t filepath_id;
    char     type[64];
    char     text[512];
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
        token_t *t = &tokens[n_tokens];
        memset(t, 0, sizeof(*t));
        for (int i = 0; i < cur.nf; i++) {
            if (!strcmp(cur.f[i].k, "token_id")) t->token_id = (uint32_t)strtoul(cur.f[i].v, NULL, 10);
            if (!strcmp(cur.f[i].k, "parent_id")) t->parent_id = (uint32_t)strtoul(cur.f[i].v, NULL, 10);
            if (!strcmp(cur.f[i].k, "filepath_id")) t->filepath_id = (uint32_t)strtoul(cur.f[i].v, NULL, 10);
            if (!strcmp(cur.f[i].k, "type")) snprintf(t->type, sizeof(t->type), "%s", cur.f[i].v);
            if (!strcmp(cur.f[i].k, "text")) snprintf(t->text, sizeof(t->text), "%s", cur.f[i].v);
        }
        n_tokens++;
        cur.nf = 0; return;
    }
    if (cur.nf < 16) {
        int nl = kl < 63 ? kl : 63; memcpy(cur.f[cur.nf].k, key, nl); cur.f[cur.nf].k[nl] = 0;
        int vl2 = vl < 511 ? vl : 511; memcpy(cur.f[cur.nf].v, val, vl2); cur.f[cur.nf].v[vl2] = 0;
        cur.nf++;
    }
}

/* find children of a given parent */
static int find_children(uint32_t parent_id, int *out, int max) {
    int n = 0;
    for (int i = 0; i < n_tokens && n < max; i++)
        if (tokens[i].parent_id == parent_id)
            out[n++] = i;
    return n;
}

/* build structural signature from parent token and its children */
static void tree_signature(int ti, char *sig, int max) {
    token_t *parent = &tokens[ti];
    int children[256];
    int nc = find_children(parent->token_id, children, 256);

    /* sort children by type for order-independent comparison */
    for (int i = 0; i < nc - 1; i++)
        for (int j = i + 1; j < nc; j++)
            if (strcmp(tokens[children[i]].type, tokens[children[j]].type) > 0) {
                int tmp = children[i]; children[i] = children[j]; children[j] = tmp;
            }

    int off = 0;
    off += snprintf(sig + off, max - off, "%s:", parent->type);

    /* count consecutive types */
    for (int i = 0; i < nc; ) {
        int j = i;
        while (j < nc && !strcmp(tokens[children[j]].type, tokens[children[i]].type)) j++;
        int cnt = j - i;
        if (i > 0) off += snprintf(sig + off, max - off, "|");
        off += snprintf(sig + off, max - off, "%s.%d", tokens[children[i]].type, cnt);
        i = j;
    }
    sig[off] = '\0';
}

int main(int argc, char *argv[]) {
    const char *type_filter = (argc >= 2) ? argv[1] : NULL;
    const char *sidecar = (argc >= 3) ? argv[2] : "../scanner/sidecar.json";

    fprintf(stderr, "Loading sidecar...\n");
    json_scan_file(sidecar, rec_cb, NULL);
    fprintf(stderr, "  %d tokens loaded\n", n_tokens);

    /* for each structural token, compute tree signature and output */
    uint32_t parent_types[65536] = {0};
    int n_parents = 0;

    for (int i = 0; i < n_tokens; i++) {
        if (type_filter && strcmp(tokens[i].type, type_filter)) continue;

        /* check if this token has children (is a structural parent) */
        int kids[256];
        int nc = find_children(tokens[i].token_id, kids, 256);
        if (nc == 0) continue;

        char sig[MAX_SIG];
        tree_signature(i, sig, sizeof(sig));

        printf("{\"token_id\":%u,\"type\":\"%s\",\"children\":%d,\"signature\":\"%s\"",
               tokens[i].token_id, tokens[i].type, nc, sig);

        /* sample a child for preview */
        printf(",\"child_types\":[");
        for (int j = 0; j < nc && j < 5; j++)
            printf("%s\"%s\"", j > 0 ? "," : "", tokens[kids[j]].type);
        printf("]");

        printf(",\"sample_text\":\"");
        for (char *s = tokens[i].text; *s && s < tokens[i].text + 60; s++) {
            if (*s == '\\') printf("\\\\"); else if (*s == '"') printf("\\\"");
            else if (*s == '\n') printf("\\n"); else putchar(*s);
        }
        printf("\"}\n");
    }

    return 0;
}
