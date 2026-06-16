/*
 * def_extract.h — Rule-based definition extraction for math variables.
 *
 * Replaces spaCy/benepar with pure-C POSIX regex + string matching.
 * Extracts natural-language definitions from TeX source.
 *
 * Usage:
 *   def_result_t results[256];
 *   int n = def_extract(tex_text, tex_len, symbols, n_symbols, results, 256);
 */
#ifndef DEF_EXTRACT_H
#define DEF_EXTRACT_H

typedef struct {
    const char *symbol;        /* variable name, e.g. "alpha", "m_e" */
    char        definition[256]; /* extracted natural-language definition */
    int         char_start;    /* byte offset in original TeX */
    int         char_end;
    int         pattern;       /* which pattern matched: 1=copula, 2=appos, 3=eqn */
} def_result_t;

int def_extract(const char *tex, int tex_len,
                const char **want_symbols, int n_want,
                def_result_t *results, int max_results);

#endif
