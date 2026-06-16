#line 1 "67.rl"
/* Pattern 67: Include Machines from 67_lib.rl */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "67.c"
static const signed char _p67_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1,
	3, 0
};

static const signed char _p67_key_offsets[] = {
	0, 0, 0
};

static const char _p67_trans_keys[] = {
	65, 66, 67, 68, 0
};

static const signed char _p67_single_lengths[] = {
	0, 4, 0
};

static const signed char _p67_range_lengths[] = {
	0, 0, 0
};

static const signed char _p67_index_offsets[] = {
	0, 0, 0
};

static const signed char _p67_cond_targs[] = {
	1, 1, 1, 1, 0, 0, 1, 0
};

static const signed char _p67_cond_actions[] = {
	1, 3, 5, 7, 0, 0, 0, 0
};

static const int p67_start = 1;
static const int p67_first_final = 1;
static const int p67_error = 0;

static const int p67_en_main = 1;


#line 14 "67.rl"

int main(void){int cs;const char *p,*pe,*eof;const char *d="ABCD";
	printf("Pattern 67: Include Machines\n\n");
	p=d;pe=p+strlen(d);eof=pe;

#line 50 "67.c"
	{
		cs = (int)p67_start;
	}
	
#line 18 "67.rl"


#line 55 "67.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p67_trans_keys + (_p67_key_offsets[cs]));
		_trans = (unsigned int)_p67_index_offsets[cs];
		
		_klen = (int)_p67_single_lengths[cs];
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
		
		_klen = (int)_p67_range_lengths[cs];
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
		cs = (int)_p67_cond_targs[_trans];
		
		if ( _p67_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p67_actions + (_p67_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 3 "67_lib.rl"
							printf(" lib1_a"); }
						
#line 130 "67.c"

						break; 
					}
					case 1:  {
							{
#line 4 "67_lib.rl"
							printf(" lib1_b"); }
						
#line 138 "67.c"

						break; 
					}
					case 2:  {
							{
#line 10 "67_lib.rl"
							printf(" lib2_c"); }
						
#line 146 "67.c"

						break; 
					}
					case 3:  {
							{
#line 11 "67_lib.rl"
							printf(" lib2_d"); }
						
#line 154 "67.c"

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
	
#line 19 "67.rl"

	printf(" -> %s ok\n",cs>=p67_first_final?"ACCEPT":"FAIL");
	assert(cs>=p67_first_final);
	printf("\nAll tests passed.\n");return 0;}
