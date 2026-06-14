/*
 * struct_norm.c — Structural normalization for math expressions + author names.
 *
 * Math:  f(x) = x^2  →  $1($2) = $2^2  (abstracts variable names)
 * Author: \author{J. Smith} → j smith (fingerprint, strips LaTeX)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

void fingerprint(const char *in, char *out, int max) {
    int o = 0, space = 0;
    while (*in && o < max - 1) {
        if (isspace((unsigned char)*in)) {
            if (!space && o > 0) { out[o++] = ' '; space = 1; }
        } else if (isalnum((unsigned char)*in) || *in == '-' || *in == '_') {
            out[o++] = (char)tolower((unsigned char)*in); space = 0;
        }
        in++;
    }
    while (o > 0 && out[o-1] == ' ') o--;
    out[o] = '\0';
}

void normalize_author(const char *in, char *out, int max) {
    const char *s = in;
    if (!strncmp(s, "\\author{", 8)) s += 8;
    int len = (int)strlen(s);
    if (len > 0 && s[len-1] == '}') len--;
    char tmp[512]; int tl = len < 511 ? len : 511;
    memcpy(tmp, s, tl); tmp[tl] = '\0';
    fingerprint(tmp, out, max);
}

/*
 * Structural normalize: replace single-letter variables with $1, $2, ...
 * Keeps operators, parens, numbers, LaTeX commands, sub/superscripts.
 * Returns output length (not counting \0).
 */
int struct_normalize_math(const char *in, char *out, int max) {
    char vars[64][32]; int nvars = 0;
    const char *p = in; int o = 0;

#define SAFE(c) do { if (o < max-1) out[o++] = (c); } while(0)

    while (*p && o < max - 1) {
        /* skip math delimiters */
        if (*p == '$') { p++; continue; }

        /* LaTeX command (\frac, \sqrt, etc) — keep verbatim with arg */
        if (*p == '\\' && isalpha((unsigned char)p[1])) {
            while (*p && isalpha((unsigned char)*p)) SAFE(*p++);
            if (*p == '{') {
                SAFE(*p++); int depth = 1;
                while (*p && depth > 0 && o < max - 1) {
                    if (*p == '{') depth++;
                    else if (*p == '}') depth--;
                    if (depth > 0) { SAFE(*p); }
                    p++;
                }
                if (depth == 0) SAFE('}');
            }
            continue;
        }

        /* subscript/superscript: keep marker + content */
        if (*p == '_' || *p == '^') {
            SAFE(*p++);
            if (*p == '{') {
                SAFE(*p++); int depth = 1;
                while (*p && depth > 0 && o < max - 1) {
                    if (*p == '{') depth++;
                    else if (*p == '}') depth--;
                    if (depth > 0) SAFE(*p);
                    p++;
                }
                if (depth == 0) SAFE('}');
            } else if (*p) {
                SAFE(*p++);
            }
            continue;
        }

        /* operators and delimiters: keep */
        if (strchr("()=+-*/<>[],|.", *p)) { SAFE(*p++); continue; }

        /* numbers: keep */
        if (isdigit((unsigned char)*p)) { while (isdigit((unsigned char)*p)) SAFE(*p++); continue; }

        /* single-letter variables: replace with $N */
        if (isalpha((unsigned char)*p)) {
            const char *vs = p; while (isalpha((unsigned char)*p)) p++;
            int vlen = (int)(p - vs);
            if (vlen == 1 && *vs == 'f' && *p == '(') { SAFE('f'); continue; }
            int vi = -1;
            for (int i = 0; i < nvars; i++)
                if ((int)strlen(vars[i]) == vlen && !strncmp(vars[i], vs, vlen)) { vi = i; break; }
            if (vi < 0 && nvars < 64) {
                memcpy(vars[nvars], vs, vlen); vars[nvars][vlen] = '\0'; vi = nvars++;
            }
            if (vi >= 0) o += snprintf(out + o, max - o, "$%d", vi + 1);
            continue;
        }

        /* whitespace: collapse */
        if (isspace((unsigned char)*p)) {
            if (o > 0 && out[o-1] != ' ') SAFE(' ');
            p++; continue;
        }

        /* anything else: copy */
        SAFE(*p++);
    }

    while (o > 0 && out[o-1] == ' ') o--;
    out[o] = '\0';
    return o;
}
