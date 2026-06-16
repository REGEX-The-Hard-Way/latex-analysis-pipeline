#line 1 "80.rl"
/* Pattern 80: C++ Lexer — tokenize identifiers, integers, floats, hex, operators, strings, comments */
#include <stdio.h>
#include <string.h>
#include <assert.h>

enum {
	TK_ID = 256, TK_INT, TK_FLOAT, TK_HEX, TK_STRING,
	TK_COMMENT, TK_LCOMMENT, TK_EQ, TK_NE, TK_AND, TK_OR,
	TK_WS
};
const char *tok_names[] = {
	[TK_ID-256]="IDENT",[TK_INT-256]="INT",[TK_FLOAT-256]="FLOAT",
	[TK_HEX-256]="HEX",[TK_STRING-256]="STR",[TK_COMMENT-256]="CMT",
	[TK_LCOMMENT-256]="LCMT",[TK_EQ-256]="EQ",[TK_NE-256]="NE",
	[TK_AND-256]="AND",[TK_OR-256]="OR",[TK_WS-256]="WS",
};

int tok_type;
const char *ts, *te;
int act;


#line 23 "80.c"
static const signed char _p80_actions[] = {
	0, 1, 2, 1, 3, 1, 5, 1,
	6, 1, 7, 1, 8, 1, 9, 1,
	10, 1, 11, 1, 12, 1, 13, 1,
	14, 1, 15, 1, 16, 1, 17, 1,
	18, 2, 0, 1, 2, 3, 4, 0
};

static const signed char _p80_key_offsets[] = {
	0, 0, 1, 3, 3, 4, 6, 7,
	9, 10, 12, 18, 19, 20, 37, 40,
	41, 46, 48, 51, 57, 0
};

static const char _p80_trans_keys[] = {
	61, 34, 92, 38, 42, 47, 42, 42,
	47, 10, 48, 57, 48, 57, 65, 70,
	97, 102, 61, 124, 32, 33, 34, 38,
	47, 48, 61, 95, 124, 9, 13, 49,
	57, 65, 90, 97, 122, 32, 9, 13,
	42, 46, 88, 120, 48, 57, 48, 57,
	46, 48, 57, 48, 57, 65, 70, 97,
	102, 95, 48, 57, 65, 90, 97, 122,
	0
};

static const signed char _p80_single_lengths[] = {
	0, 1, 2, 0, 1, 2, 1, 2,
	1, 0, 0, 1, 1, 9, 1, 1,
	3, 0, 1, 0, 1, 0
};

static const signed char _p80_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 3, 0, 0, 4, 1, 0,
	1, 1, 1, 3, 3, 0
};

static const signed char _p80_index_offsets[] = {
	0, 0, 2, 5, 6, 8, 11, 13,
	16, 18, 20, 24, 26, 28, 42, 45,
	47, 52, 54, 57, 61, 0
};

static const signed char _p80_cond_targs[] = {
	13, 0, 13, 3, 2, 2, 13, 0,
	6, 8, 0, 7, 6, 7, 15, 6,
	13, 8, 17, 13, 19, 19, 19, 13,
	13, 0, 13, 0, 14, 1, 2, 4,
	5, 16, 11, 20, 12, 14, 18, 20,
	20, 0, 14, 14, 13, 7, 6, 9,
	10, 10, 18, 13, 17, 13, 9, 18,
	13, 19, 19, 19, 13, 20, 20, 20,
	20, 13, 0, 1, 2, 3, 4, 5,
	13, 13, 8, 13, 13, 11, 12, 13,
	13, 13, 13, 13, 13, 13, 13, 0
};

static const signed char _p80_cond_actions[] = {
	11, 0, 5, 0, 0, 0, 13, 0,
	0, 0, 0, 0, 0, 0, 36, 0,
	7, 0, 0, 29, 0, 0, 0, 29,
	9, 0, 15, 0, 0, 0, 0, 0,
	0, 3, 0, 0, 0, 0, 3, 0,
	0, 0, 0, 0, 27, 0, 0, 0,
	0, 0, 3, 23, 0, 21, 0, 3,
	23, 0, 0, 0, 19, 0, 0, 0,
	0, 17, 0, 0, 0, 0, 0, 0,
	31, 31, 0, 29, 29, 0, 0, 0,
	27, 25, 23, 21, 23, 19, 17, 0
};

static const signed char _p80_to_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 33, 0, 0,
	0, 0, 0, 0, 0, 0
};

static const signed char _p80_from_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 0
};

static const signed char _p80_eof_trans[] = {
	67, 68, 69, 70, 71, 72, 73, 74,
	75, 76, 77, 78, 79, 80, 81, 82,
	83, 84, 85, 86, 87, 0
};

static const int p80_start = 13;
static const int p80_first_final = 13;
static const int p80_error = 0;

static const int p80_en_main = 13;


#line 40 "80.rl"


int main(void){int cs;const char *p,*pe,*eof;
	printf("Pattern 80: C++ Lexer (12 token types)\n\n");
	const char *src = "int x=42; float y=3.14; /* block */ // line\n"
	"char *s=\"hello\"; if(x!=0&&y>0) return 0xFF;\n";
	p=src;pe=p+strlen(p);eof=pe;

#line 127 "80.c"
	{
		cs = (int)p80_start;
		ts = 0;
		te = 0;
		act = 0;
	}
	
#line 47 "80.rl"


#line 135 "80.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe && p != eof )
			goto _out;
		_acts = ( _p80_actions + (_p80_from_state_actions[cs]));
		_nacts = (unsigned int)(*( _acts));
		_acts += 1;
		while ( _nacts > 0 ) {
			switch ( (*( _acts)) ) {
				case 2:  {
						{
#line 1 "NONE"
						{ts = p;}}
					
#line 154 "80.c"

					break; 
				}
			}
			_nacts -= 1;
			_acts += 1;
		}
		
		if ( p == eof ) {
			if ( _p80_eof_trans[cs] > 0 ) {
				_trans = (unsigned int)_p80_eof_trans[cs] - 1;
			}
		}
		else {
			_keys = ( _p80_trans_keys + (_p80_key_offsets[cs]));
			_trans = (unsigned int)_p80_index_offsets[cs];
			
			_klen = (int)_p80_single_lengths[cs];
			if ( _klen > 0 ) {
				const char *_lower = _keys;
				const char *_upper = _keys + _klen - 1;
				const char *_mid;
				while ( 1 ) {
					if ( _upper < _lower ) {
						_keys += _klen;
						_trans += (unsigned int)_klen;
						break;
					}
					
					_mid = _lower + ((_upper-_lower) >> 1);
					if ( ( (*( p))) < (*( _mid)) )
						_upper = _mid - 1;
					else if ( ( (*( p))) > (*( _mid)) )
						_lower = _mid + 1;
					else {
						_trans += (unsigned int)(_mid - _keys);
						goto _match;
					}
				}
			}
			
			_klen = (int)_p80_range_lengths[cs];
			if ( _klen > 0 ) {
				const char *_lower = _keys;
				const char *_upper = _keys + (_klen<<1) - 2;
				const char *_mid;
				while ( 1 ) {
					if ( _upper < _lower ) {
						_trans += (unsigned int)_klen;
						break;
					}
					
					_mid = _lower + (((_upper-_lower) >> 1) & ~1);
					if ( ( (*( p))) < (*( _mid)) )
						_upper = _mid - 2;
					else if ( ( (*( p))) > (*( _mid + 1)) )
						_lower = _mid + 2;
					else {
						_trans += (unsigned int)((_mid - _keys)>>1);
						break;
					}
				}
			}
			
			_match: {}
		}
		cs = (int)_p80_cond_targs[_trans];
		
		if ( _p80_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p80_actions + (_p80_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 3:  {
							{
#line 1 "NONE"
							{te = p+1;}}
						
#line 235 "80.c"

						break; 
					}
					case 4:  {
							{
#line 30 "80.rl"
							{act = 6;}}
						
#line 243 "80.c"

						break; 
					}
					case 5:  {
							{
#line 29 "80.rl"
							{te = p+1;{
#line 29 "80.rl"
									printf("%s(%.*s) ", tok_names[TK_STRING-256], (int)(te-ts), ts); {p += 1; goto _out; } }
							}}
						
#line 254 "80.c"

						break; 
					}
					case 6:  {
							{
#line 31 "80.rl"
							{te = p+1;{
#line 31 "80.rl"
									printf("%s(%.*s) ", tok_names[TK_LCOMMENT-256],(int)(te-ts),ts); {p += 1; goto _out; } }
							}}
						
#line 265 "80.c"

						break; 
					}
					case 7:  {
							{
#line 32 "80.rl"
							{te = p+1;{
#line 32 "80.rl"
									printf("%s", tok_names[TK_EQ-256]); {p += 1; goto _out; } }
							}}
						
#line 276 "80.c"

						break; 
					}
					case 8:  {
							{
#line 33 "80.rl"
							{te = p+1;{
#line 33 "80.rl"
									printf("%s", tok_names[TK_NE-256]); {p += 1; goto _out; } }
							}}
						
#line 287 "80.c"

						break; 
					}
					case 9:  {
							{
#line 34 "80.rl"
							{te = p+1;{
#line 34 "80.rl"
									printf("%s", tok_names[TK_AND-256]); {p += 1; goto _out; } }
							}}
						
#line 298 "80.c"

						break; 
					}
					case 10:  {
							{
#line 35 "80.rl"
							{te = p+1;{
#line 35 "80.rl"
									printf("%s", tok_names[TK_OR-256]); {p += 1; goto _out; } }
							}}
						
#line 309 "80.c"

						break; 
					}
					case 11:  {
							{
#line 25 "80.rl"
							{te = p;p = p - 1;{
#line 25 "80.rl"
									printf("%s(%.*s) ", tok_names[TK_ID-256],     (int)(te-ts), ts); {p += 1; goto _out; } }
							}}
						
#line 320 "80.c"

						break; 
					}
					case 12:  {
							{
#line 26 "80.rl"
							{te = p;p = p - 1;{
#line 26 "80.rl"
									printf("%s(%.*s) ", tok_names[TK_HEX-256],    (int)(te-ts), ts); {p += 1; goto _out; } }
							}}
						
#line 331 "80.c"

						break; 
					}
					case 13:  {
							{
#line 27 "80.rl"
							{te = p;p = p - 1;{
#line 27 "80.rl"
									printf("%s(%.*s) ", tok_names[TK_FLOAT-256],  (int)(te-ts), ts); {p += 1; goto _out; } }
							}}
						
#line 342 "80.c"

						break; 
					}
					case 14:  {
							{
#line 28 "80.rl"
							{te = p;p = p - 1;{
#line 28 "80.rl"
									printf("%s(%.*s) ", tok_names[TK_INT-256],    (int)(te-ts), ts); {p += 1; goto _out; } }
							}}
						
#line 353 "80.c"

						break; 
					}
					case 15:  {
							{
#line 30 "80.rl"
							{te = p;p = p - 1;{
#line 30 "80.rl"
									printf("%s(%.*s) ", tok_names[TK_COMMENT-256],(int)(te-ts), ts); {p += 1; goto _out; } }
							}}
						
#line 364 "80.c"

						break; 
					}
					case 16:  {
							{
#line 1 "-"
							{te = p;p = p - 1;}}
						
#line 372 "80.c"

						break; 
					}
					case 17:  {
							{
#line 28 "80.rl"
							{p = ((te))-1;
								{
#line 28 "80.rl"
									printf("%s(%.*s) ", tok_names[TK_INT-256],    (int)(te-ts), ts); {p += 1; goto _out; } }
							}}
						
#line 384 "80.c"

						break; 
					}
					case 18:  {
							{
#line 1 "NONE"
							{switch( act ) {
									case 0:  {
										{
#line 1 "NONE"
											{cs = 0;goto _again;}}
										break; 
									}
									case 6:  {
										p = ((te))-1;
										{
#line 30 "80.rl"
											printf("%s(%.*s) ", tok_names[TK_COMMENT-256],(int)(te-ts), ts); {p += 1; goto _out; } }
										break; 
									}
								}}
						}
						
#line 407 "80.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
		}
		
		_again: {}
		if ( p == eof ) {
			if ( cs >= 13 )
				goto _out;
		}
		else {
			_acts = ( _p80_actions + (_p80_to_state_actions[cs]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) ) {
					case 0:  {
							{
#line 1 "NONE"
							{ts = 0;}}
						
#line 433 "80.c"

						break; 
					}
					case 1:  {
							{
#line 1 "NONE"
							{act = 0;}}
						
#line 441 "80.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
		}
		_out: {}
	}
	
#line 48 "80.rl"

	printf("-> %s ok\n",cs>=p80_first_final?"ACCEPT":"FAIL");
	assert(cs>=p80_first_final);
	printf("\nAll tests passed.\n");return 0;}
