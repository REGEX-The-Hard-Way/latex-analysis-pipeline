/*
 * vocab_scan.rl — Vocabulary classifier using vocab.rl word list.
 *
 * Scans a token against 2689-line English vocabulary (academic physics/math)
 * and classifies it as: WORD, NAME, TECHNICAL, MULTI_WORD, or UNKNOWN.
 *
 * Categories inferred from patterns in vocab.rl:
 *   - Capitalized single words → NAME (Abbott, Einstein)
 *   - All-lowercase common words → WORD (about, above)
 *   - Technical/math terms → TECHNICAL (abelian, anisotropic)
 *   - Multi-word phrases → MULTI_WORD (boundary conditions)
 *   - Mixed case not all-caps → NAME
 *
 * Compiled with: ragel -m -G2 vocab_scan.rl
 *
 * Usage:
 *   char category[32];
 *   const char *word = "Trachenko";
 *   int found = classify_word(word, strlen(word), category);
 *   // found=1, category="NAME"
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

%%{
    machine vocab_classify;
    include strings "vocab.rl";
    write data;
}%%

typedef enum {
    CAT_WORD = 0,
    CAT_NAME,
    CAT_TECHNICAL,
    CAT_MULTI_WORD,
    CAT_UNKNOWN
} word_category_t;

static const char *cat_names[] = {"WORD", "NAME", "TECHNICAL", "MULTI_WORD", "UNKNOWN"};

/*
 * Classify a single word/token. Returns 1 if found in vocabulary, 0 if unknown.
 * Sets *cat to the category name.
 */
int classify_word(const char *word, int len, const char **cat) {
    if (!word || len <= 0) { *cat = cat_names[CAT_UNKNOWN]; return 0; }
    
    const char *p  = word;
    const char *pe = word + len;
    const char *eof = pe;
    int cs, act;
    const char *ts, *te;
    int found = 0;
    int is_name = 0, is_technical = 0, is_multi = 0, has_space = 0;
    
    /* pre-check: determine category hints from word properties */
    for (int i = 0; i < len; i++) {
        if (word[i] == ' ' || word[i] == '-') has_space = 1;
    }
    if (len > 1 && isupper((unsigned char)word[0]) && !has_space) is_name = 1;
    if (has_space) is_multi = 1;
    
    /* check for technical patterns: words with LaTeX symbols, Greek, etc. */
    for (int i = 0; i < len; i++) {
        if (word[i] == '{' || word[i] == '}' || word[i] == '$'
            || word[i] == '\\' || word[i] == '^' || word[i] == '_') {
            is_technical = 1; break;
        }
        /* Check for Greek letters in text */
        if (i + 4 < len && !strncmp(word + i, "alpha", 5)) is_technical = 1;
        if (i + 4 < len && !strncmp(word + i, "gamma", 5)) is_technical = 1;
    }
    
    %%{
        action matched {
            found = 1;
            fbreak;
        }
        
        main := |*
            word => matched;
            any;
        *|;
    }%%
    
    %% write init;
    %% write exec;
    
    if (found) {
        if (is_technical) *cat = cat_names[CAT_TECHNICAL];
        else if (is_name) *cat = cat_names[CAT_NAME];
        else if (is_multi) *cat = cat_names[CAT_MULTI_WORD];
        else *cat = cat_names[CAT_WORD];
    } else {
        *cat = cat_names[CAT_UNKNOWN];
    }
    
    return found;
}

/*
 * Classify a token and return the category enum directly.
 */
word_category_t classify(const char *word, int len) {
    const char *cat;
    int found = classify_word(word, len, &cat);
    if (!found) return CAT_UNKNOWN;
    if (!strcmp(cat, "WORD")) return CAT_WORD;
    if (!strcmp(cat, "NAME")) return CAT_NAME;
    if (!strcmp(cat, "TECHNICAL")) return CAT_TECHNICAL;
    if (!strcmp(cat, "MULTI_WORD")) return CAT_MULTI_WORD;
    return CAT_UNKNOWN;
}
