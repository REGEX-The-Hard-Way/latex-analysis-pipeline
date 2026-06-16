#line 1 "71.rl"
/* Pattern 71: Pull-Style Scanner — scan() returning one token via fbreak */
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct Scanner {
	int cs, act;
	const char *ts, *te;
	const char *p, *pe;
	const char *eof;
};

struct Scanner sc;


#line 16 "71.c"
static const signed char _p71_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1,
	3, 1, 4, 1, 5, 0
};

static const signed char _p71_key_offsets[] = {
	0, 0, 14, 17, 19, 0
};

static const char _p71_trans_keys[] = {
	32, 45, 47, 95, 9, 13, 42, 43,
	48, 57, 65, 90, 97, 122, 32, 9,
	13, 48, 57, 95, 48, 57, 65, 90,
	97, 122, 0
};

static const signed char _p71_single_lengths[] = {
	0, 4, 1, 0, 1, 0
};

static const signed char _p71_range_lengths[] = {
	0, 5, 1, 1, 3, 0
};

static const signed char _p71_index_offsets[] = {
	0, 0, 10, 13, 15, 0
};

static const signed char _p71_cond_targs[] = {
	2, 1, 1, 4, 2, 1, 3, 4,
	4, 0, 2, 2, 1, 3, 1, 4,
	4, 4, 4, 1, 0, 1, 1, 1,
	1, 0
};

static const signed char _p71_cond_actions[] = {
	0, 5, 5, 0, 0, 5, 0, 0,
	0, 0, 0, 0, 11, 0, 9, 0,
	0, 0, 0, 7, 0, 0, 11, 9,
	7, 0
};

static const signed char _p71_to_state_actions[] = {
	0, 1, 0, 0, 0, 0
};

static const signed char _p71_from_state_actions[] = {
	0, 3, 0, 0, 0, 0
};

static const signed char _p71_eof_trans[] = {
	21, 22, 23, 24, 25, 0
};

static const int p71_start = 1;
static const int p71_first_final = 1;
static const int p71_error = 0;

static const int p71_en_main = 1;


#line 32 "71.rl"


int scan(void) {
	if (sc.p == sc.pe && sc.p == sc.eof)
		return 0;
	
#line 37 "71.rl"

	
#line 38 "71.rl"

	
#line 39 "71.rl"

	
#line 40 "71.rl"

	
#line 41 "71.rl"

	
#line 42 "71.rl"

	
#line 43 "71.rl"


#line 96 "71.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( ( sc.p) == ( sc.pe) && ( sc.p) != ( sc.eof) )
			goto _out;
		_acts = ( _p71_actions + (_p71_from_state_actions[( sc.cs)]));
		_nacts = (unsigned int)(*( _acts));
		_acts += 1;
		while ( _nacts > 0 ) {
			switch ( (*( _acts)) ) {
				case 1:  {
						{
#line 1 "NONE"
						{( sc.ts) = ( sc.p);}}
					
#line 115 "71.c"

					break; 
				}
			}
			_nacts -= 1;
			_acts += 1;
		}
		
		if ( ( sc.p) == ( sc.eof) ) {
			if ( _p71_eof_trans[( sc.cs)] > 0 ) {
				_trans = (unsigned int)_p71_eof_trans[( sc.cs)] - 1;
			}
		}
		else {
			_keys = ( _p71_trans_keys + (_p71_key_offsets[( sc.cs)]));
			_trans = (unsigned int)_p71_index_offsets[( sc.cs)];
			
			_klen = (int)_p71_single_lengths[( sc.cs)];
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
					if ( ( (*( ( sc.p)))) < (*( _mid)) )
						_upper = _mid - 1;
					else if ( ( (*( ( sc.p)))) > (*( _mid)) )
						_lower = _mid + 1;
					else {
						_trans += (unsigned int)(_mid - _keys);
						goto _match;
					}
				}
			}
			
			_klen = (int)_p71_range_lengths[( sc.cs)];
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
					if ( ( (*( ( sc.p)))) < (*( _mid)) )
						_upper = _mid - 2;
					else if ( ( (*( ( sc.p)))) > (*( _mid + 1)) )
						_lower = _mid + 2;
					else {
						_trans += (unsigned int)((_mid - _keys)>>1);
						break;
					}
				}
			}
			
			_match: {}
		}
		( sc.cs) = (int)_p71_cond_targs[_trans];
		
		if ( _p71_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p71_actions + (_p71_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 2:  {
							{
#line 25 "71.rl"
							{( sc.te) = ( sc.p)+1;{
#line 25 "71.rl"
									
									printf("OP(%c) ", (( (*( ( sc.p))))));
									{( sc.p) += 1; goto _out; }
								}
							}}
						
#line 202 "71.c"

						break; 
					}
					case 3:  {
							{
#line 17 "71.rl"
							{( sc.te) = ( sc.p);( sc.p) = ( sc.p) - 1;{
#line 17 "71.rl"
									
									printf("ID(%.*s) ", (int)(sc.te - sc.ts), sc.ts);
									{( sc.p) += 1; goto _out; }
								}
							}}
						
#line 216 "71.c"

						break; 
					}
					case 4:  {
							{
#line 21 "71.rl"
							{( sc.te) = ( sc.p);( sc.p) = ( sc.p) - 1;{
#line 21 "71.rl"
									
									printf("NUM(%.*s) ", (int)(sc.te - sc.ts), sc.ts);
									{( sc.p) += 1; goto _out; }
								}
							}}
						
#line 230 "71.c"

						break; 
					}
					case 5:  {
							{
#line 1 "-"
							{( sc.te) = ( sc.p);( sc.p) = ( sc.p) - 1;}}
						
#line 238 "71.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
		}
		
		if ( ( sc.p) == ( sc.eof) ) {
			if ( ( sc.cs) >= 1 )
				goto _out;
		}
		else {
			_acts = ( _p71_actions + (_p71_to_state_actions[( sc.cs)]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) ) {
					case 0:  {
							{
#line 1 "NONE"
							{( sc.ts) = 0;}}
						
#line 263 "71.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
			if ( ( sc.cs) != 0 ) {
				( sc.p) += 1;
				goto _resume;
			}
		}
		_out: {}
	}
	
#line 44 "71.rl"

	return 1;
}

int main(void){
	printf("Pattern 71: Pull-Style Scanner\n\n");
	
	
#line 51 "71.rl"


#line 288 "71.c"
	{
		( sc.cs) = (int)p71_start;
		( sc.ts) = 0;
		( sc.te) = 0;
	}
	
#line 52 "71.rl"

	
	const char *input = "foo 123 + bar 456";
	sc.p = input;
	sc.pe = input + strlen(input);
	sc.eof = sc.pe;
	
	while (scan()) {
		if (sc.cs == p71_error) { printf("ERR "); break; }
	}
	printf("-> %s ok\n", sc.cs >= p71_first_final ? "ACCEPT" : "FAIL");
	assert(sc.cs >= p71_first_final);
	printf("\nAll tests passed.\n");return 0;}
