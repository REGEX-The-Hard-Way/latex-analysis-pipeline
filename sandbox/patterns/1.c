#line 1 "1.rl"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#line 5 "1.c"
static const signed char _p1_key_offsets[] = {
	0, 0, 1, 2, 3, 4, 5, 6,
	0
};

static const char _p1_trans_keys[] = {
	104, 101, 108, 108, 111, 10, 0
};

static const signed char _p1_single_lengths[] = {
	0, 1, 1, 1, 1, 1, 1, 0,
	0
};

static const signed char _p1_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0
};

static const signed char _p1_index_offsets[] = {
	0, 0, 2, 4, 6, 8, 10, 12,
	0
};

static const signed char _p1_cond_targs[] = {
	2, 0, 3, 0, 4, 0, 5, 0,
	6, 0, 7, 0, 0, 0, 1, 2,
	3, 4, 5, 6, 7, 0
};

static const int p1_start = 1;
static const int p1_first_final = 7;
static const int p1_error = 0;

static const int p1_en_main = 1;


#line 8 "1.rl"
 
int main(void) {
	static const char *t[] = {"hello\n", "hello"};
	static const int e[] = {1, 0};
	int n = 2, cs;
	const char *p, *pe, *eof;
	printf("Pattern  1: Basic Literal\n\n");
	for (int i = 0; i < n; i++) {
		p = t[i];
		pe = p + strlen(p);
		eof = pe;

#line 53 "1.c"
	{
			cs = (int)p1_start;
		}
		
#line 19 "1.rl"


#line 58 "1.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p1_trans_keys + (_p1_key_offsets[cs]));
			_trans = (unsigned int)_p1_index_offsets[cs];
			
			_klen = (int)_p1_single_lengths[cs];
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
			
			_klen = (int)_p1_range_lengths[cs];
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
			cs = (int)_p1_cond_targs[_trans];
			
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
			_out: {}
		}
		
#line 20 "1.rl"

		int r = cs >= p1_first_final;
		printf("  Input %d -> %s ok\n", i, r ? "ACCEPT" : "FAIL");
		assert(r == e[i]);
	}
	printf("\nAll %d tests passed.\n", n);
	return 0;
}
