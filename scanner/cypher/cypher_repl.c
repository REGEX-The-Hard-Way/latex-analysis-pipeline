/*
 * cypher_repl.c — Interactive Cypher query REPL.
 *
 * Usage: ./cypher_repl.out          (interactive)
 *        ./cypher_repl.out file.cypher  (batch)
 *        echo "MATCH ... RETURN ..." | ./cypher_repl.out  (pipe)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "cypher_parser.h"
#include "graph_store.h"
#include "graph_jit.h"
#include "graph_scan.h"

static void execute_query(cypher_graph_t *g, const char *query);
static void print_table(cypher_result_t *r);
static void run_dot_command(cypher_graph_t *g, const char *line);
static cypher_graph_t *g_repl = NULL;

/* Ctrl-R handler: trigger .hist (fzf history browser) */
static int ctrl_r_handler(int count, int key) {
    (void)count; (void)key;
    rl_clear_message();
    /* save readline's terminal state before launching fzf */
    rl_deprep_terminal();
    printf("\n");
    run_dot_command(g_repl, ".hist");
    /* restore readline's terminal state */
    rl_prep_terminal(0);
    rl_on_new_line();
    rl_redisplay();
    return 0;
}

static char g_histfile[1024] = "";
static char g_scan_file[1024] = "";

static int run_scan_query(const char *query) {
    /* parse query to extract label + optional CONTAINS */
    static cypher_token_t tokens[MAX_TOKENS];
    int n = cypher_lex(query, (int)strlen(query), tokens, MAX_TOKENS);
    if (n == 0) return 0;

    const char *error = NULL;
    cypher_ast_t *ast = cypher_parse(tokens, n, &error);
    if (!ast) return 0;

    const char *label = NULL;
    const char *contains = NULL;
    int limit = 200;

    /* find MATCH label */
    cypher_ast_t *match_cl = NULL;
    for (cypher_ast_t *c = ast; c; c = c->next)
        if (c->type == AST_MATCH) { match_cl = c; break; }

    if (match_cl && match_cl->bin.l && match_cl->bin.l->type == AST_PATTERN) {
        cypher_ast_t *fn = match_cl->bin.l->list.items[0];
        if (fn && fn->type == AST_NODE_PAT) {
            for (cypher_ast_t *p = fn->node.props; p; p = p->next)
                if (p->type == AST_LABEL) { label = p->str; break; }
        }
        /* check WHERE for CONTAINS */
        if (match_cl->bin.r) {
            cypher_ast_t *w = match_cl->bin.r;
            if (w->type == AST_BINARY
                && (w->bin.op == TOK_CONTAINS || w->bin.op == TOK_STARTS
                    || w->bin.op == TOK_ENDS)
                && w->bin.r && w->bin.r->type == AST_STRING)
                contains = w->bin.r->str;
        }
    }

    /* find LIMIT */
    cypher_ast_t *ret_cl = NULL;
    for (cypher_ast_t *c = ast; c; c = c->next)
        if (c->type == AST_RETURN) { ret_cl = c; break; }
    if (ret_cl) {
        for (cypher_ast_t *n = ret_cl->next; n; n = n->next)
            if (n->type == AST_LIMIT) limit = n->ival;
    }

    if (!label) { cypher_ast_free(ast); return 0; }

    cypher_result_t *result = cypher_result_new();
    cypher_result_add_col(result, "text");

    int rows = cypher_scan_sidecar(g_scan_file, label, contains, limit, result);
    cypher_ast_free(ast);

    if (rows < 0) { cypher_result_free(result); return -1; }

    print_table(result);
    cypher_result_free(result);
    return rows;
}

static void save_history(const char *query) {
    if (!g_histfile[0]) {
        const char *home = getenv("HOME");
        if (!home) home = "/tmp";
        snprintf(g_histfile, sizeof(g_histfile), "%s/.cypher_history", home);
    }
    if (!query || !query[0]) return;
    FILE *fp = fopen(g_histfile, "ab");
    if (!fp) return;
    fwrite(query, 1, strlen(query), fp);
    fputc('\n', fp);
    fclose(fp);
}

static void run_dot_command(cypher_graph_t *g, const char *line) {
    graph_store_t *gs = (graph_store_t *)cypher_graph_get_store(g);
    if (!gs) return;

    if (!strcmp(line, ".help") || !strcmp(line, ".h")) {
        printf("\n  Cypher REPL Commands:\n\n");
        printf("  .help, .h          this help\n");
        printf("  .history, .hist    browse command history with fzf\n");
        printf("  .schema            show labels and properties\n");
        printf("  .stats             show node/edge counts\n");
        printf("  .exit, .quit       exit the REPL\n\n");
        printf("  Query syntax:\n");
        printf("  CREATE (v:Label {key: val})\n");
        printf("  MATCH (v:Label) WHERE v.key = val\n");
        printf("  RETURN v.key [ORDER BY ...] [LIMIT n]\n");
        printf("  SET v.key = val\n");
        printf("  DETACH DELETE v\n\n");
        return;
    }

    if (!strcmp(line, ".schema")) {
        if (!gs->label_count) { printf("  (no labels)\n"); return; }
        for (uint32_t li = 0; li < gs->label_count; li++) {
            uint32_t cnt = 0;
            uint64_t mask = 1ULL << li;
            for (uint32_t n = 0; n < gs->node_count; n++)
                if (gs->nodes[n].label_mask & mask) cnt++;
            printf("  :%-30s %u nodes\n", gs->labels[li].name, cnt);
        }
        return;
    }

    if (!strcmp(line, ".stats")) {
        uint32_t nodes = gs_node_count(gs);
        uint32_t edges = gs_edge_total(gs);
        uint32_t props = gs->prop_count;
        printf("  Nodes: %u  Edges: %u  Properties: %u  Labels: %u\n",
               nodes, edges, props, gs->label_count);
        return;
    }

    if (!strcmp(line, ".history") || !strcmp(line, ".hist")) {
        if (!g_histfile[0]) {
            const char *home = getenv("HOME");
            if (!home) home = "/tmp";
            snprintf(g_histfile, sizeof(g_histfile), "%s/.cypher_history", home);
        }
        /* write selected query to a temp file, then read it back */
        char cmd[4096];
        snprintf(cmd, sizeof(cmd),
            "tac %s 2>/dev/null | "
            "fzf --no-sort "
            "    --preview 'echo {} | fold -s -w 80' "
            "    --preview-window 'down,30%%,border-top' "
            "    --bind 'enter:execute(echo {} > /tmp/cypher_fzf_sel)+abort' "
            "    --header 'Cypher history (search/narrow, enter to select)' "
            "> /dev/tty",
            g_histfile);
        int rc = system(cmd);
        if (rc < 0) return;
        FILE *fp = fopen("/tmp/cypher_fzf_sel", "r");
        if (!fp) return;
        char sel[65536] = "";
        size_t rlen = fread(sel, 1, sizeof(sel) - 1, fp);
        fclose(fp);
        if (rlen > 0) {
            sel[rlen] = '\0';
            while (rlen > 0 && isspace((unsigned char)sel[rlen-1]))
                sel[--rlen] = '\0';
            printf("cypher> %s\n", sel);
            execute_query(g, sel);
        }
        return;
    }
}

static int is_dot_command(const char *line) {
    return line[0] == '.' && (line[1] == 'h' || line[1] == 's' || line[1] == 'e' || line[1] == 'q');
}

static int is_exit_command(const char *line) {
    return !strcmp(line, "exit") || !strcmp(line, "quit") ||
           !strcmp(line, "q") || !strcmp(line, ".exit") || !strcmp(line, ".quit");
}

static void print_table(cypher_result_t *r) {
    if (!r->ncols) { printf("  (empty result)\n"); return; }

    int *widths = calloc((size_t)r->ncols, sizeof(int));
    for (int c = 0; c < r->ncols; c++) {
        widths[c] = (int)strlen(r->columns[c]) + 2;
        for (int i = 0; i < r->nrows; i++) {
            int w = r->rows[i][c] ? (int)strlen(r->rows[i][c]) + 2 : 4;
            if (w > widths[c]) widths[c] = w;
        }
        if (widths[c] < 6) widths[c] = 6;
    }

    /* top border */
    putchar('+');
    for (int c = 0; c < r->ncols; c++) {
        for (int i = 0; i < widths[c]; i++) putchar('-');
        putchar('+');
    }
    putchar('\n');

    /* header */
    putchar('|');
    for (int c = 0; c < r->ncols; c++) {
        printf(" %-*s |", widths[c] - 2, r->columns[c]);
    }
    putchar('\n');

    /* separator */
    putchar('+');
    for (int c = 0; c < r->ncols; c++) {
        for (int i = 0; i < widths[c]; i++) putchar('=');
        putchar('+');
    }
    putchar('\n');

    /* data */
    for (int i = 0; i < r->nrows; i++) {
        putchar('|');
        for (int c = 0; c < r->ncols; c++) {
            printf(" %-*s |", widths[c] - 2,
                   r->rows[i][c] ? r->rows[i][c] : "NULL");
        }
        putchar('\n');
    }

    /* bottom border */
    putchar('+');
    for (int c = 0; c < r->ncols; c++) {
        for (int i = 0; i < widths[c]; i++) putchar('-');
        putchar('+');
    }
    putchar('\n');

    printf("\n%d row%s\n", r->nrows, r->nrows == 1 ? "" : "s");
    free(widths);
}

static void execute_query(cypher_graph_t *g, const char *query) {
    static cypher_token_t tokens[MAX_TOKENS];
    int n = cypher_lex(query, (int)strlen(query), tokens, MAX_TOKENS);
    if (n == 0) return;

    const char *error = NULL;
    cypher_ast_t *ast = cypher_parse(tokens, n, &error);
    if (!ast) {
        fprintf(stderr, "Parse error: %s\n", error ? error : "unknown");
        return;
    }

    cypher_result_t *result = cypher_execute(g, ast, &error);
    if (!result) {
        fprintf(stderr, "Execute error: %s\n", error ? error : "unknown");
        cypher_ast_free(ast);
        return;
    }

    print_table(result);
    cypher_result_free(result);
    cypher_ast_free(ast);
}

int main(int argc, char *argv[]) {
    cypher_graph_t *g = cypher_graph_new();
    cypher_jit_init();

    int sidecar_loaded = 0;

    /* check for --scan flag (mmap + Ragel direct query, no graph store) */
    for (int ai = 1; ai < argc; ai++) {
        if (!strcmp(argv[ai], "--scan") && ai + 1 < argc) {
            strncpy(g_scan_file, argv[ai + 1], sizeof(g_scan_file) - 1);
            for (int sj = ai; sj + 2 < argc; sj++)
                argv[sj] = argv[sj + 2];
            argc -= 2;
            ai--;
        }
    }

    /* check for --sidecar flag */
    for (int ai = 1; ai < argc; ai++) {
        if (!strcmp(argv[ai], "--sidecar") && ai + 1 < argc) {
            int rc = cypher_graph_load_sidecar(g, argv[ai + 1]);
            if (rc != 0) {
                fprintf(stderr, "Failed to load sidecar: %s\n", argv[ai + 1]);
                cypher_graph_free(g);
                return 1;
            }
            sidecar_loaded = 1;
            /* remove --sidecar and its arg from argv */
            for (int sj = ai; sj + 2 < argc; sj++)
                argv[sj] = argv[sj + 2];
            argc -= 2;
            ai--;
        }
    }

    int batch_mode = 0;
    int force_interactive = 0;

    /* check for --interactive / -i flag */
    for (int ai = 1; ai < argc; ai++) {
        if (!strcmp(argv[ai], "-i") || !strcmp(argv[ai], "--interactive")) {
            force_interactive = 1;
            for (int sj = ai; sj + 1 < argc; sj++)
                argv[sj] = argv[sj + 1];
            argc--;
            ai--;
        }
    }

    for (int ai = 1; ai < argc; ai++) {
        const char *arg = argv[ai];
        if (arg[0] != '-') { batch_mode = 1; break; }
    }

    if (!batch_mode && sidecar_loaded) force_interactive = 1;

    if (batch_mode) {
        /* Batch mode: execute from file */
        for (int fi = 1; fi < argc; fi++) {
            FILE *fp = fopen(argv[fi], "rb");
            if (!fp) { fprintf(stderr, "Cannot open %s\n", argv[fi]); continue; }
            fseek(fp, 0, SEEK_END);
            long sz = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            char *buf = malloc((size_t)sz + 1);
            fread(buf, 1, (size_t)sz, fp);
            buf[sz] = '\0';
            fclose(fp);
            execute_query(g, buf);
            save_history(buf);
            free(buf);
        }
    } else if (!isatty(fileno(stdin)) && !force_interactive) {
        /* Pipe mode: read all of stdin */
        size_t cap = 4096, len = 0;
        char *buf = malloc(cap);
        while (!feof(stdin)) {
            if (len + 1024 > cap) { cap *= 2; buf = realloc(buf, cap); }
            size_t rd = fread(buf + len, 1, cap - len - 1, stdin);
            if (rd == 0) break;
            len += rd;
        }
        buf[len] = '\0';
        if (g_scan_file[0])
            run_scan_query(buf);
        else {
            execute_query(g, buf);
            save_history(buf);
        }
        free(buf);
    } else {
        /* Interactive REPL */
        g_repl = g;
        rl_initialize();
        rl_bind_keyseq("\\C-r", ctrl_r_handler);
        if (!g_histfile[0]) {
            const char *home = getenv("HOME");
            if (!home) home = "/tmp";
            snprintf(g_histfile, sizeof(g_histfile), "%s/.cypher_history", home);
        }
        read_history(g_histfile);
        stifle_history(1000);

        printf("Cypher Query REPL (openCypher subset)\n");
        printf("Type .help for commands. Type 'exit' to quit.\n\n");

        char  query[65536] = "";
        int   prompt = 1;

        while (1) {
            char *line = prompt ? readline("cypher> ") : readline("");
            if (!line) { printf("\n"); break; }

            /* trim trailing whitespace */
            int ll = (int)strlen(line);
            while (ll > 0 && isspace((unsigned char)line[ll-1])) line[--ll] = '\0';

            if (is_exit_command(line)) { free(line); break; }

            /* dot-commands */
            if (is_dot_command(line)) {
                add_history(line);
                run_dot_command(g, line);
                prompt = 1;
                query[0] = '\0';
                free(line);
                continue;
            }

            if (strlen(query) + ll + 2 < (int)sizeof(query)) {
                strcat(query, line);
                strcat(query, "\n");
            }

            /* execute on semicolon or if line ends a clause */
            int exec = 0;
            if (ll > 0 && line[ll-1] == ';') exec = 1;
            else if (!strcasecmp(line, "return") || !strncasecmp(line, "return ", 7)) exec = 1;

            if (exec) {
                add_history(query);
                if (g_scan_file[0])
                    run_scan_query(query);
                else {
                    execute_query(g, query);
                    save_history(query);
                }
                query[0] = '\0';
                prompt = 1;
            } else {
                prompt = 0;
            }
            free(line);
        }
        write_history(g_histfile);
        printf("bye\n");
    }

    cypher_graph_free(g);
    fflush(stdout);
    return 0;
}
