/*
 * spectral_cluster.c — Pure-C spectral clustering with structural normalization.
 *
 * Edge types (weighted):
 *   1. Co-occurrence (same filepath_id) — weight 1.0
 *   2. Parent-child — weight 0.5
 *   3. Text similarity (fingerprint match) — weight 0.3
 *   4. Structural similarity (math normalization) — weight 0.7
 *
 * Usage: ./spectral_cluster sidecar.json --type author --k 5 [--text-edges]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "json_scan.h"

/* from struct_norm.c */
void fingerprint(const char *in, char *out, int max);
int  struct_normalize_math(const char *in, char *out, int max);
void normalize_author(const char *in, char *out, int max);

#define MAX_NODES  5000
#define MAX_EDGES  800000
#define MAX_K      20
#define MAX_FIELDS 16
#define MAX_ITERS  100
#define EPS        1e-6

typedef struct { int i, j; double w; } edge_t;

static edge_t edges[MAX_EDGES];
static int n_edges = 0, n_nodes = 0;
static int *token_ids, *filepath_ids;
static char (*node_texts)[512];  /* text per node */
static const char *g_type_filter = NULL;
static int g_max_nodes = MAX_NODES;
static int g_text_edges = 0;

/* ---- json_scan callback ---- */
typedef struct { char k[64], v[512]; } fld_t;
typedef struct { fld_t f[MAX_FIELDS]; int nf; } rec_t;
static rec_t cur;

static void rec_cb(const char *key, int kl, const char *val, int vl, int vt, void *u) {
    (void)vt; (void)u;
    if (!key) {
        if (n_nodes >= g_max_nodes) { cur.nf = 0; return; }
        int is_match = !g_type_filter;
        unsigned long tid = 0, fid = 0;
        char text[512] = "";
        for (int i = 0; i < cur.nf; i++) {
            if (g_type_filter && !strcmp(cur.f[i].k, "type")
                && !strcmp(cur.f[i].v, g_type_filter)) is_match = 1;
            if (!strcmp(cur.f[i].k, "token_id")) tid = strtoul(cur.f[i].v, NULL, 10);
            if (!strcmp(cur.f[i].k, "filepath_id")) fid = strtoul(cur.f[i].v, NULL, 10);
            if (!strcmp(cur.f[i].k, "text")) snprintf(text, sizeof(text), "%s", cur.f[i].v);
        }
        if (is_match && tid) {
            token_ids[n_nodes] = (int)tid;
            filepath_ids[n_nodes] = (int)fid;
            snprintf(node_texts[n_nodes], 512, "%s", text);
            n_nodes++;
        }
        cur.nf = 0; return;
    }
    if (cur.nf < MAX_FIELDS) {
        int nl = kl < 63 ? kl : 63; memcpy(cur.f[cur.nf].k, key, nl); cur.f[cur.nf].k[nl] = 0;
        int vl2 = vl < 511 ? vl : 511; memcpy(cur.f[cur.nf].v, val, vl2); cur.f[cur.nf].v[vl2] = 0;
        cur.nf++;
    }
}

/* ---- graph construction ---- */
static void add_edge(int i, int j, double w) {
    if (n_edges >= MAX_EDGES) return;
    edges[n_edges].i = i; edges[n_edges].j = j; edges[n_edges].w = w;
    n_edges++;
}

static void build_graph(void) {
    /* co-occurrence edges (same filepath_id) */
    for (int i = 0; i < n_nodes; i++) {
        for (int j = i + 1; j < n_nodes; j++) {
            if (filepath_ids[i] == filepath_ids[j])
                add_edge(i, j, 1.0);
        }
    }

    /* text similarity edges */
    if (g_text_edges) {
        char fps[MAX_NODES][256];
        for (int i = 0; i < n_nodes; i++) {
            if (g_type_filter && !strcmp(g_type_filter, "author"))
                normalize_author(node_texts[i], fps[i], 256);
            else if (g_type_filter && !strcmp(g_type_filter, "math"))
                struct_normalize_math(node_texts[i], fps[i], 256);
            else
                fingerprint(node_texts[i], fps[i], 256);
        }

        double edge_w = (!strcmp(g_type_filter ? g_type_filter : "", "math")) ? 0.7 : 0.3;
        for (int i = 0; i < n_nodes; i++) {
            for (int j = i + 1; j < n_nodes; j++) {
                if (fps[i][0] && fps[j][0] && !strcmp(fps[i], fps[j]))
                    add_edge(i, j, edge_w);
            }
        }
    }
}

/* ---- power iteration ---- */
static void power_iteration(double *data, int *cols, int *row_ptr, int n,
                             double *vec, int k, double *vals) {
    double *b = calloc((size_t)n, sizeof(double));
    double *Ab = calloc((size_t)n, sizeof(double));

    for (int comp = 0; comp < k; comp++) {
        for (int i = 0; i < n; i++) b[i] = (double)rand() / RAND_MAX;
        double norm = 0;
        for (int i = 0; i < n; i++) norm += b[i] * b[i];
        norm = sqrt(norm);
        for (int i = 0; i < n; i++) b[i] /= norm;

        double lambda = 0;
        for (int iter = 0; iter < MAX_ITERS; iter++) {
            for (int i = 0; i < n; i++) {
                Ab[i] = 0;
                for (int jj = row_ptr[i]; jj < row_ptr[i+1]; jj++)
                    Ab[i] += data[jj] * b[cols[jj]];
            }
            for (int p = 0; p < comp; p++) {
                double dot = 0;
                for (int i = 0; i < n; i++) dot += Ab[i] * vec[p * n + i];
                for (int i = 0; i < n; i++) Ab[i] -= dot * vec[p * n + i];
            }
            norm = 0;
            for (int i = 0; i < n; i++) norm += Ab[i] * Ab[i];
            norm = sqrt(norm); if (norm < EPS) break;
            double new_lambda = 0;
            for (int i = 0; i < n; i++) {
                Ab[i] /= norm;
                new_lambda += Ab[i] * b[i];
                b[i] = Ab[i];
            }
            if (fabs(new_lambda - lambda) < EPS) break;
            lambda = new_lambda;
        }
        vals[comp] = lambda;
        for (int i = 0; i < n; i++) vec[comp * n + i] = b[i];
    }
    free(b); free(Ab);
}

/* ---- k-means ---- */
static void kmeans(double *emb, int n, int d, int k, int *labels) {
    double *centroids = calloc((size_t)(k * d), sizeof(double));
    for (int c = 0; c < k; c++) {
        int ri = rand() % n;
        for (int j = 0; j < d; j++) centroids[c * d + j] = emb[ri * d + j];
    }
    for (int iter = 0; iter < 50; iter++) {
        int changed = 0;
        for (int i = 0; i < n; i++) {
            double best = 1e100; int bc = 0;
            for (int c = 0; c < k; c++) {
                double dist = 0;
                for (int j = 0; j < d; j++) {
                    double diff = emb[i * d + j] - centroids[c * d + j];
                    dist += diff * diff;
                }
                if (dist < best) { best = dist; bc = c; }
            }
            if (labels[i] != bc) { labels[i] = bc; changed = 1; }
        }
        if (!changed) break;
        int *cnts = calloc((size_t)k, sizeof(int));
        memset(centroids, 0, (size_t)(k * d) * sizeof(double));
        for (int i = 0; i < n; i++) {
            int c = labels[i]; cnts[c]++;
            for (int j = 0; j < d; j++) centroids[c * d + j] += emb[i * d + j];
        }
        for (int c = 0; c < k; c++)
            if (cnts[c] > 0)
                for (int j = 0; j < d; j++) centroids[c * d + j] /= cnts[c];
        free(cnts);
    }
    free(centroids);
}

int main(int argc, char *argv[]) {
    const char *sidecar = NULL;
    int k = 5; g_type_filter = NULL; g_text_edges = 0;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--type") && i+1 < argc) g_type_filter = argv[++i];
        else if (!strcmp(argv[i], "--k") && i+1 < argc) k = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--max-nodes") && i+1 < argc) g_max_nodes = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--text-edges")) g_text_edges = 1;
        else sidecar = argv[i];
    }
    if (!sidecar) { fprintf(stderr, "Usage: %s sidecar.json [--type X] [--k N] [--text-edges]\n", argv[0]); return 1; }

    token_ids = calloc((size_t)g_max_nodes, sizeof(int));
    filepath_ids = calloc((size_t)g_max_nodes, sizeof(int));
    node_texts = calloc((size_t)g_max_nodes, 512);

    fprintf(stderr, "Loading (type=%s, max=%d, text_edges=%d)...\n",
            g_type_filter ? g_type_filter : "*", g_max_nodes, g_text_edges);
    json_scan_file(sidecar, rec_cb, NULL);
    fprintf(stderr, "  %d tokens\n", n_nodes);
    if (n_nodes < 2) return 1;

    build_graph();
    fprintf(stderr, "  %d edges\n", n_edges);
    if (n_edges == 0) return 1;

    /* CSR construction + normalization */
    int *row_ptr = calloc((size_t)(n_nodes + 1), sizeof(int));
    for (int ei = 0; ei < n_edges; ei++) { row_ptr[edges[ei].i+1]++; row_ptr[edges[ei].j+1]++; }
    for (int i = 0; i < n_nodes; i++) row_ptr[i+1] += row_ptr[i];
    double *data = calloc((size_t)row_ptr[n_nodes], sizeof(double));
    int *cols = calloc((size_t)row_ptr[n_nodes], sizeof(int));
    int *pos = calloc((size_t)n_nodes, sizeof(int));
    memcpy(pos, row_ptr, (size_t)n_nodes * sizeof(int));
    for (int ei = 0; ei < n_edges; ei++) {
        int i = edges[ei].i, j = edges[ei].j;
        data[pos[i]] = edges[ei].w; cols[pos[i]] = j; pos[i]++;
        data[pos[j]] = edges[ei].w; cols[pos[j]] = i; pos[j]++;
    }
    free(pos);
    double *deg = calloc((size_t)n_nodes, sizeof(double));
    for (int i = 0; i < n_nodes; i++)
        for (int jj = row_ptr[i]; jj < row_ptr[i+1]; jj++) deg[i] += data[jj];
    for (int i = 0; i < n_nodes; i++) {
        double is = deg[i] > 0 ? 1.0 / sqrt(deg[i]) : 0;
        for (int jj = row_ptr[i]; jj < row_ptr[i+1]; jj++) {
            double js = deg[cols[jj]] > 0 ? 1.0 / sqrt(deg[cols[jj]]) : 0;
            data[jj] *= is * js;
        }
    }

    if (k > n_nodes) k = n_nodes;
    double *vecs = calloc((size_t)(k * n_nodes), sizeof(double));
    double *vals = calloc((size_t)k, sizeof(double));
    srand(42);
    power_iteration(data, cols, row_ptr, n_nodes, vecs, k, vals);

    int *labels = calloc((size_t)n_nodes, sizeof(int));
    kmeans(vecs, n_nodes, k, k, labels);

    /* output clusters with sample texts */
    printf("[\n");
    for (int c = 0; c < k; c++) {
        int cnt = 0, sample_i = -1;
        for (int i = 0; i < n_nodes; i++) if (labels[i] == c) { cnt++; if (sample_i < 0) sample_i = i; }
        if (cnt == 0) continue;
        printf("  {\"cluster\":%d,\"count\":%d,\"algorithm\":\"spectral\",\"sample\":\"", c, cnt);
        if (sample_i >= 0) {
            for (char *s = node_texts[sample_i]; *s && s < node_texts[sample_i] + 80; s++) {
                if (*s == '\\') printf("\\\\"); else if (*s == '"') printf("\\\""); else putchar(*s);
            }
        }
        printf("\"}");
        if (c < k - 1) printf(",");
        printf("\n");
    }
    printf("]\n");

    fprintf(stderr, "Done: %d nodes in %d clusters\n", n_nodes, k);
    return 0;
}
