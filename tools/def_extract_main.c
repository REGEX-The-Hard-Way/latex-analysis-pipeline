/*
 * def_extract_main.c — Standalone driver for def_extract library.
 *
 * Reads a .tex file, auto-discovers inline math variable symbols ($...$),
 * runs definition extraction, and prints results.
 *
 * Usage: ./def_extract file.tex [--vars alpha,beta,gamma]
 *   If --vars not given, auto-discovers all $...$ symbols in the file.
 *
 * Build: gcc -O2 -I. -o def_extract def_extract_main.c def_extract.c
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "def_extract.h"

#define MAX_TEX_LEN (2 * 1024 * 1024)

/* extract symbol key from $...$ body (ported from benepar_qa.py) */
static int symbol_key(const char *body, int len, char *out, int max) {
    char buf[256]; int bl = len < 255 ? len : 255;
    memcpy(buf, body, bl); buf[bl] = 0;

    /* reject fractions */
    if (strstr(buf, "\\frac") || strstr(buf, "\\tfrac") || strstr(buf, "\\dfrac"))
        return 0;
    if (strstr(buf, "=") || strstr(buf, "\\approx") || strstr(buf, "\\equiv")
        || strstr(buf, "\\sim") || strstr(buf, "\\propto"))
        return 0;

    /* strip LaTeX markup */
    char tmp[256]; int ti = 0; char *s = buf;
    while (*s && ti < 250) {
        if (!strncmp(s, "\\rm ", 4) || !strncmp(s, "\\rm", 3)) s += 3;
        else if (!strncmp(s, "\\mathrm{", 8)) { s += 8; while (*s && *s != '}') s++; if (*s)s++; }
        else if (!strncmp(s, "\\text{", 6)) { s += 6; while (*s && *s != '}') s++; if (*s)s++; }
        else if (!strncmp(s, "\\mathbf{", 8)) { s += 8; while (*s && *s != '}') s++; if (*s)s++; }
        else if (!strncmp(s, "\\mathcal{", 9)) { s += 9; while (*s && *s != '}') s++; if (*s)s++; }
        else if (*s == '\\' || *s == '{' || *s == '}') s++;
        else tmp[ti++] = *s++;
    }
    tmp[ti] = 0;

    /* trim */
    char *trimmed = tmp;
    while (*trimmed == ' ') trimmed++;
    int tl = (int)strlen(trimmed);
    while (tl > 0 && trimmed[tl-1] == ' ') tl--;
    trimmed[tl] = 0;

    if (tl < 1 || tl > 12) return 0;
    if (!isalpha((unsigned char)trimmed[0])) return 0;
    for (int i = 0; i < tl; i++)
        if (!isalnum((unsigned char)trimmed[i]) && trimmed[i] != '_' && trimmed[i] != '^')
            return 0;

    int ol = tl < max - 1 ? tl : max - 2;
    memcpy(out, trimmed, ol); out[ol] = 0;
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file.tex [--vars alpha,beta,gamma]\n", argv[0]);
        fprintf(stderr, "  Auto-discovers all $...$ symbols if --vars not given.\n");
        return 1;
    }

    /* read entire file */
    FILE *fp = fopen(argv[1], "rb");
    if (!fp) { fprintf(stderr, "Cannot open %s\n", argv[1]); return 1; }
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz <= 0 || sz > MAX_TEX_LEN) { fclose(fp); return 1; }
    char *tex = malloc((size_t)sz + 1);
    int tex_len = (int)fread(tex, 1, (size_t)sz, fp);
    tex[tex_len] = 0;
    fclose(fp);

    /* get wanted symbols */
    char *wanted[1024]; int n_want = 0;

    int manual_vars = 0;
    for (int a = 2; a < argc; a++) {
        if (!strcmp(argv[a], "--vars") && a + 1 < argc) {
            char *tok = strtok(argv[++a], ",");
            while (tok && n_want < 1024) {
                while (*tok == ' ') tok++;
                char *end = tok + strlen(tok) - 1;
                while (end > tok && *end == ' ') *end-- = 0;
                if (*tok) wanted[n_want++] = strdup(tok);
                tok = strtok(NULL, ",");
            }
            manual_vars = 1;
        }
    }

    if (!manual_vars) {
        /* auto-discover: find all $...$ and extract symbol keys */
        char seen[2048][64]; int n_seen = 0;
        for (int i = 0; i < tex_len; i++) {
            if (tex[i] == '$') {
                int j = i + 1;
                while (j < tex_len && tex[j] != '$' && (j - i) < 60) j++;
                if (j >= tex_len || tex[j] != '$') continue;
                char sym[64] = "";
                if (symbol_key(tex + i + 1, j - i - 1, sym, sizeof(sym))) {
                    int dup = 0;
                    for (int s = 0; s < n_seen; s++)
                        if (!strcmp(seen[s], sym)) { dup = 1; break; }
                    if (!dup && n_seen < 2048) {
                        strcpy(seen[n_seen++], sym);
                        if (n_want < 1024) wanted[n_want++] = strdup(sym);
                    }
                }
                i = j;
            }
        }
        fprintf(stderr, "Auto-discovered %d symbols: ", n_want);
        for (int w = 0; w < n_want && w < 20; w++)
            fprintf(stderr, "%s%s", w ? "," : "", wanted[w]);
        if (n_want > 20) fprintf(stderr, "...");
        fprintf(stderr, "\n\n");
    }

    if (n_want == 0) {
        fprintf(stderr, "No symbols to extract.\n");
        free(tex); return 0;
    }

    /* run extraction */
    def_result_t results[256];
    int n = def_extract(tex, tex_len, (const char **)wanted, n_want, results, 256);

    /* print results */
    printf("=== %d definitions found in %s ===\n\n", n, argv[1]);
    for (int i = 0; i < n; i++) {
        def_result_t *r = &results[i];
        printf("[%d] $%s$ → \"%s\"\n", i + 1, r->symbol, r->definition);

        /* show context: 80 chars around the definition */
        int ctx_start = r->char_start > 40 ? r->char_start - 40 : 0;
        int ctx_end = r->char_end + 40;
        if (ctx_end > tex_len) ctx_end = tex_len;
        while (ctx_start > 0 && tex[ctx_start] != '\n') ctx_start--;
        while (ctx_end < tex_len && tex[ctx_end] != '\n') ctx_end++;
        if (ctx_start > 0) ctx_start++;

        printf("    context: ");
        for (int c = ctx_start; c < ctx_end && c < ctx_start + 120; c++) {
            if (c == r->char_start) printf("\033[1;33m");
            if (c == r->char_end) printf("\033[0m");
            putchar(tex[c] == '\n' ? ' ' : tex[c]);
        }
        printf("\033[0m\n\n");
    }

    /* cleanup */
    for (int w = 0; w < n_want; w++) free(wanted[w]);
    free(tex);
    return 0;
}
