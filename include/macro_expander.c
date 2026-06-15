#include "macro_expander.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define CMD_ESCAPE        0
#define CMD_RELAX         0
#define CMD_LEFT_BRACE    1
#define CMD_RIGHT_BRACE   2
#define CMD_MATH_SHIFT    3
#define CMD_TAB_MARK      4
#define CMD_CAR_RET       5
#define CMD_OUT_PARAM     5
#define CMD_MAC_PARAM     6
#define CMD_SUP_MARK      7
#define CMD_SUB_MARK      8
#define CMD_IGNORE        9
#define CMD_SPACER        10
#define CMD_LETTER        11
#define CMD_OTHER_CHAR    12
#define CMD_ACTIVE_CHAR   13
#define CMD_COMMENT       14
#define CMD_INVALID_CHAR  15

#define MAX_CHAR_CODE     15
#define MAX_COMMAND       102
#define CS_TOKEN_FLAG     0x1FFFFFF
#define MAX_CHAR_VAL      0x200000
#define TOKEN(cmd, chr)   ((int32_t)((cmd) * MAX_CHAR_VAL + (chr)))
#define TOKEN_CMD(t)      ((int)((t) / MAX_CHAR_VAL))
#define TOKEN_CHR(t)      ((int)((t) % MAX_CHAR_VAL))
#define TOKEN_IS_CS(t)    ((t) >= CS_TOKEN_FLAG)
#define TOKEN_CS_IDX(t)   ((int)((t) - CS_TOKEN_FLAG))
#define LEFT_BRACE_TOKEN    TOKEN(CMD_LEFT_BRACE, '{')
#define RIGHT_BRACE_TOKEN   TOKEN(CMD_RIGHT_BRACE, '}')
#define SPACE_TOKEN         TOKEN(CMD_SPACER, ' ')
#define MATCH_TOKEN_BASE    TOKEN(CMD_MATCH, 0)
#define END_MATCH_TOKEN     TOKEN(CMD_END_MATCH, 0)
#define OUT_PARAM_TOKEN_BASE TOKEN(CMD_OUT_PARAM, 0)
#define PROTECTED_TOKEN     (END_MATCH_TOKEN + 1)

#define CMD_UNDEFINED_CS     (MAX_COMMAND + 1)
#define CMD_EXPAND_AFTER     (MAX_COMMAND + 2)
#define CMD_NO_EXPAND        (MAX_COMMAND + 3)
#define CMD_CS_NAME          (MAX_COMMAND + 7)
#define CMD_END_CS_NAME      (MAX_COMMAND + 7 + 60)
#define CMD_THE              (MAX_COMMAND + 9)
#define CMD_CALL             (MAX_COMMAND + 11)
#define CMD_LONG_CALL        (MAX_COMMAND + 12)
#define CMD_OUTER_CALL       (MAX_COMMAND + 13)
#define CMD_LONG_OUTER_CALL  (MAX_COMMAND + 14)
#define CMD_DONT_EXPAND      (MAX_COMMAND + 16)

#define CMD_DEF_PRIM         200
#define CMD_EDEF_PRIM        201
#define CMD_XDEF_PRIM        202
#define CMD_GDEF_PRIM        203
#define CMD_NEWCOMMAND_PRIM  204
#define CMD_RENEWCOMMAND_PRIM 205
#define CMD_PROVIDECOMMAND_PRIM 206
#define CMD_LET_PRIM         207
#define CMD_AFTERGROUP_PRIM  208
#define CMD_MAKETLETTER_PRIM  209
#define CMD_MAKETOTHER_PRIM   210

#define IS_EXPANDABLE(cmd) ((cmd) > MAX_COMMAND && (cmd) != CMD_END_CS_NAME)

#define CMD_MATCH           13
#define CMD_END_MATCH       14

#define MAX_MACRO_NAME      256
#define HASH_SIZE           4096
#define HASH_MASK           (HASH_SIZE - 1)
#define NODE_POOL_SIZE      1048576
#define INPUT_STACK_SIZE    4096
#define PARAM_STACK_SIZE    256
#define MAX_EXPANSION_DEPTH 10000

typedef struct token_node {
    int32_t            info;
    struct token_node *link;
} token_node_t;

typedef struct {
    char     *name;
    int       cmd;
    intptr_t  chr;
} eq_entry_t;

typedef enum { INPUT_FILE, INPUT_TOKEN_LIST } input_state_t;

typedef struct {
    input_state_t  state;
    const char    *buffer;
    int            buf_loc, buf_limit;
    token_node_t  *tok_head, *tok_loc;
} input_frame_t;

struct macro_expander {
    token_node_t  node_pool[NODE_POOL_SIZE];
    int           node_pool_free;
    token_node_t *avail;

    eq_entry_t    eqtb[HASH_SIZE];
    int           eqtb_count;

    input_frame_t input_stack[INPUT_STACK_SIZE];
    int           input_ptr;

    token_node_t *param_stack[PARAM_STACK_SIZE];
    int           param_ptr;

    int           cur_cmd, cur_chr, cur_cs;
    int32_t       cur_tok;
    int           scanner_state;

    int           cat_code[128];

    int           scanner_status;
    token_node_t *def_ref;

    int           expansion_depth;

    char         *out_buf;
    size_t        out_len;
    size_t        out_cap;
};

static unsigned hash_str(const char *s) {
    unsigned h = 5381;
    while (*s) h = ((h << 5) + h) + (unsigned char)*s++;
    return h;
}

static token_node_t *new_node(macro_expander_t *me) {
    token_node_t *n;
    if (me->avail) {
        n = me->avail;
        me->avail = me->avail->link;
    } else if (me->node_pool_free < NODE_POOL_SIZE) {
        n = &me->node_pool[me->node_pool_free++];
    } else {
        fprintf(stderr, "macro_expander: node pool exhausted (%d nodes)\n", NODE_POOL_SIZE);
        return NULL;
    }
    n->info = 0;
    n->link = NULL;
    return n;
}

static void free_node(macro_expander_t *me, token_node_t *n) {
    n->link = me->avail;
    me->avail = n;
}

static token_node_t *new_token_list(macro_expander_t *me) {
    token_node_t *head = new_node(me);
    if (!head) return NULL;
    head->info = 1;
    head->link = NULL;
    return head;
}

static void token_list_unref(macro_expander_t *me, token_node_t *head) {
    if (!head) return;
    if (--head->info <= 0) {
        token_node_t *p = head->link;
        while (p) {
            token_node_t *next = p->link;
            free_node(me, p);
            p = next;
        }
        free_node(me, head);
    }
}

static token_node_t *token_list_append(macro_expander_t *me, token_node_t *tail, int32_t tok) {
    token_node_t *n = new_node(me);
    if (!n) return tail;
    n->info = tok;
    tail->link = n;
    return n;
}

static int eq_lookup(macro_expander_t *me, const char *name) {
    unsigned h = hash_str(name) & HASH_MASK;
    for (int probe = 0; probe < HASH_SIZE; probe++) {
        int idx = (h + probe) & HASH_MASK;
        if (!me->eqtb[idx].name) return -1;
        if (strcmp(me->eqtb[idx].name, name) == 0) return idx;
    }
    return -1;
}

static int eq_define(macro_expander_t *me, const char *name, int cmd, intptr_t chr) {
    int existing = eq_lookup(me, name);
    if (existing >= 0) {
        if (me->eqtb[existing].cmd == CMD_CALL ||
            me->eqtb[existing].cmd == CMD_LONG_CALL ||
            me->eqtb[existing].cmd == CMD_OUTER_CALL ||
            me->eqtb[existing].cmd == CMD_LONG_OUTER_CALL) {
            token_list_unref(me, (token_node_t *)me->eqtb[existing].chr);
        }
        me->eqtb[existing].cmd = cmd;
        me->eqtb[existing].chr = chr;
        return existing;
    }
    unsigned h = hash_str(name) & HASH_MASK;
    for (int probe = 0; probe < HASH_SIZE; probe++) {
        int idx = (h + probe) & HASH_MASK;
        if (!me->eqtb[idx].name) {
            me->eqtb[idx].name = strdup(name);
            if (!me->eqtb[idx].name) {
                fprintf(stderr, "macro_expander: out of memory (eq_define)\n");
                return -1;
            }
            me->eqtb[idx].cmd = cmd;
            me->eqtb[idx].chr = chr;
            me->eqtb_count++;
            return idx;
        }
    }
    fprintf(stderr, "macro_expander: hash table full\n");
    return -1;
}

static void input_push_string(macro_expander_t *me, const char *str) {
    if (me->input_ptr >= INPUT_STACK_SIZE) {
        fprintf(stderr, "macro_expander: input stack overflow\n");
        return;
    }
    input_frame_t *f = &me->input_stack[me->input_ptr++];
    f->state = INPUT_FILE;
    f->buffer = str;
    f->buf_loc = 0;
    f->buf_limit = (int)strlen(str);
    f->tok_head = NULL;
    f->tok_loc = NULL;
}

static void input_push_token_list(macro_expander_t *me, token_node_t *head) {
    if (me->input_ptr >= INPUT_STACK_SIZE) {
        fprintf(stderr, "macro_expander: input stack overflow\n");
        return;
    }
    head->info++;
    input_frame_t *f = &me->input_stack[me->input_ptr++];
    f->state = INPUT_TOKEN_LIST;
    f->buffer = NULL;
    f->tok_head = head;
    f->tok_loc = head ? head->link : NULL;
}

static void input_pop(macro_expander_t *me) {
    if (me->input_ptr > 0) {
        me->input_ptr--;
        if (me->input_stack[me->input_ptr].state == INPUT_TOKEN_LIST &&
            me->input_stack[me->input_ptr].tok_head) {
            token_list_unref(me, me->input_stack[me->input_ptr].tok_head);
        }
        if (me->input_stack[me->input_ptr].state == INPUT_FILE &&
            me->input_stack[me->input_ptr].buffer) {
            free((void *)me->input_stack[me->input_ptr].buffer);
            me->input_stack[me->input_ptr].buffer = NULL;
        }
    }
}

static void catcode_defaults(macro_expander_t *me) {
    for (int i = 0; i < 128; i++) me->cat_code[i] = CMD_OTHER_CHAR;
    me->cat_code['\\'] = CMD_ESCAPE;
    me->cat_code['{']  = CMD_LEFT_BRACE;
    me->cat_code['}']  = CMD_RIGHT_BRACE;
    me->cat_code['#']  = CMD_MAC_PARAM;
    me->cat_code['^']  = CMD_SUP_MARK;
    me->cat_code['~']  = CMD_ACTIVE_CHAR;
    me->cat_code['%']  = CMD_COMMENT;
    me->cat_code[' ']  = CMD_SPACER;
    me->cat_code['\n'] = CMD_CAR_RET;  // NEWLINE: preserve newlines (was CMD_SPACER)
    me->cat_code['\r'] = CMD_IGNORE;
    me->cat_code['\t'] = CMD_SPACER;
    for (int c = 'a'; c <= 'z'; c++) me->cat_code[c] = CMD_LETTER;
    for (int c = 'A'; c <= 'Z'; c++) me->cat_code[c] = CMD_LETTER;
    me->cat_code['@'] = CMD_LETTER;
    me->cat_code[0] = CMD_IGNORE;
}

static void expand(macro_expander_t *me);
static void get_next(macro_expander_t *me);
static token_node_t *scan_toks(macro_expander_t *me, int macro_def, int xpand);
static void get_x_token(macro_expander_t *me);

static int32_t scan_control_sequence(macro_expander_t *me, input_frame_t *f) {
    char csname[256];
    int cslen = 0, c, cat;

    if (f->buf_loc >= f->buf_limit) return SPACE_TOKEN;

    c = (unsigned char)f->buffer[f->buf_loc];
    cat = (c < 128) ? me->cat_code[c] : CMD_OTHER_CHAR;

    if (cat == CMD_LETTER) {
        while (f->buf_loc < f->buf_limit && cat == CMD_LETTER) {
            if (cslen < 255) csname[cslen++] = (char)c;
            f->buf_loc++;
            if (f->buf_loc < f->buf_limit) {
                c = (unsigned char)f->buffer[f->buf_loc];
                cat = (c < 128) ? me->cat_code[c] : CMD_OTHER_CHAR;
            }
        }
    } else {
        csname[cslen++] = (char)c;
        f->buf_loc++;
    }
    csname[cslen] = '\0';

    int cs = eq_lookup(me, csname);
    if (cs < 0) cs = eq_define(me, csname, CMD_UNDEFINED_CS, 0);
    if (cs < 0) return SPACE_TOKEN;
    return CS_TOKEN_FLAG + cs;
}

static int32_t scan_sup_mark(macro_expander_t *me, input_frame_t *f) {
    (void)me;
    if (f->buf_loc >= f->buf_limit) return -1;
    if ((unsigned char)f->buffer[f->buf_loc] != '^') return -1;

    int sup_count = 2;
    while (sup_count < 6 &&
           f->buf_loc + (sup_count - 1) < f->buf_limit &&
           f->buffer[f->buf_loc + (sup_count - 1)] == '^')
        sup_count++;

    int saved = f->buf_loc;
    f->buf_loc += (sup_count - 1);

    for (int d = 0; d < sup_count; d++) {
        if (f->buf_loc + d >= f->buf_limit) { f->buf_loc = saved; return -1; }
        int hc = (unsigned char)f->buffer[f->buf_loc + d];
        if (!((hc >= '0' && hc <= '9') || (hc >= 'a' && hc <= 'f')))
            { f->buf_loc = saved; return -1; }
    }

    int val = 0;
    for (int d = 0; d < sup_count; d++) {
        int hc = (unsigned char)f->buffer[f->buf_loc++];
        val <<= 4;
        val += (hc <= '9') ? hc - '0' : hc - 'a' + 10;
    }
    f->buf_loc--;

    if (val > 255) return -1;
    return TOKEN(CMD_OTHER_CHAR, val);
}

static int32_t get_token_from_string(macro_expander_t *me) {
    input_frame_t *f = &me->input_stack[me->input_ptr - 1];
    int c, cat;

    if (f->buf_loc >= f->buf_limit) { me->scanner_state = 3 + MAX_CHAR_CODE + MAX_CHAR_CODE; return -1; }
    c = (unsigned char)f->buffer[f->buf_loc++];

    if (c == '\r') {
        if (f->buf_loc < f->buf_limit && f->buffer[f->buf_loc] == '\n')
            f->buf_loc++;
        c = '\n';
    }

    cat = (c < 128) ? me->cat_code[c] : CMD_OTHER_CHAR;

    if (cat == CMD_COMMENT || cat == CMD_IGNORE || cat == CMD_INVALID_CHAR)
        return SPACE_TOKEN;
    if (cat == CMD_SPACER)
        return SPACE_TOKEN;
    if (cat == CMD_CAR_RET)
        return TOKEN(CMD_CAR_RET, '\n');  // Preserve newlines

    if (cat == CMD_ESCAPE)
        return scan_control_sequence(me, f);

    if (cat == CMD_ACTIVE_CHAR) {
        int cs = eq_lookup(me, "~");
        if (cs >= 0) return CS_TOKEN_FLAG + cs;
    }

    if (cat == CMD_SUP_MARK) {
        int32_t result = scan_sup_mark(me, f);
        if (result >= 0) {
            c = TOKEN_CHR(result);
            cat = (c < 128) ? me->cat_code[c] : CMD_OTHER_CHAR;
            if (cat == CMD_SPACER) return SPACE_TOKEN;
            if (cat == CMD_CAR_RET) return TOKEN(CMD_CAR_RET, '\n');  // Preserve newlines
            if (cat == CMD_ESCAPE) return scan_control_sequence(me, f);
        }
    }

    return TOKEN(cat, c);
}

static void get_next(macro_expander_t *me) {
    int32_t t;
restart:
    me->cur_cs = 0;
    if (me->input_ptr == 0) { me->cur_cmd = CMD_RELAX; me->cur_chr = 0; return; }

    input_frame_t *cur = &me->input_stack[me->input_ptr - 1];
    if (cur->state == INPUT_TOKEN_LIST) {
        if (!cur->tok_loc) { input_pop(me); goto restart; }
        t = cur->tok_loc->info;
        cur->tok_loc = cur->tok_loc->link;
    } else {
        t = get_token_from_string(me);
        if (t < 0) { input_pop(me); goto restart; }
    }

    if (TOKEN_IS_CS(t)) {
        me->cur_cs = TOKEN_CS_IDX(t);
        me->cur_cmd = me->eqtb[me->cur_cs].cmd;
        me->cur_chr = (int)me->eqtb[me->cur_cs].chr;
    } else {
        me->cur_cmd = TOKEN_CMD(t);
        me->cur_chr = TOKEN_CHR(t);
    }
    me->cur_tok = t;
}

static void get_token(macro_expander_t *me) {
    get_next(me);
    me->cur_tok = (!me->cur_cs) ? TOKEN(me->cur_cmd, me->cur_chr) : (CS_TOKEN_FLAG + me->cur_cs);
}

static void get_x_token(macro_expander_t *me) {
    get_next(me);
    while (IS_EXPANDABLE(me->cur_cmd)) {
        if (++me->expansion_depth > MAX_EXPANSION_DEPTH) {
            fprintf(stderr, "macro_expander: expansion depth exceeded (%d)\n", MAX_EXPANSION_DEPTH);
            me->cur_cmd = CMD_RELAX; me->cur_chr = 0;
            me->cur_tok = 0;
            return;
        }
        expand(me);
        get_next(me);
    }
    me->expansion_depth = 0;
    me->cur_tok = (!me->cur_cs) ? TOKEN(me->cur_cmd, me->cur_chr) : (CS_TOKEN_FLAG + me->cur_cs);
}

static token_node_t *scan_toks(macro_expander_t *me, int macro_def, int xpand) {
    (void)xpand;
    token_node_t *tail;
    int unbalance;

    me->scanner_status = 2;
    me->def_ref = new_token_list(me);
    if (!me->def_ref) { me->scanner_status = 0; return NULL; }
    tail = me->def_ref;

    if (macro_def) {
        for (;;) {
            get_token(me);
            if (TOKEN_CMD(me->cur_tok) == CMD_LEFT_BRACE)
                goto scan_body_start;
            if (TOKEN_CMD(me->cur_tok) == CMD_MAC_PARAM) {
                get_token(me);
                int param_num = 0;
                if (TOKEN_CMD(me->cur_tok) == CMD_OTHER_CHAR &&
                    TOKEN_CHR(me->cur_tok) >= '1' && TOKEN_CHR(me->cur_tok) <= '9')
                    param_num = TOKEN_CHR(me->cur_tok) - '0';
                int32_t match_tok = MATCH_TOKEN_BASE + param_num;
                if (TOKEN_CMD(me->cur_tok) == CMD_LEFT_BRACE) {
                    tail = token_list_append(me, tail, match_tok);
                    tail = token_list_append(me, tail, END_MATCH_TOKEN);
                    goto scan_body_start;
                }
                tail = token_list_append(me, tail, match_tok);
            } else {
                tail = token_list_append(me, tail, me->cur_tok);
            }
        }
    scan_body_start:
        tail = token_list_append(me, tail, END_MATCH_TOKEN);
    } else {
        get_token(me);
    }

    unbalance = 1;
    for (;;) {
        get_token(me);
        if (TOKEN_CMD(me->cur_tok) == CMD_LEFT_BRACE)
            unbalance++;
        else if (TOKEN_CMD(me->cur_tok) == CMD_RIGHT_BRACE) {
            if (--unbalance == 0) goto found;
        } else if (macro_def && TOKEN_CMD(me->cur_tok) == CMD_MAC_PARAM) {
            get_token(me);
            int param_num = 0;
            if (TOKEN_CMD(me->cur_tok) == CMD_OTHER_CHAR &&
                TOKEN_CHR(me->cur_tok) >= '1' && TOKEN_CHR(me->cur_tok) <= '9')
                param_num = TOKEN_CHR(me->cur_tok) - '0';
            if (TOKEN_CMD(me->cur_tok) == CMD_MAC_PARAM)
                me->cur_tok = TOKEN(CMD_OTHER_CHAR, '#');
            else
                me->cur_tok = TOKEN(CMD_OUT_PARAM, param_num);
        }
        tail = token_list_append(me, tail, me->cur_tok);
    }

found:
    me->scanner_status = 0;
    return tail;
}

static void out_append(macro_expander_t *me, const char *s, size_t len) {
    if (len == 0) return;
    if (me->out_len + len + 1 > me->out_cap) {
        me->out_cap = (me->out_len + len + 1) * 2;
        if (me->out_cap < 1024) me->out_cap = 1024;
        char *newbuf = realloc(me->out_buf, me->out_cap);
        if (!newbuf) {
            fprintf(stderr, "macro_expander: out of memory (out buffer)\n");
            return;
        }
        me->out_buf = newbuf;
    }
    memcpy(me->out_buf + me->out_len, s, len);
    me->out_len += len;
    me->out_buf[me->out_len] = '\0';
}

static void print_token(macro_expander_t *me, int32_t tok) {
    char buf[512];
    int n;

    if (TOKEN_IS_CS(tok)) {
        const char *name = me->eqtb[TOKEN_CS_IDX(tok)].name;
        if (name) {
            n = snprintf(buf, sizeof(buf), "\\%s", name);
            out_append(me, buf, (size_t)n);
        }
    } else {
        int cmd = TOKEN_CMD(tok), chr = TOKEN_CHR(tok);
        switch (cmd) {
        case CMD_LETTER: case CMD_OTHER_CHAR:
            buf[0] = (char)chr; out_append(me, buf, 1); break;
        case CMD_SPACER: out_append(me, " ", 1); break;
        case CMD_CAR_RET: out_append(me, "\n", 1); break;  // NEWLINE: output newline
        case CMD_LEFT_BRACE:  out_append(me, "{", 1); break;
        case CMD_RIGHT_BRACE: out_append(me, "}", 1); break;
        case CMD_MATH_SHIFT:  out_append(me, "$", 1); break;
        case CMD_TAB_MARK:    out_append(me, "&", 1); break;
        case CMD_MAC_PARAM:
            n = snprintf(buf, sizeof(buf), "#%c", chr);
            out_append(me, buf, (size_t)n); break;
        case CMD_SUP_MARK:    out_append(me, "^", 1); break;
        case CMD_SUB_MARK:    out_append(me, "_", 1); break;
        default:
            n = snprintf(buf, sizeof(buf), "<cmd=%d>", cmd);
            out_append(me, buf, (size_t)n); break;
        }
    }
}

static void macro_call(macro_expander_t *me) {
    token_node_t *ref_count = (token_node_t *)me->eqtb[me->cur_cs].chr;
    if (!ref_count || !ref_count->link) return;

    token_node_t *r = ref_count->link;
    int nparams = 0;

    {
        token_node_t *scan = r;
        int in_params = 1;
        while (scan && in_params) {
            int cmd = TOKEN_CMD(scan->info);
            if (cmd == CMD_MATCH) nparams++;
            else if (cmd == CMD_END_MATCH) in_params = 0;
            scan = scan->link;
        }
    }

    token_node_t *pstack_buf[10] = {NULL};
    {
        int arg_idx = 0;
        token_node_t *pat = r;
        int scanning = 1;
        while (pat && scanning) {
            if (TOKEN_CMD(pat->info) == CMD_MATCH) {
                token_node_t *arg_list = new_token_list(me);
                if (!arg_list) { scanning = 0; break; }
                token_node_t *arg_tail = arg_list;
                int brace_depth = 0;
                get_token(me);
                if (TOKEN_CMD(me->cur_tok) == CMD_LEFT_BRACE) {
                    brace_depth = 1;
                    while (brace_depth > 0) {
                        get_token(me);
                        if (TOKEN_CMD(me->cur_tok) == CMD_LEFT_BRACE) brace_depth++;
                        else if (TOKEN_CMD(me->cur_tok) == CMD_RIGHT_BRACE) brace_depth--;
                        if (brace_depth > 0)
                            arg_tail = token_list_append(me, arg_tail, me->cur_tok);
                    }
                } else {
                    arg_tail = token_list_append(me, arg_tail, me->cur_tok);
                }
                pstack_buf[arg_idx] = arg_list;
                arg_idx++;
            } else if (TOKEN_CMD(pat->info) == CMD_END_MATCH) {
                scanning = 0;
            }
            pat = pat->link;
        }
    }

    while (r && TOKEN_CMD(r->info) != CMD_END_MATCH) r = r->link;
    if (r) r = r->link;

    token_node_t *substituted = new_token_list(me);
    if (!substituted) {
        for (int i = 0; i < nparams && i < 10; i++)
            token_list_unref(me, pstack_buf[i]);
        return;
    }
    token_node_t *sub_tail = substituted;
    while (r) {
        if (TOKEN_CMD(r->info) == CMD_OUT_PARAM) {
            int pnum = TOKEN_CHR(r->info);
            if (pnum >= 1 && pnum <= 9) {
                token_node_t *pl = pstack_buf[pnum - 1];
                if (pl) {
                    for (token_node_t *pn = pl->link; pn; pn = pn->link)
                        sub_tail = token_list_append(me, sub_tail, pn->info);
                }
            }
        } else {
            sub_tail = token_list_append(me, sub_tail, r->info);
        }
        r = r->link;
    }

    for (int i = 0; i < nparams && i < 10; i++)
        token_list_unref(me, pstack_buf[i]);

    input_push_token_list(me, substituted);
    token_list_unref(me, substituted);
}

static void do_define(macro_expander_t *me, int is_edef, int is_global) {
    (void)is_global;
    get_token(me);
    if (!me->cur_cs) return;
    int target_cs = me->cur_cs;
    me->scanner_status = 2;
    scan_toks(me, 1, is_edef);
    me->scanner_status = 0;
    if (me->def_ref) {
        if (me->eqtb[target_cs].cmd >= CMD_CALL &&
            me->eqtb[target_cs].cmd <= CMD_LONG_OUTER_CALL) {
            token_list_unref(me, (token_node_t *)me->eqtb[target_cs].chr);
        }
        me->eqtb[target_cs].cmd = CMD_CALL;
        me->eqtb[target_cs].chr = (intptr_t)me->def_ref;
    }
}

static void do_newcommand(macro_expander_t *me) {
    get_token(me);
    if (TOKEN_CMD(me->cur_tok) != CMD_LEFT_BRACE) { return; }
    get_token(me);
    if (!me->cur_cs) { return; }
    int target_cs = me->cur_cs;
    get_token(me);
    if (TOKEN_CMD(me->cur_tok) != CMD_RIGHT_BRACE) { return; }

    int num_args = 0;
    if (me->input_ptr > 0) {
        input_frame_t *f = &me->input_stack[me->input_ptr - 1];
        if (f->state == INPUT_FILE && f->buf_loc < f->buf_limit &&
            f->buffer[f->buf_loc] == '[') {
            f->buf_loc++;
            while (f->buf_loc < f->buf_limit &&
                   f->buffer[f->buf_loc] >= '0' && f->buffer[f->buf_loc] <= '9') {
                num_args = num_args * 10 + (f->buffer[f->buf_loc] - '0');
                f->buf_loc++;
            }
            if (f->buf_loc < f->buf_limit && f->buffer[f->buf_loc] == ']')
                f->buf_loc++;
            token_node_t *param_list = new_token_list(me);
            token_node_t *ptail = param_list;
            for (int i = 1; i <= num_args && i <= 9; i++) {
                ptail = token_list_append(me, ptail, TOKEN(CMD_MAC_PARAM, '#'));
                ptail = token_list_append(me, ptail, TOKEN(CMD_OTHER_CHAR, '0' + i));
            }
            input_push_token_list(me, param_list);
            token_list_unref(me, param_list);
        }
    }

    me->scanner_status = 2;
    scan_toks(me, 1, 0);
    me->scanner_status = 0;
    if (me->def_ref) {
        if (me->eqtb[target_cs].cmd >= CMD_CALL &&
            me->eqtb[target_cs].cmd <= CMD_LONG_OUTER_CALL) {
            token_list_unref(me, (token_node_t *)me->eqtb[target_cs].chr);
        }
        me->eqtb[target_cs].cmd = CMD_CALL;
        me->eqtb[target_cs].chr = (intptr_t)me->def_ref;
    }
}

static void do_let(macro_expander_t *me) {
    get_token(me);
    if (!me->cur_cs) return;
    int target_cs = me->cur_cs;

    get_token(me);
    if (TOKEN_CMD(me->cur_tok) == CMD_OTHER_CHAR && TOKEN_CHR(me->cur_tok) == '=')
        get_token(me);

    if (me->cur_cs) {
        if (me->eqtb[target_cs].cmd >= CMD_CALL &&
            me->eqtb[target_cs].cmd <= CMD_LONG_OUTER_CALL) {
            token_list_unref(me, (token_node_t *)me->eqtb[target_cs].chr);
        }
        me->eqtb[target_cs].cmd = me->eqtb[me->cur_cs].cmd;
        me->eqtb[target_cs].chr = me->eqtb[me->cur_cs].chr;
        if (me->eqtb[target_cs].cmd == CMD_CALL ||
            me->eqtb[target_cs].cmd == CMD_LONG_CALL ||
            me->eqtb[target_cs].cmd == CMD_OUTER_CALL ||
            me->eqtb[target_cs].cmd == CMD_LONG_OUTER_CALL) {
            token_node_t *tl = (token_node_t *)me->eqtb[target_cs].chr;
            if (tl) tl->info++;
        }
    } else {
        if (me->eqtb[target_cs].cmd >= CMD_CALL &&
            me->eqtb[target_cs].cmd <= CMD_LONG_OUTER_CALL) {
            token_list_unref(me, (token_node_t *)me->eqtb[target_cs].chr);
        }
        token_node_t *single = new_token_list(me);
        if (!single) return;
        token_list_append(me, single, me->cur_tok);
        me->eqtb[target_cs].cmd = CMD_CALL;
        me->eqtb[target_cs].chr = (intptr_t)single;
    }
}

static void expand(macro_expander_t *me) {
    if (me->cur_cmd == CMD_DEF_PRIM)  { do_define(me, 0, 0); return; }
    if (me->cur_cmd == CMD_EDEF_PRIM) { do_define(me, 1, 0); return; }
    if (me->cur_cmd == CMD_XDEF_PRIM) { do_define(me, 1, 1); return; }
    if (me->cur_cmd == CMD_GDEF_PRIM) { do_define(me, 0, 1); return; }
    if (me->cur_cmd == CMD_NEWCOMMAND_PRIM)  { do_newcommand(me); return; }
    if (me->cur_cmd == CMD_RENEWCOMMAND_PRIM)  { do_newcommand(me); return; }
    if (me->cur_cmd == CMD_PROVIDECOMMAND_PRIM) { do_newcommand(me); return; }
    if (me->cur_cmd == CMD_LET_PRIM) { do_let(me); return; }

    if (me->cur_cmd == CMD_EXPAND_AFTER) {
        int32_t t;
        get_token(me); t = me->cur_tok;
        get_token(me);
        if (IS_EXPANDABLE(me->cur_cmd)) expand(me);
        else {
            token_node_t *node = new_node(me);
            if (node) {
                node->info = me->cur_tok;
                if (me->input_ptr > 0 &&
                    me->input_stack[me->input_ptr - 1].state == INPUT_TOKEN_LIST) {
                    node->link = me->input_stack[me->input_ptr - 1].tok_loc;
                    me->input_stack[me->input_ptr - 1].tok_loc = node;
                }
            }
        }
        {
            token_node_t *node = new_node(me);
            if (node) {
                node->info = t;
                if (me->input_ptr > 0 &&
                    me->input_stack[me->input_ptr - 1].state == INPUT_TOKEN_LIST) {
                    node->link = me->input_stack[me->input_ptr - 1].tok_loc;
                    me->input_stack[me->input_ptr - 1].tok_loc = node;
                }
            }
        }
        return;
    }

    if (me->cur_cmd == CMD_NO_EXPAND) {
        get_token(me);
        print_token(me, me->cur_tok);
        return;
    }

    if (me->cur_cmd == CMD_AFTERGROUP_PRIM) {
        get_token(me);
        return;
    }

    if (me->cur_cmd == CMD_MAKETLETTER_PRIM) {
        /* \makeatletter: make @ a letter (catcode 11) */
        int cs = eq_lookup(me, "@");
        if (cs >= 0) me->eqtb[cs].cmd = me->cat_code['@'];
        me->cat_code['@'] = CMD_LETTER;
        return;
    }

    if (me->cur_cmd == CMD_MAKETOTHER_PRIM) {
        /* \makeatother: make @ other (catcode 12) */
        int cs = eq_lookup(me, "@");
        if (cs >= 0) me->eqtb[cs].cmd = me->cat_code['@'];
        me->cat_code['@'] = CMD_OTHER_CHAR;
        return;
    }

    if (me->cur_cmd == CMD_CS_NAME) {
        char namebuf[256];
        int namelen = 0;
        for (;;) {
            get_x_token(me);
            if (me->cur_cs && me->eqtb[me->cur_cs].cmd == CMD_END_CS_NAME) break;
            if (!me->cur_cs) {
                int cmd = TOKEN_CMD(me->cur_tok);
                int chr = TOKEN_CHR(me->cur_tok);
                if ((cmd == CMD_LETTER || cmd == CMD_OTHER_CHAR) && namelen < 255)
                    namebuf[namelen++] = (char)chr;
            }
        }
        namebuf[namelen] = '\0';
        int cs = eq_lookup(me, namebuf);
        if (cs < 0) cs = eq_define(me, namebuf, CMD_RELAX, 0);
        if (cs >= 0) {
            token_node_t *list = new_token_list(me);
            if (list) {
                token_list_append(me, list, CS_TOKEN_FLAG + cs);
                input_push_token_list(me, list);
                token_list_unref(me, list);
            }
        }
        return;
    }

    if (me->cur_cmd >= CMD_CALL && me->cur_cmd <= CMD_LONG_OUTER_CALL) {
        macro_call(me);
        return;
    }
    if (me->cur_cmd == CMD_UNDEFINED_CS) {
        print_token(me, CS_TOKEN_FLAG + me->cur_cs);
        return;
    }
    me->cur_cmd = CMD_RELAX;
    me->cur_chr = 0;
}

static void init_primitives(macro_expander_t *me) {
    eq_define(me, "def",  CMD_DEF_PRIM, 0);
    eq_define(me, "edef", CMD_EDEF_PRIM, 0);
    eq_define(me, "xdef", CMD_XDEF_PRIM, 0);
    eq_define(me, "gdef", CMD_GDEF_PRIM, 0);
    eq_define(me, "newcommand",  CMD_NEWCOMMAND_PRIM, 0);
    eq_define(me, "renewcommand", CMD_RENEWCOMMAND_PRIM, 0);
    eq_define(me, "providecommand", CMD_PROVIDECOMMAND_PRIM, 0);
    eq_define(me, "let", CMD_LET_PRIM, 0);
    eq_define(me, "expandafter", CMD_EXPAND_AFTER, 0);
    eq_define(me, "noexpand", CMD_NO_EXPAND, 0);
    eq_define(me, "aftergroup", CMD_AFTERGROUP_PRIM, 0);
    eq_define(me, "csname",  CMD_CS_NAME, 0);
    eq_define(me, "endcsname", CMD_END_CS_NAME, 0);
    eq_define(me, "relax", CMD_RELAX, 0);
    eq_define(me, "makeatletter", CMD_MAKETLETTER_PRIM, 0);
    eq_define(me, "makeatother", CMD_MAKETOTHER_PRIM, 0);
}

macro_expander_t *macro_expander_create(void) {
    macro_expander_t *me = calloc(1, sizeof(macro_expander_t));
    if (!me) return NULL;
    catcode_defaults(me);
    init_primitives(me);
    me->out_cap = 4096;
    me->out_buf = malloc(me->out_cap);
    me->out_buf[0] = '\0';
    me->scanner_state = 3 + MAX_CHAR_CODE + MAX_CHAR_CODE;
    return me;
}

void macro_expander_destroy(macro_expander_t *me) {
    if (!me) return;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (me->eqtb[i].name) {
            if (me->eqtb[i].cmd == CMD_CALL ||
                me->eqtb[i].cmd == CMD_LONG_CALL ||
                me->eqtb[i].cmd == CMD_OUTER_CALL ||
                me->eqtb[i].cmd == CMD_LONG_OUTER_CALL) {
                token_list_unref(me, (token_node_t *)me->eqtb[i].chr);
            }
            free(me->eqtb[i].name);
            me->eqtb[i].name = NULL;
        }
    }
    for (int i = 0; i < me->input_ptr; i++) {
        if (me->input_stack[i].state == INPUT_TOKEN_LIST &&
            me->input_stack[i].tok_head) {
            token_list_unref(me, me->input_stack[i].tok_head);
        } else if (me->input_stack[i].state == INPUT_FILE &&
                   me->input_stack[i].buffer) {
            free((void *)me->input_stack[i].buffer);
            me->input_stack[i].buffer = NULL;
        }
    }
    for (int pi = 0; pi < me->param_ptr; pi++)
        token_list_unref(me, me->param_stack[pi]);
    free(me->out_buf);
    free(me);
}

void macro_expander_reset(macro_expander_t *me) {
    if (!me) return;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (me->eqtb[i].name) {
            if (me->eqtb[i].cmd == CMD_CALL ||
                me->eqtb[i].cmd == CMD_LONG_CALL ||
                me->eqtb[i].cmd == CMD_OUTER_CALL ||
                me->eqtb[i].cmd == CMD_LONG_OUTER_CALL) {
                token_list_unref(me, (token_node_t *)me->eqtb[i].chr);
            }
            free(me->eqtb[i].name);
            me->eqtb[i].name = NULL;
        }
    }
    memset(me->eqtb, 0, sizeof(me->eqtb));
    me->eqtb_count = 0;

    for (int i = 0; i < me->input_ptr; i++) {
        if (me->input_stack[i].state == INPUT_TOKEN_LIST &&
            me->input_stack[i].tok_head) {
            token_list_unref(me, me->input_stack[i].tok_head);
        } else if (me->input_stack[i].state == INPUT_FILE &&
                   me->input_stack[i].buffer) {
            free((void *)me->input_stack[i].buffer);
            me->input_stack[i].buffer = NULL;
        }
    }
    me->input_ptr = 0;
    me->param_ptr = 0;
    me->node_pool_free = 0;
    me->avail = NULL;

    me->out_len = 0;
    me->out_buf[0] = '\0';
    me->expansion_depth = 0;
    me->scanner_state = 3 + MAX_CHAR_CODE + MAX_CHAR_CODE;

    init_primitives(me);
}

void macro_expander_feed(macro_expander_t *me, const char *input, size_t len) {
    if (!me || !input) return;
    char *copy = strndup(input, len);
    if (!copy) { fprintf(stderr, "macro_expander: out of memory\n"); return; }
    input_push_string(me, copy);

    for (;;) {
        get_x_token(me);
        if (me->input_ptr == 0) break;
        print_token(me, me->cur_tok);
    }
}

void macro_expander_feed_cb(macro_expander_t *me, const char *input, size_t len,
                            macro_expander_callback cb, void *user) {
    if (!me || !input) return;
    char *copy = strndup(input, len);
    if (!copy) { fprintf(stderr, "macro_expander: out of memory\n"); return; }
    input_push_string(me, copy);

    for (;;) {
        get_x_token(me);
        if (me->input_ptr == 0) break;
        print_token(me, me->cur_tok);
    }

    if (cb && me->out_len > 0) {
        cb(me->out_buf, me->out_len, user);
        me->out_len = 0;
        me->out_buf[0] = '\0';
    }
}

const char *macro_expander_get_output(macro_expander_t *me, size_t *out_len) {
    if (!me) { if (out_len) *out_len = 0; return NULL; }
    if (out_len) *out_len = me->out_len;
    return me->out_buf;
}

int macro_expander_macro_count(macro_expander_t *me) {
    if (!me) return 0;
    int count = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (me->eqtb[i].name) {
            int cmd = me->eqtb[i].cmd;
            if (cmd == CMD_CALL || cmd == CMD_LONG_CALL ||
                cmd == CMD_OUTER_CALL || cmd == CMD_LONG_OUTER_CALL)
                count++;
        }
    }
    return count;
}

/* =========================================================================
 * Standalone binary driver
 * ========================================================================= */
#ifdef MACRO_EXPANDER_STANDALONE

static const char *cmd_to_typestr(int cmd) {
    switch (cmd) {
    case CMD_DEF_PRIM:          return "def";
    case CMD_EDEF_PRIM:         return "edef";
    case CMD_XDEF_PRIM:         return "xdef";
    case CMD_GDEF_PRIM:         return "gdef";
    case CMD_NEWCOMMAND_PRIM:   return "newcommand";
    case CMD_RENEWCOMMAND_PRIM: return "renewcommand";
    case CMD_PROVIDECOMMAND_PRIM: return "providecommand";
    case CMD_LET_PRIM:          return "let";
    default:                    return "?";
    }
}

static void process_report(macro_expander_t *me, const char *path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) { fprintf(stderr, "macro_expander: cannot open %s\n", path); return; }
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *buf = malloc(sz + 1);
    if (!buf) { fclose(fp); return; }
    size_t n = fread(buf, 1, sz, fp);
    buf[n] = '\0';
    fclose(fp);

    macro_expander_feed(me, buf, n);
    free(buf);

    printf("\n=== %s ===\n", path);
    for (int i = 0; i < HASH_SIZE; i++) {
        if (me->eqtb[i].name) {
            int cmd = me->eqtb[i].cmd;
            const char *typestr = cmd_to_typestr(cmd);
            printf("  %-18s => %-24s [%s]\n",
                   me->eqtb[i].name, "(defined)", typestr);
        }
    }
}

static void process_pipe_mode(macro_expander_t *me) {
    size_t cap = 4096, len = 0;
    char *buf = malloc(cap);
    if (!buf) return;
    while (!feof(stdin)) {
        if (len + 4096 > cap) {
            cap *= 2;
            char *newbuf = realloc(buf, cap);
            if (!newbuf) { free(buf); return; }
            buf = newbuf;
        }
        size_t rd = fread(buf + len, 1, cap - len - 1, stdin);
        if (rd == 0) break;
        len += rd;
    }
    buf[len] = '\0';

    macro_expander_feed(me, buf, len);
    free(buf);

    size_t out_len;
    const char *out = macro_expander_get_output(me, &out_len);
    if (out_len > 0) fwrite(out, 1, out_len, stdout);
}

int main(int argc, char *argv[]) {
    macro_expander_t *me = macro_expander_create();
    if (!me) { fprintf(stderr, "macro_expander: failed to initialize\n"); return 1; }

    int report_mode = 0;
    int file_start = 1;

    if (argc >= 2 && strcmp(argv[1], "--report") == 0) {
        report_mode = 1;
        file_start = 2;
    }

    if (report_mode && argc >= 3) {
        for (int i = file_start; i < argc; i++)
            process_report(me, argv[i]);
    } else {
        process_pipe_mode(me);
    }

    macro_expander_destroy(me);
    return 0;
}

#endif /* MACRO_EXPANDER_STANDALONE */
