#line 1 "cypher_lexer.rl"
/*
* cypher_lexer.rl — Ragel-based Cypher tokenizer
* Tokenizes openCypher syntax for the query REPL.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "cypher_parser.h"


#line 12 "cypher_lexer.c"
static const int cypher_lexer_start = 10;
static const int cypher_lexer_first_final = 10;
static const int cypher_lexer_error = 0;

static const int cypher_lexer_en_main = 10;


#line 14 "cypher_lexer.rl"


static int dispatch_keyword(cypher_token_t *t) {
	char upper[MAX_STR];
	int i;
	for (i = 0; t->str[i]; i++)
	upper[i] = (t->str[i] >= 'a' && t->str[i] <= 'z') ? t->str[i] - 32 : t->str[i];
	upper[i] = '\0';
	
	if (!strcmp(upper, "MATCH"))      return TOK_MATCH;
		if (!strcmp(upper, "RETURN"))     return TOK_RETURN;
		if (!strcmp(upper, "WHERE"))      return TOK_WHERE;
		if (!strcmp(upper, "CREATE"))     return TOK_CREATE;
		if (!strcmp(upper, "DELETE"))     return TOK_DELETE;
		if (!strcmp(upper, "SET"))        return TOK_SET;
		if (!strcmp(upper, "MERGE"))      return TOK_MERGE;
		if (!strcmp(upper, "ORDER"))      return TOK_ORDER;
		if (!strcmp(upper, "BY"))         return TOK_BY;
		if (!strcmp(upper, "LIMIT"))      return TOK_LIMIT;
		if (!strcmp(upper, "SKIP"))       return TOK_SKIP;
		if (!strcmp(upper, "AND"))        return TOK_AND;
		if (!strcmp(upper, "OR"))         return TOK_OR;
		if (!strcmp(upper, "NOT"))        return TOK_NOT;
		if (!strcmp(upper, "XOR"))        return TOK_XOR;
		if (!strcmp(upper, "IN"))         return TOK_IN;
		if (!strcmp(upper, "IS"))         return TOK_IS;
		if (!strcmp(upper, "AS"))         return TOK_AS;
		if (!strcmp(upper, "DISTINCT"))   return TOK_DISTINCT;
		if (!strcmp(upper, "OPTIONAL"))   return TOK_OPTIONAL;
		if (!strcmp(upper, "DETACH"))     return TOK_DETACH;
		if (!strcmp(upper, "CONTAINS"))   return TOK_CONTAINS;
		if (!strcmp(upper, "STARTS"))     return TOK_STARTS;
		if (!strcmp(upper, "ENDS"))       return TOK_ENDS;
		if (!strcmp(upper, "WITH"))       return TOK_WITH;
		if (!strcmp(upper, "ON"))         return TOK_ON;
		if (!strcmp(upper, "CASE"))       return TOK_CASE;
		if (!strcmp(upper, "WHEN"))       return TOK_WHEN;
		if (!strcmp(upper, "THEN"))       return TOK_THEN;
		if (!strcmp(upper, "ELSE"))       return TOK_ELSE;
		if (!strcmp(upper, "END"))        return TOK_END;
		if (!strcmp(upper, "DESC"))       return TOK_DESC;
		if (!strcmp(upper, "ASC"))        return TOK_ASC;
		if (!strcmp(upper, "COUNT"))      return TOK_COUNT;
		if (!strcmp(upper, "EXISTS"))     return TOK_EXISTS;
		if (!strcmp(upper, "REMOVE"))     return TOK_REMOVE;
		if (!strcmp(upper, "TRUE"))       { t->ival = 1; return TOK_BOOL; }
	if (!strcmp(upper, "FALSE"))      { t->ival = 0; return TOK_BOOL; }
	if (!strcmp(upper, "NULL"))       return TOK_NULL;
		return TOK_IDENT;
}

int cypher_lex(const char *in, int len, cypher_token_t *tokens, int max_tokens) {
	const char *p = in, *pe = in + len, *eof = pe;
	const char *ts, *te;
	int act, cs;
	int tok_count = 0;
	
	
#line 346 "cypher_lexer.rl"

	

#line 79 "cypher_lexer.c"
	{
		cs = (int)cypher_lexer_start;
		ts = 0;
		te = 0;
	}
	
#line 348 "cypher_lexer.rl"


#line 86 "cypher_lexer.c"
{
		switch ( cs ) {
			case 10:
			goto st_case_10;
			case 0:
			goto st_case_0;
			case 11:
			goto st_case_11;
			case 12:
			goto st_case_12;
			case 1:
			goto st_case_1;
			case 2:
			goto st_case_2;
			case 3:
			goto st_case_3;
			case 4:
			goto st_case_4;
			case 13:
			goto st_case_13;
			case 14:
			goto st_case_14;
			case 15:
			goto st_case_15;
			case 5:
			goto st_case_5;
			case 6:
			goto st_case_6;
			case 16:
			goto st_case_16;
			case 17:
			goto st_case_17;
			case 7:
			goto st_case_7;
			case 18:
			goto st_case_18;
			case 19:
			goto st_case_19;
			case 8:
			goto st_case_8;
			case 20:
			goto st_case_20;
			case 21:
			goto st_case_21;
			case 22:
			goto st_case_22;
			case 23:
			goto st_case_23;
			case 9:
			goto st_case_9;
		}
		_ctr2:
			{
#line 286 "cypher_lexer.rl"
			{te = p+1;{
#line 286 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_STRING;
						int l = (int)(te - ts - 2);
						if (l >= MAX_STR) l = MAX_STR - 1;
						memcpy(tokens[tok_count].str, ts + 1, l);
						tokens[tok_count].str[l] = '\0';
						tok_count++;
					}
				}
			}}
		
#line 154 "cypher_lexer.c"

		goto _st10;
		_ctr5:
			{
#line 297 "cypher_lexer.rl"
			{te = p+1;{
#line 297 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_STRING;
						int l = (int)(te - ts - 2);
						if (l >= MAX_STR) l = MAX_STR - 1;
						memcpy(tokens[tok_count].str, ts + 1, l);
						tokens[tok_count].str[l] = '\0';
						tok_count++;
					}
				}
			}}
		
#line 173 "cypher_lexer.c"

		goto _st10;
		_ctr7:
			{
#line 240 "cypher_lexer.rl"
			{p = ((te))-1;
				{
#line 240 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_SLASH;
						tok_count++;
					}
				}
			}}
		
#line 189 "cypher_lexer.c"

		goto _st10;
		_ctr10:
			{
#line 344 "cypher_lexer.rl"
			{te = p+1;{
#line 344 "cypher_lexer.rl"
					/* skip block comment */ }
			}}
		
#line 199 "cypher_lexer.c"

		goto _st10;
		_ctr11:
			{
#line 324 "cypher_lexer.rl"
			{p = ((te))-1;
				{
#line 324 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_INTEGER;
						tokens[tok_count].ival = (int)strtol(ts, NULL, 10);
						tok_count++;
					}
				}
			}}
		
#line 216 "cypher_lexer.c"

		goto _st10;
		_ctr15:
			{
#line 275 "cypher_lexer.rl"
			{te = p+1;{
#line 275 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_IDENT;
						int l = (int)(te - ts - 2);
						if (l >= MAX_STR) l = MAX_STR - 1;
						memcpy(tokens[tok_count].str, ts + 1, l);
						tokens[tok_count].str[l] = '\0';
						tok_count++;
					}
				}
			}}
		
#line 235 "cypher_lexer.c"

		goto _st10;
		_ctr18:
			{
#line 82 "cypher_lexer.rl"
			{te = p+1;{
#line 82 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_SEMI;
						tok_count++;
					}
				}
			}}
		
#line 250 "cypher_lexer.c"

		goto _st10;
		_ctr20:
			{
#line 261 "cypher_lexer.rl"
			{te = p+1;{
#line 261 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_DOLLAR;
						tok_count++;
					}
				}
			}}
		
#line 265 "cypher_lexer.c"

		goto _st10;
		_ctr21:
			{
#line 247 "cypher_lexer.rl"
			{te = p+1;{
#line 247 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_PCT;
						tok_count++;
					}
				}
			}}
		
#line 280 "cypher_lexer.c"

		goto _st10;
		_ctr22:
			{
#line 117 "cypher_lexer.rl"
			{te = p+1;{
#line 117 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_LPAREN;
						tok_count++;
					}
				}
			}}
		
#line 295 "cypher_lexer.c"

		goto _st10;
		_ctr23:
			{
#line 124 "cypher_lexer.rl"
			{te = p+1;{
#line 124 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_RPAREN;
						tok_count++;
					}
				}
			}}
		
#line 310 "cypher_lexer.c"

		goto _st10;
		_ctr24:
			{
#line 233 "cypher_lexer.rl"
			{te = p+1;{
#line 233 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_STAR;
						tok_count++;
					}
				}
			}}
		
#line 325 "cypher_lexer.c"

		goto _st10;
		_ctr25:
			{
#line 201 "cypher_lexer.rl"
			{te = p+1;{
#line 201 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_PLUS;
						tok_count++;
					}
				}
			}}
		
#line 340 "cypher_lexer.c"

		goto _st10;
		_ctr26:
			{
#line 96 "cypher_lexer.rl"
			{te = p+1;{
#line 96 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_COMMA;
						tok_count++;
					}
				}
			}}
		
#line 355 "cypher_lexer.c"

		goto _st10;
		_ctr28:
			{
#line 103 "cypher_lexer.rl"
			{te = p+1;{
#line 103 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_DOT;
						tok_count++;
					}
				}
			}}
		
#line 370 "cypher_lexer.c"

		goto _st10;
		_ctr32:
			{
#line 110 "cypher_lexer.rl"
			{te = p+1;{
#line 110 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_COLON;
						tok_count++;
					}
				}
			}}
		
#line 385 "cypher_lexer.c"

		goto _st10;
		_ctr33:
			{
#line 89 "cypher_lexer.rl"
			{te = p+1;{
#line 89 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_SEMI;
						tok_count++;
					}
				}
			}}
		
#line 400 "cypher_lexer.c"

		goto _st10;
		_ctr35:
			{
#line 159 "cypher_lexer.rl"
			{te = p+1;{
#line 159 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_EQ;
						tok_count++;
					}
				}
			}}
		
#line 415 "cypher_lexer.c"

		goto _st10;
		_ctr38:
			{
#line 131 "cypher_lexer.rl"
			{te = p+1;{
#line 131 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_LBRACKET;
						tok_count++;
					}
				}
			}}
		
#line 430 "cypher_lexer.c"

		goto _st10;
		_ctr39:
			{
#line 138 "cypher_lexer.rl"
			{te = p+1;{
#line 138 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_RBRACKET;
						tok_count++;
					}
				}
			}}
		
#line 445 "cypher_lexer.c"

		goto _st10;
		_ctr40:
			{
#line 254 "cypher_lexer.rl"
			{te = p+1;{
#line 254 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_CARET;
						tok_count++;
					}
				}
			}}
		
#line 460 "cypher_lexer.c"

		goto _st10;
		_ctr41:
			{
#line 145 "cypher_lexer.rl"
			{te = p+1;{
#line 145 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_LBRACE;
						tok_count++;
					}
				}
			}}
		
#line 475 "cypher_lexer.c"

		goto _st10;
		_ctr42:
			{
#line 268 "cypher_lexer.rl"
			{te = p+1;{
#line 268 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_PIPE;
						tok_count++;
					}
				}
			}}
		
#line 490 "cypher_lexer.c"

		goto _st10;
		_ctr43:
			{
#line 152 "cypher_lexer.rl"
			{te = p+1;{
#line 152 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_RBRACE;
						tok_count++;
					}
				}
			}}
		
#line 505 "cypher_lexer.c"

		goto _st10;
		_ctr44:
			{
#line 80 "cypher_lexer.rl"
			{te = p;p = p - 1;{
#line 80 "cypher_lexer.rl"
					/* skip */ }
			}}
		
#line 515 "cypher_lexer.c"

		goto _st10;
		_ctr45:
			{
#line 82 "cypher_lexer.rl"
			{te = p;p = p - 1;{
#line 82 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_SEMI;
						tok_count++;
					}
				}
			}}
		
#line 530 "cypher_lexer.c"

		goto _st10;
		_ctr46:
			{
#line 219 "cypher_lexer.rl"
			{te = p;p = p - 1;{
#line 219 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_MINUS;
						tok_count++;
					}
				}
			}}
		
#line 545 "cypher_lexer.c"

		goto _st10;
		_ctr48:
			{
#line 226 "cypher_lexer.rl"
			{te = p;p = p - 1;{
#line 226 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_DASH;
						tok_count++;
					}
				}
			}}
		
#line 560 "cypher_lexer.c"

		goto _st10;
		_ctr49:
			{
#line 208 "cypher_lexer.rl"
			{te = p+1;{
#line 208 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_MINUS;
						tok_count++;
					}
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_DASH;
						tok_count++;
					}
				}
			}}
		
#line 579 "cypher_lexer.c"

		goto _st10;
		_ctr50:
			{
#line 240 "cypher_lexer.rl"
			{te = p;p = p - 1;{
#line 240 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_SLASH;
						tok_count++;
					}
				}
			}}
		
#line 594 "cypher_lexer.c"

		goto _st10;
		_ctr52:
			{
#line 343 "cypher_lexer.rl"
			{te = p;p = p - 1;{
#line 343 "cypher_lexer.rl"
					/* skip line comment */ }
			}}
		
#line 604 "cypher_lexer.c"

		goto _st10;
		_ctr53:
			{
#line 324 "cypher_lexer.rl"
			{te = p;p = p - 1;{
#line 324 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_INTEGER;
						tokens[tok_count].ival = (int)strtol(ts, NULL, 10);
						tok_count++;
					}
				}
			}}
		
#line 620 "cypher_lexer.c"

		goto _st10;
		_ctr56:
			{
#line 316 "cypher_lexer.rl"
			{te = p;p = p - 1;{
#line 316 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_FLOAT;
						tokens[tok_count].fval = strtod(ts, NULL);
						tok_count++;
					}
				}
			}}
		
#line 636 "cypher_lexer.c"

		goto _st10;
		_ctr57:
			{
#line 308 "cypher_lexer.rl"
			{te = p;p = p - 1;{
#line 308 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_INTEGER;
						tokens[tok_count].ival = (int)strtoll(ts, NULL, 16);
						tok_count++;
					}
				}
			}}
		
#line 652 "cypher_lexer.c"

		goto _st10;
		_ctr58:
			{
#line 187 "cypher_lexer.rl"
			{te = p;p = p - 1;{
#line 187 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_LT;
						tok_count++;
					}
				}
			}}
		
#line 667 "cypher_lexer.c"

		goto _st10;
		_ctr59:
			{
#line 173 "cypher_lexer.rl"
			{te = p+1;{
#line 173 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_LE;
						tok_count++;
					}
				}
			}}
		
#line 682 "cypher_lexer.c"

		goto _st10;
		_ctr60:
			{
#line 166 "cypher_lexer.rl"
			{te = p+1;{
#line 166 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_NEQ;
						tok_count++;
					}
				}
			}}
		
#line 697 "cypher_lexer.c"

		goto _st10;
		_ctr61:
			{
#line 194 "cypher_lexer.rl"
			{te = p;p = p - 1;{
#line 194 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_GT;
						tok_count++;
					}
				}
			}}
		
#line 712 "cypher_lexer.c"

		goto _st10;
		_ctr62:
			{
#line 180 "cypher_lexer.rl"
			{te = p+1;{
#line 180 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						tokens[tok_count].type = TOK_GE;
						tok_count++;
					}
				}
			}}
		
#line 727 "cypher_lexer.c"

		goto _st10;
		_ctr63:
			{
#line 332 "cypher_lexer.rl"
			{te = p;p = p - 1;{
#line 332 "cypher_lexer.rl"
					
					if (tok_count < max_tokens) {
						int l = (int)(te - ts);
						if (l >= MAX_STR) l = MAX_STR - 1;
						memcpy(tokens[tok_count].str, ts, l);
						tokens[tok_count].str[l] = '\0';
						tokens[tok_count].type = dispatch_keyword(&tokens[tok_count]);
						tok_count++;
					}
				}
			}}
		
#line 746 "cypher_lexer.c"

		goto _st10;
		_st10:
		if ( p == eof )
			goto _out10;
			{
#line 1 "NONE"
			{ts = 0;}}
		
#line 755 "cypher_lexer.c"

		p+= 1;
		st_case_10:
			{
#line 1 "NONE"
			{ts = p;}}
		
#line 762 "cypher_lexer.c"

		switch( ( (*( p))) ) {
			case 9: {
				goto _st11;
			}
			case 10: {
				goto _ctr18;
			}
			case 13: {
				goto _st12;
			}
			case 32: {
				goto _st11;
			}
			case 34: {
				goto _st1;
			}
			case 36: {
				goto _ctr20;
			}
			case 37: {
				goto _ctr21;
			}
			case 39: {
				goto _st3;
			}
			case 40: {
				goto _ctr22;
			}
			case 41: {
				goto _ctr23;
			}
			case 42: {
				goto _ctr24;
			}
			case 43: {
				goto _ctr25;
			}
			case 44: {
				goto _ctr26;
			}
			case 45: {
				goto _st13;
			}
			case 46: {
				goto _ctr28;
			}
			case 47: {
				goto _ctr29;
			}
			case 48: {
				goto _ctr30;
			}
			case 58: {
				goto _ctr32;
			}
			case 59: {
				goto _ctr33;
			}
			case 60: {
				goto _st21;
			}
			case 61: {
				goto _ctr35;
			}
			case 62: {
				goto _st22;
			}
			case 91: {
				goto _ctr38;
			}
			case 93: {
				goto _ctr39;
			}
			case 94: {
				goto _ctr40;
			}
			case 96: {
				goto _st9;
			}
			case 123: {
				goto _ctr41;
			}
			case 124: {
				goto _ctr42;
			}
			case 125: {
				goto _ctr43;
			}
		}
		if ( ( (*( p))) < 65 ) {
			if ( 49 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr31;
			}
		} else if ( ( (*( p))) > 90 ) {
			if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
				goto _st23;
			}
		} else {
			goto _st23;
		}
		goto _st0;
		_st0:
		if ( p == eof )
			goto _out0;
		st_case_0:
		goto _out0;
		_st11:
		if ( p == eof )
			goto _out11;
		p+= 1;
		st_case_11:
		switch( ( (*( p))) ) {
			case 9: {
				goto _st11;
			}
			case 32: {
				goto _st11;
			}
		}
		goto _ctr44;
		_st12:
		if ( p == eof )
			goto _out12;
		p+= 1;
		st_case_12:
		if ( ( (*( p))) == 10 ) {
			goto _ctr18;
		}
		goto _ctr45;
		_st1:
		if ( p == eof )
			goto _out1;
		p+= 1;
		st_case_1:
		switch( ( (*( p))) ) {
			case 34: {
				goto _ctr2;
			}
			case 92: {
				goto _st2;
			}
		}
		goto _st1;
		_st2:
		if ( p == eof )
			goto _out2;
		p+= 1;
		st_case_2:
		goto _st1;
		_st3:
		if ( p == eof )
			goto _out3;
		p+= 1;
		st_case_3:
		switch( ( (*( p))) ) {
			case 39: {
				goto _ctr5;
			}
			case 92: {
				goto _st4;
			}
		}
		goto _st3;
		_st4:
		if ( p == eof )
			goto _out4;
		p+= 1;
		st_case_4:
		goto _st3;
		_st13:
		if ( p == eof )
			goto _out13;
		p+= 1;
		st_case_13:
		if ( ( (*( p))) == 45 ) {
			goto _st14;
		}
		goto _ctr46;
		_st14:
		if ( p == eof )
			goto _out14;
		p+= 1;
		st_case_14:
		if ( ( (*( p))) == 45 ) {
			goto _ctr49;
		}
		goto _ctr48;
		_ctr29:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 955 "cypher_lexer.c"

		goto _st15;
		_st15:
		if ( p == eof )
			goto _out15;
		p+= 1;
		st_case_15:
		switch( ( (*( p))) ) {
			case 42: {
				goto _st5;
			}
			case 47: {
				goto _st16;
			}
		}
		goto _ctr50;
		_st5:
		if ( p == eof )
			goto _out5;
		p+= 1;
		st_case_5:
		if ( ( (*( p))) == 42 ) {
			goto _st6;
		}
		goto _st5;
		_st6:
		if ( p == eof )
			goto _out6;
		p+= 1;
		st_case_6:
		switch( ( (*( p))) ) {
			case 42: {
				goto _st6;
			}
			case 47: {
				goto _ctr10;
			}
		}
		goto _st5;
		_st16:
		if ( p == eof )
			goto _out16;
		p+= 1;
		st_case_16:
		if ( ( (*( p))) == 10 ) {
			goto _ctr52;
		}
		goto _st16;
		_ctr30:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1008 "cypher_lexer.c"

		goto _st17;
		_st17:
		if ( p == eof )
			goto _out17;
		p+= 1;
		st_case_17:
		switch( ( (*( p))) ) {
			case 46: {
				goto _st7;
			}
			case 120: {
				goto _st8;
			}
		}
		if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
			goto _ctr31;
		}
		goto _ctr53;
		_st7:
		if ( p == eof )
			goto _out7;
		p+= 1;
		st_case_7:
		if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
			goto _st18;
		}
		goto _ctr11;
		_st18:
		if ( p == eof )
			goto _out18;
		p+= 1;
		st_case_18:
		if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
			goto _st18;
		}
		goto _ctr56;
		_ctr31:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1050 "cypher_lexer.c"

		goto _st19;
		_st19:
		if ( p == eof )
			goto _out19;
		p+= 1;
		st_case_19:
		if ( ( (*( p))) == 46 ) {
			goto _st7;
		}
		if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
			goto _ctr31;
		}
		goto _ctr53;
		_st8:
		if ( p == eof )
			goto _out8;
		p+= 1;
		st_case_8:
		if ( ( (*( p))) < 65 ) {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st20;
			}
		} else if ( ( (*( p))) > 70 ) {
			if ( 97 <= ( (*( p))) && ( (*( p))) <= 102 ) {
				goto _st20;
			}
		} else {
			goto _st20;
		}
		goto _ctr11;
		_st20:
		if ( p == eof )
			goto _out20;
		p+= 1;
		st_case_20:
		if ( ( (*( p))) < 65 ) {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st20;
			}
		} else if ( ( (*( p))) > 70 ) {
			if ( 97 <= ( (*( p))) && ( (*( p))) <= 102 ) {
				goto _st20;
			}
		} else {
			goto _st20;
		}
		goto _ctr57;
		_st21:
		if ( p == eof )
			goto _out21;
		p+= 1;
		st_case_21:
		switch( ( (*( p))) ) {
			case 61: {
				goto _ctr59;
			}
			case 62: {
				goto _ctr60;
			}
		}
		goto _ctr58;
		_st22:
		if ( p == eof )
			goto _out22;
		p+= 1;
		st_case_22:
		if ( ( (*( p))) == 61 ) {
			goto _ctr62;
		}
		goto _ctr61;
		_st23:
		if ( p == eof )
			goto _out23;
		p+= 1;
		st_case_23:
		if ( ( (*( p))) == 95 ) {
			goto _st23;
		}
		if ( ( (*( p))) < 65 ) {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st23;
			}
		} else if ( ( (*( p))) > 90 ) {
			if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
				goto _st23;
			}
		} else {
			goto _st23;
		}
		goto _ctr63;
		_st9:
		if ( p == eof )
			goto _out9;
		p+= 1;
		st_case_9:
		if ( ( (*( p))) == 96 ) {
			goto _ctr15;
		}
		goto _st9;
		_out10: cs = 10; goto _out; 
		_out0: cs = 0; goto _out; 
		_out11: cs = 11; goto _out; 
		_out12: cs = 12; goto _out; 
		_out1: cs = 1; goto _out; 
		_out2: cs = 2; goto _out; 
		_out3: cs = 3; goto _out; 
		_out4: cs = 4; goto _out; 
		_out13: cs = 13; goto _out; 
		_out14: cs = 14; goto _out; 
		_out15: cs = 15; goto _out; 
		_out5: cs = 5; goto _out; 
		_out6: cs = 6; goto _out; 
		_out16: cs = 16; goto _out; 
		_out17: cs = 17; goto _out; 
		_out7: cs = 7; goto _out; 
		_out18: cs = 18; goto _out; 
		_out19: cs = 19; goto _out; 
		_out8: cs = 8; goto _out; 
		_out20: cs = 20; goto _out; 
		_out21: cs = 21; goto _out; 
		_out22: cs = 22; goto _out; 
		_out23: cs = 23; goto _out; 
		_out9: cs = 9; goto _out; 
		_out: {}
	}
	
#line 349 "cypher_lexer.rl"

	
	return tok_count;
}
