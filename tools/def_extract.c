/*
 * def_extract.c — Rule-based definition extraction for math variables.
 *
 * Replaces spaCy/benepar with POSIX regex. Five pattern types:
 *   1. Copula:     "VAR is/are/denotes the NP"
 *   2. Apposition: "the NP, VAR"
 *   3. Denotes:    "VAR denotes/represents NP"
 *   4. List:       "NP1 VAR1, NP2 VAR2"
 *   5. Equation:   "VAR = expression"
 *
 * Uses placeholder substitution for $...$ math to simplify matching.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include "def_extract.h"

#define MAX_TEX_LEN  (2 * 1024 * 1024)  /* 2MB */
#define MAX_SENTS    4096
#define MAX_PLACEHOLDERS 1024
#define MAX_RESULTS  256

/* ---- placeholder tracking ---- */
typedef struct {
    int   id;
    char  symbol[64];
    int   orig_start, orig_end;
    char  raw[128];      /* original $...$ text */
} placeholder_t;

static placeholder_t phs[MAX_PLACEHOLDERS];
static int n_phs = 0;
static char processed[MAX_TEX_LEN];
static int processed_len = 0;

/* ---- symbol key extraction (from benepar_qa.py port) ---- */
static int is_relation(const char *body, int len) {
    /* check for =, \approx, \equiv, etc. */
    char tmp[128]; int tl = len < 127 ? len : 127;
    memcpy(tmp, body, tl); tmp[tl] = 0;
    return (strstr(tmp, "=") || strstr(tmp, "\\approx")
         || strstr(tmp, "\\equiv") || strstr(tmp, "\\sim")
         || strstr(tmp, "\\propto") || strstr(tmp, "\\simeq")
         || strstr(tmp, "\\cong"));
}

static int extract_symbol_key(const char *math_body, int len, char *out, int max) {
    /* strip LaTeX markup, return canonical variable name */
    char buf[256];
    int bl = len < 255 ? len : 255;
    memcpy(buf, math_body, bl); buf[bl] = 0;

    /* reject fractions as single variables */
    if (strstr(buf, "\\frac") || strstr(buf, "\\tfrac") || strstr(buf, "\\dfrac"))
        return 0;

    /* strip \rm, \mathrm, \text wrappers */
    char *s = buf;
    char tmp[256]; int ti = 0;
    while (*s && ti < 250) {
        if (!strncmp(s, "\\rm ", 4) || !strncmp(s, "\\rm", 3)) s += 3;
        else if (!strncmp(s, "\\mathrm{", 8)) { s += 8; while (*s && *s != '}') s++; if (*s) s++; }
        else if (!strncmp(s, "\\text{", 6)) { s += 6; while (*s && *s != '}') s++; if (*s) s++; }
        else if (*s == '\\' || *s == '{' || *s == '}') s++;
        else tmp[ti++] = *s++;
    }
    tmp[ti] = 0;

    /* trim */
    char *trimmed = tmp;
    while (*trimmed == ' ') trimmed++;
    int tl = (int)strlen(trimmed);
    while (tl > 0 && trimmed[tl-1] == ' ') tl--;

    if (tl < 1 || tl > 12) return 0;
    /* must start with letter */
    if (!isalpha((unsigned char)trimmed[0])) return 0;
    /* must be alphanumeric or _ or ^ */
    for (int i = 0; i < tl; i++)
        if (!isalnum((unsigned char)trimmed[i]) && trimmed[i] != '_' && trimmed[i] != '^')
            return 0;

    int ol = tl < max - 1 ? tl : max - 2;
    memcpy(out, trimmed, ol); out[ol] = 0;
    return 1;
}

/* ---- TeX preprocessing ---- */
static void preprocess(const char *tex, int tex_len) {
    /* strip \newcommand / \def bodies */
    int o = 0;
    for (int i = 0; i < tex_len && o < MAX_TEX_LEN - 1; i++) {
        if (!strncmp(tex + i, "\\newcommand", 11)
         || !strncmp(tex + i, "\\renewcommand", 13)
         || !strncmp(tex + i, "\\providecommand", 15)
         || !strncmp(tex + i, "\\def", 4)) {
            /* skip to matching } */
            int depth = 0;
            while (i < tex_len) {
                if (tex[i] == '{') depth++;
                else if (tex[i] == '}') { depth--; if (depth <= 0) { i++; break; } }
                i++;
            }
            processed[o++] = ' ';
            continue;
        }
        processed[o++] = tex[i];
    }
    processed[o] = 0;
    processed_len = o;

    /* replace $...$ with placeholders */
    char out[MAX_TEX_LEN];
    int oo = 0;
    n_phs = 0;
    for (int i = 0; i < processed_len && oo < MAX_TEX_LEN - 1; i++) {
        if (processed[i] == '$') {
            int j = i + 1;
            while (j < processed_len && processed[j] != '$') j++;
            if (j >= processed_len) { out[oo++] = processed[i]; continue; }
            int body_len = j - i - 1;
            char symbol[64] = "";
            int is_rel = is_relation(processed + i + 1, body_len);
            extract_symbol_key(processed + i + 1, body_len, symbol, sizeof(symbol));

            if (symbol[0] && !is_rel) {
                /* variable placeholder */
                int id = n_phs;
                snprintf(phs[id].symbol, sizeof(phs[id].symbol), "%s", symbol);
                phs[id].orig_start = i; phs[id].orig_end = j + 1;
                phs[id].id = id;
                memcpy(phs[id].raw, processed + i, j - i + 1); phs[id].raw[j-i+1] = 0;
                oo += snprintf(out + oo, MAX_TEX_LEN - oo, "VARSYM%04d", id);
                n_phs++;
            } else {
                /* expression/equation — keep as is or replace */
                oo += snprintf(out + oo, MAX_TEX_LEN - oo, "MATHVAL");
            }
            i = j;
        } else {
            out[oo++] = processed[i];
        }
    }
    out[oo] = 0;
    memcpy(processed, out, oo + 1);
    processed_len = oo;

    /* strip remaining TeX commands and braces for cleaner matching */
    o = 0;
    for (int i = 0; i < processed_len && o < MAX_TEX_LEN - 1; i++) {
        if (processed[i] == '\\' && isalpha((unsigned char)processed[i+1])) {
            /* skip \command */
            while (i < processed_len && isalpha((unsigned char)processed[i])) i++;
            if (processed[i] == '{') { int d = 1; i++; while (i < processed_len && d > 0) { if (processed[i]=='{') d++; else if (processed[i]=='}') d--; i++; } }
            out[o++] = ' ';
            continue;
        }
        if (processed[i] == '{' || processed[i] == '}') { out[o++] = ' '; continue; }
        out[o++] = processed[i];
    }
    out[o] = 0;
    memcpy(processed, out, o + 1);
    processed_len = o;

    /* collapse whitespace */
    o = 0; int sp = 0;
    for (int i = 0; i < processed_len; i++) {
        if (processed[i] == ' ' || processed[i] == '\t' || processed[i] == '\n') {
            if (!sp && o > 0) { out[o++] = ' '; sp = 1; }
        } else { out[o++] = processed[i]; sp = 0; }
    }
    out[o] = 0;
    memcpy(processed, out, o + 1);
    processed_len = o;
}

/* ---- sentence splitting ---- */
static int sent_starts[MAX_SENTS];
static int sent_ends[MAX_SENTS];
static int n_sents = 0;

static void split_sentences(void) {
    n_sents = 0;
    int start = 0;
    for (int i = 0; i < processed_len && n_sents < MAX_SENTS; i++) {
        if (processed[i] == '.' && (i+1 >= processed_len || processed[i+1] == ' ')) {
            sent_starts[n_sents] = start;
            sent_ends[n_sents] = i + 1;
            n_sents++;
            start = i + 2; /* skip ". " */
        }
    }
    if (start < processed_len - 1) {
        sent_starts[n_sents] = start;
        sent_ends[n_sents] = processed_len;
        n_sents++;
    }
}

/* ---- pattern matching ---- */
static int find_var_in_sent(int sent_idx, int *ph_idx, const char **wanted, int n_want) {
    int s = sent_starts[sent_idx], e = sent_ends[sent_idx];
    /* scan for VARSYMnnnn in sentence */
    char *p = processed + s;
    char *end = processed + e;
    while (p < end) {
        if (!strncmp(p, "VARSYM", 6)) {
            int id = atoi(p + 6);
            if (id >= 0 && id < n_phs) {
                /* check if this symbol is wanted */
                for (int wi = 0; wi < n_want; wi++) {
                    if (!strcmp(phs[id].symbol, wanted[wi])) {
                        *ph_idx = id;
                        return 1;
                    }
                }
            }
        }
        p++;
    }
    return 0;
}

/* ---- definition extraction patterns ---- */

static int match_copula(int sent_idx, int ph_idx, def_result_t *r) {
    /* "VARSYM is/are/denotes/represents the NP" */
    (void)sent_idx; (void)ph_idx; (void)r;
    /* TODO: regex match for copula pattern */
    return 0;
}

static int match_apposition(int sent_idx, int ph_idx, def_result_t *r) {
    /* "the NP, VARSYM" or "a NP, VARSYM" */
    int s = sent_starts[sent_idx], e = sent_ends[sent_idx];
    char sent[4096]; int sl = e - s;
    if (sl > 4095) sl = 4095;
    memcpy(sent, processed + s, sl); sent[sl] = 0;

    char pat[64]; snprintf(pat, sizeof(pat), "VARSYM%04d", ph_idx);
    char *vp = strstr(sent, pat);
    if (!vp) return 0;

    /* look left from VARSYM for a comma + NP */
    char *left = vp;
    while (left > sent && *left != ',') left--;
    if (left == sent || *left != ',') return 0;
    left++; /* after comma, find NP start */

    /* NP must start with determiner */
    while (left < vp && *left == ' ') left++;
    if (!strncmp(left, "the ", 4)) left += 4;
    else if (!strncmp(left, "a ", 2)) left += 2;
    else if (!strncmp(left, "an ", 3)) left += 3;
    else if (!strncmp(left, "its ", 4)) left += 4;
    else if (!strncmp(left, "their ", 6)) left += 6;
    else return 0;

    /* collect NP to the right until VARSYM */
    char np[256]; int nl = 0;
    while (left < vp && nl < 250) {
        if (*left == 'V' && !strncmp(left, "VARSYM", 6)) break;
        np[nl++] = *left++;
    }
    np[nl] = 0;

    /* trim trailing spaces and punctuation */
    while (nl > 0 && (np[nl-1] == ' ' || np[nl-1] == ',')) nl--;
    np[nl] = 0;
    if (nl < 3) return 0;

    snprintf(r->definition, sizeof(r->definition), "%s", np);
    r->symbol = phs[ph_idx].symbol;
    r->char_start = phs[ph_idx].orig_start;
    r->char_end = phs[ph_idx].orig_end;
    return 1;
}

static int match_copula_pattern(int sent_idx, int ph_idx, def_result_t *r) {
    /* "where VARSYM is/are/denotes/represents/stands for (the) NP" */
    int s = sent_starts[sent_idx], e = sent_ends[sent_idx];
    char sent[4096]; int sl = e - s;
    if (sl > 4095) sl = 4095;
    memcpy(sent, processed + s, sl); sent[sl] = 0;

    char pat[64]; snprintf(pat, sizeof(pat), "VARSYM%04d", ph_idx);
    char *vp = strstr(sent, pat);
    if (!vp) return 0;

    /* look right: "is|are|denotes|represents|stands for|gives|means" */
    char *cp = vp + 10; /* skip VARSYMnnnn */
    while (cp < sent + sl && *cp == ' ') cp++;

    const char *copulas[] = {"is ", "are ", "denotes ", "represents ", 
                              "stands for ", "gives ", "means ", "denote ",
                              "represent ", "give ", "mean ", NULL};
    int matched = 0;
    for (int ci = 0; copulas[ci]; ci++) {
        if (!strncmp(cp, copulas[ci], strlen(copulas[ci]))) {
            cp += strlen(copulas[ci]);
            matched = 1; break;
        }
    }
    if (!matched) return 0;

    /* skip optional "the" or "a" */
    if (!strncmp(cp, "the ", 4)) cp += 4;
    else if (!strncmp(cp, "a ", 2)) cp += 2;
    else if (!strncmp(cp, "an ", 3)) cp += 3;

    /* collect NP until end of sentence or next VARSYM or punctuation */
    char np[256]; int nl = 0;
    while (cp < sent + sl && nl < 250) {
        if (!strncmp(cp, "VARSYM", 6)) break;
        if (!strncmp(cp, "MATHVAL", 7)) break;
        if (*cp == '.' || *cp == ';') break;
        /* skip parenthetical */
        if (*cp == '(') { while (cp < sent + sl && *cp != ')') cp++; if (*cp == ')') cp++; continue; }
        np[nl++] = *cp++;
    }
    np[nl] = 0;

    /* trim */
    while (nl > 0 && (np[nl-1] == ' ' || np[nl-1] == ',' || np[nl-1] == '.')) nl--;
    np[nl] = 0;
    if (nl < 3) return 0;

    /* reject self-referential (definition contains the symbol itself) */
    if (strstr(np, phs[ph_idx].symbol)) return 0;

    snprintf(r->definition, sizeof(r->definition), "%s", np);
    r->symbol = phs[ph_idx].symbol;
    r->char_start = phs[ph_idx].orig_start;
    r->char_end = phs[ph_idx].orig_end;
    return 1;
}

static int match_equation_def(int sent_idx, int ph_idx, def_result_t *r) {
    /* "VARSYM = EXPRESSION" — equation as definition */
    int s = sent_starts[sent_idx], e = sent_ends[sent_idx];
    char sent[4096]; int sl = e - s;
    if (sl > 4095) sl = 4095;
    memcpy(sent, processed + s, sl); sent[sl] = 0;

    char pat[64]; snprintf(pat, sizeof(pat), "VARSYM%04d", ph_idx);
    char *vp = strstr(sent, pat);
    if (!vp) return 0;

    /* look right for "MATHVAL" (equation placeholder) */
    char *eq = vp + 10;
    while (eq < sent + sl && *eq != 'M') eq++;
    if (!strncmp(eq, "MATHVAL", 7)) {
        snprintf(r->definition, sizeof(r->definition), "%s", phs[ph_idx].raw);
        r->symbol = phs[ph_idx].symbol;
        r->char_start = phs[ph_idx].orig_start;
        r->char_end = phs[ph_idx].orig_end;
        return 1;
    }
    return 0;
}

/* ---- main extraction function ---- */
int def_extract(const char *tex, int tex_len,
                const char **want_symbols, int n_want,
                def_result_t *results, int max_results) {
    if (!tex || tex_len <= 0) return 0;

    preprocess(tex, tex_len);
    split_sentences();

    int n_results = 0;

    for (int si = 0; si < n_sents && n_results < max_results; si++) {
        /* find all wanted symbols in this sentence */
        for (int wi = 0; wi < n_want; wi++) {
            int phi = -1;
            /* find placeholder for this symbol */
            for (int pi = 0; pi < n_phs; pi++) {
                if (!strcmp(phs[pi].symbol, want_symbols[wi])) {
                    /* check if this placeholder appears in the sentence */
                    char pat[64]; snprintf(pat, sizeof(pat), "VARSYM%04d", pi);
                    if (strstr(processed + sent_starts[si], pat)) {
                        phi = pi; break;
                    }
                }
            }
            if (phi < 0) continue;

            /* try patterns in priority order */
            if (match_copula_pattern(si, phi, &results[n_results])
             || match_apposition(si, phi, &results[n_results])
             || match_equation_def(si, phi, &results[n_results])) {
                n_results++;
            }
        }
    }

    return n_results;
}
