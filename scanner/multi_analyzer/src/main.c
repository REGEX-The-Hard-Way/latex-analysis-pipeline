/*
 * main.c - Main entry point for multi_analyzer pipeline binary
 *
 * This reads from stdin and processes text through the chainable analyzers.
 * Usage: ./multi_analyzer.out --tokenize --lower --stem < input.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "multi_analyzer.h"

static void print_tok(const char *tok, int len, void *ctx) {
    (void)ctx;
    printf("%.*s\n", len, tok);
}

int main(int argc, char **argv) {
    int do_tok = 0, do_norm = 0, do_stem = 0;
    
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--tokenize")) do_tok = 1;
        else if (!strcmp(argv[i], "--lower")) do_norm = 1;
        else if (!strcmp(argv[i], "--stem")) do_stem = 1;
    }
    
    /* Default: tokenize if nothing specified */
    if (!do_tok && !do_norm && !do_stem) { 
        do_tok = 1; 
    }
    
    /* Create analyzer chain */
    analyzer_t *chain = analyzer_create_chain(do_tok, do_norm, do_stem);
    
    /* Read all input from stdin */
    size_t cap = 1 << 20;  /* 1MB initial buffer */
    size_t len = 0;
    char *data = malloc(cap);
    if (!data) {
        fprintf(stderr, "Out of memory\n");
        return 1;
    }
    
    int n;
    while ((n = (int)fread(data + len, 1, (int)(cap - len - 1), stdin)) > 0) {
        len += (size_t)n;
        if (len >= cap - 1) {
            cap *= 2;
            char *ndata = realloc(data, cap);
            if (!ndata) {
                fprintf(stderr, "Out of memory\n");
                free(data);
                analyzer_free_chain(chain);
                return 1;
            }
            data = ndata;
        }
    }
    data[len] = '\0';
    
    /* Run the analyzer chain */
    analyzer_run(chain, data, (int)len, print_tok, NULL);
    
    /* Cleanup */
    analyzer_free_chain(chain);
    free(data);
    
    return 0;
}
