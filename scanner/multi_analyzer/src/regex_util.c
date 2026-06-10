#include "regex_util.h"

#include <regex.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------
/* Internal: a small auto-growing byte buffer for building strings.   
/* ------------------------------------------------------------------ */

typedef struct {
    char  *data;
    size_t len;
    size_t cap;
    int    oom;   /* sticky out-of-memory flag */
} buf_t;

static void buf_init(buf_t *b)
{
    b->data = NULL;
    b->len  = 0;
    b->cap  = 0;
    b->oom  = 0;
}

static int buf_reserve(buf_t *b, size_t extra)
{
    if (b->oom) return 0;

    /* Need room for `extra` bytes plus a terminating NUL. */
    if (extra > (size_t)-1 - b->len - 1) { b->oom = 1; return 0; }
    size_t need = b->len + extra + 1;
    if (need <= b->cap) return 1;

    size_t ncap = b->cap ? b->cap : 32;
    while (ncap < need) {
        if (ncap > ((size_t)-1) / 2) { b->oom = 1; return 0; }
        ncap *= 2;
    }
    char *nd = (char *)realloc(b->data, ncap);
    if (!nd) { b->oom = 1; return 0; }
    b->data = nd;
    b->cap  = ncap;
    return 1;
}

static int buf_append(buf_t *b, const char *src, size_t n)
{
    if (n == 0) return !b->oom;
    if (!buf_reserve(b, n)) return 0;
    memcpy(b->data + b->len, src, n);
    b->len += n;
    b->data[b->len] = '\0';
    return 1;
}

static char *buf_finish(buf_t *b)
{
    if (b->oom) {
        free(b->data);
        return NULL;
    }
    if (!b->data) {
        /* Caller expects a NUL-terminated string even when nothing was
         * appended. */
        b->data = (char *)malloc(1);
        if (!b->data) return NULL;
        b->data[0] = '\0';
    }
    return b->data;
}

/* ------------------------------------------------------------------
/* regex_sub                                                         
/* ------------------------------------------------------------------ */

char *regex_sub(const char *input,
                const char *pattern,
                const char *replacement,
                int global)
{
    if (!input || !pattern || !replacement) return NULL;

    regex_t re;
    if (regcomp(&re, pattern, REG_EXTENDED) != 0) return NULL;

    buf_t out;
    buf_init(&out);

    const char *cursor   = input;
    size_t      rep_len  = strlen(replacement);
    int         done     = 0;

    while (!done) {
        regmatch_t m;
        int eflags = (cursor != input) ? REG_NOTBOL : 0;
        int rc = regexec(&re, cursor, 1, &m, eflags);
        if (rc != 0) break; /* no further matches */

        /* Append text before the match. */
        if (m.rm_so > 0) {
            if (!buf_append(&out, cursor, (size_t)m.rm_so)) goto fail;
        }

        /* Append the literal replacement. */
        if (!buf_append(&out, replacement, rep_len)) goto fail;

        if (m.rm_eo == m.rm_so) {
            /* Zero-width match: copy one byte (if any) to make progress
             * and avoid an infinite loop. */
            if (cursor[m.rm_eo] == '\0') {
                cursor += m.rm_eo;
                done = 1;
            } else {
                if (!buf_append(&out, cursor + m.rm_eo, 1)) goto fail;
                cursor += m.rm_eo + 1;
            }
        } else {
            cursor += m.rm_eo;
        }

        if (!global) {
            /* Append the rest of the string and stop. */
            if (!buf_append(&out, cursor, strlen(cursor))) goto fail;
            done = 1;
        }
    }

    if (!done) {
        /* Fell out of the loop without a "non-global" early exit:
         * append whatever remains. */
        if (!buf_append(&out, cursor, strlen(cursor))) goto fail;
    }

    regfree(&re);
    return buf_finish(&out);

fail:
    regfree(&re);
    free(out.data);
    return NULL;
}

/* ------------------------------------------------------------------
/* regex_split                                                       
/* ------------------------------------------------------------------ */

static int tokens_push(char ***arr, size_t *count, size_t *cap,
                       const char *src, size_t n)
{
    if (*count + 1 >= *cap) {
        size_t ncap = *cap ? *cap * 2 : 8;
        if (ncap <= *count + 1) return 0;
        char **na = (char **)realloc(*arr, ncap * sizeof(char *));
        if (!na) return 0;
        *arr = na;
        *cap = ncap;
    }
    char *s = (char *)malloc(n + 1);
    if (!s) return 0;
    if (n) memcpy(s, src, n);
    s[n] = '\0';
    (*arr)[(*count)++] = s;
    (*arr)[*count] = NULL; /* keep NULL-terminated as we grow */
    return 1;
}

void regex_free_split(char **tokens)
{
    if (!tokens) return;
    for (size_t i = 0; tokens[i] != NULL; ++i) free(tokens[i]);
    free(tokens);
}

char **regex_split(const char *input, const char *pattern, size_t *count_out)
{
    if (count_out) *count_out = 0;
    if (!input || !pattern) return NULL;

    regex_t re;
    if (regcomp(&re, pattern, REG_EXTENDED) != 0) return NULL;

    char **arr   = NULL;
    size_t count = 0;
    size_t cap   = 0;

    const char *cursor = input;

    for (;;) {
        regmatch_t m;
        int eflags = (cursor != input) ? REG_NOTBOL : 0;
        int rc = regexec(&re, cursor, 1, &m, eflags);
        if (rc != 0) break;

        if (m.rm_eo == m.rm_so) {
            /* Zero-width match: advance one byte to avoid infinite loop;
             * do not split here. */
            if (cursor[m.rm_eo] == '\0') break;
            /* Just move forward; we still want this byte in the current
             * token, so we cannot split at a zero-width position. */
            /* Re-anchor by trying again from cursor + 1. */
            /* To do that cleanly, pretend match end is one past. */
            /* For simplicity, just bump cursor and continue. */
            /* Note: this means split treats zero-width matches as "no
             * match" for splitting purposes, which is a sensible default. */
            cursor += 1;
            continue;
        }

        size_t seg_len = (size_t)m.rm_so;
        if (!tokens_push(&arr, &count, &cap, cursor, seg_len)) goto fail;
        cursor += m.rm_eo;
    }

    /* Final tail (always present, may be empty). */
    if (!tokens_push(&arr, &count, &cap, cursor, strlen(cursor))) goto fail;

    regfree(&re);
    if (count_out) *count_out = count;
    return arr;

fail:
    regfree(&re);
    if (arr) {
        for (size_t i = 0; i < count; ++i) free(arr[i]);
        free(arr);
    }
    if (count_out) *count_out = 0;
    return NULL;
}

/* ------------------------------------------------------------------
/* regex_find                                                        
/* ------------------------------------------------------------------ */

int regex_find(const char *input,
               const char *pattern,
               size_t *start_out,
               size_t *end_out)
{
    if (!input || !pattern) return -1;

    regex_t re;
    if (regcomp(&re, pattern, REG_EXTENDED) != 0) return -1;

    regmatch_t m;
    int rc = regexec(&re, input, 1, &m, 0);
    regfree(&re);

    if (rc == REG_NOMATCH) return 0;
    if (rc != 0) return -1;

    if (start_out) *start_out = (size_t)m.rm_so;
    if (end_out)   *end_out   = (size_t)m.rm_eo;
    return 1;
}
