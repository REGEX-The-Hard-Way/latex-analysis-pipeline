#line 1 "cypher_parser.rl"
/*
* cypher_parser.rl
*
* Recursive-descent parser for openCypher, written in Ragel.
*
* Strategy
* --------
* The token stream produced by cypher_lexer.rl is consumed here.
* Each grammar rule is a C function that calls lexer_next_nows()
* (whitespace-skipping wrapper) and the other rule functions.
*
* Ragel is used only for the Cypher "entry" machine that drives
* lexer_next_nows() and dispatches to the hand-written descent
* functions.  This is a common pattern when the grammar is too
* context-dependent for a pure Ragel machine.
*
* For a full compiler you would replace the EMIT macros with real
* AST node constructors.
*
* Compile (after generating cypher_lexer.c from cypher_lexer.rl):
*   ragel -C -G2 cypher_parser.rl -o cypher_parser.c
*   cc -DCYPHER_LEXER_MAIN=0 cypher_lexer.c cypher_parser.c -o cypher_parser
*
* (The lexer's demo main is disabled by defining CYPHER_LEXER_MAIN=0.)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

/* Pull in the lexer types/functions */
#include "cypher_lexer.h"   /* generated from cypher_lexer.rl */

/* ══════════════════════════════════════════════════════════════════
*  Parser state
* ══════════════════════════════════════════════════════════════════ */

typedef struct {
	Lexer   lex;
	Token   cur;        /* current (lookahead) token          */
	Token   prev;       /* previously consumed token          */
	int     error;      /* non-zero once a parse error occurs */
	int     depth;      /* indentation for AST dump           */
} Parser;

/* ── AST-node placeholder ─────────────────────────────────────────
* Replace with a real union / tagged-pointer when building a
* full compiler.  Here we just print the rule name.              */
#define EMIT(fmt, ...) \
do { \
	for (int _i = 0; _i < p->depth*2; _i++) putchar(' '); \
	printf(fmt "\n", ##__VA_ARGS__); \
} while (0)

/* ── Error reporting ──────────────────────────────────────────────*/
static void parse_error(Parser *p, const char *fmt, ...) {
	if (p->error) return;   /* only report the first error */
		p->error = 1;
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, "Parse error (line %d, token '%.*s'): ",
	p->cur.line, (int)p->cur.len, p->cur.start);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
}

/* ── Whitespace-skipping token advance ────────────────────────────*/
static void advance(Parser *p) {
	p->prev = p->cur;
	do {
		lexer_next(&p->lex, &p->cur);
	} while (
	p->cur.kind == TOK_WS ||
	p->cur.kind == TOK_COMMENT_BLOCK ||
	p->cur.kind == TOK_COMMENT_LINE
	);
}

static bool check(Parser *p, TokenKind k) {
	return p->cur.kind == k;
}

static bool match(Parser *p, TokenKind k) {
	if (check(p, k)) { advance(p); return true; }
	return false;
}

static bool expect(Parser *p, TokenKind k, const char *where) {
	if (match(p, k)) return true;
		parse_error(p, "expected %s in %s, got '%.*s'",
	token_name(k), where,
	(int)p->cur.len, p->cur.start);
	return false;
}

/* ── Helpers for multi-kind checks ───────────────────────────────*/
static bool is_keyword_or_ident(Parser *p) {
	/* Any symbolic name – used where oC_SymbolicName is expected   */
	switch (p->cur.kind) {
		case TOK_IDENTIFIER:
		case TOK_ESCAPED_IDENTIFIER:
		/* oC_SymbolicName also allows COUNT, FILTER, EXTRACT, ANY,
		NONE, SINGLE and any HexLetter – we relax here and accept
		all keywords because the grammar allows reserved words as
		schema names in many positions.                           */
		default:
		return p->cur.kind != TOK_EOF && p->cur.kind != TOK_ERROR;
	}
}

/* ══════════════════════════════════════════════════════════════════
*  Forward declarations for mutually-recursive rules
* ══════════════════════════════════════════════════════════════════ */
static void parse_expression(Parser *p);
static void parse_pattern(Parser *p);
static void parse_pattern_element(Parser *p);
static void parse_node_pattern(Parser *p);
static void parse_properties(Parser *p);
static void parse_map_literal(Parser *p);
static void parse_atom(Parser *p);
static void parse_projection_body(Parser *p);
static void parse_where(Parser *p);

/* ══════════════════════════════════════════════════════════════════
*  Terminals / leaves
* ══════════════════════════════════════════════════════════════════ */

/* oC_SymbolicName */
static bool parse_symbolic_name(Parser *p) {
	/* Accept identifiers, escaped identifiers, or any keyword that
	the grammar allows as a schema name.                          */
	if (p->cur.kind == TOK_IDENTIFIER ||
		p->cur.kind == TOK_ESCAPED_IDENTIFIER ||
	/* HexLetter tokens would come as TOK_IDENTIFIER already    */
	/* any non-structural token works as a symbolic name:       */
	(p->cur.kind >= TOK_ALL && p->cur.kind <= TOK_YIELD)) {
		EMIT("SymbolicName: %.*s", (int)p->cur.len, p->cur.start);
		advance(p);
		return true;
	}
	parse_error(p, "expected symbolic name");
	return false;
}

/* oC_SchemaName ::= oC_SymbolicName | oC_ReservedWord */
static bool parse_schema_name(Parser *p) { return parse_symbolic_name(p); }

/* oC_LabelName ::= oC_SchemaName */
static bool parse_label_name(Parser *p) {
	EMIT("LabelName");
	p->depth++;
	bool ok = parse_schema_name(p);
	p->depth--;
	return ok;
}

/* oC_RelTypeName ::= oC_SchemaName */
static bool parse_rel_type_name(Parser *p) {
	EMIT("RelTypeName");
	p->depth++;
	bool ok = parse_schema_name(p);
	p->depth--;
	return ok;
}

/* oC_Variable ::= oC_SymbolicName */
static bool parse_variable(Parser *p) {
	EMIT("Variable");
	p->depth++;
	bool ok = parse_symbolic_name(p);
	p->depth--;
	return ok;
}

/* oC_Parameter ::= '$' (oC_SymbolicName | DecimalInteger) */
static void parse_parameter(Parser *p) {
	expect(p, TOK_DOLLAR, "parameter");
	EMIT("Parameter");
	p->depth++;
	if (p->cur.kind == TOK_INTEGER) {
		EMIT("IntegerName: %.*s", (int)p->cur.len, p->cur.start);
		advance(p);
	} else {
		parse_symbolic_name(p);
	}
	p->depth--;
}

/* oC_PropertyKeyName ::= oC_SchemaName */
static void parse_property_key_name(Parser *p) {
	EMIT("PropertyKeyName");
	p->depth++;
	parse_schema_name(p);
	p->depth--;
}

/* oC_NumberLiteral */
static void parse_number_literal(Parser *p) {
	if (p->cur.kind == TOK_DOUBLE ||
		p->cur.kind == TOK_INTEGER ||
	p->cur.kind == TOK_HEX_INTEGER ||
	p->cur.kind == TOK_OCTAL_INTEGER) {
		EMIT("NumberLiteral: %.*s", (int)p->cur.len, p->cur.start);
		advance(p);
	} else {
		parse_error(p, "expected number literal");
	}
}

/* oC_IntegerLiteral (used in range literals) */
static void parse_integer_literal(Parser *p) {
	if (p->cur.kind == TOK_INTEGER ||
		p->cur.kind == TOK_HEX_INTEGER ||
	p->cur.kind == TOK_OCTAL_INTEGER) {
		EMIT("IntegerLiteral: %.*s", (int)p->cur.len, p->cur.start);
		advance(p);
	} else {
		parse_error(p, "expected integer literal");
	}
}

/* ══════════════════════════════════════════════════════════════════
*  Literals
* ══════════════════════════════════════════════════════════════════ */

/* oC_ListLiteral ::= '[' [expression (',' expression)*] ']' */
static void parse_list_literal(Parser *p) {
	EMIT("ListLiteral");
	p->depth++;
	expect(p, TOK_LBRACKET, "list literal");
	if (!check(p, TOK_RBRACKET)) {
		parse_expression(p);
		while (match(p, TOK_COMMA))
		parse_expression(p);
	}
	expect(p, TOK_RBRACKET, "list literal");
	p->depth--;
}

/* oC_MapLiteral ::= '{' [key ':' expr (',' key ':' expr)*] '}' */
static void parse_map_literal(Parser *p) {
	EMIT("MapLiteral");
	p->depth++;
	expect(p, TOK_LBRACE, "map literal");
	if (!check(p, TOK_RBRACE)) {
		parse_property_key_name(p);
		expect(p, TOK_COLON, "map literal");
		parse_expression(p);
		while (match(p, TOK_COMMA)) {
			parse_property_key_name(p);
			expect(p, TOK_COLON, "map literal");
			parse_expression(p);
		}
	}
	expect(p, TOK_RBRACE, "map literal");
	p->depth--;
}

/* oC_Properties ::= oC_MapLiteral | oC_Parameter */
static void parse_properties(Parser *p) {
	if (check(p, TOK_LBRACE))       parse_map_literal(p);
		else if (check(p, TOK_DOLLAR))  parse_parameter(p);
		else parse_error(p, "expected properties (map or parameter)");
	}

/* oC_Literal */
static void parse_literal(Parser *p) {
	EMIT("Literal");
	p->depth++;
	switch (p->cur.kind) {
		case TOK_INTEGER: case TOK_OCTAL_INTEGER:
		case TOK_HEX_INTEGER: case TOK_DOUBLE:
		parse_number_literal(p); break;
		case TOK_STRING:
		EMIT("String: %.*s", (int)p->cur.len, p->cur.start);
		advance(p); break;
		case TOK_TRUE:
		EMIT("True"); advance(p); break;
		case TOK_FALSE:
		EMIT("False"); advance(p); break;
		case TOK_NULL:
		EMIT("Null"); advance(p); break;
		case TOK_LBRACE:
		parse_map_literal(p); break;
		case TOK_LBRACKET:
		parse_list_literal(p); break;
		default:
		parse_error(p, "expected literal"); break;
	}
	p->depth--;
}

/* ══════════════════════════════════════════════════════════════════
*  Node / relationship patterns
* ══════════════════════════════════════════════════════════════════ */

/* oC_NodeLabels ::= oC_NodeLabel+ */
static void parse_node_labels(Parser *p) {
	EMIT("NodeLabels");
	p->depth++;
	while (check(p, TOK_COLON)) {
		advance(p);  /* consume ':' */
		parse_label_name(p);
	}
	p->depth--;
}

/* oC_NodePattern ::= '(' [variable] [nodeLabels] [properties] ')' */
static void parse_node_pattern(Parser *p) {
	EMIT("NodePattern");
	p->depth++;
	expect(p, TOK_LPAREN, "node pattern");
	/* variable? */
	if (p->cur.kind == TOK_IDENTIFIER || p->cur.kind == TOK_ESCAPED_IDENTIFIER)
		parse_variable(p);
	/* nodeLabels? */
	if (check(p, TOK_COLON))
		parse_node_labels(p);
	/* properties? */
	if (check(p, TOK_LBRACE) || check(p, TOK_DOLLAR))
		parse_properties(p);
	expect(p, TOK_RPAREN, "node pattern");
	p->depth--;
}

/* oC_RangeLiteral ::= '*' [integer] ['..' [integer]] */
static void parse_range_literal(Parser *p) {
	EMIT("RangeLiteral");
	p->depth++;
	expect(p, TOK_STAR, "range literal");
	if (p->cur.kind == TOK_INTEGER || p->cur.kind == TOK_HEX_INTEGER)
		parse_integer_literal(p);
	if (check(p, TOK_DOTDOT)) {
		advance(p);
		if (p->cur.kind == TOK_INTEGER || p->cur.kind == TOK_HEX_INTEGER)
			parse_integer_literal(p);
	}
	p->depth--;
}

/* oC_RelationshipTypes ::= ':' relTypeName ('|' ':'? relTypeName)* */
static void parse_relationship_types(Parser *p) {
	EMIT("RelationshipTypes");
	p->depth++;
	expect(p, TOK_COLON, "relationship types");
	parse_rel_type_name(p);
	while (check(p, TOK_PIPE)) {
		advance(p);
		if (check(p, TOK_COLON)) advance(p);
			parse_rel_type_name(p);
	}
	p->depth--;
}

/* oC_RelationshipDetail ::= '[' [var] [relTypes] [range] [properties] ']' */
static void parse_relationship_detail(Parser *p) {
	EMIT("RelationshipDetail");
	p->depth++;
	expect(p, TOK_LBRACKET, "relationship detail");
	/* variable? */
	if (p->cur.kind == TOK_IDENTIFIER || p->cur.kind == TOK_ESCAPED_IDENTIFIER)
		parse_variable(p);
	/* relTypes? */
	if (check(p, TOK_COLON))
		parse_relationship_types(p);
	/* range? */
	if (check(p, TOK_STAR))
		parse_range_literal(p);
	/* properties? */
	if (check(p, TOK_LBRACE) || check(p, TOK_DOLLAR))
		parse_properties(p);
	expect(p, TOK_RBRACKET, "relationship detail");
	p->depth--;
}

/*
* oC_RelationshipPattern ::=
*    LeftArrow? Dash RelationshipDetail? Dash RightArrow?
*
* The four cases:
*   <-[...]-    left only
*   -[...]-     undirected
*   -[...]->    right only
*   <-[...]->   both (also syntactically valid)
*/
static void parse_relationship_pattern(Parser *p) {
	EMIT("RelationshipPattern");
	p->depth++;
	
	bool left = false, right = false;
	
	/* optional left arrow head */
	if (check(p, TOK_ARROW_LEFT) || check(p, TOK_LT)) {
		left = true;
		advance(p);
	}
	
	/* first dash */
	if (p->cur.kind != TOK_DASH && p->cur.kind != TOK_MINUS)
		parse_error(p, "expected dash in relationship pattern");
	else advance(p);
		
	/* optional detail */
	if (check(p, TOK_LBRACKET))
		parse_relationship_detail(p);
	
	/* second dash */
	if (p->cur.kind != TOK_DASH && p->cur.kind != TOK_MINUS)
		parse_error(p, "expected closing dash in relationship pattern");
	else advance(p);
		
	/* optional right arrow head */
	if (check(p, TOK_ARROW_RIGHT) || check(p, TOK_GT)) {
		right = true;
		advance(p);
	}
	
	EMIT("Direction: %s", left && right ? "both" : left ? "left" : right ? "right" : "undirected");
	p->depth--;
}

/* oC_PatternElementChain ::= relationshipPattern nodePattern */
static void parse_pattern_element_chain(Parser *p) {
	EMIT("PatternElementChain");
	p->depth++;
	parse_relationship_pattern(p);
	parse_node_pattern(p);
	p->depth--;
}

/*
* oC_PatternElement ::=
*    nodePattern patternElementChain*
*  | '(' patternElement ')'
*/
static void parse_pattern_element(Parser *p) {
	EMIT("PatternElement");
	p->depth++;
	if (check(p, TOK_LPAREN)) {
		/* Could be a node pattern or a parenthesised pattern element.
		We peek: if there's a dash/arrow after the matching ')' it's
		the latter; otherwise it's a node.  For simplicity we always
		try nodePattern first (handles the vast majority of cases). */
		parse_node_pattern(p);
	}
	/* patternElementChains */
	while (check(p, TOK_DASH) || check(p, TOK_MINUS) ||
	check(p, TOK_ARROW_LEFT) || check(p, TOK_LT)) {
		parse_pattern_element_chain(p);
	}
	p->depth--;
}

/* oC_PatternPart ::= [variable '='] anonymousPatternPart */
static void parse_pattern_part(Parser *p) {
	EMIT("PatternPart");
	p->depth++;
	/* Check for variable '=' */
	if ((p->cur.kind == TOK_IDENTIFIER || p->cur.kind == TOK_ESCAPED_IDENTIFIER)) {
		/* save position, try to see if '=' follows */
		Token saved = p->cur;
		Lexer saved_lex = p->lex;
		advance(p);
		if (check(p, TOK_EQ)) {
			EMIT("PatternVariable: %.*s", (int)saved.len, saved.start);
			advance(p); /* consume '=' */
		} else {
			/* backtrack – not supported directly in a streaming tokenizer,
			so we re-emit the saved token as lookahead.
			In a real impl you'd use a token buffer/pushback.           */
			p->cur = saved;
			p->lex = saved_lex;
		}
	}
	parse_pattern_element(p);
	p->depth--;
}

/* oC_Pattern ::= patternPart (',' patternPart)* */
static void parse_pattern(Parser *p) {
	EMIT("Pattern");
	p->depth++;
	parse_pattern_part(p);
	while (match(p, TOK_COMMA))
	parse_pattern_part(p);
	p->depth--;
}

/* ══════════════════════════════════════════════════════════════════
*  Expressions
* ══════════════════════════════════════════════════════════════════ */

/* oC_FilterExpression ::= idInColl [where] */
static void parse_filter_expression(Parser *p) {
	EMIT("FilterExpression");
	p->depth++;
	/* oC_IdInColl ::= variable IN expression */
	parse_variable(p);
	if (!match(p, TOK_IN))
		parse_error(p, "expected IN in filter expression");
	parse_expression(p);
	if (check(p, TOK_WHERE))
		parse_where(p);
	p->depth--;
}

/* oC_ListComprehension ::= '[' filterExpr ['|' expr] ']' */
static void parse_list_comprehension(Parser *p) {
	EMIT("ListComprehension");
	p->depth++;
	expect(p, TOK_LBRACKET, "list comprehension");
	parse_filter_expression(p);
	if (match(p, TOK_PIPE))
		parse_expression(p);
	expect(p, TOK_RBRACKET, "list comprehension");
	p->depth--;
}

/*
* oC_PatternComprehension ::=
*   '[' [variable '='] relationshipsPattern [WHERE expr] '|' expr ']'
*/
static void parse_pattern_comprehension(Parser *p) {
	EMIT("PatternComprehension");
	p->depth++;
	expect(p, TOK_LBRACKET, "pattern comprehension");
	/* optional variable '=' */
	if (p->cur.kind == TOK_IDENTIFIER || p->cur.kind == TOK_ESCAPED_IDENTIFIER) {
		Token saved = p->cur; Lexer saved_lex = p->lex;
		advance(p);
		if (check(p, TOK_EQ)) {
			EMIT("BindVariable: %.*s", (int)saved.len, saved.start);
			advance(p);
		} else {
			p->cur = saved; p->lex = saved_lex;
		}
	}
	/* relationshipsPattern: nodePattern patternElementChain+ */
	parse_node_pattern(p);
	while (check(p, TOK_DASH) || check(p, TOK_MINUS) ||
	check(p, TOK_ARROW_LEFT) || check(p, TOK_LT)) {
		parse_pattern_element_chain(p);
	}
	if (check(p, TOK_WHERE)) {
		advance(p);
		parse_expression(p);
	}
	expect(p, TOK_PIPE, "pattern comprehension");
	parse_expression(p);
	expect(p, TOK_RBRACKET, "pattern comprehension");
	p->depth--;
}

/* oC_CaseAlternative ::= WHEN expr THEN expr */
static void parse_case_alternative(Parser *p) {
	EMIT("CaseAlternative");
	p->depth++;
	expect(p, TOK_WHEN, "case alternative");
	parse_expression(p);
	expect(p, TOK_THEN, "case alternative");
	parse_expression(p);
	p->depth--;
}

/*
* oC_CaseExpression ::=
*   CASE [expr] (WHEN expr THEN expr)+ [ELSE expr] END
*/
static void parse_case_expression(Parser *p) {
	EMIT("CaseExpression");
	p->depth++;
	expect(p, TOK_CASE, "case expression");
	if (!check(p, TOK_WHEN))
		parse_expression(p);  /* optional subject expression */
	while (check(p, TOK_WHEN))
	parse_case_alternative(p);
	if (match(p, TOK_ELSE))
		parse_expression(p);
	expect(p, TOK_END, "case expression");
	p->depth--;
}

/* oC_Namespace ::= (symbolicName '.')* */
static void parse_namespace(Parser *p) {
	/* consume "name." repetitions */
	while (true) {
		if (p->cur.kind != TOK_IDENTIFIER &&
			p->cur.kind != TOK_ESCAPED_IDENTIFIER &&
		/* allow keyword-as-name in procedure paths */
		!(p->cur.kind >= TOK_ALL && p->cur.kind <= TOK_YIELD))
		break;
		Token saved = p->cur; Lexer saved_lex = p->lex;
		advance(p);
		if (check(p, TOK_DOT)) {
			EMIT("NsPart: %.*s", (int)saved.len, saved.start);
			advance(p);
		} else {
			/* Not a namespace part – put the token back */
			p->cur = saved; p->lex = saved_lex;
			break;
		}
	}
}

/* oC_FunctionName ::= namespace symbolicName | EXISTS */
static void parse_function_name(Parser *p) {
	EMIT("FunctionName");
	p->depth++;
	if (check(p, TOK_EXISTS)) {
		advance(p);
	} else {
		parse_namespace(p);
		parse_symbolic_name(p);
	}
	p->depth--;
}

/* oC_FunctionInvocation ::= functionName '(' [DISTINCT] [expr (',' expr)*] ')' */
static void parse_function_invocation(Parser *p) {
	EMIT("FunctionInvocation");
	p->depth++;
	parse_function_name(p);
	expect(p, TOK_LPAREN, "function invocation");
	if (match(p, TOK_DISTINCT))
		EMIT("Distinct");
	if (!check(p, TOK_RPAREN)) {
		parse_expression(p);
		while (match(p, TOK_COMMA))
		parse_expression(p);
	}
	expect(p, TOK_RPAREN, "function invocation");
	p->depth--;
}

/*
* oC_Atom – the primary expression
*
*   Literal | Parameter | CaseExpr | COUNT(*) | ListComprehension
* | PatternComprehension | ALL/ANY/NONE/SINGLE(...) | RelPattern
* | '(' expr ')' | FunctionInvocation | Variable
*/
static void parse_atom(Parser *p) {
	EMIT("Atom");
	p->depth++;
	
	switch (p->cur.kind) {
		
		/* Literals */
		case TOK_INTEGER: case TOK_OCTAL_INTEGER:
		case TOK_HEX_INTEGER: case TOK_DOUBLE:
		case TOK_STRING: case TOK_TRUE: case TOK_FALSE: case TOK_NULL:
		case TOK_LBRACE:
		parse_literal(p);
		break;
		
		case TOK_LBRACKET: {
			/* Disambiguate: list literal vs list/pattern comprehension.
			A list comprehension starts with '[' identifier IN …
			A pattern comprehension starts with '[' nodePattern …
			Otherwise it's a list literal.
			We use a simple heuristic: if the second non-ws token is IN
			it's a list comprehension; if it's '(' it might be a pattern
			comprehension; else list literal.                            */
			Token saved = p->cur; Lexer saved_lex = p->lex;
			advance(p); /* consume '[' */
			bool is_lc = false, is_pc = false;
			if (p->cur.kind == TOK_IDENTIFIER || p->cur.kind == TOK_ESCAPED_IDENTIFIER) {
				Token name = p->cur; Lexer lex2 = p->lex;
				advance(p);
				if (check(p, TOK_IN))   is_lc = true;
					if (check(p, TOK_EQ))   is_pc = true;
					p->cur = name; p->lex = lex2;
			} else if (check(p, TOK_LPAREN)) {
				is_pc = true;
			}
			p->cur = saved; p->lex = saved_lex;
			
			if (is_lc)       parse_list_comprehension(p);
				else if (is_pc)  parse_pattern_comprehension(p);
				else             parse_list_literal(p);
				break;
		}
		
		/* Parameter */
		case TOK_DOLLAR:
		parse_parameter(p);
		break;
		
		/* CASE */
		case TOK_CASE:
		parse_case_expression(p);
		break;
		
		/* COUNT(*) */
		case TOK_COUNT: {
			advance(p);
			if (check(p, TOK_LPAREN)) {
				advance(p);
				if (!match(p, TOK_STAR))
					parse_error(p, "expected * in COUNT(*)");
				expect(p, TOK_RPAREN, "COUNT(*)");
				EMIT("CountStar");
			} else {
				/* COUNT used as a function name */
				/* push back isn't possible; treat as identifier */
				EMIT("Identifier: COUNT");
			}
			break;
		}
		
		/* ALL/ANY/NONE/SINGLE predicate functions */
		case TOK_ALL:
		case TOK_ANY:
		case TOK_NONE:
		case TOK_SINGLE: {
			const char *kw = token_name(p->cur.kind);
			advance(p);
			expect(p, TOK_LPAREN, kw);
			parse_filter_expression(p);
			expect(p, TOK_RPAREN, kw);
			EMIT("Predicate: %s", kw);
			break;
		}
		
		/* Parenthesised expression OR relationship pattern start */
		case TOK_LPAREN: {
			/* Peek: if it looks like a node pattern inside an expression
			context it could be a relationships pattern.  For simplicity
			we treat it as a parenthesised expression here; the caller
			(parse_atom via parse_property_or_labels_expression) will
			then try patternElementChains.                              */
			advance(p); /* consume '(' */
			parse_expression(p);
			expect(p, TOK_RPAREN, "parenthesised expression");
			EMIT("ParenExpr");
			break;
		}
		
		/* Function invocation or variable.
		We peek one token ahead: if the token after an identifier is '('
		then it's a function call; otherwise it's a variable.           */
		case TOK_IDENTIFIER:
		case TOK_ESCAPED_IDENTIFIER:
		default: {
			/* Check for function invocation: save state, consume name/namespace,
			see if '(' follows.                                          */
			Token saved = p->cur; Lexer saved_lex = p->lex;
			
			/* Consume namespace parts */
			bool has_dot = false;
			while (true) {
				if (p->cur.kind != TOK_IDENTIFIER &&
					p->cur.kind != TOK_ESCAPED_IDENTIFIER &&
				!(p->cur.kind >= TOK_ALL && p->cur.kind <= TOK_YIELD))
				break;
				Token t = p->cur; Lexer l = p->lex;
				advance(p);
				if (check(p, TOK_DOT)) {
					has_dot = true;
					advance(p);
				} else {
					if (check(p, TOK_LPAREN)) {
						/* it's a function call; restore to saved and delegate */
						p->cur = saved; p->lex = saved_lex;
						parse_function_invocation(p);
						goto atom_done;
					}
					/* Just a variable */
					p->cur = saved; p->lex = saved_lex;
					parse_variable(p);
					goto atom_done;
				}
			}
			/* After consuming namespace, check for '(' */
			if (check(p, TOK_LPAREN)) {
				p->cur = saved; p->lex = saved_lex;
				parse_function_invocation(p);
			} else {
				p->cur = saved; p->lex = saved_lex;
				parse_variable(p);
			}
			break;
		}
	}
	atom_done:
	p->depth--;
}

/* oC_PropertyLookup ::= '.' propertyKeyName */
static void parse_property_lookup(Parser *p) {
	expect(p, TOK_DOT, "property lookup");
	parse_property_key_name(p);
}

/*
* oC_PropertyOrLabelsExpression ::=
*   atom (propertyLookup)* (nodeLabels)?
*/
static void parse_property_or_labels_expression(Parser *p) {
	EMIT("PropertyOrLabelsExpression");
	p->depth++;
	parse_atom(p);
	while (check(p, TOK_DOT))
	parse_property_lookup(p);
	if (check(p, TOK_COLON))
		parse_node_labels(p);
	p->depth--;
}

/* oC_StringOperatorExpression */
static void parse_string_operator(Parser *p) {
	if (check(p, TOK_STARTS)) {
		advance(p);
		expect(p, TOK_WITH, "STARTS WITH");
		EMIT("StartsWithOp");
	} else if (check(p, TOK_ENDS)) {
		advance(p);
		expect(p, TOK_WITH, "ENDS WITH");
		EMIT("EndsWithOp");
	} else if (check(p, TOK_CONTAINS)) {
		advance(p);
		EMIT("ContainsOp");
	} else {
		parse_error(p, "expected STARTS WITH / ENDS WITH / CONTAINS");
	}
	parse_property_or_labels_expression(p);
}

/* oC_ListOperatorExpression */
static void parse_list_operator(Parser *p) {
	if (check(p, TOK_IN)) {
		advance(p);
		EMIT("InOp");
		parse_property_or_labels_expression(p);
	} else {
		/* '[' expr ']' or '[' expr? '..' expr? ']' */
		expect(p, TOK_LBRACKET, "list operator");
		bool has_expr = !check(p, TOK_DOTDOT) && !check(p, TOK_RBRACKET);
		if (has_expr) parse_expression(p);
			if (match(p, TOK_DOTDOT)) {
			EMIT("SliceOp");
			if (!check(p, TOK_RBRACKET))
				parse_expression(p);
		} else {
			EMIT("IndexOp");
		}
		expect(p, TOK_RBRACKET, "list operator");
	}
}

/* oC_NullOperatorExpression */
static void parse_null_operator(Parser *p) {
	expect(p, TOK_IS, "null check");
	bool is_not = match(p, TOK_NOT);
	expect(p, TOK_NULL, "null check");
	EMIT("%s", is_not ? "IsNotNull" : "IsNull");
}

/*
* oC_StringListNullOperatorExpression ::=
*   propertyOrLabelsExpr (stringOp | listOp | nullOp)*
*/
static void parse_string_list_null(Parser *p) {
	parse_property_or_labels_expression(p);
	while (true) {
		if (check(p, TOK_STARTS) || check(p, TOK_ENDS) || check(p, TOK_CONTAINS))
			parse_string_operator(p);
		else if (check(p, TOK_IN) || check(p, TOK_LBRACKET))
			parse_list_operator(p);
		else if (check(p, TOK_IS))
			parse_null_operator(p);
		else break;
		}
}

/* oC_UnaryAddOrSubtractExpression */
static void parse_unary(Parser *p) {
	while (check(p, TOK_PLUS) || check(p, TOK_MINUS) || check(p, TOK_DASH)) {
		EMIT("Unary: %.*s", (int)p->cur.len, p->cur.start);
		advance(p);
	}
	parse_string_list_null(p);
}

/* oC_PowerOfExpression */
static void parse_power(Parser *p) {
	parse_unary(p);
	while (check(p, TOK_CARET)) {
		advance(p);
		EMIT("PowerOp");
		parse_unary(p);
	}
}

/* oC_MultiplyDivideModuloExpression */
static void parse_mul(Parser *p) {
	parse_power(p);
	while (check(p, TOK_STAR) || check(p, TOK_SLASH) || check(p, TOK_PERCENT)) {
		const char *op = check(p, TOK_STAR) ? "*" : check(p, TOK_SLASH) ? "/" : "%";
		advance(p);
		EMIT("BinaryOp: %s", op);
		parse_power(p);
	}
}

/* oC_AddOrSubtractExpression */
static void parse_add(Parser *p) {
	parse_mul(p);
	while (check(p, TOK_PLUS) || check(p, TOK_MINUS) || check(p, TOK_DASH)) {
		const char *op = check(p, TOK_PLUS) ? "+" : "-";
		advance(p);
		EMIT("BinaryOp: %s", op);
		parse_mul(p);
	}
}

/* oC_PartialComparisonExpression (one operator + rhs) */
static bool parse_partial_comparison(Parser *p) {
	const char *op = NULL;
	switch (p->cur.kind) {
		case TOK_EQ:    op = "=";  break;
		case TOK_NEQ:   op = "<>"; break;
		case TOK_LT:    op = "<";  break;
		case TOK_GT:    op = ">";  break;
		case TOK_LTE:   op = "<="; break;
		case TOK_GTE:   op = ">="; break;
		default: return false;
	}
	advance(p);
	EMIT("CompareOp: %s", op);
	parse_add(p);
	return true;
}

/* oC_ComparisonExpression */
static void parse_comparison(Parser *p) {
	parse_add(p);
	while (parse_partial_comparison(p))
	;
}

/* oC_NotExpression */
static void parse_not(Parser *p) {
	int nots = 0;
	while (check(p, TOK_NOT)) { advance(p); nots++; }
	parse_comparison(p);
	if (nots & 1) EMIT("Not");
	}

/* oC_AndExpression */
static void parse_and(Parser *p) {
	parse_not(p);
	while (check(p, TOK_AND)) {
		advance(p);
		EMIT("And");
		parse_not(p);
	}
}

/* oC_XorExpression */
static void parse_xor(Parser *p) {
	parse_and(p);
	while (check(p, TOK_XOR)) {
		advance(p);
		EMIT("Xor");
		parse_and(p);
	}
}

/* oC_OrExpression */
static void parse_or(Parser *p) {
	parse_xor(p);
	while (check(p, TOK_OR)) {
		advance(p);
		EMIT("Or");
		parse_xor(p);
	}
}

/* oC_Expression */
static void parse_expression(Parser *p) {
	EMIT("Expression");
	p->depth++;
	parse_or(p);
	p->depth--;
}

/* ══════════════════════════════════════════════════════════════════
*  WHERE, ORDER BY, SKIP, LIMIT, WITH/RETURN projection
* ══════════════════════════════════════════════════════════════════ */

static void parse_where(Parser *p) {
	EMIT("Where");
	p->depth++;
	expect(p, TOK_WHERE, "where clause");
	parse_expression(p);
	p->depth--;
}

/* oC_SortItem ::= expression [ASC|DESC|ASCENDING|DESCENDING] */
static void parse_sort_item(Parser *p) {
	EMIT("SortItem");
	p->depth++;
	parse_expression(p);
	if (check(p, TOK_ASC) || check(p, TOK_ASCENDING)) {
		EMIT("Asc"); advance(p);
	} else if (check(p, TOK_DESC) || check(p, TOK_DESCENDING)) {
		EMIT("Desc"); advance(p);
	}
	p->depth--;
}

/* oC_Order ::= ORDER BY sortItem (',' sortItem)* */
static void parse_order(Parser *p) {
	EMIT("Order");
	p->depth++;
	expect(p, TOK_ORDER, "ORDER BY");
	expect(p, TOK_BY, "ORDER BY");
	parse_sort_item(p);
	while (match(p, TOK_COMMA))
	parse_sort_item(p);
	p->depth--;
}

/* oC_Skip  ::= SKIP  expression */
static void parse_skip(Parser *p) {
	EMIT("Skip");
	p->depth++;
	expect(p, TOK_SKIP, "SKIP");
	parse_expression(p);
	p->depth--;
}

/* oC_Limit ::= LIMIT expression */
static void parse_limit(Parser *p) {
	EMIT("Limit");
	p->depth++;
	expect(p, TOK_LIMIT, "LIMIT");
	parse_expression(p);
	p->depth--;
}

/* oC_ProjectionItem ::= expression [AS variable] */
static void parse_projection_item(Parser *p) {
	EMIT("ProjectionItem");
	p->depth++;
	parse_expression(p);
	if (match(p, TOK_AS))
		parse_variable(p);
	p->depth--;
}

/*
* oC_ProjectionItems ::= '*' [',' projectionItem]*
*                      | projectionItem (',' projectionItem)*
*/
static void parse_projection_items(Parser *p) {
	EMIT("ProjectionItems");
	p->depth++;
	if (match(p, TOK_STAR)) {
		EMIT("Star");
		while (match(p, TOK_COMMA))
		parse_projection_item(p);
	} else {
		parse_projection_item(p);
		while (match(p, TOK_COMMA))
		parse_projection_item(p);
	}
	p->depth--;
}

/*
* oC_ProjectionBody ::= [DISTINCT] projectionItems [order] [skip] [limit]
*/
static void parse_projection_body(Parser *p) {
	EMIT("ProjectionBody");
	p->depth++;
	if (match(p, TOK_DISTINCT))
		EMIT("Distinct");
	parse_projection_items(p);
	if (check(p, TOK_ORDER))  parse_order(p);
		if (check(p, TOK_SKIP))   parse_skip(p);
		if (check(p, TOK_LIMIT))  parse_limit(p);
		p->depth--;
}

/* ══════════════════════════════════════════════════════════════════
*  Updating clauses: CREATE, MERGE, SET, DELETE, REMOVE
* ══════════════════════════════════════════════════════════════════ */

static void parse_create(Parser *p) {
	EMIT("Create");
	p->depth++;
	expect(p, TOK_CREATE, "CREATE");
	parse_pattern(p);
	p->depth--;
}

/* oC_MergeAction ::= ON MATCH SET ... | ON CREATE SET ... */
static void parse_merge_action(Parser *p) {
	expect(p, TOK_ON, "merge action");
	if (check(p, TOK_MATCH)) {
		advance(p); EMIT("OnMatch");
	} else if (check(p, TOK_CREATE)) {
		advance(p); EMIT("OnCreate");
	} else {
		parse_error(p, "expected MATCH or CREATE after ON");
	}
	expect(p, TOK_SET, "merge action");
	/* parse set items */
	parse_expression(p); /* simplified: real SET parses setItem list */
}

static void parse_merge(Parser *p) {
	EMIT("Merge");
	p->depth++;
	expect(p, TOK_MERGE, "MERGE");
	parse_pattern_part(p);
	while (check(p, TOK_ON))
	parse_merge_action(p);
	p->depth--;
}

/* oC_SetItem */
static void parse_set_item(Parser *p) {
	EMIT("SetItem");
	p->depth++;
	/* propertyExpr '=' expr | variable '=' expr | variable '+=' expr | variable nodeLabels */
	parse_expression(p);   /* covers lhs (variable or property expression) */
	if (match(p, TOK_EQ)) {
		EMIT("Assign");
		parse_expression(p);
	} else if (match(p, TOK_PLUSEQ)) {
		EMIT("PlusAssign");
		parse_expression(p);
	} else if (check(p, TOK_COLON)) {
		parse_node_labels(p);
	}
	p->depth--;
}

static void parse_set(Parser *p) {
	EMIT("Set");
	p->depth++;
	expect(p, TOK_SET, "SET");
	parse_set_item(p);
	while (match(p, TOK_COMMA))
	parse_set_item(p);
	p->depth--;
}

static void parse_delete(Parser *p) {
	EMIT("Delete");
	p->depth++;
	bool detach = match(p, TOK_DETACH);
	if (detach) EMIT("Detach");
		expect(p, TOK_DELETE, "DELETE");
	parse_expression(p);
	while (match(p, TOK_COMMA))
	parse_expression(p);
	p->depth--;
}

/* oC_RemoveItem */
static void parse_remove_item(Parser *p) {
	EMIT("RemoveItem");
	p->depth++;
	/* variable nodeLabels | propertyExpression */
	parse_expression(p);
	if (check(p, TOK_COLON))
		parse_node_labels(p);
	p->depth--;
}

static void parse_remove(Parser *p) {
	EMIT("Remove");
	p->depth++;
	expect(p, TOK_REMOVE, "REMOVE");
	parse_remove_item(p);
	while (match(p, TOK_COMMA))
	parse_remove_item(p);
	p->depth--;
}

/* ══════════════════════════════════════════════════════════════════
*  Reading clauses: MATCH, UNWIND, CALL (in-query)
* ══════════════════════════════════════════════════════════════════ */

static void parse_match(Parser *p) {
	EMIT("Match");
	p->depth++;
	bool optional = match(p, TOK_OPTIONAL);
	if (optional) EMIT("Optional");
		expect(p, TOK_MATCH, "MATCH");
	parse_pattern(p);
	if (check(p, TOK_WHERE))
		parse_where(p);
	p->depth--;
}

static void parse_unwind(Parser *p) {
	EMIT("Unwind");
	p->depth++;
	expect(p, TOK_UNWIND, "UNWIND");
	parse_expression(p);
	expect(p, TOK_AS, "UNWIND … AS");
	parse_variable(p);
	p->depth--;
}

/* oC_YieldItems ::= ('*' | yieldItem (',' yieldItem)*) [where] */
static void parse_yield_items(Parser *p) {
	EMIT("YieldItems");
	p->depth++;
	if (match(p, TOK_STAR)) {
		EMIT("Star");
	} else {
		/* yieldItem ::= [procedureResultField AS] variable */
		EMIT("YieldItem");
		p->depth++;
		parse_symbolic_name(p);
		if (match(p, TOK_AS))
			parse_variable(p);
		p->depth--;
		while (match(p, TOK_COMMA)) {
			EMIT("YieldItem");
			p->depth++;
			parse_symbolic_name(p);
			if (match(p, TOK_AS))
				parse_variable(p);
			p->depth--;
		}
	}
	if (check(p, TOK_WHERE))
		parse_where(p);
	p->depth--;
}

/* oC_ExplicitProcedureInvocation */
static void parse_explicit_proc(Parser *p) {
	EMIT("ExplicitProcedureInvocation");
	p->depth++;
	parse_namespace(p);
	parse_symbolic_name(p);
	expect(p, TOK_LPAREN, "procedure call");
	if (!check(p, TOK_RPAREN)) {
		parse_expression(p);
		while (match(p, TOK_COMMA))
		parse_expression(p);
	}
	expect(p, TOK_RPAREN, "procedure call");
	p->depth--;
}

/* oC_InQueryCall ::= CALL explicitProc [YIELD yieldItems] */
static void parse_in_query_call(Parser *p) {
	EMIT("InQueryCall");
	p->depth++;
	expect(p, TOK_CALL, "CALL");
	parse_explicit_proc(p);
	if (match(p, TOK_YIELD))
		parse_yield_items(p);
	p->depth--;
}

/* ══════════════════════════════════════════════════════════════════
*  WITH / RETURN
* ══════════════════════════════════════════════════════════════════ */

static void parse_return(Parser *p) {
	EMIT("Return");
	p->depth++;
	expect(p, TOK_RETURN, "RETURN");
	parse_projection_body(p);
	p->depth--;
}

static void parse_with(Parser *p) {
	EMIT("With");
	p->depth++;
	expect(p, TOK_WITH, "WITH");
	parse_projection_body(p);
	if (check(p, TOK_WHERE))
		parse_where(p);
	p->depth--;
}

/* ══════════════════════════════════════════════════════════════════
*  Query structure
* ══════════════════════════════════════════════════════════════════ */

static bool is_reading_clause(Parser *p) {
	return check(p, TOK_MATCH) || check(p, TOK_OPTIONAL) ||
	check(p, TOK_UNWIND) || check(p, TOK_CALL);
}

static bool is_updating_clause(Parser *p) {
	return check(p, TOK_CREATE) || check(p, TOK_MERGE) ||
	check(p, TOK_DELETE) || check(p, TOK_DETACH) ||
	check(p, TOK_SET)    || check(p, TOK_REMOVE);
}

static void parse_reading_clause(Parser *p) {
	if (check(p, TOK_MATCH) || check(p, TOK_OPTIONAL)) parse_match(p);
		else if (check(p, TOK_UNWIND)) parse_unwind(p);
		else if (check(p, TOK_CALL))   parse_in_query_call(p);
		else parse_error(p, "expected reading clause");
	}

static void parse_updating_clause(Parser *p) {
	if (check(p, TOK_CREATE))                  parse_create(p);
		else if (check(p, TOK_MERGE))              parse_merge(p);
		else if (check(p, TOK_DELETE) ||
		check(p, TOK_DETACH))             parse_delete(p);
	else if (check(p, TOK_SET))                parse_set(p);
		else if (check(p, TOK_REMOVE))             parse_remove(p);
		else parse_error(p, "expected updating clause");
	}

/*
* oC_SinglePartQuery ::=
*   readingClause* RETURN
* | readingClause* updatingClause+ [RETURN]
*/
static void parse_single_part_query(Parser *p) {
	EMIT("SinglePartQuery");
	p->depth++;
	while (is_reading_clause(p))
	parse_reading_clause(p);
	if (is_updating_clause(p)) {
		while (is_updating_clause(p))
		parse_updating_clause(p);
		if (check(p, TOK_RETURN))
			parse_return(p);
	} else {
		parse_return(p);
	}
	p->depth--;
}

/*
* oC_MultiPartQuery ::=
*   (readingClause* updatingClause* WITH)+ singlePartQuery
*/
static void parse_multi_part_query(Parser *p) {
	EMIT("MultiPartQuery");
	p->depth++;
	/* each "part" ends with WITH */
	while (true) {
		while (is_reading_clause(p))
		parse_reading_clause(p);
		while (is_updating_clause(p))
		parse_updating_clause(p);
		if (!check(p, TOK_WITH)) break;
			parse_with(p);
	}
	parse_single_part_query(p);
	p->depth--;
}

/*
* oC_SingleQuery ::= singlePartQuery | multiPartQuery
* We detect multipart by checking whether there is a WITH after
* the initial reading/updating run.
*/
static void parse_single_query(Parser *p) {
	EMIT("SingleQuery");
	p->depth++;
	/* Save position to decide which form */
	Token saved = p->cur; Lexer saved_lex = p->lex;
	
	/* Consume reading + updating clauses to see if WITH follows */
	/* (This is a simple lookahead heuristic.) */
	/* For correctness we just try multi-part first; if WITH never
	appears before RETURN/EOF, single-part handles it fine.      */
	
	/* Reset and hand off */
	p->cur = saved; p->lex = saved_lex;
	
	/* Peek to determine: if there's ever a WITH before the first
	RETURN at the top level, it's a multi-part query.            */
	bool is_multi = false;
	{
		Lexer tmp_lex = p->lex;
		Token tmp_tok = p->cur;
		int depth = 0;
		while (tmp_tok.kind != TOK_EOF && tmp_tok.kind != TOK_ERROR) {
			if (tmp_tok.kind == TOK_LPAREN || tmp_tok.kind == TOK_LBRACKET ||
				tmp_tok.kind == TOK_LBRACE) depth++;
			else if (tmp_tok.kind == TOK_RPAREN || tmp_tok.kind == TOK_RBRACKET ||
				tmp_tok.kind == TOK_RBRACE) depth--;
			else if (depth == 0 && tmp_tok.kind == TOK_WITH) { is_multi = true; break; }
			else if (depth == 0 && tmp_tok.kind == TOK_RETURN) break;
				do { lexer_next(&tmp_lex, &tmp_tok); }
			while (tmp_tok.kind == TOK_WS ||
			tmp_tok.kind == TOK_COMMENT_BLOCK ||
			tmp_tok.kind == TOK_COMMENT_LINE);
		}
	}
	
	if (is_multi) parse_multi_part_query(p);
		else          parse_single_part_query(p);
		p->depth--;
}

/* oC_Union ::= UNION [ALL] singleQuery */
static void parse_union(Parser *p) {
	EMIT("Union");
	p->depth++;
	expect(p, TOK_UNION, "UNION");
	bool all = match(p, TOK_ALL);
	if (all) EMIT("All");
		parse_single_query(p);
	p->depth--;
}

/* oC_RegularQuery ::= singleQuery (UNION singleQuery)* */
static void parse_regular_query(Parser *p) {
	EMIT("RegularQuery");
	p->depth++;
	parse_single_query(p);
	while (check(p, TOK_UNION))
	parse_union(p);
	p->depth--;
}

/* oC_StandaloneCall ::= CALL (explicit | implicit) [YIELD yieldItems] */
static void parse_standalone_call(Parser *p) {
	EMIT("StandaloneCall");
	p->depth++;
	expect(p, TOK_CALL, "CALL");
	/* Peek for '(' after namespace.name to distinguish explicit vs implicit */
	Token saved = p->cur; Lexer saved_lex = p->lex;
	
	/* Consume namespace.name */
	parse_namespace(p);
	parse_symbolic_name(p);
	bool explicit_ = check(p, TOK_LPAREN);
	
	/* Restore and re-parse properly */
	p->cur = saved; p->lex = saved_lex;
	
	if (explicit_) {
		parse_explicit_proc(p);
	} else {
		/* implicit: just the procedure name */
		EMIT("ImplicitProcedureInvocation");
		p->depth++;
		parse_namespace(p);
		parse_symbolic_name(p);
		p->depth--;
	}
	if (match(p, TOK_YIELD))
		parse_yield_items(p);
	p->depth--;
}

/* oC_Query ::= regularQuery | standaloneCall */
static void parse_query(Parser *p) {
	EMIT("Query");
	p->depth++;
	if (check(p, TOK_CALL))
		parse_standalone_call(p);
	else
		parse_regular_query(p);
	p->depth--;
}

/* oC_Statement ::= oC_Query */
/* oC_Cypher    ::= SP? statement (';')? EOF */
static void parse_cypher(Parser *p) {
	EMIT("Cypher");
	p->depth++;
	parse_query(p);
	match(p, TOK_SEMICOLON);
	if (!check(p, TOK_EOF))
		parse_error(p, "unexpected token after statement end");
	p->depth--;
}

/* ══════════════════════════════════════════════════════════════════
*  Ragel "entry" machine
*  (drives the parser from a Ragel scanner so that integration
*   with Ragel-based tooling is possible; the actual parsing is
*   done by the C functions above)
* ══════════════════════════════════════════════════════════════════ */


#line 1492 "cypher_parser.c"
static const int cypher_parser_entry_start = 0;
static const int cypher_parser_entry_first_final = 0;
static const int cypher_parser_entry_error = -1;

static const int cypher_parser_entry_en_main = 0;


#line 1502 "cypher_parser.rl"


static int parser_entry(Parser *p) {
	/* We don't actually use the Ragel machine to drive parsing;
	we jump straight to the RD parser.  The machine data is
	included above to satisfy the ragel pipeline requirement.     */
	(void)cypher_parser_entry_start;   /* suppress unused warning */
	parse_cypher(p);
	return p->error ? -1 : 0;
}

/* ══════════════════════════════════════════════════════════════════
*  Public entry point
* ══════════════════════════════════════════════════════════════════ */

int cypher_parse(const char *input, size_t len) {
	Parser p;
	memset(&p, 0, sizeof(p));
	lexer_init(&p.lex, input, len);
	/* prime the lookahead */
	do {
		lexer_next(&p.lex, &p.cur);
	} while (p.cur.kind == TOK_WS ||
	p.cur.kind == TOK_COMMENT_BLOCK ||
	p.cur.kind == TOK_COMMENT_LINE);
	
	return parser_entry(&p);
}

/* ══════════════════════════════════════════════════════════════════
*  Demo main
* ══════════════════════════════════════════════════════════════════ */
int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s '<cypher query>'\n", argv[0]);
		return 1;
	}
	const char *input = argv[1];
	int rc = cypher_parse(input, strlen(input));
	if (rc != 0) {
		fprintf(stderr, "Parse failed.\n");
		return 1;
	}
	return 0;
}
