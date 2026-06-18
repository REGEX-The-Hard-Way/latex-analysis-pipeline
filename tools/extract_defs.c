/*
 * extract_defs.c — Extract math variable definitions from parse trees.
 *
 * Usage: extract_defs file.parsed.txt [sidecar.tok]
 *
 * Reads constituency parse trees (one ROOT per sentence), finds CD
 * nodes containing numeric token IDs, matches syntactic patterns to
 * extract the defining noun phrase, and outputs TSV:
 *   token_id \t math_text \t definition
 *
 * If sidecar.tok is provided, token IDs are mapped to their original
 * math text (e.g. "$\alpha$") for the second column.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "parse_tree.h"

/* from match_patterns.c */
int  match_definition(tree_t *t, int cd_idx, char *buf, int bufsz);
bool n_is_cd_id(tree_t *t, int idx);

/* ---- sidecar.tok parser ---- */
typedef struct {
    char *token_id;
    char *token_text;
} idmap_entry_t;

typedef struct {
    idmap_entry_t *entries;
    int n;
} idmap_t;

static idmap_t *load_sidecar(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) return NULL;
    idmap_t *m = calloc(1, sizeof(*m));
    m->entries = malloc(4096 * sizeof(idmap_entry_t));
    char line[4096];
    while (fgets(line, sizeof(line), fp) && m->n < 4096) {
        char *tid = strstr(line, "token_id:");
        char *tok = strstr(line, "<tok:");
        if (!tid || !tok) continue;
        tid += 9;  /* skip "token_id:" */
        char *tc = strchr(tid, ',');
        if (!tc) continue;
        *tc = '\0';
        tok += 5;  /* skip "<tok:" */
        char *te = strstr(tok, ">}");
        if (!te) { te = strchr(tok, '>'); }
        if (!te) continue;
        *te = '\0';
        m->entries[m->n].token_id = strdup(tid);
        m->entries[m->n].token_text = strdup(tok);
        m->n++;
    }
    fclose(fp);
    return m;
}

static char *idmap_lookup(idmap_t *m, const char *tid) {
    if (!m) return NULL;
    for (int i = 0; i < m->n; i++)
        if (!strcmp(m->entries[i].token_id, tid))
            return m->entries[i].token_text;
    return NULL;
}

static void free_idmap(idmap_t *m) {
    if (!m) return;
    for (int i = 0; i < m->n; i++) {
        free(m->entries[i].token_id);
        free(m->entries[i].token_text);
    }
    free(m->entries);
    free(m);
}

/* ---- read file ---- */
static char *read_file(const char *path, size_t *out_len) {
    FILE *fp = fopen(path, "r");
    if (!fp) return NULL;
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *buf = malloc((size_t)sz + 1);
    size_t n = fread(buf, 1, (size_t)sz, fp);
    buf[n] = '\0';
    fclose(fp);
    if (out_len) *out_len = n;
    return buf;
}

/* ---- main ---- */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: extract_defs file.parsed.txt [sidecar.tok]\n");
        return 1;
    }

    idmap_t *idmap = NULL;
    if (argc >= 3)
        idmap = load_sidecar(argv[2]);

    size_t flen;
    char *text = read_file(argv[1], &flen);
    if (!text) { fprintf(stderr, "Cannot read %s\n", argv[1]); return 1; }

    /* Split input into individual trees (separated by ROOT) */
    char *p = text;
    int tree_count = 0;
    while (*p) {
        /* find next (ROOT */
        char *root = strstr(p, "(ROOT");
        if (!root) break;
        /* find matching close paren */
        int depth = 0;
        char *end = root;
        while (*end) {
            if (*end == '(') depth++;
            else if (*end == ')') { depth--; if (depth == 0) { end++; break; } }
            end++;
        }

        tree_t *t = parse_tree(root);
        if (t && t->n_nodes > 0) {
            tree_count++;
            for (int i = 0; i < t->n_nodes; i++) {
                if (!n_is_cd_id(t, i)) continue;
                char defn[512];
                int dlen = match_definition(t, i, defn, sizeof(defn));
                char *tid = n_text(t, i);
                char *math = idmap_lookup(idmap, tid);
                if (!math) math = tid;
                printf("%s\t%s\t%s\n", tid, math,
                       dlen >= 3 ? defn : "");
            }
        }
        free_tree(t);
        p = end;
    }

    free(text);
    free_idmap(idmap);
    return 0;
}
