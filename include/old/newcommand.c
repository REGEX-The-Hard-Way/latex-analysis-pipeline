/*
 * newcommand.c — LaTeX Macro Handler
 *
 * Properly handles: \newcommand, \renewcommand, \def, \edef, \xdef,
 * \let, \expandafter, \aftergroup, \noexpand
 *
 * Build: gcc -O2 newcommand.c -o newcommand.out
 * Usage: ./newcommand.out < file.tex > expanded.tex
 *        ./newcommand.out --report file1.tex file2.tex ...
 */

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Safe string copy: always NUL-terminates, copies at most dstsize-1 chars */
static void safe_strcpy(char *dst, const char *src, int dstsize) {
    if (dstsize <= 0) return;
    int i;
    for (i = 0; i < dstsize - 1 && src[i]; i++)
        dst[i] = src[i];
    dst[i] = '\0';
}

/* ------------------------------------------------------------------ */
/*  Configuration                                                     */
/* ------------------------------------------------------------------ */
#define MAX_MACRO_NAME    128
#define MAX_MACRO_BODY    16384
#define MAX_MACROS        4096
#define MAX_INPUT_SIZE    (64 * 1024 * 1024)  /* 64 MB max input */
#define OUTPUT_CHUNK      (1024 * 1024)
#define PARAM_COUNT       10
#define MAX_PUSHBACK      65536   /* pushback buffer for \expandafter expansions */
#define MAX_AFTERGROUP    64     /* deferred tokens from \aftergroup */

/* ------------------------------------------------------------------ */
/*  Macro types                                                       */
/* ------------------------------------------------------------------ */
typedef enum {
    MT_NEWCOMMAND,
    MT_RENEWCOMMAND,
    MT_PROVIDECOMMAND,
    MT_DEF,
    MT_EDEF,        /* expand body at definition time */
    MT_XDEF,        /* global \edef */
    MT_GDEF,        /* \gdef — global def */
    MT_LET,         /* \let\a\b — copy */
    MT_NONE
} MacroType;

/* ------------------------------------------------------------------ */
/*  Macro definition storage                                          */
/* ------------------------------------------------------------------ */
typedef struct {
    char   name[MAX_MACRO_NAME];
    char   body[MAX_MACRO_BODY];
    int    num_params;       /* 0-9: how many #1..#9 the macro takes */
    MacroType type;
    int    active;           /* 1 if defined */
    char   let_target[MAX_MACRO_NAME]; /* for MT_LET: target name */
} MacroDef;

typedef struct {
    MacroDef entries[MAX_MACROS];
    int      count;
} MacroDB;

static MacroDB g_db;

/* ------------------------------------------------------------------ */
/*  Macro DB operations                                               */
/* ------------------------------------------------------------------ */
static void db_init(void) {
    memset(&g_db, 0, sizeof(g_db));
}

static MacroDef *db_find(const char *name) {
    for (int i = 0; i < g_db.count; i++) {
        if (g_db.entries[i].active && !strcmp(g_db.entries[i].name, name))
            return &g_db.entries[i];
    }
    return NULL;
}

static MacroDef *db_add(const char *name) {
    if (g_db.count >= MAX_MACROS) {
        fprintf(stderr, "WARNING: too many macros (max %d)\n", MAX_MACROS);
        return NULL;
    }
    MacroDef *m = &g_db.entries[g_db.count++];
    memset(m, 0, sizeof(*m));
    safe_strcpy(m->name, name, MAX_MACRO_NAME - 1);
    m->name[MAX_MACRO_NAME - 1] = '\0';
    m->active = 1;
    return m;
}

static void db_store(MacroDef *m) {
    MacroDef *existing = db_find(m->name);
    if (existing) {
        /* overwrite */
        *existing = *m;
    } else {
        MacroDef *slot = db_add(m->name);
        if (slot) *slot = *m;
    }
}

/* ------------------------------------------------------------------ */
/*  Output buffer                                                     */
/* ------------------------------------------------------------------ */
static char  *g_outbuf = NULL;
static size_t g_outlen = 0;
static size_t g_outcap = 0;

static void out_init(void) {
    g_outcap = OUTPUT_CHUNK;
    g_outbuf = (char *)malloc(g_outcap);
    g_outlen = 0;
    if (g_outbuf) g_outbuf[0] = '\0';
}

static void out_append(const char *s, int len) {
    if (len <= 0) return;
    if (g_outlen + len + 1 > g_outcap) {
        size_t newcap = (g_outlen + len + 1) * 2;
        char *nb = (char *)realloc(g_outbuf, newcap);
        if (!nb) {
            fprintf(stderr, "FATAL: out of memory\n");
            exit(1);
        }
        g_outbuf = nb;
        g_outcap = newcap;
    }
    memcpy(g_outbuf + g_outlen, s, len);
    g_outlen += len;
    g_outbuf[g_outlen] = '\0';
}

static void out_append_str(const char *s) {
    out_append(s, (int)strlen(s));
}

static void out_append_char(char c) {
    out_append(&c, 1);
}

/* ------------------------------------------------------------------ */
/*  Input scanner helpers                                             */
/* ------------------------------------------------------------------ */
typedef struct {
    const char *p;
    const char *pe;     /* end of input */
    const char *start;  /* original start */
} Scanner;

static Scanner g_sc;

/* Pushback buffer — used by \expandafter to inject expanded tokens
 * back into the input stream so that chained commands can re-parse. */
static char  g_pushback[MAX_PUSHBACK];
static int   g_pushback_len = 0;
static int   g_pushback_pos = 0;

static void pushback_clear(void) {
    g_pushback_len = 0;
    g_pushback_pos = 0;
}

/* Append text to the pushback buffer (inserts at current read position).
 * Returns 1 on success, 0 on overflow. */
static int pushback_text(const char *s, int len) {
    if (len <= 0) return 1;
    int remaining = g_pushback_len - g_pushback_pos;
    if (g_pushback_pos + remaining + len > MAX_PUSHBACK) {
        return 0;  /* overflow — caller should fall back to literal output */
    }
    /* Move unread pushback data forward to make room */
    memmove(g_pushback + g_pushback_pos + len,
            g_pushback + g_pushback_pos, remaining);
    memcpy(g_pushback + g_pushback_pos, s, len);
    g_pushback_len += len;
    return 1;
}

/* Expand a macro and push the result into the pushback buffer for
 * re-scanning. On overflow or excessive depth, leaves the expansion
 * in the output buffer or outputs the command name literally. */

/* Group-depth tracking for \aftergroup */
static int  g_group_depth = 0;
static char g_aftergroup_tokens[MAX_AFTERGROUP][MAX_MACRO_BODY];
static int  g_aftergroup_depths[MAX_AFTERGROUP];
static int  g_aftergroup_count = 0;

/* Expansion depth guard — prevents infinite recursion in \def\a{\a}.
 * Incremented on each macro expansion, never decremented within one
 * top-level process_input call. */
static int  g_expansion_depth = 0;
#define MAX_EXPANSION_DEPTH 500

/* Forward declarations for cross-calls */
static void expand_macro(MacroDef *m, char args[PARAM_COUNT][MAX_MACRO_BODY], int nargs_provided);
static void expand_to_pushback(MacroDef *m, char args[PARAM_COUNT][MAX_MACRO_BODY], int np);
static void out_append_str(const char *s);

static void aftergroup_add(const char *tok) {
    if (g_aftergroup_count >= MAX_AFTERGROUP) return;
    safe_strcpy(g_aftergroup_tokens[g_aftergroup_count], tok, MAX_MACRO_BODY - 1);
    g_aftergroup_tokens[g_aftergroup_count][MAX_MACRO_BODY - 1] = '\0';
    g_aftergroup_depths[g_aftergroup_count] = g_group_depth;
    g_aftergroup_count++;
}

/* Called when a '}' is encountered; flushes any aftergroup tokens
 * whose group depth has been exited. */
static void aftergroup_flush(void) {
    for (int i = 0; i < g_aftergroup_count; i++) {
        if (g_aftergroup_depths[i] == g_group_depth) {
            /* Look up and expand the deferred token into pushback */
            MacroDef *m = db_find(g_aftergroup_tokens[i]);
            if (m && m->active) {
                char dummy[PARAM_COUNT][MAX_MACRO_BODY] = {{0}};
                expand_to_pushback(m, dummy, 0);
            } else {
                out_append_str(g_aftergroup_tokens[i]);
            }
            /* Mark as flushed */
            g_aftergroup_tokens[i][0] = '\0';
        }
    }
}

static int sc_eof(void) {
    if (g_pushback_pos < g_pushback_len) return 0;
    return g_sc.p >= g_sc.pe;
}

static int sc_peek(void) {
    if (g_pushback_pos < g_pushback_len)
        return (unsigned char)g_pushback[g_pushback_pos];
    return (g_sc.p >= g_sc.pe) ? EOF : (unsigned char)*g_sc.p;
}

static int sc_getc(void) {
    if (g_pushback_pos < g_pushback_len)
        return (unsigned char)g_pushback[g_pushback_pos++];
    return (g_sc.p >= g_sc.pe) ? EOF : (unsigned char)*g_sc.p++;
}

/* Skip a TeX comment: from % to end of line (consuming the newline) */
static void sc_skip_comment(void) {
    while (!sc_eof() && *g_sc.p != '\n')
        g_sc.p++;
    if (!sc_eof() && *g_sc.p == '\n')
        g_sc.p++;
}

/* Skip whitespace and comments */
static void sc_skip_ws_and_comments(void) {
    while (!sc_eof()) {
        if (*g_sc.p == '%') {
            sc_skip_comment();
        } else if (*g_sc.p == ' ' || *g_sc.p == '\t' || *g_sc.p == '\r' || *g_sc.p == '\n') {
            g_sc.p++;
        } else {
            break;
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Token / group extraction                                          */
/* ------------------------------------------------------------------ */

/*
 * Extract a balanced brace group, including the outer braces.
 * Returns 1 on success, 0 on failure.
 * The extracted text goes into 'buf' (with NUL terminator), up to 'bufsize'.
 * On success, the scanner is positioned after the closing '}'.
 */
static int extract_braced_group(char *buf, int bufsize) {
    sc_skip_ws_and_comments();
    if (sc_peek() != '{') return 0;

    int depth = 0;
    int pos = 0;

    while (!sc_eof() && pos < bufsize - 1) {
        char c = sc_getc();
        buf[pos++] = c;

        if (c == '{') {
            depth++;
        } else if (c == '}') {
            depth--;
            if (depth == 0) {
                buf[pos] = '\0';
                return 1;
            }
        } else if (c == '\\') {
            /* Backslash — next char is part of a command, not a brace */
            if (!sc_eof() && pos < bufsize - 1) {
                buf[pos++] = sc_getc();
            }
        } else if (c == '%') {
            /* Comment — undo the '%' we stored and skip to end of line */
            pos--;
            sc_skip_comment();
        }
    }
    buf[pos] = '\0';
    return 0; /* unbalanced */
}

/*
 * Extract an optional argument in [brackets].
 * Returns 1 if found, 0 otherwise. Scanner left after ']'.
 */
static int extract_optional_arg(char *buf, int bufsize) {
    sc_skip_ws_and_comments();
    if (sc_peek() != '[') return 0;

    int pos = 0;
    sc_getc(); /* consume '[' */
    buf[pos++] = '[';

    while (!sc_eof() && pos < bufsize - 1) {
        char c = sc_getc();
        buf[pos++] = c;
        if (c == ']') {
            buf[pos] = '\0';
            return 1;
        } else if (c == '\\' && !sc_eof() && pos < bufsize - 1) {
            buf[pos++] = sc_getc();
        }
    }
    buf[pos] = '\0';
    return 0;
}

/*
 * Extract a TeX command name (letters and @) after a backslash.
 * Assumes the backslash has already been consumed.
 * Returns the command name in 'buf'.
 */
static int extract_cmdname(char *buf, int bufsize) {
    int pos = 0;
    sc_skip_ws_and_comments();
    while (!sc_eof() && pos < bufsize - 1) {
        char c = sc_peek();
        if (isalpha((unsigned char)c) || c == '@') {
            buf[pos++] = sc_getc();
        } else {
            break;
        }
    }
    buf[pos] = '\0';
    return pos > 0;
}

/*
 * Extract a single token: either a single character, or \command .
 * On success, returns 1 and fills 'buf'. On EOF, returns 0.
 */
static int extract_single_token(char *buf, int bufsize) {
    sc_skip_ws_and_comments();
    if (sc_eof()) return 0;

    int pos = 0;
    char c = sc_getc();

    if (c == '\\') {
        buf[pos++] = '\\';
        /* Check for special single-char escapes like \\, \{, etc. */
        if (!sc_eof()) {
            char c2 = sc_peek();
            if (!isalpha((unsigned char)c2) && c2 != '@') {
                /* Single non-letter character after backslash */
                buf[pos++] = sc_getc();
                buf[pos] = '\0';
                return 1;
            }
        }
        /* Multi-letter command name */
        while (!sc_eof() && pos < bufsize - 1) {
            char c2 = sc_peek();
            if (isalpha((unsigned char)c2) || c2 == '@') {
                buf[pos++] = sc_getc();
            } else {
                break;
            }
        }
        buf[pos] = '\0';
        return 1;
    } else if (c == '{') {
        /* Return the '{' as a token */
        buf[0] = '{'; buf[1] = '\0';
        return 1;
    } else if (c == '}') {
        buf[0] = '}'; buf[1] = '\0';
        return 1;
    } else {
        buf[0] = c; buf[1] = '\0';
        return 1;
    }
}

/*
 * Extract a LaTeX token: \cmd, {group}, or single character.
 * Skips leading whitespace and comments.
 * Returns 1 on success, 0 on EOF.
 */
static int extract_next_token(char *buf, int bufsize) {
    sc_skip_ws_and_comments();
    if (sc_eof()) return 0;

    if (sc_peek() == '\\') {
        /* \command or single-char like \\ */
        int pos = 0;
        buf[pos++] = sc_getc();  /* consume \ */
        if (!sc_eof()) {
            char c = sc_peek();
            if (!isalpha((unsigned char)c) && c != '@') {
                buf[pos++] = sc_getc();
            } else {
                while (!sc_eof() && pos < bufsize - 1) {
                    char c2 = sc_peek();
                    if (isalpha((unsigned char)c2) || c2 == '@')
                        buf[pos++] = sc_getc();
                    else
                        break;
                }
            }
        }
        buf[pos] = '\0';
        return 1;
    } else if (sc_peek() == '{') {
        return extract_braced_group(buf, bufsize);
    } else {
        buf[0] = sc_getc();
        buf[1] = '\0';
        return 1;
    }
}

/*
 * Extract a macro-use argument: either a single token or a braced group.
 * This is what gets substituted for #1, #2, etc.
 */
static int extract_macro_arg(char *buf, int bufsize) {
    sc_skip_ws_and_comments();
    if (sc_eof()) return 0;

    if (sc_peek() == '{') {
        if (!extract_braced_group(buf, bufsize))
            return 0;
        /* Strip outer braces from the argument */
        int len = (int)strlen(buf);
        if (len >= 2 && buf[0] == '{' && buf[len-1] == '}') {
            memmove(buf, buf + 1, len - 2);
            buf[len - 2] = '\0';
        }
        return 1;
    }

    /* Single token */
    return extract_single_token(buf, bufsize);
}

/* ------------------------------------------------------------------ */
/*  Command parsing                                                   */
/* ------------------------------------------------------------------ */

/*
 * newcommand_format:  \newcommand{\name}[n]{body}
 * renewcommand_format: \renewcommand{\name}[n]{body}
 * All forms:
 *   \newcommand{\name}[n]{body}
 *   \newcommand{\name}{body}
 */
static int parse_newcommand(MacroType mtype) {
    char name_braced[MAX_MACRO_NAME + 4];
    char opt_buf[32];
    char body[MAX_MACRO_BODY];
    char clean_name[MAX_MACRO_NAME];
    int  num_args = 0;

    sc_skip_ws_and_comments();
    if (sc_eof()) return 0;

    if (sc_peek() == '{') {
        /* Form: \newcommand{\name}[n]{body} */
        if (!extract_braced_group(name_braced, sizeof(name_braced)))
            return 0;

        int blen = (int)strlen(name_braced);
        if (blen < 4 || name_braced[0] != '{' || name_braced[blen-1] != '}' || name_braced[1] != '\\')
            return 0;

        int nl = blen - 2;
        if (nl >= MAX_MACRO_NAME) nl = MAX_MACRO_NAME - 1;
        memcpy(clean_name, name_braced + 1, nl);
        clean_name[nl] = '\0';
    } else if (sc_peek() == '\\') {
        /* Form: \newcommand\name[n]{body} (no braces around name) */
        sc_getc(); /* consume \ */
        if (!extract_cmdname(clean_name + 1, MAX_MACRO_NAME - 2)) {
            /* Single-char command */
            if (!sc_eof()) {
                clean_name[0] = '\\';
                clean_name[1] = sc_getc();
                clean_name[2] = '\0';
            } else {
                return 0;
            }
        } else {
            /* Shift: extract_cmdname put name (no \\) in clean_name+1 */
            memmove(clean_name, clean_name + 1, strlen(clean_name + 1) + 1);
            /* Prepend backslash */
            char tmp[MAX_MACRO_NAME];
            tmp[0] = '\\';
            safe_strcpy(tmp + 1, clean_name, MAX_MACRO_NAME - 1);
            safe_strcpy(clean_name, tmp, MAX_MACRO_NAME - 1);
            clean_name[MAX_MACRO_NAME - 1] = '\0';
        }
    } else {
        return 0;
    }

    /* Check for optional [n] */
    if (extract_optional_arg(opt_buf, sizeof(opt_buf))) {
        /* Parse digits from opt_buf like "[2]" */
        for (char *p = opt_buf; *p; p++) {
            if (isdigit((unsigned char)*p)) {
                num_args = num_args * 10 + (*p - '0');
            }
        }
        if (num_args > 9) num_args = 9;
    }

    /* Extract {body} */
    if (!extract_braced_group(body, sizeof(body)))
        return 0;

    /* Build macro definition */
    MacroDef m;
    memset(&m, 0, sizeof(m));
    safe_strcpy(m.name, clean_name, MAX_MACRO_NAME - 1);
    m.name[MAX_MACRO_NAME - 1] = '\0';
    m.num_params = num_args;

    /* body has outer braces — strip them for storage */
    int body_len = (int)strlen(body);
    if (body_len >= 2 && body[0] == '{' && body[body_len-1] == '}') {
        memcpy(m.body, body + 1, body_len - 2);
        m.body[body_len - 2] = '\0';
    } else {
        safe_strcpy(m.body, body, MAX_MACRO_BODY - 1);
    }
    m.type = mtype;
    m.active = 1;

    /* For renewcommand, check that macro exists or create it */
    if (mtype == MT_RENEWCOMMAND) {
        MacroDef *existing = db_find(clean_name);
        if (!existing) {
            fprintf(stderr, "WARNING: \\renewcommand{%s}: macro not previously defined\n",
                    clean_name);
        }
    } else if (mtype == MT_NEWCOMMAND) {
        MacroDef *existing = db_find(clean_name);
        if (existing) {
            fprintf(stderr, "WARNING: \\newcommand{%s}: macro already defined\n",
                    clean_name);
        }
    } else if (mtype == MT_PROVIDECOMMAND) {
        /* Only define if not already defined */
        MacroDef *existing = db_find(clean_name);
        if (existing) return 1; /* silently skip */
    }

    db_store(&m);
    return 1;
}

/* Forward declaration for edef/xdef body expansion */
static void process_input(void);

/*
 * def_format:  \def\name#1#2...{body}
 * edef_format: \edef\name#1#2...{body}
 * xdef_format: \xdef\name#1#2...{body}
 */
static int parse_def(MacroType mtype) {
    sc_skip_ws_and_comments();
    if (sc_eof()) return 0;
    if (sc_peek() != '\\') return 0;
    sc_getc(); /* consume backslash */

    /* Extract command name (without backslash) */
    char cmdname[MAX_MACRO_NAME];
    if (!extract_cmdname(cmdname, MAX_MACRO_NAME))
        return 0;

    /* Prepend backslash for storage */
    char fullname[MAX_MACRO_NAME];
    fullname[0] = '\\';
    safe_strcpy(fullname + 1, cmdname, MAX_MACRO_NAME - 2);
    fullname[MAX_MACRO_NAME - 1] = '\0';

    /* Count and skip parameter designators #1, #2, ..., #9 */
    int num_params = 0;

    while (!sc_eof()) {
        sc_skip_ws_and_comments();
        if (sc_eof()) break;

        if (sc_peek() == '{') break;  /* body starts */
        if (sc_peek() == '#') {
            sc_getc(); /* consume # */
            if (!sc_eof() && sc_peek() >= '1' && sc_peek() <= '9') {
                int n = sc_getc() - '0';
                if (n > num_params) num_params = n;
            } else {
                /* Not a param marker — something else */
                break;
            }
        } else {
            break;
        }
    }

    /* Extract body */
    char body[MAX_MACRO_BODY];
    if (!extract_braced_group(body, sizeof(body)))
        return 0;

    /* For \edef and \xdef: expand the body at definition time.
     * We run the body through a simplified expansion pass that
     * resolves all currently-defined macros. */
    if (mtype == MT_EDEF || mtype == MT_XDEF) {
        /* Save scanner state */
        const char *save_p = g_sc.p, *save_pe = g_sc.pe, *save_start = g_sc.start;
        int save_pb_pos = g_pushback_pos, save_pb_len = g_pushback_len;

        /* Save output and redirect to a temp buffer */
        char *save_outbuf = g_outbuf;
        size_t save_outlen = g_outlen, save_outcap = g_outcap;

        /* Use a separate buffer to avoid overlap with body[] */
        char expanded[MAX_MACRO_BODY];
        expanded[0] = '\0';

        /* Strip outer braces for the expansion pass */
        int blen = (int)strlen(body);
        const char *expand_input;
        if (blen >= 2 && body[0] == '{' && body[blen-1] == '}') {
            body[blen-1] = '\0';
            expand_input = body + 1;
        } else {
            expand_input = body;
        }

        /* Set up scanner for body expansion */
        g_sc.p = expand_input;
        g_sc.pe = expand_input + strlen(expand_input);
        g_sc.start = expand_input;
        pushback_clear();

        /* Redirect output to temp buffer */
        g_outbuf = expanded;
        g_outlen = 0;
        g_outcap = sizeof(expanded);

        /* Run the main processing loop on just the body text */
        process_input();
        expanded[g_outlen] = '\0';

        /* Copy expanded result back to body */
        safe_strcpy(body, expanded, MAX_MACRO_BODY - 1);
        body[MAX_MACRO_BODY - 1] = '\0';

        /* Restore scanner */
        g_sc.p = save_p; g_sc.pe = save_pe; g_sc.start = save_start;
        g_pushback_pos = save_pb_pos; g_pushback_len = save_pb_len;

        /* Restore output */
        g_outbuf = save_outbuf;
        g_outlen = save_outlen;
        g_outcap = save_outcap;
    }

    MacroDef m;
    memset(&m, 0, sizeof(m));
    safe_strcpy(m.name, fullname, MAX_MACRO_NAME - 1);
    m.name[MAX_MACRO_NAME - 1] = '\0';
    m.num_params = num_params;
    m.type = mtype;

    int body_len = (int)strlen(body);
    if (body_len >= 2 && body[0] == '{' && body[body_len-1] == '}') {
        memcpy(m.body, body + 1, body_len - 2);
        m.body[body_len - 2] = '\0';
    } else {
        safe_strcpy(m.body, body, MAX_MACRO_BODY - 1);
    }
    m.active = 1;
    db_store(&m);
    return 1;
}

/*
 * let_format: \let\namea\nameb  or  \let\namea=\nameb
 */
static int parse_let(void) {
    sc_skip_ws_and_comments();
    if (sc_eof() || sc_peek() != '\\') return 0;
    sc_getc(); /* consume backslash */

    /* Extract first command name (without backslash) */
    char namea[MAX_MACRO_NAME];
    if (!extract_cmdname(namea, MAX_MACRO_NAME)) {
        /* Single-char command */
        if (!sc_eof()) { namea[0] = sc_getc(); namea[1] = '\0'; }
        else return 0;
    }

    sc_skip_ws_and_comments();
    if (sc_eof()) return 0;

    /* Optional = sign */
    if (sc_peek() == '=') {
        sc_getc();
        sc_skip_ws_and_comments();
    }

    /* Extract second command (target) */
    if (sc_eof() || sc_peek() != '\\') return 0;
    sc_getc(); /* consume backslash */
    char nameb[MAX_MACRO_NAME];
    if (!extract_cmdname(nameb, MAX_MACRO_NAME)) {
        if (!sc_eof()) { nameb[0] = sc_getc(); nameb[1] = '\0'; }
        else return 0;
    }

    /* Store as a let-entry; prepend backslash */
    MacroDef m;
    memset(&m, 0, sizeof(m));
    int na_len = (int)strlen(namea);
    if (na_len >= MAX_MACRO_NAME - 2) na_len = MAX_MACRO_NAME - 2;
    m.name[0] = '\\';
    memcpy(m.name + 1, namea, na_len);
    m.name[1 + na_len] = '\0';

    int nb_len = (int)strlen(nameb);
    if (nb_len >= MAX_MACRO_NAME - 2) nb_len = MAX_MACRO_NAME - 2;
    m.let_target[0] = '\\';
    memcpy(m.let_target + 1, nameb, nb_len);
    m.let_target[1 + nb_len] = '\0';
    m.type = MT_LET;
    m.active = 1;
    db_store(&m);
    return 1;
}

/* ------------------------------------------------------------------ */
/*  Macro expansion                                                   */
/* ------------------------------------------------------------------ */

/*
 * Given a macro definition and arguments, produce the expanded text.
 * Uses g_outbuf append.
 */
static void expand_macro(MacroDef *m, char args[PARAM_COUNT][MAX_MACRO_BODY], int nargs_provided) {
    if (!m) return;

    if (m->type == MT_LET) {
        /* \let copies: resolve the target macro (max 10 levels) */
        MacroDef *target = db_find(m->let_target);
        static int let_depth = 0;
        if (target && target->active && target != m && let_depth < 10) {
            let_depth++;
            expand_macro(target, args, nargs_provided);  /* pass through args */
            let_depth--;
        } else {
            out_append_str(m->let_target);
        }
        return;
    }

    /* Walk through body, substituting #1..#9 with arguments */
    const char *p = m->body;
    while (*p) {
        if (*p == '#') {
            p++;
            if (*p >= '1' && *p <= '9') {
                int n = *p - '0';
                if (n <= nargs_provided && args[n][0]) {
                    out_append_str(args[n]);
                }
                p++;
            } else if (*p == '#') {
                /* ## — output single # */
                out_append_char('#');
                p++;
            } else {
                out_append_char('#');
                out_append_char(*p);
                p++;
            }
        } else {
            out_append_char(*p);
            p++;
        }
    }
}
static void expand_to_pushback(MacroDef *m, char args[PARAM_COUNT][MAX_MACRO_BODY], int np) {
    if (g_expansion_depth >= MAX_EXPANSION_DEPTH) {
        out_append_str(m->name);  /* circuit breaker */
        return;
    }
    g_expansion_depth++;
    size_t save_len = g_outlen;
    expand_macro(m, args, np);
    int expanded = (int)(g_outlen - save_len);
    if (expanded > 0 && pushback_text(g_outbuf + save_len, expanded)) {
        g_outlen = save_len;  /* moved to pushback — roll back output */
    } else if (expanded > 0) {
        /* pushback overflow — trip circuit breaker */
        g_expansion_depth = MAX_EXPANSION_DEPTH;
    }
}

/* ------------------------------------------------------------------ */
/*  Main processing loop                                              */
/* ------------------------------------------------------------------ */

static void process_input(void) {
    g_expansion_depth = 0;  /* reset per file */
    while (!sc_eof()) {
        /* Check for backslash */
        if (sc_peek() != '\\') {
            /* Check for comment */
            if (sc_peek() == '%') {
                /* Pass comment through as-is */
                const char *start = g_sc.p;
                sc_skip_comment();
                out_append(start, (int)(g_sc.p - start));
            } else {
                char c = sc_getc();
                /* Track group depth for \aftergroup */
                if (c == '{') g_group_depth++;
                else if (c == '}') {
                    out_append_char(c);
                    if (g_group_depth > 0) {
                        g_group_depth--;
                        aftergroup_flush();
                    }
                    continue;
                }
                out_append_char(c);
            }
            continue;
        }

        sc_getc(); /* consume \ */

        sc_skip_ws_and_comments();

        /* Now extract the command name */
        if (sc_eof()) {
            out_append_char('\\');
            continue;
        }

        char cmdname[MAX_MACRO_NAME];

        if (!isalpha((unsigned char)sc_peek()) && sc_peek() != '@') {
            /* Single-character command like \\, \{, \$, etc. */
            cmdname[0] = '\\';
            cmdname[1] = sc_getc();
            cmdname[2] = '\0';
        } else {
            cmdname[0] = '\\';
            if (!extract_cmdname(cmdname + 1, MAX_MACRO_NAME - 1)) {
                /* Failed to get cmdname */
                out_append_char('\\');
                continue;
            }
        }

        /* --- Dispatch on command --- */

        /* 1. \newcommand */
        if (!strcmp(cmdname, "\\newcommand")) {
            parse_newcommand(MT_NEWCOMMAND);
            continue;
        }

        /* 2. \renewcommand */
        if (!strcmp(cmdname, "\\renewcommand")) {
            parse_newcommand(MT_RENEWCOMMAND);
            continue;
        }

        /* 2b. \providecommand — like newcommand but no overwrite */
        if (!strcmp(cmdname, "\\providecommand")) {
            parse_newcommand(MT_PROVIDECOMMAND);
            continue;
        }

        /* 3. \def */
        if (!strcmp(cmdname, "\\def")) {
            parse_def(MT_DEF);
            continue;
        }

        /* 4. \edef */
        if (!strcmp(cmdname, "\\edef")) {
            parse_def(MT_EDEF);
            continue;
        }

        /* 5. \xdef */
        if (!strcmp(cmdname, "\\xdef")) {
            parse_def(MT_XDEF);
            continue;
        }

        /* 5b. \gdef — global def */
        if (!strcmp(cmdname, "\\gdef")) {
            parse_def(MT_GDEF);
            continue;
        }

        /* 6. \let */
        if (!strcmp(cmdname, "\\let")) {
            parse_let();
            continue;
        }

        /* 7. \noexpand — suppress expansion of the next token */
        if (!strcmp(cmdname, "\\noexpand")) {
            char tok[MAX_MACRO_BODY];
            if (extract_next_token(tok, sizeof(tok)))
                out_append_str(tok);
            continue;
        }

        /* 8. \expandafter — expand tok2 before tok1.
         * Uses pushback: inserts tok2's expansion then tok1 back into
         * the input stream so that definition commands can re-parse. */
        if (!strcmp(cmdname, "\\expandafter")) {
            char tok1[MAX_MACRO_BODY];
            if (!extract_next_token(tok1, sizeof(tok1)))
                continue;

            /* If tok1 is \expandafter, recurse on the inner chain */
            if (!strcmp(tok1, "\\expandafter")) {
                char tok2[MAX_MACRO_BODY], tok3[MAX_MACRO_BODY];
                if (!extract_next_token(tok2, sizeof(tok2))) {
                    out_append_str(tok1); continue;
                }
                if (!extract_next_token(tok3, sizeof(tok3))) {
                    out_append_str(tok1); out_append_str(tok2); continue;
                }
                /* Expand tok3 */
                MacroDef *m3 = db_find(tok3);
                if (m3 && m3->active) {
                    char dummy[PARAM_COUNT][MAX_MACRO_BODY] = {{0}};
                    expand_to_pushback(m3, dummy, 0);
                } else {
                    pushback_text(tok3, (int)strlen(tok3));
                }
                pushback_text(tok2, (int)strlen(tok2));
                pushback_text(tok1, (int)strlen(tok1));
                continue;
            }

            /* Simple case: \expandafter\tok1\tok2 */
            char tok2[MAX_MACRO_BODY];
            if (!extract_next_token(tok2, sizeof(tok2))) {
                out_append_str(tok1); continue;
            }

            /* Expand tok2 first via pushback capture */
            MacroDef *m2 = db_find(tok2);
            if (m2 && m2->active) {
                char dummy[PARAM_COUNT][MAX_MACRO_BODY] = {{0}};
                expand_to_pushback(m2, dummy, 0);
            } else {
                pushback_text(tok2, (int)strlen(tok2));
            }
            pushback_text(tok1, (int)strlen(tok1));
            continue;
        }

        /* 9. \aftergroup — defer token until current group ends */
        if (!strcmp(cmdname, "\\aftergroup")) {
            char tok[MAX_MACRO_BODY];
            if (extract_next_token(tok, sizeof(tok)))
                aftergroup_add(tok);
            continue;
        }

        /* --- User-defined macro call --- */
        MacroDef *m = db_find(cmdname);
        if (m && m->active) {
            /* Resolve \let chains to get real param count */
            MacroDef *resolved = m;
            int let_chain = 0;
            while (resolved && resolved->type == MT_LET && let_chain < 10) {
                resolved = db_find(resolved->let_target);
                let_chain++;
            }
            int np = resolved ? resolved->num_params : m->num_params;

            /* Extract arguments */
            char args[PARAM_COUNT][MAX_MACRO_BODY];
            memset(args, 0, sizeof(args));

            for (int i = 1; i <= np && i <= 9; i++) {
                if (!sc_eof()) {
                    extract_macro_arg(args[i], MAX_MACRO_BODY);
                }
            }

            expand_to_pushback(m, args, np);
            continue;
        }

        /* --- Not a recognized command — pass through --- */
        out_append_str(cmdname);
    }
}

/* ------------------------------------------------------------------ */
/*  Report mode                                                       */
/* ------------------------------------------------------------------ */
static int g_report_mode = 0;

static void print_report(const char *filename) {
    if (!g_report_mode) return;
    (void)filename; /* used only for section header */
    printf("\n=== %s ===\n", filename);
    for (int i = 0; i < g_db.count; i++) {
        MacroDef *m = &g_db.entries[i];
        if (!m->active) continue;
        const char *typestr = "?";
        switch (m->type) {
            case MT_NEWCOMMAND:      typestr = "newcommand"; break;
            case MT_RENEWCOMMAND:    typestr = "renewcommand"; break;
            case MT_PROVIDECOMMAND:  typestr = "providecommand"; break;
            case MT_DEF:             typestr = "def"; break;
            case MT_EDEF:            typestr = "edef"; break;
            case MT_XDEF:            typestr = "xdef"; break;
            case MT_GDEF:            typestr = "gdef"; break;
            case MT_LET:             typestr = "let"; break;
            default: break;
        }
        if (m->type == MT_LET) {
            printf("  %-18s => %-24s [%s, %d args]\n",
                   m->name, m->let_target, typestr, m->num_params);
        } else {
            /* Print body, replacing newlines for readability */
            char display_body[MAX_MACRO_BODY];
            safe_strcpy(display_body, m->body, MAX_MACRO_BODY - 1);
            display_body[MAX_MACRO_BODY - 1] = '\0';
            for (char *p = display_body; *p; p++)
                if (*p == '\n' || *p == '\r') *p = ' ';
            printf("  %-18s => %s [%s, %d args]\n",
                   m->name, display_body, typestr, m->num_params);
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Main                                                              */
/* ------------------------------------------------------------------ */
int main(int argc, char *argv[]) {
    if (argc >= 2 && !strcmp(argv[1], "--report")) {
        g_report_mode = 1;
    }

    if (g_report_mode && argc >= 3) {
        /* Report mode: process each file, print definitions found */
        for (int fi = 2; fi < argc; fi++) {
            FILE *fp = fopen(argv[fi], "rb");
            if (!fp) {
                fprintf(stderr, "ERROR: cannot open %s\n", argv[fi]);
                continue;
            }
            fseek(fp, 0, SEEK_END);
            long fsize = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            if (fsize <= 0 || fsize > MAX_INPUT_SIZE) {
                fprintf(stderr, "ERROR: %s too large or empty (%ld bytes)\n",
                        argv[fi], fsize);
                fclose(fp);
                continue;
            }
            char *inbuf = (char *)malloc(fsize + 1);
            if (!inbuf) {
                fclose(fp);
                continue;
            }
            fread(inbuf, 1, fsize, fp);
            fclose(fp);
            inbuf[fsize] = '\0';

            db_init();
            out_init();
            g_sc.p = inbuf;
            g_sc.pe = inbuf + fsize;
            g_sc.start = inbuf;

            process_input();
            print_report(argv[fi]);

            free(inbuf);
            free(g_outbuf);
        }
        return 0;
    }

    /* Default: stdin to stdout expansion mode */
    /* Read entire stdin */
    char *inbuf = (char *)malloc(MAX_INPUT_SIZE);
    if (!inbuf) {
        fprintf(stderr, "ERROR: out of memory\n");
        return 1;
    }

    size_t total = 0;
    size_t n;
    while ((n = fread(inbuf + total, 1, MAX_INPUT_SIZE - total - 1, stdin)) > 0) {
        total += n;
        if (total >= MAX_INPUT_SIZE - 1) break;
    }
    inbuf[total] = '\0';

    db_init();
    out_init();
    g_sc.p = inbuf;
    g_sc.pe = inbuf + total;
    g_sc.start = inbuf;

    process_input();

    fwrite(g_outbuf, 1, g_outlen, stdout);

    free(g_outbuf);
    free(inbuf);
    return 0;
}
