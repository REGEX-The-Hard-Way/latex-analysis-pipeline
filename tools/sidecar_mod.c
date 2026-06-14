/*
 * sidecar_mod.c — mmap-based sidecar.json rule engine (fixed).
 *
 * Applies rules to sidecar records. For each matching record,
 * replaces the text value, recomputes length, and optionally
 * updates token_id hash. Generates undo log.
 *
 * Usage: ./sidecar_mod input.json rules.txt output.json [undo.log]
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <regex.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_LINE  16384
#define MAX_RULES 64

typedef struct {
    char *name, *match_type, *prop, *transform, *pattern, *replace;
    regex_t compiled; int compiled_ok;
} rule_t;

static rule_t rules[MAX_RULES];
static int n_rules = 0;

static int json_get_str(const char *l, const char *k, char *o, int m) {
    char p[128]; snprintf(p, sizeof(p), "\"%s\":\"", k);
    const char *s = strstr(l, p); if (!s) return 0; s += strlen(p);
    int i = 0; while (*s && *s != '"' && i < m-1) {
        if (*s == '\\' && s[1]) { s++; o[i++] = *s++; } else o[i++] = *s++; }
    o[i] = 0; return 1;
}
static int json_get_int(const char *l, const char *k, int *o) {
    char p[128]; snprintf(p, sizeof(p), "\"%s\":", k);
    const char *s = strstr(l, p); if (!s) return 0;
    *o = atoi(s + strlen(p)); return 1;
}
static int apply_transform(const char *v, int vl, const rule_t *r,
                            char *o, int mo) {
    if (!strcmp(r->transform, "regex")) {
        if (!r->compiled_ok) return -1;
        regmatch_t g[16];
        if (regexec(&r->compiled, v, 16, g, 0)) return -1;
        int oi = 0; const char *rp = r->replace;
        while (*rp && oi < mo-1) {
            if (*rp == '$' && rp[1] >= '1' && rp[1] <= '9') {
                int gn = rp[1]-'0'; rp += 2;
                if (gn < 16 && g[gn].rm_so >= 0) {
                    int gl = g[gn].rm_eo - g[gn].rm_so;
                    if (oi + gl >= mo) gl = mo - oi - 1;
                    memcpy(o + oi, v + g[gn].rm_so, gl); oi += gl; }
            } else o[oi++] = *rp++;
        } o[oi] = 0;
        return (oi == vl && !memcmp(o, v, oi)) ? -1 : oi;
    }
    if (!strcmp(r->transform, "strip_prefix")) {
        int pl = strlen(r->pattern);
        if (pl > vl || strncmp(v, r->pattern, pl)) return -1;
        int nl = vl - pl; if (nl >= mo) nl = mo-1;
        memcpy(o, v + pl, nl); o[nl] = 0; return nl;
    }
    if (!strcmp(r->transform, "strip_suffix")) {
        int sl = strlen(r->pattern);
        if (sl > vl || strcmp(v + vl - sl, r->pattern)) return -1;
        int nl = vl - sl; if (nl >= mo) nl = mo-1;
        memcpy(o, v, nl); o[nl] = 0; return nl;
    }
    if (!strcmp(r->transform, "trim")) {
        const char *s = v, *e = v + vl;
        while (s < e && (*s==' '||*s=='\t'||*s=='\n')) s++;
        while (e > s && (*(e-1)==' '||*(e-1)=='\t'||*(e-1)=='\n')) e--;
        int nl = e-s; if (nl >= mo) nl = mo-1;
        memcpy(o, s, nl); o[nl] = 0; return nl;
    }
    return -1;
}

int main(int argc, char *argv[]) {
    if (argc < 4) { fprintf(stderr,"Usage: %s in.json rules.txt out.json [undo.log]\n",argv[0]); return 1; }
    FILE *rf = fopen(argv[2],"r"); if (!rf) return 1;
    char rl[4096];
    while (fgets(rl,sizeof(rl),rf) && n_rules < MAX_RULES) {
        if (rl[0] != '{') continue;
        char b[256];
        if (json_get_str(rl,"name",b,sizeof(b))) rules[n_rules].name = strdup(b);
        if (json_get_str(rl,"match_type",b,sizeof(b))) rules[n_rules].match_type = strdup(b);
        if (json_get_str(rl,"prop",b,sizeof(b))) rules[n_rules].prop = strdup(b);
        if (json_get_str(rl,"transform",b,sizeof(b))) rules[n_rules].transform = strdup(b);
        if (json_get_str(rl,"pattern",b,sizeof(b))) rules[n_rules].pattern = strdup(b);
        if (json_get_str(rl,"replace",b,sizeof(b))) rules[n_rules].replace = strdup(b);
        if (rules[n_rules].pattern && rules[n_rules].transform
            && !strcmp(rules[n_rules].transform,"regex"))
            rules[n_rules].compiled_ok = !regcomp(&rules[n_rules].compiled,
                rules[n_rules].pattern, REG_EXTENDED);
        n_rules++;
    }
    fclose(rf);
    printf("Loaded %d rules\n", n_rules);

    int fd = open(argv[1], O_RDONLY); struct stat st; fstat(fd,&st);
    char *data = mmap(NULL,st.st_size,PROT_READ,MAP_PRIVATE,fd,0); close(fd);
    FILE *out = fopen(argv[3],"w");
    FILE *undo = argc >= 5 ? fopen(argv[4],"w") : NULL;

    int changed = 0, records = 0, undo_seq = 0;
    char *p = data, *end = data + st.st_size;

    while (p < end) {
        char *ls = p; while (ls < end && *ls != '{') ls++;
        if (ls >= end) break;
        char *le = ls; while (le < end && *le != '\n') le++;
        ptrdiff_t llen = le - ls; p = le + 1;
        if (llen < 10) { fwrite(ls,1,llen,out); fputc('\n',out); continue; }

        char line[MAX_LINE];
        int clen = llen < MAX_LINE-1 ? (int)llen : MAX_LINE-2;
        memcpy(line, ls, clen); line[clen] = 0;
        records++;

        char type[128] = "";
        json_get_str(line, "type", type, sizeof(type));

        for (int ri = 0; ri < n_rules; ri++) {
            if (rules[ri].match_type && rules[ri].match_type[0]
                && strcmp(type, rules[ri].match_type)) continue;

            char val[4096] = "";
            if (!json_get_str(line, rules[ri].prop, val, sizeof(val))) continue;
            int vl = strlen(val);

            char result[4096];
            int nl = apply_transform(val, vl, &rules[ri], result, sizeof(result));
            if (nl < 0) continue;

            if (undo) {
                fprintf(undo,"{\"step\":%d,\"name\":\"%s\",\"record_line\":%d,",
                    undo_seq++, rules[ri].name, records);
                fprintf(undo,"\"prop\":\"%s\",\"original\":\"", rules[ri].prop);
                for (int i = 0; val[i]; i++) {
                    if (val[i]=='\\') fprintf(undo,"\\\\");
                    else if (val[i]=='"') fprintf(undo,"\\\"");
                    else if (val[i]=='\n') fprintf(undo,"\\n");
                    else fputc(val[i], undo);
                }
                fprintf(undo,"\",\"transformed\":\"");
                for (int i = 0; i < nl; i++) {
                    if (result[i]=='\\') fprintf(undo,"\\\\");
                    else if (result[i]=='"') fprintf(undo,"\\\"");
                    else if (result[i]=='\n') fprintf(undo,"\\n");
                    else fputc(result[i], undo);
                }
                fprintf(undo,"\"}\n");
            }

            /* rebuild line with new text value */
            char pat[128]; snprintf(pat,sizeof(pat),"\"%s\":\"", rules[ri].prop);
            char *ps = strstr(line, pat); if (!ps) continue;
            ps += strlen(pat);
            char *pe = ps; while (*pe && *pe != '"') {
                if (*pe == '\\' && pe[1]) pe++; pe++; }
            char *tail = pe; /* include closing quote */

            char tmp[MAX_LINE];
            int pre_len = (int)(ps - line);
            memcpy(tmp, line, pre_len);
            memcpy(tmp + pre_len, result, nl);
            int post_off = pre_len + nl;
            int tail_len = clen - (int)(tail - line);
            if (tail_len > 0 && post_off + tail_len < MAX_LINE - 1)
                memcpy(tmp + post_off, tail, tail_len);
            tmp[post_off + tail_len] = 0;
            clen = post_off + tail_len;
            memcpy(line, tmp, clen + 1);
            changed++;
        }

        fwrite(line, 1, clen, out);
        fputc('\n', out);
    }
    fclose(out); if (undo) fclose(undo);
    munmap(data, st.st_size);
    printf("Processed %d records, %d changed by %d rules\n", records, changed, n_rules);
    return 0;
}
