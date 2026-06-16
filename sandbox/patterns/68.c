#line 1 "68.rl"
/* Pattern 68: Concurrent Machines — simultaneous pattern matching */
#include <stdio.h>
#include <string.h>
#include <assert.h>


#line 7 "68.c"
static const signed char _p68_actions[] = {
	0, 1, 2, 1, 3, 2, 0, 1,
	2, 1, 0, 0
};

static const signed char _p68_key_offsets[] = {
	0, 0, 1, 2, 3, 5, 7, 10,
	19, 28, 37, 46, 55, 0
};

static const char _p68_trans_keys[] = {
	34, 42, 42, 42, 47, 34, 42, 34,
	42, 47, 32, 34, 47, 9, 13, 65,
	90, 97, 122, 32, 34, 42, 9, 13,
	65, 90, 97, 122, 32, 34, 42, 9,
	13, 65, 90, 97, 122, 32, 34, 42,
	9, 13, 65, 90, 97, 122, 32, 34,
	42, 9, 13, 65, 90, 97, 122, 32,
	34, 47, 9, 13, 65, 90, 97, 122,
	0
};

static const signed char _p68_single_lengths[] = {
	0, 1, 1, 1, 2, 2, 3, 3,
	3, 3, 3, 3, 3, 0
};

static const signed char _p68_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 3,
	3, 3, 3, 3, 3, 0
};

static const signed char _p68_index_offsets[] = {
	0, 0, 2, 4, 6, 9, 12, 16,
	23, 30, 37, 44, 51, 0
};

static const signed char _p68_cond_targs[] = {
	7, 1, 3, 0, 4, 3, 4, 8,
	3, 8, 6, 5, 8, 6, 9, 5,
	7, 1, 2, 7, 12, 12, 0, 8,
	5, 4, 8, 11, 11, 3, 9, 9,
	6, 9, 10, 10, 5, 9, 9, 6,
	9, 10, 10, 5, 8, 5, 4, 8,
	11, 11, 3, 7, 1, 2, 7, 12,
	12, 0, 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11, 12, 0
};

static const signed char _p68_cond_actions[] = {
	3, 0, 0, 0, 0, 0, 0, 1,
	0, 3, 0, 0, 3, 0, 1, 0,
	0, 0, 0, 0, 5, 5, 0, 0,
	0, 0, 0, 5, 5, 0, 0, 3,
	0, 0, 5, 5, 0, 0, 3, 0,
	0, 8, 8, 0, 0, 0, 0, 0,
	8, 8, 0, 0, 0, 0, 0, 8,
	8, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

static const int p68_start = 7;
static const int p68_first_final = 7;
static const int p68_error = 0;

static const int p68_en_main = 7;


#line 23 "68.rl"

int main(void){int cs;const char *p,*pe,*eof;
	const char *d="abc \"hello\" /*c*/ x def";
	printf("Pattern 68: Concurrent Machines\n\n");
	p=d;pe=p+strlen(d);eof=pe;

#line 80 "68.c"
	{
		cs = (int)p68_start;
	}
	
#line 28 "68.rl"


#line 85 "68.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p68_trans_keys + (_p68_key_offsets[cs]));
		_trans = (unsigned int)_p68_index_offsets[cs];
		
		_klen = (int)_p68_single_lengths[cs];
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
		
		_klen = (int)_p68_range_lengths[cs];
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
		cs = (int)_p68_cond_targs[_trans];
		
		if ( _p68_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p68_actions + (_p68_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 8 "68.rl"
							printf("CHAR[%c] ", (( (*( p))))); }
						
#line 160 "68.c"

						break; 
					}
					case 1:  {
							{
#line 9 "68.rl"
							printf("WORD ");          }
						
#line 168 "68.c"

						break; 
					}
					case 2:  {
							{
#line 10 "68.rl"
							printf("CMT ");           }
						
#line 176 "68.c"

						break; 
					}
					case 3:  {
							{
#line 11 "68.rl"
							printf("STR ");           }
						
#line 184 "68.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
		}
		
		if ( cs != 0 ) {
			p += 1;
			goto _resume;
		}
		_out: {}
	}
	
#line 29 "68.rl"

	printf("-> %s ok\n",cs>=p68_first_final?"ACCEPT":"FAIL");
	assert(cs>=p68_first_final);
	printf("\nAll tests passed.\n");return 0;}
