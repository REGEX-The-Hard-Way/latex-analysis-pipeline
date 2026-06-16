#line 1 "13.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "13.c"
static const signed char _p13_key_offsets[] = {
	0, 0, 2, 4, 6, 7, 9, 11,
	12, 14, 16, 17, 0
};

static const char _p13_trans_keys[] = {
	65, 90, 97, 122, 97, 122, 32, 48,
	57, 48, 57, 58, 48, 57, 48, 57,
	10, 0
};

static const signed char _p13_single_lengths[] = {
	0, 0, 0, 0, 1, 0, 0, 1,
	0, 0, 1, 0, 0
};

static const signed char _p13_range_lengths[] = {
	0, 1, 1, 1, 0, 1, 1, 0,
	1, 1, 0, 0, 0
};

static const signed char _p13_index_offsets[] = {
	0, 0, 2, 4, 6, 8, 10, 12,
	14, 16, 18, 20, 0
};

static const signed char _p13_cond_targs[] = {
	2, 0, 3, 0, 4, 0, 5, 0,
	6, 0, 7, 0, 8, 0, 9, 0,
	10, 0, 11, 0, 0, 0, 1, 2,
	3, 4, 5, 6, 7, 8, 9, 10,
	11, 0
};

static const int p13_start = 1;
static const int p13_first_final = 11;
static const int p13_error = 0;

static const int p13_en_main = 1;


#line 4 "13.rl"

int main(void){static const char *t[]={"Mon 12:30\n","Cat 99:99\n","MON 12:30\n","Mon12:30\n"};static const int e[]={1,1,0,0};int n=4,cs;const char *p,*pe,*eof;
	printf("Pattern 13: Regex Literals\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 50 "13.c"
	{
			cs = (int)p13_start;
		}
		
#line 8 "13.rl"


#line 55 "13.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p13_trans_keys + (_p13_key_offsets[cs]));
			_trans = (unsigned int)_p13_index_offsets[cs];
			
			_klen = (int)_p13_single_lengths[cs];
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
			
			_klen = (int)_p13_range_lengths[cs];
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
			cs = (int)_p13_cond_targs[_trans];
			
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
			_out: {}
		}
		
#line 9 "13.rl"

		int r=cs>=p13_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
