#line 1 "14.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "14.c"
static const signed char _p14_key_offsets[] = {
	0, 0, 2, 4, 5, 7, 9, 11,
	12, 14, 16, 18, 20, 21, 0
};

static const char _p14_trans_keys[] = {
	48, 51, 48, 57, 32, 65, 90, 97,
	122, 97, 122, 32, 48, 57, 48, 57,
	48, 57, 48, 57, 10, 0
};

static const signed char _p14_single_lengths[] = {
	0, 0, 0, 1, 0, 0, 0, 1,
	0, 0, 0, 0, 1, 0, 0
};

static const signed char _p14_range_lengths[] = {
	0, 1, 1, 0, 1, 1, 1, 0,
	1, 1, 1, 1, 0, 0, 0
};

static const signed char _p14_index_offsets[] = {
	0, 0, 2, 4, 6, 8, 10, 12,
	14, 16, 18, 20, 22, 24, 0
};

static const signed char _p14_cond_targs[] = {
	2, 0, 3, 0, 4, 0, 5, 0,
	6, 0, 7, 0, 8, 0, 9, 0,
	10, 0, 11, 0, 12, 0, 13, 0,
	0, 0, 1, 2, 3, 4, 5, 6,
	7, 8, 9, 10, 11, 12, 13, 0
};

static const int p14_start = 1;
static const int p14_first_final = 13;
static const int p14_error = 0;

static const int p14_en_main = 1;


#line 4 "14.rl"

int main(void){static const char *t[]={"12 Jan 2023\n","31 Dec 1999\n","00 AAA 0000\n","12Jan2023\n"};static const int e[]={1,1,0,0};int n=4,cs;const char *p,*pe,*eof;
	printf("Pattern 14: Concatenation\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 50 "14.c"
	{
			cs = (int)p14_start;
		}
		
#line 8 "14.rl"


#line 55 "14.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p14_trans_keys + (_p14_key_offsets[cs]));
			_trans = (unsigned int)_p14_index_offsets[cs];
			
			_klen = (int)_p14_single_lengths[cs];
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
			
			_klen = (int)_p14_range_lengths[cs];
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
			cs = (int)_p14_cond_targs[_trans];
			
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
			_out: {}
		}
		
#line 9 "14.rl"

		int r=cs>=p14_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
