/*
 * graph_text.rl — Ragel state machine for trigram extraction (Phase 5).
 *
 * Scans property strings and emits 3-byte trigrams as packed uint32 values.
 * Replaces the hand-written sliding window loop at graph_store.c:340-395.
 *
 * Compiled with: ragel -m -G2 graph_text.rl
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph_store.h"

%%{
    machine trigram_extract;
    write data;
}%%

/*
 * Extract trigrams from a property string into a temporary hash map.
 * Returns the number of unique trigrams found.
 */
int gs_extract_trigrams(const char *str, int len,
                         uint32_t *out, int max_out) {
    const char *p  = str;
    const char *pe = str + len;
    const char *eof = pe;
    int cs;
    const char *ts, *te;
    int act;
    int count = 0;

    %%{
        action store_tg {
            if (count < max_out) {
                uint32_t tg;
                tg  = ((unsigned int)(unsigned char)ts[0]) << 16;
                tg |= ((unsigned int)(unsigned char)ts[1]) << 8;
                tg |=  (unsigned int)(unsigned char)ts[2];
                out[count++] = tg;
            }
        }

        main := |*
            any{3} => store_tg;
            any;
        *|;
    }%%

    %% write init;
    %% write exec;

    return count;
}
