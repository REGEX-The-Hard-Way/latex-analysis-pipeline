/*
 * vocab_classify.c — Lightweight English word classifier from vocab.rl.
 *
 * Uses binary search over 13,439-word sorted vocabulary array.
 * Classifies tokens as: WORD, NAME, TECHNICAL, MULTI_WORD, or UNKNOWN.
 *
 * Categories inferred heuristically:
 *   - Found in vocab, starts uppercase → NAME
 *   - Found in vocab, contains space/hyphen → MULTI_WORD
 *   - Found in vocab, contains LaTeX/technical markers → TECHNICAL
 *   - Found in vocab, otherwise → WORD
 *   - Not found → UNKNOWN
 *
 * Usage:
 *   const char *cat;
 *   int found = vocab_classify("Trachenko", strlen("Trachenko"), &cat);
 *   // found=1, cat="NAME"
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "/tmp/vocab_words.h"

static int vocab_cmp(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

int vocab_classify(const char *word, int len, const char **cat) {
    if (!word || len <= 0) { *cat = "UNKNOWN"; return 0; }
    
    /* create null-terminated copy for bsearch */
    char buf[256];
    int bl = len < 255 ? len : 255;
    memcpy(buf, word, bl); buf[bl] = '\0';
    
    const char *key = buf;
    const char **found = (const char **)bsearch(&key, vocab_words,
        VOCAB_COUNT, sizeof(char *), vocab_cmp);
    
    if (!found) { *cat = "UNKNOWN"; return 0; }
    
    /* heuristic classification */
    int has_upper = 0, has_space = 0, has_tech = 0;
    for (int i = 0; i < len; i++) {
        if (isupper((unsigned char)word[i]) && i > 0) has_upper = 1;
        if (word[i] == ' ' || word[i] == '-') has_space = 1;
        if (word[i] == '{' || word[i] == '}' || word[i] == '\\'
            || word[i] == '^' || word[i] == '_') has_tech = 1;
    }
    if (len > 0 && isupper((unsigned char)word[0]) && !has_upper) has_upper = 1;
    
    if (has_tech) *cat = "TECHNICAL";
    else if (has_upper && !has_space) *cat = "NAME";
    else if (has_space) *cat = "MULTI_WORD";
    else *cat = "WORD";
    
    return 1;
}

/* Quick test */
#ifdef TEST
int main() {
    const char *tests[] = {"about","Trachenko","Einstein","abelian",
        "boundary conditions","Introduction","Hamiltonian",
        "supersymmetry","quantum","x","unknownword","electron","mass",NULL};
    for (const char **t = tests; *t; t++) {
        const char *c; int f = vocab_classify(*t, strlen(*t), &c);
        printf("  %-25s → %-10s (%s)\n", *t, c, f ? "found" : "unknown");
    }
    return 0;
}
#endif
