/*
 * cypher_lexer.h – public interface for the Ragel-generated Cypher tokenizer.
 *
 * Include this from cypher_parser.rl (or any other translation unit that
 * needs to call lexer_init / lexer_next directly).
 */

#ifndef CYPHER_LEXER_H
#define CYPHER_LEXER_H

#include <stddef.h>

/* ── Token kinds ─────────────────────────────────────────────────── */
typedef enum {
    /* literals */
    TOK_INTEGER,
    TOK_OCTAL_INTEGER,
    TOK_HEX_INTEGER,
    TOK_DOUBLE,
    TOK_STRING,
    TOK_TRUE,
    TOK_FALSE,
    TOK_NULL,

    /* keywords */
    TOK_ALL,
    TOK_AND,
    TOK_ANY,
    TOK_ADD,
    TOK_AS,
    TOK_ASC,
    TOK_ASCENDING,
    TOK_BY,
    TOK_CALL,
    TOK_CASE,
    TOK_CONSTRAINT,
    TOK_CONTAINS,
    TOK_COUNT,
    TOK_CREATE,
    TOK_DELETE,
    TOK_DESC,
    TOK_DESCENDING,
    TOK_DETACH,
    TOK_DISTINCT,
    TOK_DO,
    TOK_DROP,
    TOK_ELSE,
    TOK_END,
    TOK_ENDS,
    TOK_EXISTS,
    TOK_EXTRACT,
    TOK_FALSE_KW = TOK_FALSE,   /* alias */
    TOK_FILTER,
    TOK_FOR,
    TOK_IN,
    TOK_IS,
    TOK_LIMIT,
    TOK_MANDATORY,
    TOK_MATCH,
    TOK_MERGE,
    TOK_NONE,
    TOK_NOT,
    TOK_OF,
    TOK_ON,
    TOK_OPTIONAL,
    TOK_OR,
    TOK_ORDER,
    TOK_REMOVE,
    TOK_REQUIRE,
    TOK_RETURN,
    TOK_SCALAR,
    TOK_SET,
    TOK_SINGLE,
    TOK_SKIP,
    TOK_STARTS,
    TOK_THEN,
    TOK_UNION,
    TOK_UNIQUE,
    TOK_UNWIND,
    TOK_WHEN,
    TOK_WHERE,
    TOK_WITH,
    TOK_XOR,
    TOK_YIELD,

    /* punctuation / operators */
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_DOT,
    TOK_DOTDOT,
    TOK_COMMA,
    TOK_SEMICOLON,
    TOK_COLON,
    TOK_PIPE,
    TOK_STAR,
    TOK_PLUS,
    TOK_MINUS,
    TOK_SLASH,
    TOK_PERCENT,
    TOK_CARET,
    TOK_EQ,
    TOK_NEQ,
    TOK_LT,
    TOK_GT,
    TOK_LTE,
    TOK_GTE,
    TOK_PLUSEQ,
    TOK_DOLLAR,
    TOK_ARROW_LEFT,
    TOK_ARROW_RIGHT,
    TOK_DASH,

    /* identifiers */
    TOK_IDENTIFIER,
    TOK_ESCAPED_IDENTIFIER,

    /* whitespace / comments */
    TOK_WS,
    TOK_COMMENT_BLOCK,
    TOK_COMMENT_LINE,

    TOK_EOF,
    TOK_ERROR
} TokenKind;

/* ── Token ───────────────────────────────────────────────────────── */
typedef struct {
    TokenKind   kind;
    const char *start;
    size_t      len;
    int         line;
} Token;

/* ── Lexer state ─────────────────────────────────────────────────── */
typedef struct {
    const char *p;
    const char *pe;
    const char *eof;
    const char *ts;
    const char *te;
    int         cs;
    int         act;
    int         line;
} Lexer;

/* ── API ─────────────────────────────────────────────────────────── */
void lexer_init(Lexer *lex, const char *input, size_t len);
int  lexer_next(Lexer *lex, Token *tok);
const char *token_name(TokenKind k);

#endif /* CYPHER_LEXER_H */
