#line 1 "75.rl"
/* Pattern 75: Printf Parser — %[flags][width][.precision][length]conversion */
#include <stdio.h>
#include <string.h>
#include <assert.h>


#line 7 "75.c"
static const signed char _p75_actions[] = {
	0, 1, 6, 2, 0, 6, 2, 1,
	6, 2, 2, 6, 2, 3, 6, 2,
	4, 6, 2, 5, 6, 3, 1, 2,
	6, 3, 5, 0, 6, 0
};

static const signed char _p75_key_offsets[] = {
	0, 1, 28, 49, 63, 0
};

static const char _p75_trans_keys[] = {
	37, 32, 35, 37, 43, 45, 46, 48,
	65, 76, 88, 97, 104, 106, 108, 116,
	120, 122, 49, 57, 69, 71, 99, 105,
	110, 112, 115, 117, 37, 65, 76, 88,
	97, 104, 106, 108, 116, 120, 122, 48,
	57, 69, 71, 99, 105, 110, 112, 115,
	117, 37, 65, 88, 97, 105, 115, 117,
	120, 69, 71, 99, 103, 110, 112, 37,
	46, 65, 76, 88, 97, 104, 106, 108,
	116, 120, 122, 48, 57, 69, 71, 99,
	105, 110, 112, 115, 117, 0
};

static const signed char _p75_single_lengths[] = {
	1, 17, 11, 8, 12, 0
};

static const signed char _p75_range_lengths[] = {
	0, 5, 5, 3, 5, 0
};

static const signed char _p75_index_offsets[] = {
	0, 2, 25, 42, 54, 0
};

static const signed char _p75_cond_targs[] = {
	1, 0, 1, 1, 1, 1, 1, 2,
	1, 0, 3, 0, 0, 3, 3, 3,
	3, 0, 3, 4, 0, 0, 0, 0,
	0, 1, 0, 3, 0, 0, 3, 3,
	3, 3, 0, 3, 2, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 2,
	0, 3, 0, 0, 3, 3, 3, 3,
	0, 3, 4, 0, 0, 0, 0, 0,
	0, 1, 2, 3, 4, 0
};

static const signed char _p75_cond_actions[] = {
	3, 1, 6, 6, 25, 6, 6, 1,
	21, 18, 15, 18, 18, 15, 15, 15,
	15, 18, 15, 9, 18, 18, 18, 18,
	1, 25, 18, 15, 18, 18, 15, 15,
	15, 15, 18, 15, 12, 18, 18, 18,
	18, 1, 25, 18, 18, 18, 18, 18,
	18, 18, 18, 18, 18, 1, 25, 1,
	18, 15, 18, 18, 15, 15, 15, 15,
	18, 15, 9, 18, 18, 18, 18, 1,
	0, 0, 0, 0, 0, 0
};

static const int p75_start = 0;
static const int p75_first_final = 0;
static const int p75_error = -1;

static const int p75_en_main = 0;


#line 26 "75.rl"

int main(void){int cs;const char *p,*pe,*eof;
	printf("Pattern 75: Printf Parser\n");
	p="hello %-20.10ld world %% end";pe=p+strlen(p);eof=pe;

#line 81 "75.c"
	{
		cs = (int)p75_start;
	}
	
#line 30 "75.rl"


#line 86 "75.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p75_trans_keys + (_p75_key_offsets[cs]));
		_trans = (unsigned int)_p75_index_offsets[cs];
		
		_klen = (int)_p75_single_lengths[cs];
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
		
		_klen = (int)_p75_range_lengths[cs];
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
		cs = (int)_p75_cond_targs[_trans];
		
		if ( _p75_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p75_actions + (_p75_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 8 "75.rl"
							printf("\n  FORMAT: "); }
						
#line 161 "75.c"

						break; 
					}
					case 1:  {
							{
#line 9 "75.rl"
							printf("flag=%c ", (( (*( p))))); }
						
#line 169 "75.c"

						break; 
					}
					case 2:  {
							{
#line 10 "75.rl"
							printf("w"); }
						
#line 177 "75.c"

						break; 
					}
					case 3:  {
							{
#line 11 "75.rl"
							printf("p"); }
						
#line 185 "75.c"

						break; 
					}
					case 4:  {
							{
#line 12 "75.rl"
							printf("len=%c ", (( (*( p))))); }
						
#line 193 "75.c"

						break; 
					}
					case 5:  {
							{
#line 13 "75.rl"
							printf("conv=%c ", (( (*( p))))); }
						
#line 201 "75.c"

						break; 
					}
					case 6:  {
							{
#line 14 "75.rl"
							printf("%c", (( (*( p))))); }
						
#line 209 "75.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
		}
		
		p += 1;
		goto _resume;
		_out: {}
	}
	
#line 31 "75.rl"

	printf("\n-> %s ok\n",cs>=p75_first_final?"ACCEPT":"FAIL");assert(cs>=p75_first_final);
	printf("\nAll tests passed.\n");return 0;}
