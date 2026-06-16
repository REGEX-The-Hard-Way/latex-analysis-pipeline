#line 1 "73.rl"
/* Pattern 73: EOF fgoto — fhold+fgoto to post-processing on %eof */
#include <stdio.h>
#include <string.h>
#include <assert.h>


#line 7 "73.c"
static const signed char _p73_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 0
};

static const signed char _p73_key_offsets[] = {
	0, 0, 2, 4, 0
};

static const char _p73_trans_keys[] = {
	48, 57, 48, 57, 0
};

static const signed char _p73_single_lengths[] = {
	0, 0, 0, 0, 0
};

static const signed char _p73_range_lengths[] = {
	0, 1, 1, 0, 0
};

static const signed char _p73_index_offsets[] = {
	0, 0, 2, 4, 0
};

static const signed char _p73_cond_targs[] = {
	2, 0, 2, 0, 0, 0, 1, 2,
	3, 0
};

static const signed char _p73_cond_actions[] = {
	1, 0, 1, 0, 0, 0, 0, 3,
	5, 0
};

static const signed char _p73_eof_trans[] = {
	6, 7, 8, 9, 0
};

static const int p73_start = 1;
static const int p73_first_final = 2;
static const int p73_error = 0;

static const int p73_en_extra = 3;
static const int p73_en_main = 1;


#line 19 "73.rl"

int main(void){int cs;const char *p,*pe,*eof;
	printf("Pattern 73: %%eof with fhold+fgoto\n\n");
	p="123";pe=p+strlen(p);eof=pe;

#line 57 "73.c"
	{
		cs = (int)p73_start;
	}
	
#line 23 "73.rl"


#line 62 "73.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe && p != eof )
			goto _out;
		if ( p == eof ) {
			if ( _p73_eof_trans[cs] > 0 ) {
				_trans = (unsigned int)_p73_eof_trans[cs] - 1;
			}
		}
		else {
			_keys = ( _p73_trans_keys + (_p73_key_offsets[cs]));
			_trans = (unsigned int)_p73_index_offsets[cs];
			
			_klen = (int)_p73_single_lengths[cs];
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
			
			_klen = (int)_p73_range_lengths[cs];
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
		cs = (int)_p73_cond_targs[_trans];
		
		if ( _p73_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p73_actions + (_p73_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 8 "73.rl"
							printf("num=%c ", (( (*( p))))); }
						
#line 144 "73.c"

						break; 
					}
					case 1:  {
							{
#line 9 "73.rl"
							printf("EOF_EXTRA "); {p = p - 1; } {cs = (3);goto _again;} }
						
#line 152 "73.c"

						break; 
					}
					case 2:  {
							{
#line 10 "73.rl"
							printf("DONE"); }
						
#line 160 "73.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
		}
		
		_again: {}
		if ( p == eof ) {
			if ( cs >= 2 )
				goto _out;
		}
		else {
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
		}
		_out: {}
	}
	
#line 24 "73.rl"

	printf("-> %s ok\n",cs>=p73_first_final?"ACCEPT":"FAIL");
	printf("\nAll tests passed.\n");return 0;}
