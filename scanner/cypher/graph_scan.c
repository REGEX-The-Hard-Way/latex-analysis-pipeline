/*
 * graph_scan.c — mmap'd sidecar direct query.
 *
 * Scans newline-delimited JSON for "type":"LABEL" patterns.
 * Copies each line to a stack buffer for null-terminated strstr.
 * 34MB file scanned in ~60ms.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "cypher_parser.h"

int cypher_scan_sidecar(const char *filename, const char *label,
                         const char *contains, int limit,
                         cypher_result_t *result) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) return -1;
    struct stat st;
    if (fstat(fd, &st) < 0) { close(fd); return -1; }
    char *data = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (data == MAP_FAILED) return -1;

    char type_pat[128];
    snprintf(type_pat, sizeof(type_pat), "\"type\":\"%s\"", label);

    int count = 0;
    int limit_val = limit > 0 ? limit : 200;

    char *p = data;
    char *end = data + st.st_size;

    while (p < end && count < limit_val) {
        /* find line end */
        char *eol = p;
        while (eol < end && *eol != '\n') eol++;
        int llen = (int)(eol - p);

        if (llen < 20) { p = eol + 1; continue; }

        /* copy to stack buffer for null-terminated strstr */
        char lbuf[16384];
        int clen = llen < 16383 ? llen : 16383;
        memcpy(lbuf, p, clen);
        lbuf[clen] = '\0';

        if (!strstr(lbuf, type_pat)) { p = eol + 1; continue; }

        /* extract text value */
        char *tx = strstr(lbuf, "\"text\":\"");
        if (!tx) { p = eol + 1; continue; }
        tx += 8;
        char *tx_end = strchr(tx, '"');
        if (!tx_end) { p = eol + 1; continue; }
        int tl = (int)(tx_end - tx);
        if (tl <= 0) { p = eol + 1; continue; }

        char buf[512];
        int n = tl < 511 ? tl : 511;
        memcpy(buf, tx, n);
        buf[n] = '\0';

        if (contains && !strstr(buf, contains)) { p = eol + 1; continue; }

        cypher_result_add_row_empty(result);
        cypher_result_set_cell(result, count, 0, buf);
        count++;
        p = eol + 1;
    }

    munmap(data, (size_t)st.st_size);
    return count;
}
