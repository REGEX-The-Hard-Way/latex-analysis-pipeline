#line 1 "66.rl"
/* Pattern 66: Import Machines from 66_atoms.rl */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "66.c"
static const signed char _p66_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1,
	3, 0
};

static const signed char _p66_key_offsets[] = {
	0, 0, 4, 5, 6, 7, 8, 0
};

static const char _p66_trans_keys[] = {
	10, 32, 98, 102, 97, 114, 111, 111,
	10, 32, 98, 102, 0
};

static const signed char _p66_single_lengths[] = {
	0, 4, 1, 1, 1, 1, 4, 0
};

static const signed char _p66_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0
};

static const signed char _p66_index_offsets[] = {
	0, 0, 5, 7, 9, 11, 13, 0
};

static const signed char _p66_cond_targs[] = {
	6, 6, 2, 4, 0, 3, 0, 6,
	0, 5, 0, 6, 0, 6, 6, 2,
	4, 0, 0, 1, 2, 3, 4, 5,
	6, 0
};

static const signed char _p66_cond_actions[] = {
	7, 5, 0, 0, 0, 0, 0, 3,
	0, 0, 0, 1, 0, 7, 5, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0
};

static const int p66_start = 1;
static const int p66_first_final = 6;
static const int p66_error = 0;

static const int p66_en_main = 1;


#line 21 "66.rl"

int main(void){int cs;const char *p,*pe,*eof;const char *d="foo bar foo\n";
	printf("Pattern 66: Import Machines\n\n");
	p=d;pe=p+strlen(d);eof=pe;

#line 57 "66.c"
	{
		cs = (int)p66_start;
	}
	
#line 25 "66.rl"


#line 62 "66.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p66_trans_keys + (_p66_key_offsets[cs]));
		_trans = (unsigned int)_p66_index_offsets[cs];
		
		_klen = (int)_p66_single_lengths[cs];
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
		
		_klen = (int)_p66_range_lengths[cs];
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
		cs = (int)_p66_cond_targs[_trans];
		
		if ( _p66_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p66_actions + (_p66_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 8 "66.rl"
							printf("foo "); }
						
#line 137 "66.c"

						break; 
					}
					case 1:  {
							{
#line 9 "66.rl"
							printf("bar "); }
						
#line 145 "66.c"

						break; 
					}
					case 2:  {
							{
#line 10 "66.rl"
							printf("SP ");  }
						
#line 153 "66.c"

						break; 
					}
					case 3:  {
							{
#line 11 "66.rl"
							printf("NL");   }
						
#line 161 "66.c"

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
	
#line 26 "66.rl"

	printf(" -> %s ok\n",cs>=p66_first_final?"ACCEPT":"FAIL");
	assert(cs>=p66_first_final);
	printf("\nAll tests passed.\n");return 0;}
