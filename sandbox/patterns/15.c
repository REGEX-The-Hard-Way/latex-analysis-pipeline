#line 1 "15.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "15.c"
static const signed char _p15_key_offsets[] = {
	0, 0, 9, 17, 26, 35, 44, 51,
	60, 69, 78, 87, 96, 105, 0
};

static const char _p15_trans_keys[] = {
	95, 101, 102, 105, 119, 65, 90, 97,
	122, 10, 95, 48, 57, 65, 90, 97,
	122, 10, 95, 108, 48, 57, 65, 90,
	97, 122, 10, 95, 115, 48, 57, 65,
	90, 97, 122, 10, 95, 101, 48, 57,
	65, 90, 97, 122, 95, 48, 57, 65,
	90, 97, 122, 10, 95, 111, 48, 57,
	65, 90, 97, 122, 10, 95, 114, 48,
	57, 65, 90, 97, 122, 10, 95, 102,
	48, 57, 65, 90, 97, 122, 10, 95,
	104, 48, 57, 65, 90, 97, 122, 10,
	95, 105, 48, 57, 65, 90, 97, 122,
	10, 95, 108, 48, 57, 65, 90, 97,
	122, 0
};

static const signed char _p15_single_lengths[] = {
	0, 5, 2, 3, 3, 3, 1, 3,
	3, 3, 3, 3, 3, 0, 0
};

static const signed char _p15_range_lengths[] = {
	0, 2, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 0, 0
};

static const signed char _p15_index_offsets[] = {
	0, 0, 8, 14, 21, 28, 35, 40,
	47, 54, 61, 68, 75, 82, 0
};

static const signed char _p15_cond_targs[] = {
	2, 3, 7, 9, 10, 2, 2, 0,
	13, 2, 2, 2, 2, 0, 13, 2,
	4, 2, 2, 2, 0, 13, 2, 5,
	2, 2, 2, 0, 13, 2, 6, 2,
	2, 2, 0, 2, 2, 2, 2, 0,
	13, 2, 8, 2, 2, 2, 0, 13,
	2, 6, 2, 2, 2, 0, 13, 2,
	6, 2, 2, 2, 0, 13, 2, 11,
	2, 2, 2, 0, 13, 2, 12, 2,
	2, 2, 0, 13, 2, 5, 2, 2,
	2, 0, 0, 0, 1, 2, 3, 4,
	5, 6, 7, 8, 9, 10, 11, 12,
	13, 0
};

static const int p15_start = 1;
static const int p15_first_final = 13;
static const int p15_error = 0;

static const int p15_en_main = 1;


#line 9 "15.rl"

int main(void){static const char *t[]={"abc\n","if\n","else\n","for\n","myVar\n","while\n"};static const int e[]={1,0,0,0,1,0};int n=6,cs;const char *p,*pe,*eof;
	printf("Pattern 15: Intersection &\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 69 "15.c"
	{
			cs = (int)p15_start;
		}
		
#line 13 "15.rl"


#line 74 "15.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p15_trans_keys + (_p15_key_offsets[cs]));
			_trans = (unsigned int)_p15_index_offsets[cs];
			
			_klen = (int)_p15_single_lengths[cs];
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
			
			_klen = (int)_p15_range_lengths[cs];
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
			cs = (int)_p15_cond_targs[_trans];
			
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
			_out: {}
		}
		
#line 14 "15.rl"

		int r=cs>=p15_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
