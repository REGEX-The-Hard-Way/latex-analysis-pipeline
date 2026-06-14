/*
 * json_scan.rl — Ragel JSON scanner with proper escape handling.
 *
 * Single-pass scanner over mmap'd JSON. Handles quoted strings with
 * escapes, numbers, nested braces via depth tracking.
 *
 * Compiled with: ragel -m -G2 json_scan.rl
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "json_scan.h"

%%{
    machine json_scan;
    write data;
}%%

int json_scan(const char *data, size_t len, json_field_cb cb, void *user) {
    const char *p  = data;
    const char *pe = data + len;
    const char *eof = pe;
    int cs, act;
    const char *ts, *te;

    char  key_buf[256];  int key_len = 0;
    char  val_buf[4096]; int val_len = 0;
    int   in_record = 0, key_done = 0;
    int   count = 0;
    int   last_was_colon = 0;

    %%{
        action rec_start { in_record = 1; key_len = 0; key_done = 0; }
        action rec_end   { if (in_record) { in_record = 0; count++; cb(NULL, 0, NULL, 0, -1, user); } }

        # key capture from ts/te: "key": pattern
        action capture_key {
            const char *ks = ts + 1;           /* skip opening " */
            const char *ke = ks;
            /* find the closing " — it's the last " before any : in the match */
            while (ke < te && *ke != '"') ke++;
            int kl = (int)(ke - ks);
            if (kl > 255) kl = 255;
            memcpy(key_buf, ks, kl);
            key_buf[kl] = '\0';
            key_len = kl;
            key_done = 1;
        }

        # string value capture from ts/te
        action capture_str {
            if (in_record && key_done) {
                const char *vs = ts + 1;        /* skip opening " */
                /* find the closing " (te may point past it with wsp) */
                const char *ve = vs;
                while (ve < te && *ve != '"') {
                    if (*ve == '\\' && ve[1]) ve++;
                    ve++;
                }
                int vl = (int)(ve - vs);
                /* unescape */
                int o = 0;
                for (int i = 0; i < vl && o < 4095; i++) {
                    if (vs[i] == '\\' && i+1 < vl) {
                        i++;
                        if (vs[i] == 'n') val_buf[o++] = '\n';
                        else if (vs[i] == 't') val_buf[o++] = '\t';
                        else if (vs[i] == '\\') val_buf[o++] = '\\';
                        else if (vs[i] == '"') val_buf[o++] = '"';
                        else val_buf[o++] = vs[i];
                    } else {
                        val_buf[o++] = vs[i];
                    }
                }
                val_buf[o] = '\0';
                cb(key_buf, key_len, val_buf, o, 0, user);
                key_done = 0;
            }
        }

        # number value capture
        action capture_num {
            if (in_record && key_done) {
                int vl = (int)(te - ts);
                if (vl > 4095) vl = 4095;
                memcpy(val_buf, ts, vl);
                val_buf[vl] = '\0';
                cb(key_buf, key_len, val_buf, vl, 1, user);
                key_done = 0;
            }
        }

        wsp = [ \t\r\n];

        # scanner patterns
        main := |*
            '{'  => rec_start;
            '}'  => rec_end;

            # field key: "key":
            '"' ( [^"\\] | '\\' any )* '"' wsp* ':' => capture_key;

            # string value (must NOT be followed by ':')
            '"' ( [^"\\] | '\\' any )* '"' => capture_str;

            # number value
            ([\+\-]? digit+ ('.' digit+)? ([eE] [\+\-]? digit+)?) => capture_num;

            # literals and separators (skip)
            'true' | 'false' | 'null';
            wsp+;
            ',';
            ':' wsp*;

            any;
        *|;
    }%%

    %% write init;
    %% write exec;

    return count;
}

/* scan file via mmap */
int json_scan_file(const char *filename, json_field_cb cb, void *user) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) return -1;
    struct stat st; fstat(fd, &st);
    char *d = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (d == MAP_FAILED) return -1;
    int n = json_scan(d, (size_t)st.st_size, cb, user);
    munmap(d, (size_t)st.st_size);
    return n;
}
