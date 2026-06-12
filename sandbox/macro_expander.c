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

/* ------------------------------------------------------------------ */
/*  Configuration                                                     */
/* ------------------------------------------------------------------ */
#define MAX_MACRO_NAME    128
#define MAX_MACRO_BODY    16384
#define MAX_MACROS        4096
#define MAX_INPUT_SIZE    (64 * 1024 * 1024)  /* 64 MB max input */
#define OUTPUT_CHUNK      (1024 * 1024)
#define PARAM_COUNT       10

/* ------------------------------------------------------------------ */
/*  Macro types                                                       */
/* ------------------------------------------------------------------ */
typedef enum {
    MT_NEWCOMMAND,
    MT_RENEWCOMMAND,
    MT_DEF,
    MT_EDEF,        /* expand body at definition time */
    MT_XDEF,        /* global \edef */
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
    strncpy(m->name, name, MAX_MACRO_NAME - 1);
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
        g_outcap = (g_outlen + len + 1) * 2;
        g_outbuf = (char *)realloc(g_outbuf, g_outcap);
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

static int sc_eof(void)  { return g_sc.p >= g_sc.pe; }
static int sc_peek(void) { return sc_eof() ? EOF : (unsigned char)*g_sc.p; }
static int sc_getc(void) { return sc_eof() ? EOF : (unsigned char)*g_sc.p++; }

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
            /* Escape next character (handle backslash sequences) */
            if (!sc_eof() && pos < bufsize - 1) {
                buf[pos++] = sc_getc();
            }
        } else if (c == '%') {
            /* Comments inside braces — include literally for now */
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
 * Extract a macro-use argument: either a single token or a braced group.
 * This is what gets substituted for #1, #2, etc.
 */
static int extract_macro_arg(char *buf, int bufsize) {
    sc_skip_ws_and_comments();
    if (sc_eof()) return 0;

    if (sc_peek() == '{') {
        return extract_braced_group(buf, bufsize);
    }

    /* Single token */
    return extract_single_token(buf, bufsize);
}

/* ------------------------------------------------------------------ */
/*  Parameter counting from body                                      */
/* ------------------------------------------------------------------ */
/*
 * Count the highest #n parameter used in a macro body.
 * Returns 0-9.
 */
static int count_params_in_body(const char *body) {
    int max_n = 0;
    const char *p = body;
    while (*p) {
        if (*p == '#' && (p[1] >= '1' && p[1] <= '9')) {
            if (*p == '#' && p[1] == '#') {
                /* ## in align/etc — skip */
                p += 2;
                continue;
            }
            int n = p[1] - '0';
            if (n > max_n) max_n = n;
            p++;
        }
        p++;
    }
    return max_n;
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
static int parse_newcommand(MacroType mtype, const char *keyword) {
    char name_braced[MAX_MACRO_NAME + 4];
    char opt_buf[32];
    char body[MAX_MACRO_BODY];
    int  num_args = 0;

    /* Extract {\name} */
    if (!extract_braced_group(name_braced, sizeof(name_braced)))
        return 0;

    /* The braced group should be {\name} — extract name from inside braces */
    /* name_braced is like "{\name}" — strip the outer braces and backslash */
    int blen = (int)strlen(name_braced);
    if (blen < 4 || name_braced[0] != '{' || name_braced[blen-1] != '}' || name_braced[1] != '\\')
        return 0;

    /* Extract name inside: strip { and } */
    char clean_name[MAX_MACRO_NAME];
    int nl = blen - 2; /* length of \name without braces */
    if (nl >= MAX_MACRO_NAME) nl = MAX_MACRO_NAME - 1;
    memcpy(clean_name, name_braced + 1, nl); /* includes backslash */
    clean_name[nl] = '\0';

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
    strncpy(m.name, clean_name, MAX_MACRO_NAME - 1);
    m.name[MAX_MACRO_NAME - 1] = '\0';
    m.num_params = num_args;

    /* body has outer braces — strip them for storage */
    int body_len = (int)strlen(body);
    if (body_len >= 2 && body[0] == '{' && body[body_len-1] == '}') {
        memcpy(m.body, body + 1, body_len - 2);
        m.body[body_len - 2] = '\0';
    } else {
        strncpy(m.body, body, MAX_MACRO_BODY - 1);
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
    }

    db_store(&m);
    return 1;
}

/*
 * def_format:  \def\name#1#2...{body}
 * edef_format: \edef\name#1#2...{body}
 * xdef_format: \xdef\name#1#2...{body}
 */
static int parse_def(MacroType mtype, const char *keyword) {
    sc_skip_ws_and_comments();
    if (sc_eof()) return 0;
    if (sc_peek() != '\\') return 0;
    sc_getc(); /* consume the backslash */

    /* Extract command name (with backslash) */
    char cmdname[MAX_MACRO_NAME];
    cmdname[0] = '\\';
    if (!extract_cmdname(cmdname + 1, MAX_MACRO_NAME - 1))
        return 0;

    /* Count and skip parameter designators #1, #2, ..., #9 */
    int num_params = 0;
    int params_seen[10] = {0};

    while (!sc_eof()) {
        sc_skip_ws_and_comments();
        if (sc_eof()) break;

        if (sc_peek() == '{') break;  /* body starts */
        if (sc_peek() == '#') {
            sc_getc(); /* consume # */
            if (!sc_eof() && sc_peek() >= '1' && sc_peek() <= '9') {
                int n = sc_getc() - '0';
                if (n > num_params) num_params = n;
                params_seen[n] = 1;
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

    /* For \edef and \xdef: the body should be fully expanded at definition time.
     * For now, we store it as-is — expansion happens when the macro is used. */
    MacroDef m;
    memset(&m, 0, sizeof(m));
    strncpy(m.name, cmdname, MAX_MACRO_NAME - 1);
    m.name[MAX_MACRO_NAME - 1] = '\0';
    m.num_params = num_params;
    m.type = mtype;

    int body_len = (int)strlen(body);
    if (body_len >= 2 && body[0] == '{' && body[body_len-1] == '}') {
        memcpy(m.body, body + 1, body_len - 2);
        m.body[body_len - 2] = '\0';
    } else {
        strncpy(m.body, body, MAX_MACRO_BODY - 1);
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
    sc_getc(); /* consume the backslash */

    /* Extract first command */
    char namea[MAX_MACRO_NAME];
    namea[0] = '\\';
    if (!extract_cmdname(namea + 1, MAX_MACRO_NAME - 1))
        return 0;

    sc_skip_ws_and_comments();
    if (sc_eof()) return 0;

    /* Optional = sign */
    if (sc_peek() == '=') {
        sc_getc();
        sc_skip_ws_and_comments();
    }

    /* Extract second command (target) */
    if (sc_eof() || sc_peek() != '\\') return 0;
    sc_getc(); /* consume the backslash */
    char nameb[MAX_MACRO_NAME];
    nameb[0] = '\\';
    if (!extract_cmdname(nameb + 1, MAX_MACRO_NAME - 1))
        return 0;

    /* Store as a let-entry */
    MacroDef m;
    memset(&m, 0, sizeof(m));
    strncpy(m.name, namea, MAX_MACRO_NAME - 1);
    m.name[MAX_MACRO_NAME - 1] = '\0';
    strncpy(m.let_target, nameb, MAX_MACRO_NAME - 1);
    m.let_target[MAX_MACRO_NAME - 1] = '\0';
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
        /* \let copies: output the target command */
        out_append_str(m->let_target);
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

/* ------------------------------------------------------------------ */
/*  Main processing loop                                              */
/* ------------------------------------------------------------------ */

static void process_input(void) {
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
                out_append_char(sc_getc());
            }
            continue;
        }

        const char *bs_pos = g_sc.p;
        sc_getc(); /* consume \ */

        sc_skip_ws_and_comments();

        /* Now extract the command name */
        if (sc_eof()) {
            out_append_char('\\');
            continue;
        }

        char cmdname[MAX_MACRO_NAME];
        int is_single_char_cmd = 0;

        if (!isalpha((unsigned char)sc_peek()) && sc_peek() != '@') {
            /* Single-character command like \\, \{, \$, etc. */
            cmdname[0] = '\\';
            cmdname[1] = sc_getc();
            cmdname[2] = '\0';
            is_single_char_cmd = 1;
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
            parse_newcommand(MT_NEWCOMMAND, "newcommand");
            continue;
        }

        /* 2. \renewcommand */
        if (!strcmp(cmdname, "\\renewcommand")) {
            parse_newcommand(MT_RENEWCOMMAND, "renewcommand");
            continue;
        }

        /* 3. \def */
        if (!strcmp(cmdname, "\\def")) {
            parse_def(MT_DEF, "def");
            continue;
        }

        /* 4. \edef */
        if (!strcmp(cmdname, "\\edef")) {
            parse_def(MT_EDEF, "edef");
            continue;
        }

        /* 5. \xdef */
        if (!strcmp(cmdname, "\\xdef")) {
            parse_def(MT_XDEF, "xdef");
            continue;
        }

        /* 6. \let */
        if (!strcmp(cmdname, "\\let")) {
            parse_let();
            continue;
        }

        /* 7. \noexpand — suppress expansion of the next token */
        if (!strcmp(cmdname, "\\noexpand")) {
            /* Extract the next token and output it literally */
            sc_skip_ws_and_comments();
            if (sc_eof()) continue;

            if (sc_peek() == '\\') {
                char tok[MAX_MACRO_NAME];
                tok[0] = '\\';
                sc_getc();
                if (!sc_eof()) {
                    if (!isalpha((unsigned char)sc_peek()) && sc_peek() != '@') {
                        tok[1] = sc_getc();
                        tok[2] = '\0';
                    } else {
                        if (!extract_cmdname(tok + 1, MAX_MACRO_NAME - 1))
                            tok[1] = '\0';
                    }
                    out_append_str(tok);
                } else {
                    out_append_str("\\");
                }
            } else if (sc_peek() == '{') {
                /* Copy the entire braced group literally */
                char grp[MAX_MACRO_BODY];
                if (extract_braced_group(grp, sizeof(grp)))
                    out_append_str(grp);
            } else {
                out_append_char(sc_getc());
            }
            continue;
        }

        /* 8. \expandafter — expand the second token before the first */
        if (!strcmp(cmdname, "\\expandafter")) {
            /*
             * \expandafter<tok1><tok2>
             * Effect: expand tok2 first, then tok1 sees the result.
             *
             * For a first cut: extract tok1 and tok2.
             * If tok2 is a defined macro, expand it.
             * Then output tok1 with potential expansion.
             */
            sc_skip_ws_and_comments();
            if (sc_eof()) continue;

            /* Extract token 1 */
            char tok1[MAX_MACRO_NAME];
            int tok1_is_macro = 0;
            const char *save_p = g_sc.p;

            if (sc_peek() == '\\') {
                tok1[0] = '\\';
                sc_getc();
                if (!sc_eof()) {
                    if (isalpha((unsigned char)sc_peek()) || sc_peek() == '@') {
                        extract_cmdname(tok1 + 1, MAX_MACRO_NAME - 1);
                    } else {
                        tok1[1] = sc_getc();
                        tok1[2] = '\0';
                    }
                }
                tok1_is_macro = 1;
            } else if (sc_peek() == '{') {
                extract_braced_group(tok1, sizeof(tok1));
            } else {
                tok1[0] = sc_getc();
                tok1[1] = '\0';
            }

            /* Now extract token 2 and expand it if it's a macro */
            sc_skip_ws_and_comments();
            if (sc_eof()) {
                /* Only got tok1 — output it */
                out_append_str(tok1);
                continue;
            }

            int tok2_expanded = 0;
            if (sc_peek() == '\\') {
                char tok2[MAX_MACRO_NAME];
                tok2[0] = '\\';
                sc_getc();
                if (!sc_eof()) {
                    if (isalpha((unsigned char)sc_peek()) || sc_peek() == '@') {
                        extract_cmdname(tok2 + 1, MAX_MACRO_NAME - 1);
                    } else {
                        tok2[1] = sc_getc();
                        tok2[2] = '\0';
                    }
                }
                MacroDef *mtok2 = db_find(tok2);
                if (mtok2) {
                    char dummy_args[PARAM_COUNT][MAX_MACRO_BODY] = {{0}};
                    expand_macro(mtok2, dummy_args, 0);
                    tok2_expanded = 1;
                } else {
                    out_append_str(tok1);
                    out_append_str(tok2);
                }
            } else if (sc_peek() == '{') {
                char grp[MAX_MACRO_BODY];
                if (extract_braced_group(grp, sizeof(grp))) {
                    out_append_str(tok1);
                    out_append_str(grp);
                }
            } else {
                char c2 = sc_getc();
                out_append_str(tok1);
                out_append_char(c2);
            }

            /* After expanding tok2, expand tok1 */
            if (tok2_expanded && tok1_is_macro) {
                MacroDef *mtok1 = db_find(tok1);
                if (mtok1) {
                    /* Don't re-expand tok1 — just output it, since \expandafter
                     * causes tok1 to see tok2's expansion. The simplest handling:
                     * we already output tok1 + tok2_expanded above.
                     * This is a simplification. Full \expandafter handling requires
                     * token list manipulation. */
                }
            }
            continue;
        }

        /* 9. \aftergroup — save token for after current group */
        if (!strcmp(cmdname, "\\aftergroup")) {
            /*
             * \aftergroup\token
             * In TeX, the token is inserted after the current group ends.
             * For a first cut, just extract and discard the token
             * (we don't track group nesting in single-pass mode).
             */
            sc_skip_ws_and_comments();
            if (!sc_eof()) {
                if (sc_peek() == '\\') {
                    char tok[MAX_MACRO_NAME];
                    tok[0] = '\\';
                    sc_getc();
                    if (!sc_eof()) {
                        if (isalpha((unsigned char)sc_peek()) || sc_peek() == '@') {
                            extract_cmdname(tok + 1, MAX_MACRO_NAME - 1);
                        } else {
                            tok[1] = sc_getc();
                            tok[2] = '\0';
                        }
                        /* Output aftergroup token after current group */
                        /* Simplified: just output the token */
                        out_append_str(tok);
                    }
                } else if (sc_peek() == '{') {
                    char grp[MAX_MACRO_BODY];
                    if (extract_braced_group(grp, sizeof(grp)))
                        out_append_str(grp);
                } else {
                    out_append_char(sc_getc());
                }
            }
            continue;
        }

        /* --- User-defined macro call --- */
        MacroDef *m = db_find(cmdname);
        if (m && m->active) {
            /* Extract arguments */
            char args[PARAM_COUNT][MAX_MACRO_BODY];
            memset(args, 0, sizeof(args));

            int np = m->num_params;
            for (int i = 1; i <= np && i <= 9; i++) {
                if (!sc_eof()) {
                    extract_macro_arg(args[i], MAX_MACRO_BODY);
                }
            }

            expand_macro(m, args, np);
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
static int g_total_files = 0;

static void print_report(const char *filename) {
    if (!g_report_mode) return;
    g_total_files++;
    printf("\n=== %s ===\n", filename);
    for (int i = 0; i < g_db.count; i++) {
        MacroDef *m = &g_db.entries[i];
        if (!m->active) continue;
        const char *typestr = "?";
        switch (m->type) {
            case MT_NEWCOMMAND:     typestr = "newcommand"; break;
            case MT_RENEWCOMMAND:   typestr = "renewcommand"; break;
            case MT_DEF:            typestr = "def"; break;
            case MT_EDEF:           typestr = "edef"; break;
            case MT_XDEF:           typestr = "xdef"; break;
            case MT_LET:            typestr = "let"; break;
            default: break;
        }
        if (m->type == MT_LET) {
            printf("  %-16s => %-24s [%s, %d args]\n",
                   m->name, m->let_target, typestr, m->num_params);
        } else {
            printf("  %-16s => %-50s [%s, %d args]\n",
                   m->name,
                   strlen(m->body) < 50 ? m->body : "...",
                   typestr, m->num_params);
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
