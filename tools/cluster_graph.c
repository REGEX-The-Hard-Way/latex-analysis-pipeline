/*
 * cluster_graph.c — Extract token co-occurrence edges from sidecar.
 *
 * Two edge types:
 *   1. Same-file: tokens with identical filepath_id
 *   2. Parent-child: token → parent_id link
 *
 * Output: JSON edges
 * Usage: ./cluster_graph sidecar.json [type_filter]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "json_scan.h"

#define MAX_TOKENS 65536
#define MAX_FP_TOKENS 4096
#define MAX_FIELDS 16

typedef struct {
    uint32_t token_id, parent_id, filepath_id;
    char type[64], text[512];
} token_t;

typedef struct { char key[64], val[512]; int vtype; } field_t;
typedef struct { field_t f[MAX_FIELDS]; int nf; } rec_t;

static token_t tokens[MAX_TOKENS];
static int n_tokens = 0;
static rec_t cur;

/* per-filepath token buckets */
typedef struct { uint32_t fps[4096]; int n; } fp_bucket_t;
static fp_bucket_t fp_buckets[4096];
static int n_buckets = 0;

static fp_bucket_t *find_bucket(uint32_t fpid) {
    for (int i = 0; i < n_buckets; i++)
        if (fp_buckets[i].n > 0 && fp_buckets[i].fps[0] == fpid) return &fp_buckets[i];
    if (n_buckets < 4096) {
        fp_buckets[n_buckets].fps[0] = fpid;
        fp_buckets[n_buckets].n = 1;
        return &fp_buckets[n_buckets++];
    }
    return NULL;
}

static void add_to_bucket(uint32_t fpid, uint32_t tid) {
    fp_bucket_t *b = NULL;
    for (int i = 0; i < n_buckets; i++)
        if (fp_buckets[i].n > 0 && fp_buckets[i].fps[0] == fpid) { b = &fp_buckets[i]; break; }
    if (!b && n_buckets < 4096) { b = &fp_buckets[n_buckets++]; b->fps[0] = fpid; b->n = 0; }
    if (b && b->n < 4096) b->fps[b->n++] = tid;
}

static const char *g_filter = NULL;

static void rec_cb(const char *key, int klen, const char *val,
                    int vlen, int vtype, void *u) {
    (void)u;
    if (!key) {
        token_t *t = &tokens[n_tokens];
        memset(t, 0, sizeof(*t));
        for (int i = 0; i < cur.nf; i++) {
            if (!strcmp(cur.f[i].key, "token_id")) t->token_id = (uint32_t)atol(cur.f[i].val);
            if (!strcmp(cur.f[i].key, "parent_id")) t->parent_id = (uint32_t)atol(cur.f[i].val);
            if (!strcmp(cur.f[i].key, "filepath_id")) t->filepath_id = (uint32_t)atol(cur.f[i].val);
            if (!strcmp(cur.f[i].key, "type")) snprintf(t->type, sizeof(t->type), "%s", cur.f[i].val);
            if (!strcmp(cur.f[i].key, "text")) snprintf(t->text, sizeof(t->text), "%s", cur.f[i].val);
        }
        if (g_filter && strcmp(t->type, g_filter)) { cur.nf = 0; return; }
        add_to_bucket(t->filepath_id, n_tokens);
        n_tokens++;
        cur.nf = 0;
        return;
    }
    if (cur.nf < MAX_FIELDS) {
        int nl = klen < 63 ? klen : 63; memcpy(cur.f[cur.nf].key, key, nl); cur.f[cur.nf].key[nl] = 0;
        int vl = vlen < 511 ? vlen : 511; memcpy(cur.f[cur.nf].val, val, vl); cur.f[cur.nf].val[vl] = 0;
        cur.f[cur.nf].vtype = vtype; cur.nf++;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) { fprintf(stderr, "Usage: %s sidecar.json [type_filter]\n", argv[0]); return 1; }
    g_filter = (argc >= 3) ? argv[2] : NULL;

    json_scan_file(argv[1], rec_cb, NULL);

    /* emit same-file edges */
    for (int bi = 0; bi < n_buckets; bi++) {
        fp_bucket_t *b = &fp_buckets[bi];
        for (int i = 0; i < b->n; i++) {
            for (int j = i + 1; j < b->n; j++) {
                printf("{\"src\":%u,\"dst\":%u,\"weight\":1.0,\"edge_type\":\"same_file\"}\n",
                       tokens[b->fps[i]].token_id, tokens[b->fps[j]].token_id);
            }
        }
    }

    /* emit parent-child edges */
    for (int i = 0; i < n_tokens; i++) {
        if (tokens[i].parent_id != tokens[i].filepath_id) {
            for (int j = 0; j < n_tokens; j++) {
                if (tokens[j].token_id == tokens[i].parent_id) {
                    printf("{\"src\":%u,\"dst\":%u,\"weight\":0.5,\"edge_type\":\"parent_child\"}\n",
                           tokens[i].token_id, tokens[j].token_id);
                    break;
                }
            }
        }
    }

    fprintf(stderr, "Extracted: %d tokens in %d filepath buckets\n", n_tokens, n_buckets);
    return 0;
}
