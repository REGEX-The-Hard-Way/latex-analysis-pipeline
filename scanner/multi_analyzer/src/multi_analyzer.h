/**
 * multi_analyzer.h - Chainable Ragel analyzer framework
 *
 * Public API for the chainable text processing pipeline.
 */

#ifndef MULTI_ANALYZER_H
#define MULTI_ANALYZER_H

#include <stddef.h>

/* Analyzer function type */
typedef void (*analyzer_fn)(const char *input, int len,
                            void (*emit)(const char *tok, int len, void *ctx),
                            void *ctx);

/* Analyzer chain node */
typedef struct analyzer {
    analyzer_fn process;
    struct analyzer *next;
} analyzer_t;

/**
 * Create a chain of analyzers based on flags
 * @param do_tokenize Enable tokenizer
 * @param do_lower Enable normalizer (lowercase + strip punctuation)
 * @param do_stem Enable stemmer (Porter2)
 * @return Pointer to the head of the analyzer chain
 */
analyzer_t *analyzer_create_chain(int do_tokenize, int do_lower, int do_stem);

/**
 * Free an analyzer chain
 * @param chain The chain to free
 */
void analyzer_free_chain(analyzer_t *chain);

/**
 * Run text through an analyzer chain
 * @param chain The analyzer chain
 * @param input The input text
 * @param len Length of input text
 * @param emit Callback function for each token
 * @param ctx User context passed to emit callback
 */
void analyzer_run(analyzer_t *chain, const char *input, int len,
                  void (*emit)(const char *tok, int len, void *ctx), void *ctx);

/**
 * Tokenizer analyzer function
 */
void analyzer_tokenizer_run(const char *in, int len,
                           void (*emit)(const char*, int, void*), void *ctx);

#endif /* MULTI_ANALYZER_H */
