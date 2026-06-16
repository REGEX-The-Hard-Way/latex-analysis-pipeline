/*
 * cypher_parser.h — Token types, AST nodes, and parser API.
 */
#ifndef CYPHER_PARSER_H
#define CYPHER_PARSER_H

#include <stdint.h>

#define MAX_STR    256
#define MAX_TOKENS 1024
#define MAX_ROWS   200

typedef enum {
    TOK_EOF = 0,
    TOK_SEMI, TOK_COMMA, TOK_DOT, TOK_COLON,
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACKET, TOK_RBRACKET, TOK_LBRACE, TOK_RBRACE,
    TOK_EQ, TOK_NEQ, TOK_LT, TOK_GT, TOK_LE, TOK_GE,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PCT, TOK_CARET,
    TOK_DOLLAR, TOK_PIPE, TOK_DASH,
    TOK_IDENT, TOK_STRING, TOK_INTEGER, TOK_FLOAT, TOK_BOOL, TOK_NULL,
    /* keywords */
    TOK_MATCH, TOK_RETURN, TOK_WHERE, TOK_CREATE, TOK_DELETE, TOK_SET, TOK_MERGE,
    TOK_ORDER, TOK_BY, TOK_LIMIT, TOK_SKIP, TOK_WITH, TOK_UNWIND,
    TOK_AND, TOK_OR, TOK_NOT, TOK_XOR,
    TOK_IN, TOK_IS, TOK_AS, TOK_DISTINCT, TOK_OPTIONAL, TOK_DETACH,
    TOK_CONTAINS, TOK_STARTS, TOK_ENDS,
    TOK_ON, TOK_CASE, TOK_WHEN, TOK_THEN, TOK_ELSE, TOK_END,
    TOK_DESC, TOK_ASC, TOK_COUNT, TOK_EXISTS, TOK_REMOVE, TOK_UNION,
    TOK_DOTDOT,
} cypher_tok_type_t;

typedef struct {
    cypher_tok_type_t type;
    char   str[MAX_STR];
    int    ival;
    double fval;
} cypher_token_t;

int cypher_lex(const char *in, int len, cypher_token_t *tokens, int max_tokens);

/* AST node types */
typedef enum {
    AST_QUERY, AST_MATCH, AST_RETURN, AST_CREATE, AST_SET, AST_DELETE, AST_MERGE, AST_UNWIND, AST_CASE,
    AST_PATTERN, AST_NODE_PAT, AST_REL_PAT,
    AST_PROPERTY, AST_MAP_ENTRY,
    AST_IDENT, AST_STRING, AST_INTEGER, AST_FLOAT, AST_BOOL, AST_NULL,
    AST_BINARY, AST_UNARY, AST_NOT, AST_PROP, AST_LABEL,
    AST_ORDER_ITEM, AST_COLUMN, AST_FUNCALL,
    AST_LIMIT, AST_SKIP, AST_WITH,     AST_LIST, AST_REMOVE
} cypher_ast_type_t;

typedef struct cypher_ast {
    cypher_ast_type_t type;
    char str[MAX_STR];
    union {
        double fval;
        int    ival;
        struct { char op; struct cypher_ast *l; struct cypher_ast *r; } bin;
        struct { char op; struct cypher_ast *a; } una;
        struct { struct cypher_ast *e; struct cypher_ast *n; } prop;
        struct { struct cypher_ast *name; struct cypher_ast *props; } node;
        struct { struct cypher_ast *name; struct cypher_ast *props;
                 struct cypher_ast *labels; int dir;
                 int varlen_min; int varlen_max; } rel;
        struct { struct cypher_ast **items; int n; } list;
        struct { struct cypher_ast *l; struct cypher_ast *r; } pair;
        struct { struct cypher_ast *name; struct cypher_ast *as; } col;
        struct { struct cypher_ast *func; struct cypher_ast **args; int n; } call;
    };
    struct cypher_ast *next;
} cypher_ast_t;

cypher_ast_t *cypher_parse(cypher_token_t *tokens, int n, const char **error);
void cypher_ast_free(cypher_ast_t *a);

/* Graph store API */
typedef uint64_t cypher_node_id_t;

typedef struct cypher_graph cypher_graph_t;

typedef enum {
    CYPHER_NODE = 0,
    CYPHER_REL  = 1
} cypher_ent_type_t;

cypher_graph_t *cypher_graph_new(void);
void cypher_graph_free(cypher_graph_t *g);
void *cypher_graph_get_store(cypher_graph_t *g);
cypher_node_id_t cypher_graph_add_node(cypher_graph_t *g);
void cypher_graph_set_label(cypher_graph_t *g, cypher_node_id_t n, const char *label);
void cypher_graph_set_prop_str(cypher_graph_t *g, cypher_node_id_t n,
                                 const char *key, const char *val);
void cypher_graph_set_prop_num(cypher_graph_t *g, cypher_node_id_t n,
                                 const char *key, double val);
cypher_node_id_t cypher_graph_add_rel(cypher_graph_t *g, cypher_node_id_t src,
                                        cypher_node_id_t dst, const char *type);

const char *cypher_graph_get_label(cypher_graph_t *g, cypher_node_id_t n);
const char *cypher_graph_get_prop(cypher_graph_t *g, cypher_node_id_t n,
                                    const char *key);

/* Query execution API */
typedef struct cypher_result {
    char   **columns;
    int      ncols;
    char   ***rows;
    int      nrows;
    int      cap;
} cypher_result_t;

cypher_result_t *cypher_result_new(void);
void cypher_result_add_col(cypher_result_t *r, const char *col);
void cypher_result_add_row_empty(cypher_result_t *r);
void cypher_result_set_cell(cypher_result_t *r, int row, int col, const char *val);
void cypher_result_free(cypher_result_t *r);

cypher_result_t *cypher_execute(cypher_graph_t *g, cypher_ast_t *ast,
                                 const char **error);

int cypher_graph_load_sidecar(cypher_graph_t *g, const char *filename);

#endif /* CYPHER_PARSER_H */
