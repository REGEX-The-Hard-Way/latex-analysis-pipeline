/*
 * graph_scan.rl — Ragel state machine for direct mmap'd JSON queries.
 *
 * Scans "type":"LABEL" patterns in newline-delimited JSON using a
 * single-pass Ragel scanner. Eliminates strstr() passes and stack
 * buffer copies per line.
 *
 * Compiled with: ragel -m -G2 graph_scan.rl
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "cypher_parser.h"

%%{
    machine json_line_scan;
    write data;
}%%

int cypher_scan_sidecar(const char *filename, const char *label,
                         const char *contains, int limit,
                         cypher_result_t *result) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) return -1;
    struct stat st;
    if (fstat(fd, &st) < 0) { close(fd); return -1; }
    const char *p  = (const char *)mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (p == MAP_FAILED) return -1;
    const char *pe = p + st.st_size;
    const char *eof = pe;

    int cs, act;
    const char *ts, *te;
    int count = 0;
    int limit_val = limit > 0 ? limit : MAX_ROWS;
    int type_match = 0;
    char text_buf[2048];

    %%{
        action start_rec  { type_match = 0; }
        action found_type {
            const char *vs = ts + 8; /* skip "type":" */
            int vl = (int)(te - vs - 1);
            type_match = (vl == (int)strlen(label)
                       && !strncmp(vs, label, vl));
        }
        action found_text {
            if (type_match && count < limit_val) {
                const char *vs = ts + 8; /* skip "text":" */
                int vl = (int)(te - vs - 1);
                /* unescape and capture */
                int ti = 0;
                for (int i = 0; i < vl && ti < 2047; i++) {
                    if (vs[i] == '\\' && i+1 < vl) {
                        i++;
                        if (vs[i] == 'n') text_buf[ti++] = '\n';
                        else if (vs[i] == 't') text_buf[ti++] = '\t';
                        else if (vs[i] == '\\') text_buf[ti++] = '\\';
                        else text_buf[ti++] = vs[i];
                    } else {
                        text_buf[ti++] = vs[i];
                    }
                }
                text_buf[ti] = '\0';
                if (!contains || strstr(text_buf, contains)) {
                    cypher_result_add_row_empty(result);
                    cypher_result_set_cell(result, count, 0, text_buf);
                    count++;
                }
                type_match = 0;
            }
        }

        # Scanner patterns: match JSON fields on each line
        main := |*
            '{'  => start_rec;
            '"type":"' [^"]+ '"' => found_type;
            '"text":"' ([^"\\] | '\\' any)* '"' => found_text;
            any;
        *|;
    }%%

    %% write init;
    %% write exec;

    munmap((void *)p, (size_t)st.st_size);
    return count;
}
