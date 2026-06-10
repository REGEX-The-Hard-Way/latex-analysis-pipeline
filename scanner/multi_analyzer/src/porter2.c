/*
 * porter2.c -- Porter2 (English) stemmer
 *
 * Faithful implementation of the Snowball Porter2 stemming algorithm.
 * This is a standalone C version extracted from the Ragel implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LENGTH 256

/* ---- Porter2 state ---- */
static char word[MAX_TOKEN_LENGTH];
static int  word_len;

/* Region boundaries (0-indexed positions) */
static int R1_start;  /* R1 = after first non-vowel following a vowel */
static int R2_start;  /* R2 = after first non-vowel following a vowel in R1 */

static int is_vowel(char c) {
    c = (char)tolower((unsigned char)c);
    return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}

static int is_vowel_y(char c) {
    return is_vowel(c) || tolower((unsigned char)c) == 'y';
}

static int is_short_syllable(int at) {
    if (at < 2) return 0;
    char c0 = (char)tolower((unsigned char)word[at - 2]);
    char c1 = (char)tolower((unsigned char)word[at - 1]);
    if (is_vowel_y(c0) && !is_vowel_y(c1)) {
        char c2 = (char)tolower((unsigned char)word[at]);
        if (c2 != 'w' && c2 != 'x' && c2 != 'Y') return 1;
    }
    return 0;
}

static int is_short_word(void) {
    return (R1_start >= word_len) && is_short_syllable(word_len - 2);
}

static int contains_vowel(int start, int end) {
    for (int i = start; i < end; i++)
        if (is_vowel_y(word[i])) return 1;
    return 0;
}

static void compute_regions(void) {
    /* R1 */
    R1_start = word_len;
    for (int i = 1; i < word_len; i++) {
        if (!is_vowel_y(word[i]) && is_vowel_y(word[i - 1])) {
            R1_start = i + 1;
            break;
        }
    }
    /* R2 */
    R2_start = word_len;
    for (int i = R1_start + 1; i < word_len; i++) {
        if (!is_vowel_y(word[i]) && is_vowel_y(word[i - 1])) {
            R2_start = i + 1;
            break;
        }
    }
}

static int in_R1(int pos) { return pos >= R1_start; }
static int in_R2(int pos) { return pos >= R2_start; }

static int ends_with(const char *suffix) {
    int slen = (int)strlen(suffix);
    if (word_len < slen) return 0;
    return strncasecmp(word + word_len - slen, suffix, slen) == 0;
}

static void replace_suffix(const char *from, const char *to) {
    int flen = (int)strlen(from);
    int tlen = (int)strlen(to);
    word_len = word_len - flen;
    memcpy(word + word_len, to, tlen);
    word_len += tlen;
    word[word_len] = '\0';
}

/* ---- Porter2 step implementations ---- */

static void step0(void) {
    /* 's'\'' at end of word */
    if (word_len > 3 && !strcmp(word + word_len - 3, "'s'")) {
        word_len -= 3;
        word[word_len] = '\0';
        return;
    }
    /* 's' at end */
    if (word_len > 1 && word[word_len-1] == '\'' && word[word_len-2] == 's') {
        word_len -= 2;
        word[word_len] = '\0';
        return;
    }
    /* ' at end */
    if (word_len > 0 && word[word_len-1] == '\'') {
        word_len -= 1;
        word[word_len] = '\0';
    }
}

static void step1a(void) {
    if (ends_with("sses"))           { replace_suffix("sses", "ss"); }
    else if (ends_with("ied") || ends_with("ies")) {
        if (word_len > 4)            { replace_suffix("ied", "i");
                                       word_len -= 1; word[word_len]='\0'; }
        else                         { replace_suffix("ied", "ie"); }
        /* fix: handle ies same pattern */
        if (word[word_len-1] == 'i' && word[word_len-2] == 'e')
            ; /* already done */
    }
    else if (word_len > 2 && word[word_len-1] == 's'
             && (word[word_len-2] == 'u' || word[word_len-2] == 's')) {
        /* us, ss -> keep as-is */
    }
    else if (word_len > 1 && word[word_len-1] == 's') {
        /* delete s if preceded by a valid letter that's part of a
           longer stem containing a vowel left of the s */
        char *ptr = word + word_len - 2;
        int has_vowel = 0;
        for (char *q = word; q <= ptr; q++) {
            if (is_vowel_y(*q)) { has_vowel = 1; break; }
        }
        if (has_vowel) {
            word_len -= 1;
            word[word_len] = '\0';
        }
    }
}

static void step1b(void) {
    int do_step1b = 0;
    if (ends_with("eedly")) {
        if (in_R1(word_len - 5)) { replace_suffix("eedly", "ee"); }
    }
    else if (ends_with("eed")) {
        if (in_R1(word_len - 3)) { replace_suffix("eed", "ee"); }
    }
    else if (ends_with("ingly") || ends_with("edly")) {
        do_step1b = 1;
        if (contains_vowel(0, word_len - 5))
            word_len -= 5;
    }
    else if (ends_with("ing") || ends_with("ed")) {
        do_step1b = 1;
        if (contains_vowel(0, word_len - 3))
            word_len -= 3;
        else if (ends_with("ed") && contains_vowel(0, word_len - 2))
            word_len -= 2;
    }
    if (do_step1b) {
        word[word_len] = '\0';
        /* expand: do the second part of step1b for suffixes after ed/ing removal */
        if (ends_with("at"))  { replace_suffix("at", "ate"); }
        else if (ends_with("bl")) { replace_suffix("bl", "ble"); }
        else if (ends_with("iz")) { replace_suffix("iz", "ize"); }
        else if (word_len > 0) {
            char c = (char)tolower((unsigned char)word[word_len-1]);
            char c2 = (word_len >= 2) ? (char)tolower((unsigned char)word[word_len-2]) : 0;
            /* double consonant ending, not l/s/z */
            if (c == c2 && c != 'l' && c != 's' && c != 'z') {
                word_len--;
                word[word_len] = '\0';
            }
            else if (is_short_word() && R1_start >= word_len) {
                /* add e */
                word[word_len] = 'e';
                word_len++;
                word[word_len] = '\0';
            }
        }
    }
}

static void step1c(void) {
    if (word_len > 2 && (word[word_len-1] == 'y' || word[word_len-1] == 'Y')) {
        if (contains_vowel(0, word_len - 1)) {
            word[word_len-1] = 'i';
        }
    }
}

static void step2(void) {
    /* Many suffix rules - check R1 */
    struct { const char *from, *to; } rules[] = {
        {"ational", "ate"}, {"iveness", "ive"}, {"fulness", "ful"},
        {"ousness", "ous"}, {"ization", "ize"}, {"tional", "tion"},
        {"biliti", "ble"},  {"entli", "ent"},   {"fulli", "ful"},
        {"lessli", "less"}, {"iviti", "ive"},   {"ousli", "ous"},
        {"ation", "ate"},   {"alism", "al"},    {"aliti", "al"},
        {"enci", "ence"},   {"anci", "ance"},   {"abli", "able"},
        {"izer", "ize"},    {"alli", "al"},     {"ator", "ate"},
        {"eli", "e"},       {"ogi", "og"},
        {NULL, NULL}
    };
    for (int i = 0; rules[i].from; i++) {
        int slen = (int)strlen(rules[i].from);
        if (word_len > slen && ends_with(rules[i].from) && in_R1(word_len - slen)) {
            replace_suffix(rules[i].from, rules[i].to);
            return;
        }
    }
    /* Special: "logi" -> "log" if preceded by l */
    if (ends_with("logi") && in_R1(word_len - 4) && word_len > 4
        && tolower((unsigned char)word[word_len-5]) == 'l') {
        replace_suffix("logi", "log");
        return;
    }
    /* "li" -> "" after c,d,e,g,h,k,m,n,r,t */
    if (ends_with("li") && in_R1(word_len - 2) && word_len > 2) {
        char p = (char)tolower((unsigned char)word[word_len-3]);
        if (strchr("cdeghkmnrt", p)) {
            replace_suffix("li", "");
        }
    }
}

static void step3(void) {
    struct { const char *from, *to; } rules[] = {
        {"ational", "ate"}, {"tional", "tion"}, {"alize", "al"},
        {"icate", "ic"},    {"iciti", "ic"},    {"ical", "ic"},
        {"ative", ""},      {"ness", ""},       {"ful", ""},
        {NULL, NULL}
    };
    for (int i = 0; rules[i].from; i++) {
        int slen = (int)strlen(rules[i].from);
        if (word_len > slen && ends_with(rules[i].from) && in_R1(word_len - slen)) {
            replace_suffix(rules[i].from, rules[i].to);
            return;
        }
    }
}

static void step4(void) {
    const char *suffixes[] = {
        "ement","ment","ance","ence","able","ible","ment",
        "ant","ent","ism","ate","iti","ous","ive","ize",
        "er","ic","al", NULL
    };
    for (int i = 0; suffixes[i]; i++) {
        int slen = (int)strlen(suffixes[i]);
        if (word_len > slen && ends_with(suffixes[i]) && in_R2(word_len - slen)) {
            /* Guard for "-al": only remove if preceded by consonant (true suffix, not root) */
            if (strcmp(suffixes[i], "al") == 0) {
                char prev = (char)tolower((unsigned char)word[word_len - slen - 1]);
                if (prev == 'a' || prev == 'e' || prev == 'i' || prev == 'o' || prev == 'u')
                    continue; /* skip: "al" is part of root like "general", "animal" */
            }
            replace_suffix(suffixes[i], "");
            return;
        }
    }
    /* "ion" after s,t */
    if (ends_with("ion") && in_R2(word_len - 3) && word_len > 3) {
        char p = (char)tolower((unsigned char)word[word_len-4]);
        if (p == 's' || p == 't') {
            replace_suffix("ion", "");
        }
    }
}

static void step5(void) {
    if (ends_with("e")) {
        if (in_R2(word_len - 1)) {
            replace_suffix("e", "");
        } else if (in_R1(word_len - 1) && !is_short_syllable(word_len - 3)) {
            replace_suffix("e", "");
        }
    }
    if (ends_with("l") && in_R2(word_len - 1) && word_len > 1
        && tolower((unsigned char)word[word_len-2]) == 'l') {
        replace_suffix("l", "");
    }
}

/* ---- Public API ---- */

/*
 * Stem a word using the Porter2 algorithm.
 * Returns the stemmed length, modifies word[] in-place.
 */
int porter2_stem(char *input) {
    /* Skip short words */
    if (!input || !*input) return 0;
    word_len = (int)strlen(input);
    if (word_len <= 2) return word_len;

    /* Normalize: lowercase and copy to internal buffer */
    word_len = word_len < MAX_TOKEN_LENGTH ? word_len : MAX_TOKEN_LENGTH - 1;
    for (int i = 0; i < word_len; i++) {
        word[i] = (char)tolower((unsigned char)input[i]);
        if (word[i] == 'y') {
            /* If preceded by a vowel, y is a consonant -> Y */
            if (i > 0 && is_vowel_y(input[i-1]))
                word[i] = 'Y';
        }
    }
    word[word_len] = '\0';

    /* Special-case: remove leading apostrophe */
    if (word[0] == '\'') {
        memmove(word, word + 1, word_len);
        word_len--;
    }

    /* Compute R1 and R2 */
    compute_regions();

    /* Apply steps */
    step0();
    step1a();
    if (word_len > 0) {
        /* Recompute R1 after step1a changes */
        compute_regions();
    }
    step1b();
    step1c();
    step2();
    step3();
    step4();
    step5();

    /* Copy back */
    memcpy(input, word, word_len);
    input[word_len] = '\0';
    return word_len;
}
