/*
 * multi_analyzer.rl — Chainable Ragel analyzer framework
 *
 * Each analyzer is a Ragel scanner that reads from a buffer and writes
 * tokens to a callback. Analysers can be chained: output of one becomes
 * input of the next.
 *
 * Built-in analyzers:
 *   tokenizer  — split on whitespace, emit words + hash_ids
 *   normalizer — lowercase, strip punctuation (via regex_util)
 *   stemmer    — apply porter2 stemming
 *
 * Usage as library:
 *   analyzer_t *chain = analyzer_chain(3,
 *       analyzer_tokenizer(),
 *       analyzer_normalizer(),
 *       analyzer_stemmer());
 *   analyzer_run(chain, input, len, callback, userdata);
 *
 * Or as pipeline binary:
 *   ./multi_analyzer.out --tokenize --lower --stem < input.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "regex_util.h"


extern int porter2_stem(char *word);

/* ================================================================
 *  Analyzer type
 * ================================================================ */
typedef void (*analyzer_fn)(const char *input, int len,
                            void (*emit)(const char *tok, int len, void *ctx),
                            void *ctx);

typedef struct analyzer {
    analyzer_fn        process;
    struct analyzer   *next;
} analyzer_t;

static analyzer_t *analyzer_chain(analyzer_fn *fns, int n) {
    analyzer_t *head = NULL, *tail = NULL;
    for (int i = 0; i < n; i++) {
        analyzer_t *a = calloc(1, sizeof(*a));
        a->process = fns[i];
        if (!head) head = a;
        if (tail) tail->next = a;
        tail = a;
    }
    return head;
}

/* ================================================================
 *  Emit buffer — accumulates tokens
 * ================================================================ */
typedef struct {
    char  *buf;
    int    len;
    int    cap;
} emit_buf_t;

static void emit_append(const char *tok, int len, void *ctx) {
    emit_buf_t *eb = (emit_buf_t *)ctx;
    if (eb->len + len + 1 > eb->cap) {
        eb->cap = eb->cap ? eb->cap * 2 : 4096;
        if (eb->cap < eb->len + len + 1) eb->cap = eb->len + len + 1;
        eb->buf = realloc(eb->buf, (size_t)eb->cap);
    }
    memcpy(eb->buf + eb->len, tok, (size_t)len);
    eb->len += len;
    eb->buf[eb->len++] = ' ';
}

/* ================================================================
 *  Run chain: feed input through all analyzers sequentially
 * ================================================================ */
void analyzer_run_chain(analyzer_t *chain, const char *input, int len,
                       void (*final_emit)(const char *tok, int len, void *ctx),
                       void *ctx) {
    if (!chain) {
        
        const char *p = input, *end = input + len;
        while (p < end) {
            while (p < end && (*p == ' ' || *p == '\n' || *p == '\t')) p++;
            if (p >= end) break;
            const char *s = p;
            while (p < end && *p != ' ' && *p != '\n' && *p != '\t') p++;
            final_emit(s, (int)(p - s), ctx);
        }
        return;
    }
    
    emit_buf_t eb = {0};
    chain->process(input, len, emit_append, &eb);
    if (eb.len > 0) eb.buf[--eb.len] = '\0';
    
    analyzer_run_chain(chain->next, eb.buf, eb.len, final_emit, ctx);
    free(eb.buf);
}

/* ================================================================
 *  Analyzer implementations (Ragel scanners)
 * ================================================================ */


%%{
  machine ana_tokenizer;
  
  apostrophe = '\'';
  word   = alpha+ ([apostrophe\-] alpha+)*;
  hash   = '  ' digit+ '  ';
  number = digit+ ('.' digit+)?;
  
  main := |*
    hash   => { emit(ts, (int)(te-ts), ctx); };
    word   => { emit(ts, (int)(te-ts), ctx); };
    number => { emit(ts, (int)(te-ts), ctx); };
    any;
  *|;
}%%

%% write data;

static void analyzer_tokenizer_run(const char *in, int len,
                                    void (*emit)(const char*,int,void*), void *ctx) {
    int cs = 0, act = 0;
    const char *p = in, *pe = in + len;
    const char *ts = NULL, *te = NULL;
    const char *eof = pe;
    %% write init;
    %% write exec;
}

analyzer_fn analyzer_tokenizer(void) { return analyzer_tokenizer_run; }


void analyzer_normalizer_run(const char *in, int len,
                             void (*emit)(const char*,int,void*), void *ctx) {
    
    const char *p = in, *end = in + len;
    while (p < end) {
        while (p < end && (*p == ' ' || *p == '\n' || *p == '\t')) p++;
        if (p >= end) break;
        const char *s = p;
        while (p < end && *p != ' ' && *p != '\n' && *p != '\t') p++;
        int tl = (int)(p - s);

        if (tl >= 5 && s[0] == ' ' && s[1] == ' ' && isdigit((unsigned char)s[2])) {
            emit(s, tl, ctx);
            continue;
        }

        char buf[256];
        int bl = 0;
        for (int i = 0; i < tl && bl < 250; i++) {
            char c = (char)tolower((unsigned char)s[i]);
            if (isalnum((unsigned char)c) || c == '-' || c == '\'')
                buf[bl++] = c;
        }
        if (bl > 0) {
            buf[bl] = '\0';
            
            char *cleaned = regex_sub(buf, "^[^a-z]+", "", 1);
            char *cleaned2 = regex_sub(cleaned ? cleaned : buf, "[^a-z]+$", "", 1);
            const char *final = cleaned2 ? cleaned2 : (cleaned ? cleaned : buf);
            if (*final) emit(final, (int)strlen(final), ctx);
            free(cleaned2);
            free(cleaned);
        }
    }
}

analyzer_fn analyzer_normalizer(void) { return analyzer_normalizer_run; }


void analyzer_stemmer_run(const char *in, int len,
                          void (*emit)(const char*,int,void*), void *ctx) {
    const char *p = in, *end = in + len;
    while (p < end) {
        while (p < end && (*p == ' ' || *p == '\n' || *p == '\t')) p++;
        if (p >= end) break;
        const char *s = p;
        while (p < end && *p != ' ' && *p != '\n' && *p != '\t') p++;
        int tl = (int)(p - s);

        if (tl >= 5 && s[0] == ' ' && s[1] == ' ' && isdigit((unsigned char)s[2])) {
            emit(s, tl, ctx);
            continue;
        }

        char buf[256];
        int bl = tl < 255 ? tl : 255;
        memcpy(buf, s, (size_t)bl);
        buf[bl] = '\0';
        porter2_stem(buf);
        int sl = (int)strlen(buf);
        if (sl > 0) emit(buf, sl, ctx);
    }
}

analyzer_fn analyzer_stemmer(void) { return analyzer_stemmer_run; }

/* ================================================================
 *  Public API
 * ================================================================ */

analyzer_t *analyzer_chain_from_flags(int do_tok, int do_norm, int do_stem) {
    analyzer_fn fns[3]; 
    int nf = 0;
    if (do_tok)  fns[nf++] = analyzer_tokenizer();
    if (do_norm) fns[nf++] = analyzer_normalizer();
    if (do_stem) fns[nf++] = analyzer_stemmer();
    return analyzer_chain(fns, nf);
}

void analyzer_free_chain(analyzer_t *chain) {
    while (chain) {
        analyzer_t *next = chain->next;
        free(chain);
        chain = next;
    }
}

void analyzer_run(analyzer_t *chain, const char *input, int len,
                  void (*emit)(const char *tok, int len, void *ctx), void *ctx) {
    analyzer_run_chain(chain, input, len, emit, ctx);
}

analyzer_t *analyzer_create_chain(int do_tokenize, int do_lower, int do_stem) {
    return analyzer_chain_from_flags(do_tokenize, do_lower, do_stem);
}


