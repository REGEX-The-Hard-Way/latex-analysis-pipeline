/*
 * match_patterns.c — Pattern matchers for extracting definitions from parse trees.
 *
 * 15 syntactic patterns, tried in priority order.
 * Each pattern takes (tree_t *, int cd_node_idx, char *buf, int bufsz)
 * and returns the number of bytes written to buf, or 0 for no match.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parse_tree.h"

/* ---- helpers ---- */

static bool n_is_leaf(tree_t *t, int idx) {
    return idx >= 0 && idx < t->n_nodes && t->nodes[idx].first_child < 0;
}

bool n_is_cd_id(tree_t *t, int idx) {
    if (idx < 0 || idx >= t->n_nodes) return false;
    node_t *n = &t->nodes[idx];
    if (n->label != L_CD || n->text_len < 1) return false;
    char *w = t->text + n->text_off;
    for (int i = 0; i < n->text_len; i++)
        if (!isdigit((unsigned char)w[i])) return false;
    return n->text_len >= 4;
}

static bool node_contains(tree_t *t, int ancestor, int target) {
    if (ancestor < 0 || target < 0) return false;
    if (ancestor == target) return true;
    for (int c = t->nodes[ancestor].first_child; c >= 0;
         c = t->nodes[c].next_sibling)
        if (node_contains(t, c, target)) return true;
    return false;
}

static int append_word(tree_t *t, int leaf_idx, char *buf, int *bp, int bufsz) {
    node_t *n = &t->nodes[leaf_idx];
    int wlen = n->text_len;
    char *w = t->text + n->text_off;
    if (wlen < 1) return 0;
    /* skip pure punctuation */
    if (wlen == 1 && strchr(".,:;!?\"'`", w[0])) return 0;
    if (!strcmp(w, "''") || !strcmp(w, "``")) return 0;
    if (!strcmp(w, "--") || !strcmp(w, "-")) return 0;
    /* add space before word if needed */
    if (*bp > 0 && buf[(*bp)-1] != ' ' && *bp + 1 < bufsz)
        buf[(*bp)++] = ' ';
    int space = bufsz - *bp - 1;
    if (space <= 0) return 0;
    int copy = wlen < space ? wlen : space;
    memcpy(buf + *bp, w, (size_t)copy);
    *bp += copy;
    return copy;
}

static void collect_subtree(tree_t *t, int node_idx, int skip_idx,
                            char *buf, int *bp, int bufsz) {
    if (node_idx < 0 || node_idx >= t->n_nodes) return;
    if (node_idx == skip_idx) return;
    node_t *n = &t->nodes[node_idx];
    if (n->first_child < 0) {
        append_word(t, node_idx, buf, bp, bufsz);
        return;
    }
    for (int c = n->first_child; c >= 0; c = t->nodes[c].next_sibling)
        collect_subtree(t, c, skip_idx, buf, bp, bufsz);
}

static bool starts_with_article(tree_t *t, int np_idx) {
    if (np_idx < 0) return false;
    int c = t->nodes[np_idx].first_child;
    /* check first leaf descendant */
    while (c >= 0 && t->nodes[c].first_child >= 0)
        c = t->nodes[c].first_child;
    if (c < 0) return false;
    char *w = n_text(t, c);
    if (!w) return false;
    return !strcmp(w, "the") || !strcmp(w, "The") ||
           !strcmp(w, "a") || !strcmp(w, "A") ||
           !strcmp(w, "an") || !strcmp(w, "An");
}

static int word_count(const char *s, int len) {
    int wc = 0, in_word = 0;
    for (int i = 0; i < len; i++) {
        if (s[i] == ' ') { in_word = 0; }
        else if (!in_word) { wc++; in_word = 1; }
    }
    return wc;
}

/*
 * Collect NP phrase: all words from children of np_idx EXCEPT the CD leaf.
 * Stops collecting at the child that contains the CD.
 */
static int collect_np_phrase(tree_t *t, int np_idx, int cd_idx,
                             char *buf, int bufsz) {
    int bp = 0;
    bool found = false;
    for (int c = t->nodes[np_idx].first_child; c >= 0;
         c = t->nodes[c].next_sibling) {
        bool has_cd = node_contains(t, c, cd_idx);
        if (has_cd) {
            found = true;
            collect_subtree(t, c, cd_idx, buf, &bp, bufsz);
        } else if (!found) {
            collect_subtree(t, c, -1, buf, &bp, bufsz);
        }
        if (found && !has_cd) break;
    }
    buf[bp] = '\0';
    return bp;
}

/*
 * Collect from parent NP: all preceding NP siblings + target NP phrase.
 */
static int collect_with_siblings(tree_t *t, int parent_np, int target_np,
                                 int cd_idx, char *buf, int bufsz) {
    int bp = 0;
    bool found = false;
    for (int c = t->nodes[parent_np].first_child; c >= 0;
         c = t->nodes[c].next_sibling) {
        if (c == target_np || node_contains(t, c, cd_idx)) {
            found = true;
            collect_subtree(t, c, cd_idx, buf, &bp, bufsz);
        } else if (!found) {
            int cl = t->nodes[c].label;
            if (cl == L_NP || cl == L_ADJP || cl == L_JJ || cl == L_CD ||
                cl == L_DT || cl == L_NN || cl == L_NNS || cl == L_NNP) {
                collect_subtree(t, c, -1, buf, &bp, bufsz);
            }
        }
    }
    buf[bp] = '\0';
    return bp;
}

/* ---- pattern matchers ---- */

typedef int (*pat_fn)(tree_t *, int, char *, int);

/* P1: apposition — CD inside ADJP under NP */
static int p1_apposition(tree_t *t, int cd_idx, char *buf, int bufsz) {
    for (int n = cd_idx; n >= 0; n = t->nodes[n].parent) {
        if (t->nodes[n].label == L_ADJP) {
            int np = t->nodes[n].parent;
            if (np >= 0 && t->nodes[np].label == L_NP) {
                int len = collect_np_phrase(t, np, cd_idx, buf, bufsz);
                if (len >= 3) return len;
            }
        }
        if (t->nodes[n].label == L_NP) {
            int len = collect_np_phrase(t, n, cd_idx, buf, bufsz);
            if (len >= 3 && starts_with_article(t, n)) return len;
        }
    }
    return 0;
}

/* P2: NP-before — CD is direct child of NP */
static int p2_np_before(tree_t *t, int cd_idx, char *buf, int bufsz) {
    int p = t->nodes[cd_idx].parent;
    if (p < 0 || t->nodes[p].label != L_NP) return 0;
    int len = collect_np_phrase(t, p, cd_idx, buf, bufsz);
    return (len >= 3) ? len : 0;
}

/* P3: copula — (S (NP ..CD..) (VP (VBZ is/are) (NP def))) */
static int p3_copula(tree_t *t, int cd_idx, char *buf, int bufsz) {
    /* walk up to S */
    int s_node = -1;
    for (int n = t->nodes[cd_idx].parent; n >= 0; n = t->nodes[n].parent) {
        if (t->nodes[n].label == L_S) { s_node = n; break; }
    }
    if (s_node < 0) return 0;

    /* find VP with copula verb */
    for (int c = t->nodes[s_node].first_child; c >= 0;
         c = t->nodes[c].next_sibling) {
        if (t->nodes[c].label != L_VP) continue;
        if (node_contains(t, c, cd_idx)) continue;
        /* check for copula: VBZ is/are/was/were, VBP are, VBD was/were */
        int vb = t->nodes[c].first_child;
        while (vb >= 0 && t->nodes[vb].label != L_VBZ &&
               t->nodes[vb].label != L_VBP && t->nodes[vb].label != L_VBD)
            vb = t->nodes[vb].next_sibling;
        if (vb < 0) continue;
        char *vw = n_text(t, t->nodes[vb].first_child);
        if (!vw) continue;
        if (strcmp(vw, "is") && strcmp(vw, "are") &&
            strcmp(vw, "was") && strcmp(vw, "were") &&
            strcmp(vw, "Is") && strcmp(vw, "Are"))
            continue;
        /* find NP under this VP */
        for (int vc = t->nodes[c].first_child; vc >= 0;
             vc = t->nodes[vc].next_sibling) {
            if (t->nodes[vc].label == L_NP) {
                int len = collect_np_phrase(t, vc, cd_idx, buf, bufsz);
                if (len >= 3) return len;
            }
        }
    }
    return 0;
}

/* P4: hyphenated — CD inside PP chain, merge preceding NP siblings */
static int p4_hyphenated(tree_t *t, int cd_idx, char *buf, int bufsz) {
    /* walk up through PP chain to enclosing NP */
    int cur = cd_idx;
    while (cur >= 0 && t->nodes[cur].label != L_NP) {
        if (t->nodes[cur].label == L_NP) break;
        cur = t->nodes[cur].parent;
    }
    if (cur < 0 || t->nodes[cur].label != L_NP) return 0;

    /* check if there's a parent NP with preceding NP siblings */
    int par = t->nodes[cur].parent;
    if (par < 0 || t->nodes[par].label != L_NP) {
        /* just return the inner NP phrase */
        int len = collect_np_phrase(t, cur, cd_idx, buf, bufsz);
        return (len >= 3) ? len : 0;
    }

    int len = collect_with_siblings(t, par, cur, cd_idx, buf, bufsz);
    return (len >= 3) ? len : 0;
}

/* P5: PP-nested — walk up to article-starting NP */
static int p5_pp_nested(tree_t *t, int cd_idx, char *buf, int bufsz) {
    int best = -1, best_len = 0;
    for (int n = t->nodes[cd_idx].parent; n >= 0; n = t->nodes[n].parent) {
        if (t->nodes[n].label != L_NP) continue;
        char tmp[512];
        int len = collect_np_phrase(t, n, cd_idx, tmp, sizeof(tmp));
        if (len < 3) continue;
        bool art = starts_with_article(t, n);
        if (art && len > best_len) { best = n; best_len = len; break; }
        if (!art && len > best_len) { best = n; best_len = len; }
    }
    if (best < 0) return 0;
    return collect_np_phrase(t, best, cd_idx, buf, bufsz);
}

/* helper: find a verb pattern in S */
static bool has_vp_pattern(tree_t *t, int s_node, const char *verb,
                           const char *prep, int *out_np) {
    for (int c = t->nodes[s_node].first_child; c >= 0;
         c = t->nodes[c].next_sibling) {
        if (t->nodes[c].label != L_VP) continue;
        /* find VBN verb */
        int vbn = t->nodes[c].first_child;
        while (vbn >= 0) {
            if (t->nodes[vbn].label == L_VBN) {
                int leaf = t->nodes[vbn].first_child;
                if (leaf >= 0) {
                    char *w = n_text(t, leaf);
                    if (w && !strcmp(w, verb)) break;
                }
            }
            vbn = t->nodes[vbn].next_sibling;
        }
        if (vbn < 0) continue;
        /* find PP with prep */
        for (int pp = t->nodes[c].first_child; pp >= 0;
             pp = t->nodes[pp].next_sibling) {
            if (t->nodes[pp].label != L_PP) continue;
            int in = t->nodes[pp].first_child;
            if (in < 0 || t->nodes[in].label != L_IN) continue;
            int ileaf = t->nodes[in].first_child;
            if (ileaf < 0) continue;
            char *iw = n_text(t, ileaf);
            if (!iw || strcmp(iw, prep)) continue;
            /* find NP under PP */
            for (int np = t->nodes[pp].first_child; np >= 0;
                 np = t->nodes[np].next_sibling) {
                if (t->nodes[np].label == L_NP) {
                    *out_np = np; return true;
                }
            }
        }
    }
    return false;
}

/* P6-P10: verb+preposition patterns */
static int verb_prep_pat(tree_t *t, int cd_idx, char *buf, int bufsz,
                         const char *verb, const char *prep) {
    int s_node = -1;
    for (int n = t->nodes[cd_idx].parent; n >= 0; n = t->nodes[n].parent)
        if (t->nodes[n].label == L_S) { s_node = n; break; }
    if (s_node < 0) return 0;
    int def_np = -1;
    if (!has_vp_pattern(t, s_node, verb, prep, &def_np)) return 0;
    int len = collect_np_phrase(t, def_np, cd_idx, buf, bufsz);
    return (len >= 3) ? len : 0;
}

static int p6_given_by(tree_t *t, int cd_idx, char *buf, int bufsz)
    { return verb_prep_pat(t, cd_idx, buf, bufsz, "given", "by"); }
static int p7_defined_as(tree_t *t, int cd_idx, char *buf, int bufsz)
    { return verb_prep_pat(t, cd_idx, buf, bufsz, "defined", "as"); }
static int p8_denoted_by(tree_t *t, int cd_idx, char *buf, int bufsz)
    { return verb_prep_pat(t, cd_idx, buf, bufsz, "denoted", "by"); }
static int p9_known_as(tree_t *t, int cd_idx, char *buf, int bufsz)
    { return verb_prep_pat(t, cd_idx, buf, bufsz, "known", "as"); }
static int p10_referred_to(tree_t *t, int cd_idx, char *buf, int bufsz)
    { return verb_prep_pat(t, cd_idx, buf, bufsz, "referred", "to"); }

/* P11: relative clause — SBAR sibling with copula */
static int p11_relative_clause(tree_t *t, int cd_idx, char *buf, int bufsz) {
    /* Find NP containing CD */
    int np = -1;
    for (int n = cd_idx; n >= 0; n = t->nodes[n].parent)
        if (t->nodes[n].label == L_NP) { np = n; break; }
    if (np < 0) return 0;
    /* Check parent for SBAR sibling with copula */
    int par = t->nodes[np].parent;
    if (par < 0) return 0;
    for (int c = t->nodes[par].first_child; c >= 0;
         c = t->nodes[c].next_sibling) {
        if (c == np) continue;
        if (t->nodes[c].label != L_SBAR) continue;
        /* check for WHNP which/that + copula VP */
        int wh = t->nodes[c].first_child;
        while (wh >= 0 && t->nodes[wh].label != L_WHNP &&
               t->nodes[wh].label != L_WHADVP && t->nodes[wh].label != L_WDT)
            wh = t->nodes[wh].next_sibling;
        if (wh < 0) continue;
        /* find S under SBAR */
        for (int sc = t->nodes[c].first_child; sc >= 0;
             sc = t->nodes[sc].next_sibling) {
            if (t->nodes[sc].label != L_S) continue;
            int def_np = -1;
            if (has_vp_pattern(t, sc, "is", "", &def_np) ||
                has_vp_pattern(t, sc, "are", "", &def_np)) {
                /* "is" has no prep — look directly for NP under VP */
                for (int vc = t->nodes[sc].first_child; vc >= 0;
                     vc = t->nodes[vc].next_sibling) {
                    if (t->nodes[vc].label != L_VP) continue;
                    for (int vcc = t->nodes[vc].first_child; vcc >= 0;
                         vcc = t->nodes[vcc].next_sibling) {
                        if (t->nodes[vcc].label == L_NP) {
                            int len = collect_np_phrase(t, vcc, cd_idx, buf, bufsz);
                            if (len >= 3) return len;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

/* P12: parenthetical */
static int p12_parenthetical(tree_t *t, int cd_idx, char *buf, int bufsz) {
    /* Find NP containing CD */
    int np = -1;
    for (int n = cd_idx; n >= 0; n = t->nodes[n].parent)
        if (t->nodes[n].label == L_NP) { np = n; break; }
    if (np < 0) return 0;
    /* Check parent for LRB ... RRB containing NP */
    int par = t->nodes[np].parent;
    if (par < 0) return 0;
    for (int c = t->nodes[par].first_child; c >= 0;
         c = t->nodes[c].next_sibling) {
        if (c == np) continue;
        /* look for (NP with LRB/RRB or just an NP sibling) */
        if (t->nodes[c].label == L_NP) {
            int len = collect_np_phrase(t, c, cd_idx, buf, bufsz);
            if (len >= 3) return len;
        }
    }
    return 0;
}

/* P13: such as */
static int p13_such_as(tree_t *t, int cd_idx, char *buf, int bufsz) {
    int s_node = -1;
    for (int n = cd_idx; n >= 0; n = t->nodes[n].parent)
        if (t->nodes[n].label == L_S) { s_node = n; break; }
    if (s_node < 0) return 0;
    /* find PP (JJ such) (IN as) NP */
    for (int c = t->nodes[s_node].first_child; c >= 0;
         c = t->nodes[c].next_sibling) {
        if (t->nodes[c].label != L_PP) {
            /* check inside VP too */
            if (t->nodes[c].label == L_VP) {
                for (int vc = t->nodes[c].first_child; vc >= 0;
                     vc = t->nodes[vc].next_sibling) {
                    if (t->nodes[vc].label != L_PP) continue;
                    /* check JJ such + IN as */
                    int jj = t->nodes[vc].first_child;
                    if (jj < 0 || t->nodes[jj].label != L_JJ) continue;
                    char *jw = n_text(t, t->nodes[jj].first_child);
                    if (!jw || strcmp(jw, "such")) continue;
                    int in = t->nodes[jj].next_sibling;
                    if (in < 0 || t->nodes[in].label != L_IN) continue;
                    char *iw = n_text(t, t->nodes[in].first_child);
                    if (!iw || strcmp(iw, "as")) continue;
                    for (int np = t->nodes[vc].first_child; np >= 0;
                         np = t->nodes[np].next_sibling) {
                        if (t->nodes[np].label == L_NP) {
                            int len = collect_np_phrase(t, np, cd_idx, buf, bufsz);
                            if (len >= 3) return len;
                        }
                    }
                }
            }
            continue;
        }
    }
    return 0;
}

/* P14: title phrase — outer NP with NP-left + NP-right(contains CD) */
static int p14_title_phrase(tree_t *t, int cd_idx, char *buf, int bufsz) {
    /* Walk up NP chain, find deepest NP with article */
    int best = -1;
    for (int n = t->nodes[cd_idx].parent; n >= 0; n = t->nodes[n].parent) {
        if (t->nodes[n].label == L_NP && starts_with_article(t, n))
            { best = n; }
    }
    if (best < 0) return 0;
    int len = collect_np_phrase(t, best, cd_idx, buf, bufsz);
    return (len >= 3) ? len : 0;
}

/* P15: fallback — deepest NP with 3+ words */
static int p15_fallback(tree_t *t, int cd_idx, char *buf, int bufsz) {
    int best = -1, best_len = 0;
    for (int n = t->nodes[cd_idx].parent; n >= 0; n = t->nodes[n].parent) {
        if (t->nodes[n].label != L_NP) continue;
        char tmp[512]; int len = collect_np_phrase(t, n, cd_idx, tmp, sizeof(tmp));
        if (len > best_len) { best = n; best_len = len; }
    }
    if (best < 0) return 0;
    int len = collect_np_phrase(t, best, cd_idx, buf, bufsz);
    return (len >= 3) ? len : 0;
}

/* ---- pattern table ---- */
static pat_fn patterns[] = {
    p1_apposition,
    p2_np_before,
    p3_copula,
    p4_hyphenated,
    p5_pp_nested,
    p6_given_by,
    p7_defined_as,
    p8_denoted_by,
    p9_known_as,
    p10_referred_to,
    p11_relative_clause,
    p12_parenthetical,
    p13_such_as,
    p14_title_phrase,
    p15_fallback,
};
#define N_PATTERNS (int)(sizeof(patterns)/sizeof(patterns[0]))

int match_definition(tree_t *t, int cd_idx, char *buf, int bufsz) {
    buf[0] = '\0';
    for (int p = 0; p < N_PATTERNS; p++) {
        int len = patterns[p](t, cd_idx, buf, bufsz);
        if (len >= 3) return len;
    }
    return 0;
}
