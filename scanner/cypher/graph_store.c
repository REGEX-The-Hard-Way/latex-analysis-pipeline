/*
 * graph_store.c — High-performance graph storage engine.
 *
 * Struct-of-arrays layout, CSR adjacency, columnar properties,
 * khashl hash indexes (klib), arena value heap.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph_store.h"

#define GS_INIT_NODES   1048576   /* 1M initial */
#define GS_INIT_EDGES   2097152   /* 2M initial */
#define GS_INIT_PROPS   4194304   /* 4M initial */
#define GS_INIT_BITMAP  32768     /* bits for 2M nodes */

/* ------------------------------------------------------------------ */
/*  Hashing                                                           */
/* ------------------------------------------------------------------ */

uint32_t gs_hash_str(const char *s) {
    uint32_t h = 0;
    while (*s) h = h * 31 + (unsigned char)*s++;
    return h;
}

/* ------------------------------------------------------------------ */
/*  Arena allocator                                                    */
/* ------------------------------------------------------------------ */

static char *arena_alloc(graph_store_t *gs, size_t len) {
    if (gs->val_used + len > gs->val_cap) {
        size_t new_cap = gs->val_cap ? gs->val_cap * 2 : 65536;
        while (new_cap < gs->val_used + len) new_cap *= 2;
        gs->val_data = realloc(gs->val_data, new_cap);
        gs->val_cap = new_cap;
    }
    char *p = gs->val_data + gs->val_used;
    gs->val_used += len;
    return p;
}

/* ------------------------------------------------------------------ */
/*  Lifecycle                                                         */
/* ------------------------------------------------------------------ */
/*  Lifecycle                                                         */
/* ------------------------------------------------------------------ */

graph_store_t *gs_create(void) {
    graph_store_t *gs = calloc(1, sizeof(*gs));
    if (!gs) return NULL;

    gs->node_cap = GS_INIT_NODES;
    gs->nodes = calloc(gs->node_cap, sizeof(gs_node_t));
    gs->edge_cap = GS_INIT_EDGES;
    gs->edges = calloc(gs->edge_cap, sizeof(gs_edge_t));
    gs->edge_heads = malloc(gs->node_cap * sizeof(uint32_t));
    gs->prop_cap = GS_INIT_PROPS;
    gs->props  = malloc(gs->prop_cap * sizeof(gs_prop_t));

    gs->bitmap_words = GS_INIT_BITMAP;
    gs->bitmap = calloc(gs->bitmap_words, sizeof(uint64_t));
    gs->collect = malloc(gs->node_cap * sizeof(uint32_t));

    if (!gs->nodes || !gs->edges || !gs->edge_heads || !gs->props
        || !gs->bitmap || !gs->collect) {
        gs_destroy(gs);
        return NULL;
    }
    for (uint32_t i = 0; i < gs->node_cap; i++)
        gs->edge_heads[i] = 0xFFFFFFFF;

    return gs;
}

void gs_destroy(graph_store_t *gs) {
    if (!gs) return;
    free(gs->nodes);
    free(gs->edges);
    free(gs->edge_heads);
    free(gs->props);
    free(gs->bitmap);
    free(gs->collect);
    if (gs->label_idx) gs_lidx_destroy(gs->label_idx);
    kv_destroy(gs->label_nodes);
    if (gs->prop_idx)  gs_lidx_destroy(gs->prop_idx);
    kv_destroy(gs->prop_nodes);
    if (gs->edge_idx)  gs_lidx_destroy(gs->edge_idx);
    kv_destroy(gs->edge_nodes);
    free(gs->val_data);
    free(gs);
}

/* ------------------------------------------------------------------ */
/*  Mutations                                                         */
/* ------------------------------------------------------------------ */

uint32_t gs_add_node(graph_store_t *gs) {
    if (gs->node_count >= gs->node_cap) {
        uint32_t nc = gs->node_cap * 2;
        gs->nodes = realloc(gs->nodes, nc * sizeof(gs_node_t));
        gs->edge_heads = realloc(gs->edge_heads, nc * sizeof(uint32_t));
        for (uint32_t i = gs->node_cap; i < nc; i++)
            gs->edge_heads[i] = 0xFFFFFFFF;
        gs->node_cap = nc;
        gs->bitmap_words = (nc + 63) / 64;
        gs->bitmap = realloc(gs->bitmap, gs->bitmap_words * sizeof(uint64_t));
        gs->collect = realloc(gs->collect, nc * sizeof(uint32_t));
    }
    uint32_t n = gs->node_count++;
    memset(&gs->nodes[n], 0, sizeof(gs_node_t));
    gs->nodes[n].props_off = 0xFFFFFFFF;
    return n;
}

int gs_label_index(graph_store_t *gs, const char *label) {
    uint32_t h = gs_hash_str(label);
    for (uint32_t i = 0; i < gs->label_count; i++)
        if (gs->labels[i].hash == h && !strcmp(gs->labels[i].name, label))
            return (int)i;
    if (gs->label_count >= GS_MAX_LABELS) return -1;
    uint32_t idx = gs->label_count++;
    strncpy(gs->labels[idx].name, label, 127);
    gs->labels[idx].hash = h;
    return (int)idx;
}

void gs_set_label(graph_store_t *gs, uint32_t node, const char *label) {
    if (node >= gs->node_count) return;
    int idx = gs_label_index(gs, label);
    if (idx < 0) return;
    gs->nodes[node].label_mask |= (1ULL << (uint32_t)idx);
}

uint32_t gs_add_edge(graph_store_t *gs, uint32_t src, uint32_t dst,
                     const char *type) {
    if (src >= gs->node_count || dst >= gs->node_count) return 0xFFFFFFFF;
    if (gs->edge_count >= gs->edge_cap) {
        gs->edge_cap *= 2;
        gs->edges = realloc(gs->edges, gs->edge_cap * sizeof(gs_edge_t));
    }
    uint32_t ei = gs->edge_count++;
    gs->edges[ei].dst = dst;
    gs->edges[ei].type = gs_hash_str(type);
    gs->edges[ei].next = gs->edge_heads[src];
    gs->edge_heads[src] = ei;
    gs->nodes[src].edge_count++;
    return ei;
}

/* ----- property storage ----- */

static uint32_t prop_put(graph_store_t *gs, uint32_t node,
                          const char *key, const char *val, int is_num) {
    if (node >= gs->node_count) return 0xFFFFFFFF;
    if (gs->prop_count >= gs->prop_cap) {
        gs->prop_cap *= 2;
        gs->props = realloc(gs->props, gs->prop_cap * sizeof(gs_prop_t));
    }
    uint32_t pi = gs->prop_count++;
    gs->props[pi].key_hash = gs_hash_str(key);
    size_t vl = strlen(val);
    /* store value: [is_num:1B][data:vl+1B] */
    char *vbuf = arena_alloc(gs, vl + 2);
    vbuf[0] = (char)is_num;
    memcpy(vbuf + 1, val, vl + 1);
    gs->props[pi].val_off = (uint32_t)(vbuf - gs->val_data);

    if (gs->nodes[node].props_off == 0xFFFFFFFF)
        gs->nodes[node].props_off = pi;
    return pi;
}

void gs_add_prop_str(graph_store_t *gs, uint32_t node,
                     const char *key, const char *val) {
    prop_put(gs, node, key, val, 0);
}

void gs_add_prop_num(graph_store_t *gs, uint32_t node,
                     const char *key, double val) {
    char buf[32];
    int n = snprintf(buf, sizeof(buf), "%.15g", val);
    if (n < 0) return;
    prop_put(gs, node, key, buf, 1);
}

void gs_set_prop_str(graph_store_t *gs, uint32_t node,
                     const char *key, const char *val) {
    prop_put(gs, node, key, val, 0);
}

void gs_set_prop_num(graph_store_t *gs, uint32_t node,
                     const char *key, double val) {
    char buf[32];
    int n = snprintf(buf, sizeof(buf), "%.15g", val);
    if (n < 0) return;
    prop_put(gs, node, key, buf, 1);
}

/* ----- index building ----- */

void gs_build_indexes(graph_store_t *gs) {
    uint32_t lc = gs->label_count;
    if (lc == 0) return;

    /* allocate per-label node arrays */
    uint32_t **lnodes = calloc(lc, sizeof(uint32_t *));
    uint32_t  *lcaps  = calloc(lc, sizeof(uint32_t));
    uint32_t  *lcounts = calloc(lc, sizeof(uint32_t));

    for (uint32_t li = 0; li < lc; li++) {
        lcaps[li] = 256;
        lnodes[li] = malloc(256 * sizeof(uint32_t));
    }

    /* collect nodes per label */
    for (uint32_t n = 0; n < gs->node_count; n++) {
        uint64_t mask = gs->nodes[n].label_mask;
        for (uint32_t li = 0; li < lc; li++) {
            if (mask & (1ULL << li)) {
                if (lcounts[li] >= lcaps[li]) {
                    lcaps[li] *= 2;
                    lnodes[li] = realloc(lnodes[li], lcaps[li] * sizeof(uint32_t));
                }
                lnodes[li][lcounts[li]++] = n;
            }
        }
    }

    /* build khashl index + contiguous node list */
    if (gs->label_idx) gs_lidx_destroy(gs->label_idx);
    kv_destroy(gs->label_nodes);
    kv_init(gs->label_nodes);

    gs->label_idx = gs_lidx_init();

    for (uint32_t li = 0; li < lc; li++) {
        uint32_t key = gs->labels[li].hash;
        uint32_t cnt = lcounts[li];
        if (cnt == 0) continue;

        gs_node_range_t r;
        r.off = (uint32_t)kv_size(gs->label_nodes);
        r.cnt = cnt;

        for (uint32_t j = 0; j < cnt; j++)
            kv_push(uint32_t, gs->label_nodes, lnodes[li][j]);

        int absent;
        khint_t slot = gs_lidx_put(gs->label_idx, key, &absent);
        kh_val(gs->label_idx, slot) = r;
    }

    for (uint32_t li = 0; li < lc; li++) free(lnodes[li]);
    free(lnodes); free(lcaps); free(lcounts);

    /* build property key index */
    kvec_t(uint32_t) *pkv = NULL;
    uint32_t pk_cap = 0, pk_cnt = 0;
    uint32_t *pk_keys = calloc(256, sizeof(uint32_t));
    pk_cap = 256;

    for (uint32_t n = 0; n < gs->node_count; n++) {
        if (gs->nodes[n].props_off == 0xFFFFFFFF) continue;
        for (uint32_t pi = gs->nodes[n].props_off;
             pi < gs->prop_count && gs->props[pi].key_hash != 0xFFFFFFFF; pi++) {
            uint32_t kh = gs->props[pi].key_hash;
            int found = 0;
            for (uint32_t i = 0; i < pk_cnt; i++)
                if (pk_keys[i] == kh) { found = 1; break; }
            if (!found) {
                if (pk_cnt >= pk_cap) {
                    pk_cap *= 2;
                    pk_keys = realloc(pk_keys, pk_cap * sizeof(uint32_t));
                }
                pk_keys[pk_cnt++] = kh;
            }
        }
    }

    pkv = calloc(pk_cnt, sizeof(kvec_t(uint32_t)));
    for (uint32_t i = 0; i < pk_cnt; i++) kv_init(pkv[i]);

    for (uint32_t n = 0; n < gs->node_count; n++) {
        if (gs->nodes[n].props_off == 0xFFFFFFFF) continue;
        for (uint32_t pi = gs->nodes[n].props_off;
             pi < gs->prop_count && gs->props[pi].key_hash != 0xFFFFFFFF; pi++) {
            uint32_t kh = gs->props[pi].key_hash;
            for (uint32_t i = 0; i < pk_cnt; i++)
                if (pk_keys[i] == kh) { kv_push(uint32_t, pkv[i], n); break; }
        }
    }

    if (gs->prop_idx) gs_lidx_destroy(gs->prop_idx);
    kv_destroy(gs->prop_nodes);
    kv_init(gs->prop_nodes);
    gs->prop_idx = gs_lidx_init();

    for (uint32_t i = 0; i < pk_cnt; i++) {
        uint32_t cnt = (uint32_t)kv_size(pkv[i]);
        if (cnt == 0) continue;
        gs_node_range_t r;
        r.off = (uint32_t)kv_size(gs->prop_nodes);
        r.cnt = cnt;
        for (uint32_t j = 0; j < cnt; j++)
            kv_push(uint32_t, gs->prop_nodes, kv_A(pkv[i], j));
        int absent;
        khint_t slot = gs_lidx_put(gs->prop_idx, pk_keys[i], &absent);
        kh_val(gs->prop_idx, slot) = r;
    }

    for (uint32_t i = 0; i < pk_cnt; i++) kv_destroy(pkv[i]);
    free(pkv);
    free(pk_keys);
}

/* ------------------------------------------------------------------ */
/*  Queries                                                           */
/* ------------------------------------------------------------------ */

uint32_t gs_label_count(graph_store_t *gs, const char *label) {
    if (!gs->label_idx) return 0;
    khint_t slot = gs_lidx_get(gs->label_idx, gs_hash_str(label));
    if (slot == kh_end(gs->label_idx)) return 0;
    return kh_val(gs->label_idx, slot).cnt;
}

uint32_t gs_label_nodes(graph_store_t *gs, const char *label,
                         uint32_t *out, uint32_t max_out) {
    if (!gs->label_idx) {
        /* fallback: linear scan */
        int li = gs_label_index(gs, label);
        if (li < 0) return 0;
        uint32_t count = 0;
        uint64_t mask = 1ULL << (uint32_t)li;
        for (uint32_t n = 0; n < gs->node_count && count < max_out; n++)
            if (gs->nodes[n].label_mask & mask) out[count++] = n;
        return count;
    }
    khint_t slot = gs_lidx_get(gs->label_idx, gs_hash_str(label));
    if (slot == kh_end(gs->label_idx)) return 0;
    gs_node_range_t r = kh_val(gs->label_idx, slot);
    uint32_t cnt = r.cnt;
    if (cnt > max_out) cnt = max_out;
    memcpy(out, &kv_A(gs->label_nodes, r.off), cnt * sizeof(uint32_t));
    return cnt;
}

/* ----- bitmap collection ----- */

void gs_collect_begin(graph_store_t *gs) {
    gs->collect_count = 0;
    memset(gs->bitmap, 0xFF, gs->bitmap_words * sizeof(uint64_t));
}

void gs_collect_add_label(graph_store_t *gs, const char *label) {
    int li = gs_label_index(gs, label);
    if (li < 0) { memset(gs->bitmap, 0, gs->bitmap_words * sizeof(uint64_t)); return; }
    uint64_t mask = 1ULL << (uint32_t)li;
    for (uint32_t n = 0; n < gs->node_count; n++) {
        uint32_t w = n >> 6, b = n & 63;
        if (!(gs->nodes[n].label_mask & mask))
            gs->bitmap[w] &= ~(1ULL << b);
    }
}

void gs_collect_add_filter_prop_str(graph_store_t *gs, const char *key,
                                     const char *val) {
    uint32_t kh = gs_hash_str(key);
    for (uint32_t n = 0; n < gs->node_count; n++) {
        uint32_t w = n >> 6, b = n & 63;
        if (!(gs->bitmap[w] & (1ULL << b))) continue;
        int match = 0;
        if (gs->nodes[n].props_off != 0xFFFFFFFF) {
            for (uint32_t pi = gs->nodes[n].props_off;
                 pi < gs->prop_count && gs->props[pi].key_hash != 0xFFFFFFFF; pi++) {
                if (gs->props[pi].key_hash == kh) {
                    char *vbuf = gs->val_data + gs->props[pi].val_off;
                    if (vbuf[0] == 0 && !strcmp(vbuf + 1, val)) { match = 1; break; }
                }
            }
        }
        if (!match) gs->bitmap[w] &= ~(1ULL << b);
    }
}

void gs_collect_add_filter_prop_num(graph_store_t *gs, const char *key,
                                     double val, char op) {
    uint32_t kh = gs_hash_str(key);
    for (uint32_t n = 0; n < gs->node_count; n++) {
        uint32_t w = n >> 6, b = n & 63;
        if (!(gs->bitmap[w] & (1ULL << b))) continue;
        int match = 0;
        if (gs->nodes[n].props_off != 0xFFFFFFFF) {
            for (uint32_t pi = gs->nodes[n].props_off;
                 pi < gs->prop_count && gs->props[pi].key_hash != 0xFFFFFFFF; pi++) {
                if (gs->props[pi].key_hash == kh) {
                    char *vbuf = gs->val_data + gs->props[pi].val_off;
                    if (vbuf[0] == 1) {
                        double dv = strtod(vbuf + 1, NULL);
                        if (op == '=' && dv == val) match = 1;
                        else if (op == '>' && dv > val) match = 1;
                        else if (op == '<' && dv < val) match = 1;
                    }
                    break;
                }
            }
        }
        if (!match) gs->bitmap[w] &= ~(1ULL << b);
    }
}

uint32_t gs_collect_drain(graph_store_t *gs, uint32_t *out, uint32_t max_out) {
    uint32_t count = 0;
    for (uint32_t n = 0; n < gs->node_count && count < max_out; n++) {
        uint32_t w = n >> 6, b = n & 63;
        if (gs->bitmap[w] & (1ULL << b))
            out[count++] = n;
    }
    return count;
}

/* ------------------------------------------------------------------ */
/*  Property access                                                    */
/* ------------------------------------------------------------------ */

const char *gs_prop_str(graph_store_t *gs, uint32_t node, const char *key) {
    if (node >= gs->node_count) return NULL;
    uint32_t kh = gs_hash_str(key);
    if (gs->nodes[node].props_off != 0xFFFFFFFF) {
        for (uint32_t pi = gs->nodes[node].props_off;
             pi < gs->prop_count && gs->props[pi].key_hash != 0xFFFFFFFF; pi++) {
            if (gs->props[pi].key_hash == kh) {
                char *vbuf = gs->val_data + gs->props[pi].val_off;
                if (vbuf[0] == 0) return vbuf + 1;
            }
        }
    }
    return NULL;
}

double gs_prop_num(graph_store_t *gs, uint32_t node, const char *key) {
    const char *s = gs_prop_str(gs, node, key);
    if (!s) {
        uint32_t kh = gs_hash_str(key);
        if (gs->nodes[node].props_off != 0xFFFFFFFF) {
            for (uint32_t pi = gs->nodes[node].props_off;
                 pi < gs->prop_count && gs->props[pi].key_hash != 0xFFFFFFFF; pi++) {
                if (gs->props[pi].key_hash == kh) {
                    char *vbuf = gs->val_data + gs->props[pi].val_off;
                    if (vbuf[0] == 1) return strtod(vbuf + 1, NULL);
                }
            }
        }
        return 0.0;
    }
    return strtod(s, NULL);
}

const char *gs_label_name(graph_store_t *gs, uint32_t idx) {
    if (idx >= gs->label_count) return NULL;
    return gs->labels[idx].name;
}

/* ------------------------------------------------------------------ */
/*  Edge traversal                                                     */
/* ------------------------------------------------------------------ */

uint32_t gs_edge_count(graph_store_t *gs, uint32_t node) {
    if (node >= gs->node_count) return 0;
    return gs->nodes[node].edge_count;
}

uint32_t gs_edge_dst(graph_store_t *gs, uint32_t node, uint32_t pos) {
    if (node >= gs->node_count) return 0xFFFFFFFF;
    uint32_t ei = gs->edge_heads[node];
    uint32_t c = 0;
    while (ei != 0xFFFFFFFF && c < pos && ei < gs->edge_count) {
        ei = gs->edges[ei].next;
        c++;
    }
    return (ei != 0xFFFFFFFF && ei < gs->edge_count) ? gs->edges[ei].dst : 0xFFFFFFFF;
}

uint32_t gs_edge_type(graph_store_t *gs, uint32_t node, uint32_t pos) {
    if (node >= gs->node_count) return 0;
    uint32_t ei = gs->edge_heads[node];
    uint32_t c = 0;
    while (ei != 0xFFFFFFFF && c < pos && ei < gs->edge_count) {
        ei = gs->edges[ei].next;
        c++;
    }
    return (ei != 0xFFFFFFFF && ei < gs->edge_count) ? gs->edges[ei].type : 0;
}

const char *gs_edge_type_name(graph_store_t *gs, uint32_t type_hash) {
    (void)gs;
    static char buf[32];
    snprintf(buf, sizeof(buf), "t%u", type_hash);
    return buf;
}

/* ------------------------------------------------------------------ */
/*  Property key index query                                           */
/* ------------------------------------------------------------------ */

uint32_t gs_prop_key_nodes(graph_store_t *gs, const char *key,
                            uint32_t *out, uint32_t max_out) {
    if (!gs->prop_idx) return 0;
    uint32_t kh = gs_hash_str(key);
    khint_t slot = gs_lidx_get(gs->prop_idx, kh);
    if (slot == kh_end(gs->prop_idx)) return 0;
    gs_node_range_t r = kh_val(gs->prop_idx, slot);
    uint32_t cnt = r.cnt;
    if (cnt > max_out) cnt = max_out;
    memcpy(out, &kv_A(gs->prop_nodes, r.off), cnt * sizeof(uint32_t));
    return cnt;
}

/* ------------------------------------------------------------------ */
/*  Stats                                                              */
/* ------------------------------------------------------------------ */

uint32_t gs_node_cap(graph_store_t *gs) { return gs->node_cap; }
uint32_t gs_node_count(graph_store_t *gs) { return gs->node_count; }
uint32_t gs_edge_total(graph_store_t *gs) { return gs->edge_count; }
