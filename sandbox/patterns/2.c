#line 1 "2.rl"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#line 5 "2.c"
static const signed char _p2_key_offsets[] = {
	0, 0, 4, 5, 6, 7, 8, 9,
	10, 11, 12, 13, 14, 15, 16, 0
};

static const char _p2_trans_keys[] = {
	98, 102, 104, 119, 97, 114, 10, 111,
	111, 101, 108, 108, 111, 114, 108, 100,
	0
};

static const signed char _p2_single_lengths[] = {
	0, 4, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 0, 0
};

static const signed char _p2_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

static const signed char _p2_index_offsets[] = {
	0, 0, 5, 7, 9, 11, 13, 15,
	17, 19, 21, 23, 25, 27, 29, 0
};

static const signed char _p2_cond_targs[] = {
	2, 5, 7, 10, 0, 3, 0, 4,
	0, 14, 0, 6, 0, 4, 0, 8,
	0, 9, 0, 6, 0, 11, 0, 12,
	0, 13, 0, 4, 0, 0, 0, 1,
	2, 3, 4, 5, 6, 7, 8, 9,
	10, 11, 12, 13, 14, 0
};

static const int p2_start = 1;
static const int p2_first_final = 14;
static const int p2_error = 0;

static const int p2_en_main = 1;


#line 8 "2.rl"

int main(void) {
	static const char *t[] = {"hello\n", "world\n", "foo\n", "bar\n",
		"baz\n",   "hello",   ""};
	static const int e[] = {1, 1, 1, 1, 0, 0, 0};
	int n = 7, cs;
	const char *p, *pe, *eof;
	printf("Pattern  2: Alternation\n\n");
	for (int i = 0; i < n; i++) {
		p = t[i];
		pe = p + strlen(p);
		eof = pe;

#line 59 "2.c"
	{
			cs = (int)p2_start;
		}
		
#line 20 "2.rl"


#line 64 "2.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p2_trans_keys + (_p2_key_offsets[cs]));
			_trans = (unsigned int)_p2_index_offsets[cs];
			
			_klen = (int)_p2_single_lengths[cs];
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
			
			_klen = (int)_p2_range_lengths[cs];
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
			cs = (int)_p2_cond_targs[_trans];
			
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
			_out: {}
		}
		
#line 21 "2.rl"

		int r = cs >= p2_first_final;
		printf("  Input %d -> %s ok\n", i, r ? "ACCEPT" : "FAIL");
		assert(r == e[i]);
	}
	printf("\nAll %d tests passed.\n", n);
	return 0;
}
