#include "json_escape.h"
#include <stdio.h>

int json_escape(char *dst, size_t dst_size, const char *src, size_t src_len) {
    size_t w = 0;
    for (size_t i = 0; i < src_len; i++) {
        unsigned char c = (unsigned char)src[i];
        /* Reserve enough space for the worst case: \u0000 (6 bytes) */
        if (w + 7 > dst_size) return -1;

        switch (c) {
        case '"':  dst[w++] = '\\'; dst[w++] = '"';  break;
        case '\\': dst[w++] = '\\'; dst[w++] = '\\'; break;
        case '\b': dst[w++] = '\\'; dst[w++] = 'b';  break;
        case '\f': dst[w++] = '\\'; dst[w++] = 'f';  break;
        case '\n': dst[w++] = '\\'; dst[w++] = 'n';  break;
        case '\r': dst[w++] = '\\'; dst[w++] = 'r';  break;
        case '\t': dst[w++] = '\\'; dst[w++] = 't';  break;
        default:
            if (c < 0x20) {
                /* control character → \\u00XX */
                w += snprintf(dst + w, dst_size - w, "\\u%04x", c);
            } else {
                dst[w++] = (char)c;
            }
            break;
        }
    }
    dst[w] = '\0';
    return (int)w;
}
