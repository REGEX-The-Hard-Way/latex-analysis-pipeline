#line 1 "62.rl"
/* Pattern 62: getkey — parse array of structs by type field */
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct { int type; const char *name; } Token;
Token tokens[] = {
	{ 'A', "alpha" },
	{ 'B', "beta"  },
	{ 'Z', "zeta"  },
	{ 0,   NULL     },
};
#define TOKEN_COUNT 3


#line 16 "62.c"
static const signed char _p62_actions[] = {
	0, 1, 1, 1, 2, 2, 0, 1,
	0
};

static const signed char _p62_key_offsets[] = {
	0, 0, 0
};

static const int _p62_trans_keys[] = {
	0
};

static const signed char _p62_single_lengths[] = {
	0, 0, 0
};

static const signed char _p62_range_lengths[] = {
	0, 0, 0
};

static const signed char _p62_index_offsets[] = {
	0, 1, 0
};

static const signed char _p62_cond_targs[] = {
	1, 1, 0, 1, 0
};

static const signed char _p62_cond_actions[] = {
	5, 1, 0, 3, 0
};

static const signed char _p62_eof_trans[] = {
	3, 4, 0
};

static const int p62_start = 0;
static const int p62_first_final = 1;
static const int p62_error = -1;

static const int p62_en_main = 0;


#line 27 "62.rl"

int main(void){int cs;Token *p,*pe,*eof;p=tokens;pe=p+TOKEN_COUNT;eof=pe;
	printf("Pattern 62: getkey — struct array parsing\n\n");
	printf("  Tokens:");

#line 64 "62.c"
	{
		cs = (int)p62_start;
	}
	
#line 31 "62.rl"


#line 69 "62.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const int * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe && p != eof )
			goto _out;
		if ( p == eof ) {
			if ( _p62_eof_trans[cs] > 0 ) {
				_trans = (unsigned int)_p62_eof_trans[cs] - 1;
			}
		}
		else {
			_keys = ( _p62_trans_keys + (_p62_key_offsets[cs]));
			_trans = (unsigned int)_p62_index_offsets[cs];
			
			_klen = (int)_p62_single_lengths[cs];
			if ( _klen > 0 ) {
				const int *_lower = _keys;
				const int *_upper = _keys + _klen - 1;
				const int *_mid;
				while ( 1 ) {
					if ( _upper < _lower ) {
						_keys += _klen;
						_trans += (unsigned int)_klen;
						break;
					}
					
					_mid = _lower + ((_upper-_lower) >> 1);
					if ( (p->type) < (*( _mid)) )
						_upper = _mid - 1;
					else if ( (p->type) > (*( _mid)) )
						_lower = _mid + 1;
					else {
						_trans += (unsigned int)(_mid - _keys);
						goto _match;
					}
				}
			}
			
			_klen = (int)_p62_range_lengths[cs];
			if ( _klen > 0 ) {
				const int *_lower = _keys;
				const int *_upper = _keys + (_klen<<1) - 2;
				const int *_mid;
				while ( 1 ) {
					if ( _upper < _lower ) {
						_trans += (unsigned int)_klen;
						break;
					}
					
					_mid = _lower + (((_upper-_lower) >> 1) & ~1);
					if ( (p->type) < (*( _mid)) )
						_upper = _mid - 2;
					else if ( (p->type) > (*( _mid + 1)) )
						_lower = _mid + 2;
					else {
						_trans += (unsigned int)((_mid - _keys)>>1);
						break;
					}
				}
			}
			
			_match: {}
		}
		cs = (int)_p62_cond_targs[_trans];
		
		if ( _p62_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p62_actions + (_p62_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 20 "62.rl"
							printf("{"); }
							
#line 151 "62.c"

							break; 
						}
						case 1:  {
								{
#line 21 "62.rl"
								printf(" %s", p->name); }
							
#line 159 "62.c"

							break; 
						}
						case 2:  {
								{
#line 22 "62.rl"
								printf(" }\n"); }
						
#line 167 "62.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
		}
		
		if ( p == eof ) {
			if ( cs >= 1 )
				goto _out;
		}
		else {
			p += 1;
			goto _resume;
		}
		_out: {}
	}
	
#line 32 "62.rl"

	if(cs>=p62_first_final)printf("ACCEPT\n");else printf("FAIL\n");
	assert(cs>=p62_first_final);
	printf("\nAll tests passed.\n");return 0;}
