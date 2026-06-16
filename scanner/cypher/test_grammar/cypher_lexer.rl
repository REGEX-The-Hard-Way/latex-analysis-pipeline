/*
 * cypher_lexer.rl
 *
 * Ragel tokenizer for openCypher (Apache License 2.0, openCypher community).
 * Targets C with the default table-driven backend.
 *
 * Compile:
 *   ragel -C -T1 cypher_lexer.rl -o cypher_lexer.c
 *   cc -o cypher_lexer cypher_lexer.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    TOK_ADD,
    TOK_ANY,
    TOK_WHERE,
    TOK_WHEN,
    TOK_WITH,
    TOK_XOR,
    TOK_YIELD,

    /* punctuation / operators */
    TOK_LPAREN,          /* ( */
    TOK_RPAREN,          /* ) */
    TOK_LBRACKET,        /* [ */
    TOK_RBRACKET,        /* ] */
    TOK_LBRACE,          /* { */
    TOK_RBRACE,          /* } */
    TOK_DOT,             /* . */
    TOK_DOTDOT,          /* .. */
    TOK_COMMA,           /* , */
    TOK_SEMICOLON,       /* ; */
    TOK_COLON,           /* : */
    TOK_PIPE,            /* | */
    TOK_STAR,            /* * */
    TOK_PLUS,            /* + */
    TOK_MINUS,           /* - (ASCII only; see dash set below) */
    TOK_SLASH,           /* / */
    TOK_PERCENT,         /* % */
    TOK_CARET,           /* ^ */
    TOK_EQ,              /* = */
    TOK_NEQ,             /* <> */
    TOK_LT,              /* < */
    TOK_GT,              /* > */
    TOK_LTE,             /* <= */
    TOK_GTE,             /* >= */
    TOK_PLUSEQ,          /* += */
    TOK_DOLLAR,          /* $ (parameter prefix) */

    /* arrow / dash Unicode variants are folded into */
    TOK_ARROW_LEFT,      /* any left-arrow-head codepoint */
    TOK_ARROW_RIGHT,     /* any right-arrow-head codepoint */
    TOK_DASH,            /* any dash codepoint */

    /* identifiers */
    TOK_IDENTIFIER,
    TOK_ESCAPED_IDENTIFIER,  /* `...` */

    /* whitespace / comments (normally skipped; exposed for completeness) */
    TOK_WS,
    TOK_COMMENT_BLOCK,
    TOK_COMMENT_LINE,

    TOK_EOF,
    TOK_ERROR
} TokenKind;

static const char *token_name(TokenKind k) {
    switch (k) {
#define N(x) case TOK_##x: return #x
        N(INTEGER); N(OCTAL_INTEGER); N(HEX_INTEGER); N(DOUBLE); N(STRING);
        N(TRUE); N(FALSE); N(NULL);
        N(ALL); N(AND); N(AS); N(ASC); N(ASCENDING); N(BY);
        N(CALL); N(CASE); N(CONSTRAINT); N(CONTAINS); N(COUNT); N(CREATE);
        N(DELETE); N(DESC); N(DESCENDING); N(DETACH); N(DISTINCT);
        N(DO); N(DROP); N(ELSE); N(END); N(ENDS); N(EXISTS);
        N(EXTRACT); N(FILTER); N(FOR); N(IN); N(IS); N(LIMIT);
        N(MANDATORY); N(MATCH); N(MERGE); N(NONE); N(NOT);
        N(OF); N(ON); N(OPTIONAL); N(OR); N(ORDER);
        N(REMOVE); N(REQUIRE); N(RETURN); N(SCALAR); N(SET);
        N(SINGLE); N(SKIP); N(STARTS); N(THEN); N(UNION);
        N(UNIQUE); N(UNWIND); N(ADD); N(ANY); N(WHERE); N(WHEN);
        N(WITH); N(XOR); N(YIELD);
        N(LPAREN); N(RPAREN); N(LBRACKET); N(RBRACKET);
        N(LBRACE); N(RBRACE);
        N(DOT); N(DOTDOT); N(COMMA); N(SEMICOLON); N(COLON); N(PIPE);
        N(STAR); N(PLUS); N(MINUS); N(SLASH); N(PERCENT); N(CARET);
        N(EQ); N(NEQ); N(LT); N(GT); N(LTE); N(GTE); N(PLUSEQ);
        N(DOLLAR); N(ARROW_LEFT); N(ARROW_RIGHT); N(DASH);
        N(IDENTIFIER); N(ESCAPED_IDENTIFIER);
        N(WS); N(COMMENT_BLOCK); N(COMMENT_LINE);
        N(EOF); N(ERROR);
#undef N
        default: return "UNKNOWN";
    }
}

/* ── Token struct ────────────────────────────────────────────────── */
typedef struct {
    TokenKind kind;
    const char *start;
    size_t      len;
    int         line;
} Token;

/* ── Lexer state ─────────────────────────────────────────────────── */
typedef struct {
    const char *p;      /* current position */
    const char *pe;     /* one past end of input */
    const char *eof;    /* same as pe for complete buffers */
    const char *ts;     /* token start (set by ragel) */
    const char *te;     /* token end   (set by ragel) */
    int         cs;     /* current FSM state */
    int         act;    /* last action */
    int         line;
} Lexer;

%%{
    machine cypher_lexer;

    # ── Helpers ────────────────────────────────────────────────────

    SPACE     = ' ';
    TAB       = '\t';
    LF        = '\n';
    CR        = '\r';
    VT        = '\v';
    FF        = '\f';

    # Additional Unicode whitespace passed through as raw bytes (UTF-8 encoded).
    # We list the relevant code-points from the grammar.
    UNI_WS = (
        "\xc2\xa0"         | # U+00A0  NO-BREAK SPACE
        "\xe1\x9a\x80"     | # U+1680  OGHAM SPACE MARK
        "\xe1\xa0\x8e"     | # U+180E  MONGOLIAN VOWEL SEPARATOR
        "\xe2\x80\x80"     | # U+2000
        "\xe2\x80\x81"     | # U+2001
        "\xe2\x80\x82"     | # U+2002
        "\xe2\x80\x83"     | # U+2003
        "\xe2\x80\x84"     | # U+2004
        "\xe2\x80\x85"     | # U+2005
        "\xe2\x80\x86"     | # U+2006
        "\xe2\x80\x87"     | # U+2007 (FIGURE SPACE – listed as whitespace)
        "\xe2\x80\x88"     | # U+2008
        "\xe2\x80\x89"     | # U+2009
        "\xe2\x80\x8a"     | # U+200A
        "\xe2\x80\xa8"     | # U+2028  LINE SEPARATOR
        "\xe2\x80\xa9"     | # U+2029  PARAGRAPH SEPARATOR
        "\xe2\x80\xaf"     | # U+202F
        "\xe2\x81\x9f"     | # U+205F
        "\xe3\x80\x80"       # U+3000  IDEOGRAPHIC SPACE
    );

    WHITESPACE = SPACE | TAB | LF | CR | VT | FF |
                 '\x1c' | '\x1d' | '\x1e' | '\x1f' |  # FS GS RS US
                 UNI_WS;

    comment_block = '/*' ( [^*] | ('*' [^/]) )* '*/';
    comment_line  = '//' ( [^\n\r] )* (CR? (LF | '\0'));

    SP = (WHITESPACE | comment_block | comment_line)+;

    # ── Numeric literals ───────────────────────────────────────────

    ZeroDigit    = '0';
    NonZeroOct   = [1-7];
    NonZeroDigit = NonZeroOct | '8' | '9';
    OctDigit     = ZeroDigit | NonZeroOct;
    Digit        = ZeroDigit | NonZeroDigit;
    HexLetter    = [a-fA-F];
    HexDigit     = Digit | HexLetter;

    DecimalInteger = ZeroDigit | (NonZeroDigit Digit*);
    OctalInteger   = ZeroDigit OctDigit+;
    HexInteger     = '0x' HexDigit+;

    ExponentDecimalReal =
        ( Digit+ | (Digit+ '.' Digit+) | ('.' Digit+) )
        [eE] '-'? Digit+;
    RegularDecimalReal = Digit* '.' Digit+;

    # ── String literals ────────────────────────────────────────────

    EscapedChar = '\\' (
        '\\' | '\'' | '"' | [bBfFnNrRtT] |
        [uU] HexDigit{4} |
        [uU] HexDigit{8}
    );
    StringDoubleQ = '"'  ( [^"\\] | EscapedChar )* '"';
    StringSingleQ = '\'' ( [^'\\] | EscapedChar )* '\'';
    StringLiteral = StringDoubleQ | StringSingleQ;

    # ── Identifiers ────────────────────────────────────────────────
    # openCypher uses Unicode ID_Start / ID_Continue (plus _ from Pc, $ from Sc).
    # For ASCII-range correctness we cover the common case; full Unicode
    # processing would require a post-pass with a Unicode library.

    IdentStart = [a-zA-Z_] | '\xc2\xa4';   # rough ASCII + currency fallback
    IdentCont  = IdentStart | Digit | '\xc2\xb7';

    UnescapedName    = IdentStart IdentCont*;
    EscapedName      = ('`' [^`]* '`')+;

    # ── Arrow / dash Unicode variants ─────────────────────────────

    LeftArrow  = '<' |
                 "\xe2\x9f\xa8" |   # U+27E8
                 "\xe3\x80\x88" |   # U+3008
                 "\xef\xb9\xa4" |   # UFE64
                 "\xef\xbc\x9c";    # UFF1C

    RightArrow = '>' |
                 "\xe2\x9f\xa9" |   # U+27E9
                 "\xe3\x80\x89" |   # U+3009
                 "\xef\xb9\xa5" |   # UFE65
                 "\xef\xbc\x9e";    # UFF1E

    Dash = '-' |
           "\xc2\xad"   |   # U+00AD  SOFT HYPHEN
           "\xe2\x80\x90" | # U+2010
           "\xe2\x80\x91" | # U+2011
           "\xe2\x80\x92" | # U+2012
           "\xe2\x80\x93" | # U+2013
           "\xe2\x80\x94" | # U+2014
           "\xe2\x80\x95" | # U+2015
           "\xe2\x88\x92" | # U+2212  MINUS SIGN
           "\xef\xb9\x98" | # UFE58
           "\xef\xb9\xa3" | # UFE63
           "\xef\xbc\x8d";  # UFF0D

    # ── Case-insensitive keywords (generated from the grammar) ─────
    # Each is followed by a negative lookahead for IdentCont to avoid
    # matching prefixes of identifiers.  Ragel doesn't have lookahead
    # natively, so we rely on longest-match (the scanner machine picks
    # the longer alternative when there is ambiguity).

    KW_ALL        = [Aa][Ll][Ll];
    KW_AND        = [Aa][Nn][Dd];
    KW_ANY        = [Aa][Nn][Yy];
    KW_ADD        = [Aa][Dd][Dd];
    KW_AS         = [Aa][Ss];
    KW_ASC        = [Aa][Ss][Cc];
    KW_ASCENDING  = [Aa][Ss][Cc][Ee][Nn][Dd][Ii][Nn][Gg];
    KW_BY         = [Bb][Yy];
    KW_CALL       = [Cc][Aa][Ll][Ll];
    KW_CASE       = [Cc][Aa][Ss][Ee];
    KW_CONSTRAINT = [Cc][Oo][Nn][Ss][Tt][Rr][Aa][Ii][Nn][Tt];
    KW_CONTAINS   = [Cc][Oo][Nn][Tt][Aa][Ii][Nn][Ss];
    KW_COUNT      = [Cc][Oo][Uu][Nn][Tt];
    KW_CREATE     = [Cc][Rr][Ee][Aa][Tt][Ee];
    KW_DELETE     = [Dd][Ee][Ll][Ee][Tt][Ee];
    KW_DESC       = [Dd][Ee][Ss][Cc];
    KW_DESCENDING = [Dd][Ee][Ss][Cc][Ee][Nn][Dd][Ii][Nn][Gg];
    KW_DETACH     = [Dd][Ee][Tt][Aa][Cc][Hh];
    KW_DISTINCT   = [Dd][Ii][Ss][Tt][Ii][Nn][Cc][Tt];
    KW_DO         = [Dd][Oo];
    KW_DROP       = [Dd][Rr][Oo][Pp];
    KW_ELSE       = [Ee][Ll][Ss][Ee];
    KW_END        = [Ee][Nn][Dd];
    KW_ENDS       = [Ee][Nn][Dd][Ss];
    KW_EXISTS     = [Ee][Xx][Ii][Ss][Tt][Ss];
    KW_EXTRACT    = [Ee][Xx][Tt][Rr][Aa][Cc][Tt];
    KW_FALSE      = [Ff][Aa][Ll][Ss][Ee];
    KW_FILTER     = [Ff][Ii][Ll][Tt][Ee][Rr];
    KW_FOR        = [Ff][Oo][Rr];
    KW_IN         = [Ii][Nn];
    KW_IS         = [Ii][Ss];
    KW_LIMIT      = [Ll][Ii][Mm][Ii][Tt];
    KW_MANDATORY  = [Mm][Aa][Nn][Dd][Aa][Tt][Oo][Rr][Yy];
    KW_MATCH      = [Mm][Aa][Tt][Cc][Hh];
    KW_MERGE      = [Mm][Ee][Rr][Gg][Ee];
    KW_NONE       = [Nn][Oo][Nn][Ee];
    KW_NOT        = [Nn][Oo][Tt];
    KW_NULL       = [Nn][Uu][Ll][Ll];
    KW_OF         = [Oo][Ff];
    KW_ON         = [Oo][Nn];
    KW_OPTIONAL   = [Oo][Pp][Tt][Ii][Oo][Nn][Aa][Ll];
    KW_OR         = [Oo][Rr];
    KW_ORDER      = [Oo][Rr][Dd][Ee][Rr];
    KW_REMOVE     = [Rr][Ee][Mm][Oo][Vv][Ee];
    KW_REQUIRE    = [Rr][Ee][Qq][Uu][Ii][Rr][Ee];
    KW_RETURN     = [Rr][Ee][Tt][Uu][Rr][Nn];
    KW_SCALAR     = [Ss][Cc][Aa][Ll][Aa][Rr];
    KW_SET        = [Ss][Ee][Tt];
    KW_SINGLE     = [Ss][Ii][Nn][Gg][Ll][Ee];
    KW_SKIP       = [Ss][Kk][Ii][Pp];
    KW_STARTS     = [Ss][Tt][Aa][Rr][Tt][Ss];
    KW_THEN       = [Tt][Hh][Ee][Nn];
    KW_TRUE       = [Tt][Rr][Uu][Ee];
    KW_UNION      = [Uu][Nn][Ii][Oo][Nn];
    KW_UNIQUE     = [Uu][Nn][Ii][Qq][Uu][Ee];
    KW_UNWIND     = [Uu][Nn][Ww][Ii][Nn][Dd];
    KW_WHEN       = [Ww][Hh][Ee][Nn];
    KW_WHERE      = [Ww][Hh][Ee][Rr][Ee];
    KW_WITH       = [Ww][Ii][Tt][Hh];
    KW_XOR        = [Xx][Oo][Rr];
    KW_YIELD      = [Yy][Ii][Ee][Ll][Dd];

    # ── Scanner ────────────────────────────────────────────────────
    # We use the 'scanner' construct so Ragel applies longest-match.
    # Keywords are tried before identifiers; when a keyword pattern
    # ends at an IdentCont boundary, the identifier rule will win
    # (longer match), turning e.g. "MATCH2" into an identifier.

    main := |*

        SP                  { tok.kind = TOK_WS;                fbreak; };
        comment_block       { tok.kind = TOK_COMMENT_BLOCK;     fbreak; };
        comment_line        { tok.kind = TOK_COMMENT_LINE;      fbreak; };

        # -- Keywords: must appear before UnescapedName to give longest-match
        #    a chance to pick the identifier when characters follow.

        KW_ASCENDING  IdentCont*  { tok.kind = (te > ts+9) ? TOK_IDENTIFIER : TOK_ASCENDING;  fbreak; };
        KW_DESCENDING IdentCont*  { tok.kind = (te > ts+10)? TOK_IDENTIFIER : TOK_DESCENDING; fbreak; };
        KW_CONSTRAINT IdentCont*  { tok.kind = (te > ts+10)? TOK_IDENTIFIER : TOK_CONSTRAINT; fbreak; };
        KW_MANDATORY  IdentCont*  { tok.kind = (te > ts+9) ? TOK_IDENTIFIER : TOK_MANDATORY;  fbreak; };
        KW_OPTIONAL   IdentCont*  { tok.kind = (te > ts+8) ? TOK_IDENTIFIER : TOK_OPTIONAL;   fbreak; };
        KW_DISTINCT   IdentCont*  { tok.kind = (te > ts+8) ? TOK_IDENTIFIER : TOK_DISTINCT;   fbreak; };
        KW_CONTAINS   IdentCont*  { tok.kind = (te > ts+8) ? TOK_IDENTIFIER : TOK_CONTAINS;   fbreak; };
        KW_REQUIRE    IdentCont*  { tok.kind = (te > ts+7) ? TOK_IDENTIFIER : TOK_REQUIRE;    fbreak; };
        KW_EXTRACT    IdentCont*  { tok.kind = (te > ts+7) ? TOK_IDENTIFIER : TOK_EXTRACT;    fbreak; };
        KW_DETACH     IdentCont*  { tok.kind = (te > ts+6) ? TOK_IDENTIFIER : TOK_DETACH;     fbreak; };
        KW_STARTS     IdentCont*  { tok.kind = (te > ts+6) ? TOK_IDENTIFIER : TOK_STARTS;     fbreak; };
        KW_REMOVE     IdentCont*  { tok.kind = (te > ts+6) ? TOK_IDENTIFIER : TOK_REMOVE;     fbreak; };
        KW_RETURN     IdentCont*  { tok.kind = (te > ts+6) ? TOK_IDENTIFIER : TOK_RETURN;     fbreak; };
        KW_SCALAR     IdentCont*  { tok.kind = (te > ts+6) ? TOK_IDENTIFIER : TOK_SCALAR;     fbreak; };
        KW_SINGLE     IdentCont*  { tok.kind = (te > ts+6) ? TOK_IDENTIFIER : TOK_SINGLE;     fbreak; };
        KW_UNWIND     IdentCont*  { tok.kind = (te > ts+6) ? TOK_IDENTIFIER : TOK_UNWIND;     fbreak; };
        KW_UNIQUE     IdentCont*  { tok.kind = (te > ts+6) ? TOK_IDENTIFIER : TOK_UNIQUE;     fbreak; };
        KW_EXISTS     IdentCont*  { tok.kind = (te > ts+6) ? TOK_IDENTIFIER : TOK_EXISTS;     fbreak; };
        KW_FILTER     IdentCont*  { tok.kind = (te > ts+6) ? TOK_IDENTIFIER : TOK_FILTER;     fbreak; };
        KW_CREATE     IdentCont*  { tok.kind = (te > ts+6) ? TOK_IDENTIFIER : TOK_CREATE;     fbreak; };
        KW_DELETE     IdentCont*  { tok.kind = (te > ts+6) ? TOK_IDENTIFIER : TOK_DELETE;     fbreak; };
        KW_MERGE      IdentCont*  { tok.kind = (te > ts+5) ? TOK_IDENTIFIER : TOK_MERGE;      fbreak; };
        KW_MATCH      IdentCont*  { tok.kind = (te > ts+5) ? TOK_IDENTIFIER : TOK_MATCH;      fbreak; };
        KW_LIMIT      IdentCont*  { tok.kind = (te > ts+5) ? TOK_IDENTIFIER : TOK_LIMIT;      fbreak; };
        KW_FALSE      IdentCont*  { tok.kind = (te > ts+5) ? TOK_IDENTIFIER : TOK_FALSE;      fbreak; };
        KW_UNION      IdentCont*  { tok.kind = (te > ts+5) ? TOK_IDENTIFIER : TOK_UNION;      fbreak; };
        KW_YIELD      IdentCont*  { tok.kind = (te > ts+5) ? TOK_IDENTIFIER : TOK_YIELD;      fbreak; };
        KW_WHERE      IdentCont*  { tok.kind = (te > ts+5) ? TOK_IDENTIFIER : TOK_WHERE;      fbreak; };
        KW_WHEN       IdentCont*  { tok.kind = (te > ts+4) ? TOK_IDENTIFIER : TOK_WHEN;       fbreak; };
        KW_WITH       IdentCont*  { tok.kind = (te > ts+4) ? TOK_IDENTIFIER : TOK_WITH;       fbreak; };
        KW_THEN       IdentCont*  { tok.kind = (te > ts+4) ? TOK_IDENTIFIER : TOK_THEN;       fbreak; };
        KW_TRUE       IdentCont*  { tok.kind = (te > ts+4) ? TOK_IDENTIFIER : TOK_TRUE;       fbreak; };
        KW_SKIP       IdentCont*  { tok.kind = (te > ts+4) ? TOK_IDENTIFIER : TOK_SKIP;       fbreak; };
        KW_NONE       IdentCont*  { tok.kind = (te > ts+4) ? TOK_IDENTIFIER : TOK_NONE;       fbreak; };
        KW_NULL       IdentCont*  { tok.kind = (te > ts+4) ? TOK_IDENTIFIER : TOK_NULL;       fbreak; };
        KW_CASE       IdentCont*  { tok.kind = (te > ts+4) ? TOK_IDENTIFIER : TOK_CASE;       fbreak; };
        KW_CALL       IdentCont*  { tok.kind = (te > ts+4) ? TOK_IDENTIFIER : TOK_CALL;       fbreak; };
        KW_ELSE       IdentCont*  { tok.kind = (te > ts+4) ? TOK_IDENTIFIER : TOK_ELSE;       fbreak; };
        KW_ENDS       IdentCont*  { tok.kind = (te > ts+4) ? TOK_IDENTIFIER : TOK_ENDS;       fbreak; };
        KW_COUNT      IdentCont*  { tok.kind = (te > ts+5) ? TOK_IDENTIFIER : TOK_COUNT;      fbreak; };
        KW_ORDER      IdentCont*  { tok.kind = (te > ts+5) ? TOK_IDENTIFIER : TOK_ORDER;      fbreak; };
        KW_AND        IdentCont*  { tok.kind = (te > ts+3) ? TOK_IDENTIFIER : TOK_AND;        fbreak; };
        KW_ANY        IdentCont*  { tok.kind = (te > ts+3) ? TOK_IDENTIFIER : TOK_ANY;        fbreak; };
        KW_ADD        IdentCont*  { tok.kind = (te > ts+3) ? TOK_IDENTIFIER : TOK_ADD;        fbreak; };
        KW_ALL        IdentCont*  { tok.kind = (te > ts+3) ? TOK_IDENTIFIER : TOK_ALL;        fbreak; };
        KW_ASC        IdentCont*  { tok.kind = (te > ts+3) ? TOK_IDENTIFIER : TOK_ASC;        fbreak; };
        KW_NOT        IdentCont*  { tok.kind = (te > ts+3) ? TOK_IDENTIFIER : TOK_NOT;        fbreak; };
        KW_XOR        IdentCont*  { tok.kind = (te > ts+3) ? TOK_IDENTIFIER : TOK_XOR;        fbreak; };
        KW_FOR        IdentCont*  { tok.kind = (te > ts+3) ? TOK_IDENTIFIER : TOK_FOR;        fbreak; };
        KW_SET        IdentCont*  { tok.kind = (te > ts+3) ? TOK_IDENTIFIER : TOK_SET;        fbreak; };
        KW_END        IdentCont*  { tok.kind = (te > ts+3) ? TOK_IDENTIFIER : TOK_END;        fbreak; };
        KW_DESC       IdentCont*  { tok.kind = (te > ts+4) ? TOK_IDENTIFIER : TOK_DESC;       fbreak; };
        KW_DROP       IdentCont*  { tok.kind = (te > ts+4) ? TOK_IDENTIFIER : TOK_DROP;       fbreak; };
        KW_IN         IdentCont*  { tok.kind = (te > ts+2) ? TOK_IDENTIFIER : TOK_IN;         fbreak; };
        KW_IS         IdentCont*  { tok.kind = (te > ts+2) ? TOK_IDENTIFIER : TOK_IS;         fbreak; };
        KW_OR         IdentCont*  { tok.kind = (te > ts+2) ? TOK_IDENTIFIER : TOK_OR;         fbreak; };
        KW_ON         IdentCont*  { tok.kind = (te > ts+2) ? TOK_IDENTIFIER : TOK_ON;         fbreak; };
        KW_OF         IdentCont*  { tok.kind = (te > ts+2) ? TOK_IDENTIFIER : TOK_OF;         fbreak; };
        KW_DO         IdentCont*  { tok.kind = (te > ts+2) ? TOK_IDENTIFIER : TOK_DO;         fbreak; };
        KW_BY         IdentCont*  { tok.kind = (te > ts+2) ? TOK_IDENTIFIER : TOK_BY;         fbreak; };
        KW_AS         IdentCont*  { tok.kind = (te > ts+2) ? TOK_IDENTIFIER : TOK_AS;         fbreak; };

        # -- Identifiers (unescaped & escaped) --
        UnescapedName  { tok.kind = TOK_IDENTIFIER;          fbreak; };
        EscapedName    { tok.kind = TOK_ESCAPED_IDENTIFIER;  fbreak; };

        # -- Numeric literals (order matters: longer patterns first) --
        HexInteger              { tok.kind = TOK_HEX_INTEGER;   fbreak; };
        OctalInteger            { tok.kind = TOK_OCTAL_INTEGER;  fbreak; };
        ExponentDecimalReal     { tok.kind = TOK_DOUBLE;         fbreak; };
        RegularDecimalReal      { tok.kind = TOK_DOUBLE;         fbreak; };
        DecimalInteger          { tok.kind = TOK_INTEGER;        fbreak; };

        # -- String literals --
        StringLiteral           { tok.kind = TOK_STRING;         fbreak; };

        # -- Multi-character punctuation (before single-char) --
        '..'   { tok.kind = TOK_DOTDOT;   fbreak; };
        '<>'   { tok.kind = TOK_NEQ;      fbreak; };
        '<='   { tok.kind = TOK_LTE;      fbreak; };
        '>='   { tok.kind = TOK_GTE;      fbreak; };
        '+='   { tok.kind = TOK_PLUSEQ;   fbreak; };

        # -- Arrow / dash Unicode variants --
        LeftArrow   { tok.kind = TOK_ARROW_LEFT;   fbreak; };
        RightArrow  { tok.kind = TOK_ARROW_RIGHT;  fbreak; };
        Dash        { tok.kind = TOK_DASH;          fbreak; };

        # -- Single-character punctuation --
        '('  { tok.kind = TOK_LPAREN;    fbreak; };
        ')'  { tok.kind = TOK_RPAREN;    fbreak; };
        '['  { tok.kind = TOK_LBRACKET;  fbreak; };
        ']'  { tok.kind = TOK_RBRACKET;  fbreak; };
        '{'  { tok.kind = TOK_LBRACE;    fbreak; };
        '}'  { tok.kind = TOK_RBRACE;    fbreak; };
        '.'  { tok.kind = TOK_DOT;       fbreak; };
        ','  { tok.kind = TOK_COMMA;     fbreak; };
        ';'  { tok.kind = TOK_SEMICOLON; fbreak; };
        ':'  { tok.kind = TOK_COLON;     fbreak; };
        '|'  { tok.kind = TOK_PIPE;      fbreak; };
        '*'  { tok.kind = TOK_STAR;      fbreak; };
        '+'  { tok.kind = TOK_PLUS;      fbreak; };
        '/'  { tok.kind = TOK_SLASH;     fbreak; };
        '%'  { tok.kind = TOK_PERCENT;   fbreak; };
        '^'  { tok.kind = TOK_CARET;     fbreak; };
        '='  { tok.kind = TOK_EQ;        fbreak; };
        '$'  { tok.kind = TOK_DOLLAR;    fbreak; };

        any  { tok.kind = TOK_ERROR;     fbreak; };

    *|;

    write data;
}%%

/* ── Public API ──────────────────────────────────────────────────── */

void lexer_init(Lexer *lex, const char *input, size_t len) {
    lex->p    = input;
    lex->pe   = input + len;
    lex->eof  = lex->pe;
    lex->line = 1;
    %% write init;
}

/*
 * lexer_next – fills *tok and returns 1, or returns 0 at EOF.
 * Whitespace / comment tokens are returned as TOK_WS / TOK_COMMENT_*.
 * Callers that want to skip them should loop until a non-WS token.
 */
int lexer_next(Lexer *lex, Token *tok) {
    if (lex->p >= lex->pe) {
        tok->kind  = TOK_EOF;
        tok->start = lex->p;
        tok->len   = 0;
        tok->line  = lex->line;
        return 0;
    }

    %% write exec;

    tok->start = lex->ts;
    tok->len   = (size_t)(lex->te - lex->ts);
    tok->line  = lex->line;

    /* track newlines */
    for (const char *c = lex->ts; c < lex->te; c++)
        if (*c == '\n') lex->line++;

    return 1;
}

/* ── Demo main ───────────────────────────────────────────────────── */
#ifdef CYPHER_LEXER_MAIN
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <cypher-query>\n", argv[0]);
        return 1;
    }
    const char *input = argv[1];
    size_t len = strlen(input);

    Lexer lex;
    lexer_init(&lex, input, len);

    Token tok;
    while (lexer_next(&lex, &tok)) {
        if (tok.kind == TOK_WS ||
            tok.kind == TOK_COMMENT_BLOCK ||
            tok.kind == TOK_COMMENT_LINE)
            continue;   /* skip whitespace in demo output */
        printf("%-22s  |%.*s|\n",
               token_name(tok.kind),
               (int)tok.len, tok.start);
    }
    printf("%-22s\n", token_name(TOK_EOF));
    return 0;
}
#endif /* CYPHER_LEXER_MAIN */
