/*
 * def_extract_main.c — Standalone driver for def_extract library.
 *
 * Usage: ./def_extract file.tex [--vars alpha,beta] [--json] [--all]
 *   --vars: manual list of symbols
 *   --json: output JSON lines instead of human-readable
 *   --all:  extract ALL symbols (default: auto-discover)
 *   --limit N: max results (default: 256)
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "def_extract.h"

#define MAX_TEX (2 * 1024 * 1024)

static int symbol_key(const char *body, int len, char *out, int max) {
    char buf[256]; int bl = len < 255 ? len : 255;
    memcpy(buf, body, bl); buf[bl] = 0;
    if (strstr(buf, "\\frac") || strstr(buf, "\\tfrac") || strstr(buf, "\\dfrac")
     || strstr(buf, "\\binom") || strstr(buf, "\\choose")) return 0;
    if (strstr(buf, "=") || strstr(buf, "\\approx") || strstr(buf, "\\equiv")
     || strstr(buf, "\\sim")  || strstr(buf, "\\propto")) return 0;

    char tmp[256]; int ti = 0; const char *s = buf;
    while (*s && ti < 250) {
        if (!strncmp(s, "\\mathrm{", 8))  { s += 8;  while (*s && *s != '}') s++; if (*s)s++; continue; }
        if (!strncmp(s, "\\mathbf{", 8))  { s += 8;  while (*s && *s != '}') s++; if (*s)s++; continue; }
        if (!strncmp(s, "\\mathcal{", 9)) { s += 9;  while (*s && *s != '}') s++; if (*s)s++; continue; }
        if (!strncmp(s, "\\mathbb{", 8))  { s += 8;  while (*s && *s != '}') s++; if (*s)s++; continue; }
        if (!strncmp(s, "\\text{", 6))    { s += 6;  while (*s && *s != '}') s++; if (*s)s++; continue; }
        if (!strncmp(s, "\\rm ", 4) || !strncmp(s, "\\rm", 3)) { s += 3; continue; }
        if (*s == '\\' || *s == '{' || *s == '}') { s++; continue; }
        tmp[ti++] = *s++;
    }
    tmp[ti] = 0;
    char *t = tmp; while (*t == ' ') t++;
    int tl = (int)strlen(t); while (tl > 0 && t[tl-1] == ' ') tl--; t[tl] = 0;
    if (tl < 1 || tl > 12) return 0;
    if (!isalpha((unsigned char)t[0])) return 0;
    for (int i = 0; i < tl; i++)
        if (!isalnum((unsigned char)t[i]) && t[i] != '_' && t[i] != '^') return 0;
    int ol = tl < max - 1 ? tl : max - 2;
    memcpy(out, t, ol); out[ol] = 0;
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file.tex [--vars a,b,c] [--json] [--all] [--limit N]\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "rb");
    if (!fp) { fprintf(stderr, "Cannot open %s\n", argv[1]); return 1; }
    fseek(fp, 0, SEEK_END); long sz = ftell(fp); fseek(fp, 0, SEEK_SET);
    if (sz <= 0 || sz > MAX_TEX) { fclose(fp); return 1; }
    char *tex = malloc((size_t)sz + 1);
    int tex_len = (int)fread(tex, 1, (size_t)sz, fp); tex[tex_len] = 0; fclose(fp);

    char *wanted[2048]; int n_want = 0;
    int json_mode = 0, limit = 256;

    for (int a = 2; a < argc; a++) {
        if (!strcmp(argv[a], "--json")) { json_mode = 1; continue; }
        if (!strcmp(argv[a], "--all")) { continue; } /* handled below */
        if (!strcmp(argv[a], "--limit") && a + 1 < argc) { limit = atoi(argv[++a]); continue; }
        if (!strcmp(argv[a], "--vars") && a + 1 < argc) {
            char *tok = strtok(argv[++a], ",");
            while (tok && n_want < 2048) {
                while (*tok == ' ') tok++;
                char *end = tok + strlen(tok) - 1;
                while (end > tok && *end == ' ') *end-- = 0;
                if (*tok) wanted[n_want++] = strdup(tok);
                tok = strtok(NULL, ",");
            }
            continue;
        }
    }

    if (n_want == 0) {
        char seen[2048][64]; int n_seen = 0;
        for (int i = 0; i < tex_len; i++) {
            if (tex[i] == '$') {
                int j = i + 1;
                while (j < tex_len && tex[j] != '$' && j - i < 80) j++;
                if (j >= tex_len || tex[j] != '$') continue;
                char sym[64] = "";
                if (symbol_key(tex + i + 1, j - i - 1, sym, sizeof(sym))) {
                    int dup = 0;
                    for (int s = 0; s < n_seen; s++)
                        if (!strcmp(seen[s], sym)) { dup = 1; break; }
                    if (!dup && n_seen < 2048) {
                        strcpy(seen[n_seen++], sym);
                        if (n_want < 2048) wanted[n_want++] = strdup(sym);
                    }
                }
                i = j;
            }
        }
        if (!json_mode) fprintf(stderr, "Auto-discovered %d symbols\n", n_want);
    }

    if (n_want == 0) { fprintf(stderr, "No symbols found.\n"); free(tex); return 0; }

    def_result_t results[512];
    if (limit > 512) limit = 512;
    int n = def_extract(tex, tex_len, (const char **)wanted, n_want, results, limit);

    if (json_mode) {
        printf("[\n");
        for (int i = 0; i < n; i++) {
            def_result_t *r = &results[i];
            printf("  {\"symbol\":\"%s\",\"definition\":\"", r->symbol);
            for (char *p = r->definition; *p; p++) {
                if (*p == '"') printf("\\\"");
                else if (*p == '\\') printf("\\\\");
                else if (*p == '\n') printf("\\n");
                else putchar(*p);
            }
            printf("\",\"span\":[%d,%d],\"pattern\":%d}%s\n",
                   r->char_start, r->char_end, r->pattern, i + 1 < n ? "," : "");
        }
        printf("]\n");
    } else {
        const char *pnames[] = {"", "copula", "apposition", "equation", "defines", "parenthetical"};
        printf("=== %d definitions ===\n\n", n);
        for (int i = 0; i < n; i++) {
            def_result_t *r = &results[i];
            printf("[%d] $%s$ → \"%s\"  [%s]\n",
                   i + 1, r->symbol, r->definition, pnames[r->pattern]);
            int cs = r->char_start > 40 ? r->char_start - 40 : 0;
            int ce = r->char_end + 40;
            if (ce > tex_len) ce = tex_len;
            while (cs > 0 && tex[cs] != '\n') cs--;
            while (ce < tex_len && tex[ce] != '\n') ce++;
            if (cs > 0) cs++;
            printf("    "); for (int c = cs; c < ce && c < cs + 120; c++)
                putchar(tex[c] == '\n' ? ' ' : tex[c]);
            printf("\n\n");
        }
    }

    for (int w = 0; w < n_want; w++) free(wanted[w]);
    free(tex);
    return 0;
}
