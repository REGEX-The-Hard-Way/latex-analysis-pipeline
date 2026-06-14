/*
 * def_extract.h — Rule-based definition extraction for math variables.
 *
 * Replaces spaCy/benepar (several GB) with POSIX regex patterns.
 * Extracts natural-language definitions from TeX source using
 * copula, apposition, denotes, and list-apposition patterns.
 *
 * Usage:
 *   def_result_t results[64];
 *   int n = def_extract(tex_text, symbols, n_symbols, results, 64);
 */
#ifndef DEF_EXTRACT_H
#define DEF_EXTRACT_H

typedef struct {
    const char *symbol;       /* variable name, e.g. "alpha", "m_e" */
    char        definition[256]; /* extracted natural-language definition */
    int         char_start;   /* byte offset in original TeX */
    int         char_end;
} def_result_t;

int def_extract(const char *tex, int tex_len,
                const char **want_symbols, int n_want,
                def_result_t *results, int max_results);

#endif
