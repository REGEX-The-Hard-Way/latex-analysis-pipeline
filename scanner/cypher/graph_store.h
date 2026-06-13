/*
 * graph_store.h — High-performance in-memory graph storage engine.
 *
 * Targets 2M+ nodes, 10M+ edges with sub-millisecond queries.
 *
 * Key design:
 *   - Struct-of-arrays: filter reads only relevant columns
 *   - CSR adjacency: O(degree) edge traversal
 *   - Columnar props: key_hash + val_offset arrays, arena value heap
 *   - Bitmap labels: 64-bit bitmap for O(1) label presence test
 *   - No MVCC, no locks, no transactions — single-writer, batch-load model
 */
#ifndef GRAPH_STORE_H
#define GRAPH_STORE_H

#include <stdint.h>
#include <stddef.h>
#include "khashl.h"
#include "kvec.h"

#define GS_MAX_LABELS      64     /* max distinct labels total */
#define GS_MAX_PROPS_PER   8      /* max properties per node */
#define GS_INDEX_LOAD      0.7    /* hash table load factor */

/* ---------- node header (32 bytes) ---------- */
typedef struct {
    uint32_t first_edge;         /* start index in edge CSR */
    uint32_t edge_count;         /* out-degree */
    uint64_t label_mask;         /* bitmap of label indices (up to 64 labels) */
    uint32_t props_off;          /* offset into property table (row index) */
    uint32_t prop_count;         /* number of properties on this node */
} gs_node_t;

/* ---------- edge CSR (12 bytes) ---------- */
typedef struct {
    uint32_t dst;                /* target node index */
    uint32_t type;               /* edge type hash */
    uint32_t next;               /* next edge from same src (linked list in array) */
} gs_edge_t;

/* ---------- property row ---------- */
typedef struct {
    uint32_t key_hash;           /* hash of property key */
    uint32_t val_off;            /* offset into value heap */
} gs_prop_t;

/* ---------- label string table ---------- */
typedef struct {
    char     name[128];
    uint32_t hash;
} gs_label_t;

/* ---------- hash index (klib khashl-based) ---------- */
typedef struct { uint32_t off; uint32_t cnt; } gs_node_range_t;
KHASHL_MAP_INIT(static, gs_lidx_t, gs_lidx, uint32_t, gs_node_range_t, kh_hash_dummy, kh_eq_generic)

/* ---------- graph store ---------- */
typedef struct {
    /* node storage */
    gs_node_t  *nodes;
    uint32_t    node_cap;
    uint32_t    node_count;

    /* edge storage (CSR) */
    gs_edge_t  *edges;
    uint32_t    edge_cap;
    uint32_t    edge_count;
    uint32_t   *edge_heads;      /* edge_heads[node] = first_edge or 0xFFFFFFFF */

    /* property storage (columnar) */
    gs_prop_t  *props;
    uint32_t    prop_cap;
    uint32_t    prop_count;

    /* label table */
    gs_label_t  labels[GS_MAX_LABELS];
    uint32_t    label_count;

    /* value arena */
    char        *val_data;
    size_t       val_used;
    size_t       val_cap;

    /* hash indexes */
    gs_lidx_t       *label_idx;     /* label_hash → {offset, count} */
    kvec_t(uint32_t) label_nodes;   /* contiguous node lists */
    gs_lidx_t       *prop_idx;      /* prop_key_hash → {offset, count} */
    kvec_t(uint32_t) prop_nodes;    /* contiguous node lists */
    gs_lidx_t       *edge_idx;      /* edge_type_hash → {offset, count} (unused) */
    kvec_t(uint32_t) edge_nodes;    /* (unused) */

    /* trigram text index (Phase 5) */
    gs_lidx_t       *text_idx;      /* trigram_hash → {offset, count} */
    kvec_t(uint32_t) text_nodes;    /* contiguous node lists */

    /* filepath interning: filepath_id → filepath string */
    void          *fp_table;      /* fpt_t* : filepath_id (uint32) → char* */

    /* scratch buffer for queries */
    uint64_t    *bitmap;
    uint32_t     bitmap_words;
    uint32_t    *collect;
    uint32_t     collect_cap;
    uint32_t     collect_count;
} graph_store_t;

/* ---------- lifecycle ---------- */
graph_store_t *gs_create(void);
void           gs_destroy(graph_store_t *gs);
uint32_t       gs_hash_str(const char *s);

/* ---------- mutations ---------- */
uint32_t gs_add_node(graph_store_t *gs);
void     gs_set_label(graph_store_t *gs, uint32_t node, const char *label);
int      gs_label_index(graph_store_t *gs, const char *label); /* get or create */
void     gs_add_prop_str(graph_store_t *gs, uint32_t node,
                          const char *key, const char *val);
void     gs_add_prop_num(graph_store_t *gs, uint32_t node,
                           const char *key, double val);
void     gs_add_prop_int(graph_store_t *gs, uint32_t node,
                          const char *key, uint32_t val);
void     gs_set_prop_str(graph_store_t *gs, uint32_t node,
                          const char *key, const char *val);
void     gs_set_prop_num(graph_store_t *gs, uint32_t node,
                          const char *key, double val);
uint32_t gs_add_edge(graph_store_t *gs, uint32_t src, uint32_t dst,
                     const char *type);
void     gs_build_indexes(graph_store_t *gs);
void     gs_build_text_index(graph_store_t *gs);

/* ---------- queries ---------- */

/* Count of nodes matching label (uses index if built) */
uint32_t gs_label_count(graph_store_t *gs, const char *label);

/* Fill `out` with node IDs matching label (returns count) */
uint32_t gs_label_nodes(graph_store_t *gs, const char *label,
                         uint32_t *out, uint32_t max_out);
uint32_t gs_prop_key_nodes(graph_store_t *gs, const char *key,
                            uint32_t *out, uint32_t max_out);
uint32_t gs_text_search(graph_store_t *gs, const char *prop_key,
                         const char *search_str, uint32_t *out, uint32_t max_out);

/* Bitmap operations on collected sets */
void gs_collect_begin(graph_store_t *gs);
void gs_collect_add_label(graph_store_t *gs, const char *label);
void gs_collect_add_filter_prop_str(graph_store_t *gs, const char *key,
                                     const char *val);
void gs_collect_add_filter_prop_num(graph_store_t *gs, const char *key,
                                     double val, char op);
uint32_t gs_collect_drain(graph_store_t *gs, uint32_t *out, uint32_t max_out);

/* ---------- property access ---------- */
const char *gs_prop_str(graph_store_t *gs, uint32_t node, const char *key);
double      gs_prop_num(graph_store_t *gs, uint32_t node, const char *key);
uint32_t    gs_prop_int(graph_store_t *gs, uint32_t node, const char *key);
const char *gs_label_name(graph_store_t *gs, uint32_t idx);

/* filepath interning */
void        gs_fp_put(graph_store_t *gs, uint32_t filepath_id, const char *path);
const char *gs_fp_get(graph_store_t *gs, uint32_t filepath_id);

/* ---------- edge traversal ---------- */
uint32_t gs_edge_count(graph_store_t *gs, uint32_t node);
uint32_t gs_edge_dst(graph_store_t *gs, uint32_t node, uint32_t edge_idx);
uint32_t gs_edge_type(graph_store_t *gs, uint32_t node, uint32_t edge_idx);
const char *gs_edge_type_name(graph_store_t *gs, uint32_t type_hash);

/* ---------- stats ---------- */
uint32_t gs_node_cap(graph_store_t *gs);
uint32_t gs_node_count(graph_store_t *gs);
uint32_t gs_edge_total(graph_store_t *gs);

#endif /* GRAPH_STORE_H */
