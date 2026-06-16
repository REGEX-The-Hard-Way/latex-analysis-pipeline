/*
 * simple_expander.c — String-based LaTeX macro expander.
 *
 * Handles: \newcommand, \renewcommand, \providecommand, \def, \let, \newenvironment.
 * Passes everything else through unchanged.  Preserves comments and whitespace.
 *
 * Usage: ./simple_expander < input.tex > output.tex
 *
 * Build: gcc -O2 -o simple_expander simple_expander.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_MACROS   4096
#define MAX_NAME      256
#define MAX_BODY     8192
#define MAX_ARGS       10
#define MAX_INPUT  (4*1024*1024)

typedef struct {
    char name[MAX_NAME];
    char body[MAX_BODY];
    int  nargs;               /* number of #1..#9 parameters */
    char *arg_bodies[MAX_ARGS]; /* for \newenvironment: begin/end bodies */
    int  is_env;               /* 1 if this is a \newenvironment */
} macro_t;

static macro_t macros[MAX_MACROS];
static int n_macros = 0;

static char  input[MAX_INPUT];
static char  output[MAX_INPUT];
static int   in_len, out_len;

/* ---- helpers ---- */
static int is_letter(int c) { return isalpha(c) || c == '@'; }
static int is_space(int c)  { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }

static void skip_ws(const char *s, int *i) {
    while (*i < in_len && is_space(s[*i])) (*i)++;
}

/* read balanced { ... } including nested braces */
static int read_balanced(const char *s, int *i, char *out, int max) {
    if (*i >= in_len || s[*i] != '{') return 0;
    int depth = 1, start = *i + 1;
    (*i)++;
    while (*i < in_len && depth > 0) {
        if (s[*i] == '{') depth++;
        else if (s[*i] == '}') depth--;
        if (depth > 0) (*i)++;
    }
    if (depth != 0) return 0;
    int len = *i - start;
    if (len >= max) len = max - 1;
    memcpy(out, s + start, len);
    out[len] = 0;
    (*i)++; /* skip closing } */
    return 1;
}

/* read optional [n] argument count */
static int read_opt_count(const char *s, int *i) {
    if (*i >= in_len || s[*i] != '[') return 0;
    (*i)++;
    int n = 0;
    while (*i < in_len && s[*i] >= '0' && s[*i] <= '9')
        n = n * 10 + (s[*i++] - '0');
    if (*i < in_len && s[*i] == ']') (*i)++;
    return n;
}

/* copy characters from input to output literally */
static void copy_literal(int start, int end) {
    if (end <= start) return;
    if (out_len + (end - start) >= MAX_INPUT - 1) return;
    memcpy(output + out_len, input + start, end - start);
    out_len += (end - start);
}

/* ---- macro storage ---- */
static macro_t *find_macro(const char *name) {
    for (int i = 0; i < n_macros; i++)
        if (!strcmp(macros[i].name, name)) return &macros[i];
    return NULL;
}

static macro_t *add_macro(const char *name) {
    macro_t *m = find_macro(name);
    if (!m) {
        if (n_macros >= MAX_MACROS) return NULL;
        m = &macros[n_macros++];
        memset(m, 0, sizeof(*m));
    }
    strncpy(m->name, name, MAX_NAME - 1);
    return m;
}

static void copy_arg(char *dst, const char *src, int max) {
    int i = 0;
    while (*src == ' ' || *src == '\t') src++; /* trim leading ws */
    while (*src && i < max - 1) dst[i++] = *src++;
    while (i > 0 && (dst[i-1] == ' ' || dst[i-1] == '\t')) i--; /* trim trailing ws */
    dst[i] = 0;
}

/* ---- \def parsing: \def\name#1#2{body} ---- */
static int parse_def(const char *s, int *i) {
    /* after \def */
    skip_ws(s, i);
    if (*i >= in_len || s[*i] != '\\') return 0;
    (*i)++; /* skip \ */
    
    char name[MAX_NAME];
    int nl = 0;
    while (*i < in_len && is_letter(s[*i]) && nl < MAX_NAME - 1)
        name[nl++] = s[*i++];
    name[nl] = 0;
    if (nl == 0) return 0;
    
    /* read parameter markers #1 #2 ... */
    int nargs = 0;
    while (*i < in_len && s[*i] == '#') {
        (*i)++;
        if (*i < in_len && s[*i] >= '1' && s[*i] <= '9') {
            int n = s[*i] - '0';
            if (n > nargs) nargs = n;
            (*i)++;
        } else break;
    }
    
    /* read body { ... } */
    skip_ws(s, i);
    char body[MAX_BODY];
    if (!read_balanced(s, i, body, MAX_BODY)) return 0;
    
    macro_t *m = add_macro(name);
    if (!m) return 0;
    strncpy(m->body, body, MAX_BODY - 1);
    m->nargs = nargs;
    return 1;
}

/* ---- \newcommand{\name}[n]{body} ---- */
static int parse_newcommand(const char *s, int *i) {
    skip_ws(s, i);
    char name[MAX_NAME], body[MAX_BODY];
    
    /* read {\name} or \name */
    if (*i < in_len && s[*i] == '{') {
        if (!read_balanced(s, i, name, MAX_NAME)) return 0;
        /* strip leading \ if present */
        char *p = name;
        if (*p == '\\') memmove(p, p+1, strlen(p));
    } else if (*i < in_len && s[*i] == '\\') {
        (*i)++;
        int nl = 0;
        while (*i < in_len && is_letter(s[*i]) && nl < MAX_NAME - 1)
            name[nl++] = s[*i++];
        name[nl] = 0;
    } else return 0;
    
    int nargs = read_opt_count(s, i);
    int arg_count = nargs;
    
    /* read {body} — may be followed by second {body} for \newenvironment */
    skip_ws(s, i);
    if (!read_balanced(s, i, body, MAX_BODY)) return 0;
    
    macro_t *m = add_macro(name);
    if (!m) return 0;
    strncpy(m->body, body, MAX_BODY - 1);
    m->nargs = nargs;
    return 1;
}

/* ---- \newenvironment{name}[n]{begin}{end} ---- */
static int parse_newenvironment(const char *s, int *i) {
    skip_ws(s, i);
    char name[MAX_NAME];
    if (!read_balanced(s, i, name, MAX_NAME)) return 0;
    
    int nargs = read_opt_count(s, i);
    
    skip_ws(s, i);
    char begin_body[MAX_BODY], end_body[MAX_BODY];
    if (!read_balanced(s, i, begin_body, MAX_BODY)) return 0;
    skip_ws(s, i);
    if (!read_balanced(s, i, end_body, MAX_BODY)) return 0;
    
    macro_t *m = add_macro(name);
    if (!m) return 0;
    m->nargs = nargs;
    m->is_env = 1;
    
    /* store begin body in arg_bodies[0], end in arg_bodies[1] */
    m->arg_bodies[0] = malloc(strlen(begin_body) + 1);
    m->arg_bodies[1] = malloc(strlen(end_body) + 1);
    strcpy(m->arg_bodies[0], begin_body);
    strcpy(m->arg_bodies[1], end_body);
    return 1;
}

/* ---- \let\foo\bar ---- */
static int parse_let(const char *s, int *i) {
    skip_ws(s, i);
    if (*i >= in_len || s[*i] != '\\') return 0;
    (*i)++;
    char newname[MAX_NAME]; int nl = 0;
    while (*i < in_len && is_letter(s[*i]) && nl < MAX_NAME - 1)
        newname[nl++] = s[*i++];
    newname[nl] = 0;
    
    skip_ws(s, i);
    if (*i >= in_len || s[*i] != '\\') return 0;
    (*i)++;
    char oldname[MAX_NAME]; int ol = 0;
    while (*i < in_len && is_letter(s[*i]) && ol < MAX_NAME - 1)
        oldname[ol++] = s[*i++];
    oldname[ol] = 0;
    
    macro_t *old = find_macro(oldname);
    if (!old) return 0;
    macro_t *m = add_macro(newname);
    if (!m) return 0;
    strncpy(m->body, old->body, MAX_BODY - 1);
    m->nargs = old->nargs;
    return 1;
}

/* ---- expand a macro call ---- */
static int expand_macro(macro_t *m, const char *s, int *i, int is_env_end) {
    (void)is_env_end;
    
    if (m->is_env) {
        /* \begin{name} — output begin body */
        if (m->arg_bodies[0]) {
            int bl = (int)strlen(m->arg_bodies[0]);
            if (out_len + bl < MAX_INPUT - 1) {
                memcpy(output + out_len, m->arg_bodies[0], bl);
                out_len += bl;
            }
        }
    } else if (m->nargs == 0) {
        /* simple expansion: just output body */
        int bl = (int)strlen(m->body);
        if (out_len + bl < MAX_INPUT - 1) {
            memcpy(output + out_len, m->body, bl);
            out_len += bl;
        }
    } else {
        /* macro with arguments: read args, substitute #1..#n in body */
        char *args[MAX_ARGS] = {0};
        for (int a = 0; a < m->nargs; a++) {
            skip_ws(s, i);
            char arg_buf[MAX_BODY];
            if (*i < in_len && s[*i] == '{') {
                read_balanced(s, i, arg_buf, MAX_BODY);
            } else {
                /* single-token argument */
                int al = 0;
                if (*i < in_len && s[*i] == '\\') {
                    arg_buf[al++] = s[(*i)++];
                    while (*i < in_len && is_letter(s[*i]) && al < MAX_BODY - 1)
                        arg_buf[al++] = s[(*i)++];
                } else if (*i < in_len) {
                    arg_buf[al++] = s[(*i)++];
                }
                arg_buf[al] = 0;
            }
            args[a] = strdup(arg_buf);
        }
        
        /* substitute #1..#n in body */
        const char *b = m->body;
        while (*b) {
            if (*b == '#' && b[1] >= '1' && b[1] <= '9') {
                int an = b[1] - '1';
                b += 2;
                if (an < m->nargs && args[an]) {
                    int al = (int)strlen(args[an]);
                    if (out_len + al < MAX_INPUT - 1) {
                        memcpy(output + out_len, args[an], al);
                        out_len += al;
                    }
                } else {
                    if (out_len + 2 < MAX_INPUT - 1) {
                        output[out_len++] = '#';
                        output[out_len++] = '1' + an;
                    }
                }
            } else {
                if (out_len + 1 < MAX_INPUT - 1)
                    output[out_len++] = *b;
                b++;
            }
        }
        
        for (int a = 0; a < m->nargs; a++) free(args[a]);
    }
    return 1;
}

/* ---- main processing ---- */
int main(void) {
    /* read entire input */
    in_len = (int)fread(input, 1, MAX_INPUT - 1, stdin);
    input[in_len] = 0;
    
    out_len = 0;
    int i = 0;
    
    /* Pass 1: find all macro definitions and remove them from output */
    /* We do this in a single pass: copy literal text to output, 
       process macro definitions (store but don't output),
       expand macro calls. */
    
    while (i < in_len) {
        int saved = i;
        
        /* skip whitespace */
        if (is_space(input[i])) {
            output[out_len++] = input[i++];
            continue;
        }
        
        /* comment line — pass through */
        if (input[i] == '%') {
            int start = i;
            while (i < in_len && input[i] != '\n') i++;
            copy_literal(start, i);
            continue;
        }
        
        /* control sequence */
        if (input[i] == '\\') {
            int cs_start = i;
            i++; /* skip \ */
            
            /* read command name */
            char cmd[64]; int cl = 0;
            while (i < in_len && is_letter(input[i]) && cl < 63)
                cmd[cl++] = input[i++];
            cmd[cl] = 0;
            
            if (!strcmp(cmd, "newcommand") || !strcmp(cmd, "renewcommand") || 
                !strcmp(cmd, "providecommand")) {
                int si = i;
                if (parse_newcommand(input, &i)) {
                    /* definition parsed, don't output it */
                    continue;
                }
                i = si; /* restore on failure */
                copy_literal(cs_start, i);
                continue;
            }
            
            if (!strcmp(cmd, "def")) {
                int si = i;
                if (parse_def(input, &i)) continue;
                i = si;
                copy_literal(cs_start, i);
                continue;
            }
            
            if (!strcmp(cmd, "edef") || !strcmp(cmd, "xdef") || !strcmp(cmd, "gdef")) {
                int si = i;
                if (parse_def(input, &i)) continue;
                i = si;
                copy_literal(cs_start, i);
                continue;
            }
            
            if (!strcmp(cmd, "let")) {
                int si = i;
                if (parse_let(input, &i)) continue;
                i = si;
                copy_literal(cs_start, i);
                continue;
            }
            
            if (!strcmp(cmd, "newenvironment")) {
                int si = i;
                if (parse_newenvironment(input, &i)) continue;
                i = si;
                copy_literal(cs_start, i);
                continue;
            }
            
            if (!strcmp(cmd, "renewenvironment")) {
                int si = i;
                if (parse_newenvironment(input, &i)) continue;
                i = si;
                copy_literal(cs_start, i);
                continue;
            }
            
            /* Is this a macro call? */
            if (cl > 0) {
                macro_t *m = find_macro(cmd);
                if (m) {
                    expand_macro(m, input, &i, 0);
                    continue;
                }
            }
            
            /* Not a macro — copy literally */
            copy_literal(cs_start, i);
            continue;
        }
        
        /* regular character */
        output[out_len++] = input[i++];
    }
    
    output[out_len] = 0;
    fwrite(output, 1, out_len, stdout);
    
    /* report */
    fprintf(stderr, "simple_expander: %d macros defined, %d bytes in, %d bytes out\n",
            n_macros, in_len, out_len);
    return 0;
}
