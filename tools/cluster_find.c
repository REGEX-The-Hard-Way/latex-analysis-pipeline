/*
 * cluster_find.c — OpenRefine-style clustering for sidecar data.
 *
 * Uses json_scan Ragel scanner for proper field extraction.
 * Two algorithms: fingerprint (normalize→hash→group) and Levenshtein.
 *
 * Usage: ./cluster_find sidecar.json [type_filter] [--levenshtein N]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "json_scan.h"

#define MAX_RECORDS 65536
#define MAX_CLUSTERS 65536
#define MAX_MEMBERS 256
#define MAX_TEXT    4096

/* ---- fingerprint ---- */
static void normalize(const char *in, char *out, int max) {
    int o = 0, space = 0;
    while (*in && o < max - 1) {
        if (isspace((unsigned char)*in)) {
            if (!space && o > 0) { out[o++] = ' '; space = 1; }
        } else if (isalnum((unsigned char)*in) || *in == '-' || *in == '_') {
            out[o++] = (char)tolower((unsigned char)*in); space = 0;
        }
        in++;
    }
    while (o > 0 && out[o-1] == ' ') o--;
    out[o] = '\0';
}

static uint32_t hash_str(const char *s) {
    uint32_t h = 0;
    while (*s) h = h * 31 + (unsigned char)*s++;
    return h;
}

/* ---- Levenshtein ---- */
static int levenshtein(const char *a, int la, const char *b, int lb) {
    if (la == 0) return lb; if (lb == 0) return la;
    int d[2][MAX_TEXT];
    for (int j = 0; j <= lb; j++) d[0][j] = j;
    for (int i = 1; i <= la; i++) {
        int cur = i & 1, prev = !cur;
        d[cur][0] = i;
        for (int j = 1; j <= lb; j++) {
            int cost = (a[i-1] == b[j-1]) ? 0 : 1;
            int m = d[prev][j] + 1;
            if (d[cur][j-1] + 1 < m) m = d[cur][j-1] + 1;
            if (d[prev][j-1] + cost < m) m = d[prev][j-1] + cost;
            d[cur][j] = m;
        }
    }
    return d[la & 1][lb];
}

/* ---- cluster store ---- */
typedef struct {
    uint32_t hash;
    int      type; /* 0=fingerprint, 1=levenshtein */
    char    *canonical;
    char    *members[MAX_MEMBERS];
    int      count;
} cluster_t;

static cluster_t clusters[MAX_CLUSTERS];
static int n_clusters = 0;

static cluster_t *find_or_create(uint32_t h) {
    for (int i = 0; i < n_clusters; i++)
        if (clusters[i].hash == h) return &clusters[i];
    if (n_clusters >= MAX_CLUSTERS) return NULL;
    clusters[n_clusters].hash = h;
    clusters[n_clusters].count = 0;
    return &clusters[n_clusters++];
}

static void add_member(cluster_t *c, const char *val) {
    if (c->count >= MAX_MEMBERS) return;
    for (int i = 0; i < c->count; i++)
        if (!strcmp(c->members[i], val)) return;
    c->members[c->count++] = strdup(val);
    if (c->count == 1) c->canonical = c->members[0];
    if (strlen(val) < strlen(c->canonical)) c->canonical = c->members[c->count-1];
}

static int cluster_cmp(const void *a, const void *b) {
    const cluster_t *ca = a, *cb = b;
    return cb->count - ca->count;
}

/* ---- record accumulator for json_scan ---- */
#define MAX_FIELDS 16
typedef struct { char key[64], val[MAX_TEXT]; int vtype; } field_t;
typedef struct { field_t f[MAX_FIELDS]; int nf; } record_t;

static record_t cur_rec;
static char **all_vals; static int n_all;
static const char *g_filter;
static int g_use_lev, g_lev_thresh;

static void rec_collect(const char *key, int klen, const char *val, int vlen, int vtype, void *u) {
    (void)u;
    if (!key) {
        /* record complete — extract type + text */
        char type[128] = "", text[MAX_TEXT] = "";
        for (int i = 0; i < cur_rec.nf; i++) {
            if (!strcmp(cur_rec.f[i].key, "type")) snprintf(type, sizeof(type), "%s", cur_rec.f[i].val);
            if (!strcmp(cur_rec.f[i].key, "text")) snprintf(text, sizeof(text), "%s", cur_rec.f[i].val);
        }
        cur_rec.nf = 0;
        if (g_filter && strcmp(type, g_filter)) return;
        if (!text[0]) return;

        if (!g_use_lev) {
            char fp[MAX_TEXT]; normalize(text, fp, sizeof(fp));
            if (!fp[0]) return;
            uint32_t h = hash_str(fp);
            cluster_t *c = find_or_create(h);
            if (c) { c->type = 0; add_member(c, text); }
        }
        if (n_all < MAX_RECORDS) all_vals[n_all++] = strdup(text);
        return;
    }
    /* accumulate field */
    if (cur_rec.nf < MAX_FIELDS) {
        int nl = klen < 63 ? klen : 63; memcpy(cur_rec.f[cur_rec.nf].key, key, nl); cur_rec.f[cur_rec.nf].key[nl] = 0;
        int vl = vlen < MAX_TEXT-1 ? vlen : MAX_TEXT-2; memcpy(cur_rec.f[cur_rec.nf].val, val, vl); cur_rec.f[cur_rec.nf].val[vl] = 0;
        cur_rec.f[cur_rec.nf].vtype = vtype; cur_rec.nf++;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) { fprintf(stderr, "Usage: %s sidecar.json [type_filter] [--levenshtein N]\n", argv[0]); return 1; }
    g_filter = (argc >= 3 && argv[2][0] != '-') ? argv[2] : NULL;
    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "--levenshtein") && i+1 < argc) { g_use_lev = 1; g_lev_thresh = atoi(argv[i+1]); }
    }

    all_vals = calloc(MAX_RECORDS, sizeof(char *));
    int n = json_scan_file(argv[1], rec_collect, NULL);

    /* Levenshtein pass */
    if (g_use_lev && n_all > 0) {
        char *seen = calloc((size_t)n_all, 1);
        for (int i = 0; i < n_all; i++) {
            if (seen[i]) continue;
            cluster_t *c = find_or_create((uint32_t)(i + 1000000));
            c->type = 1; add_member(c, all_vals[i]); seen[i] = 1;
            for (int j = i + 1; j < n_all; j++) {
                if (seen[j]) continue;
                int d = levenshtein(all_vals[i], (int)strlen(all_vals[i]), all_vals[j], (int)strlen(all_vals[j]));
                if (d <= g_lev_thresh) { add_member(c, all_vals[j]); seen[j] = 1; }
            }
        }
        free(seen);
    }

    /* output */
    qsort(clusters, n_clusters, sizeof(cluster_t), cluster_cmp);
    for (int i = 0; i < n_clusters; i++) {
        if (clusters[i].count < 2) continue;
        printf("{\"type\":\"%s\",\"canonical\":\"", g_filter ? g_filter : "*");
        for (char *s = clusters[i].canonical; *s; s++) {
            if (*s == '\\') printf("\\\\"); else if (*s == '"') printf("\\\""); else putchar(*s);
        }
        printf("\",\"members\":[");
        for (int j = 0; j < clusters[i].count; j++) {
            if (j > 0) printf(","); printf("\"");
            for (char *s = clusters[i].members[j]; *s; s++) {
                if (*s == '\\') printf("\\\\"); else if (*s == '"') printf("\\\""); else putchar(*s);
            }
            printf("\"");
        }
        printf("],\"count\":%d,\"algorithm\":\"%s\"}\n", clusters[i].count,
               clusters[i].type ? "levenshtein" : "fingerprint");
    }
    return 0;
}
