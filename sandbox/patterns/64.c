#line 1 "64.rl"
/* Pattern 64: write Statement Family — data, init, exec, start, first_final, error, exports */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "64.c"
static const signed char _p64_key_offsets[] = {
	0, 0, 1, 2, 3, 4, 5, 6,
	0
};

static const char _p64_trans_keys[] = {
	104, 101, 108, 108, 111, 10, 0
};

static const signed char _p64_single_lengths[] = {
	0, 1, 1, 1, 1, 1, 1, 0,
	0
};

static const signed char _p64_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0
};

static const signed char _p64_index_offsets[] = {
	0, 0, 2, 4, 6, 8, 10, 12,
	0
};

static const signed char _p64_cond_targs[] = {
	2, 0, 3, 0, 4, 0, 5, 0,
	6, 0, 7, 0, 0, 0, 1, 2,
	3, 4, 5, 6, 7, 0
};

static const int p64_start = 1;
static const int p64_first_final = 7;
static const int p64_error = 0;

static const int p64_en_main = 1;


#line 9 "64.rl"



#line 45 "64.c"
#define p64_ex_f64_final 88


#line 11 "64.rl"

/* No separate data — already inside machine block */

int main(void){int cs;const char *p,*pe,*eof;p="hello\n";pe=p+strlen(p);eof=pe;
	printf("Pattern 64: write Statement Family\n\n");
	printf("  Start state = %d\n", p64_start);
	printf("  First final = %d\n", p64_first_final);
	printf("  Error state = %d\n", p64_error);
	printf("  Exported X  = %d\n", p64_ex_f64_final);

#line 57 "64.c"
	{
		cs = (int)p64_start;
	}
	
#line 20 "64.rl"


#line 62 "64.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p64_trans_keys + (_p64_key_offsets[cs]));
		_trans = (unsigned int)_p64_index_offsets[cs];
		
		_klen = (int)_p64_single_lengths[cs];
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
		
		_klen = (int)_p64_range_lengths[cs];
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
		cs = (int)_p64_cond_targs[_trans];
		
		
		if ( cs != 0 ) {
			p += 1;
			goto _resume;
		}
		_out: {}
	}
	
#line 21 "64.rl"

	printf("  Result      -> %s\n",cs>=p64_first_final?"ACCEPT":"FAIL");
	assert(cs>=p64_first_final);
	printf("\nAll tests passed.\n");return 0;}
