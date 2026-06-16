#line 1 "16.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "16.c"
static const signed char _p16_key_offsets[] = {
	0, 0, 3, 7, 10, 14, 16, 0
};

static const char _p16_trans_keys[] = {
	97, 98, 122, 10, 98, 97, 122, 10,
	97, 122, 10, 99, 97, 122, 97, 122,
	0
};

static const signed char _p16_single_lengths[] = {
	0, 1, 2, 1, 2, 0, 0, 0
};

static const signed char _p16_range_lengths[] = {
	0, 1, 1, 1, 1, 1, 0, 0
};

static const signed char _p16_index_offsets[] = {
	0, 0, 3, 7, 10, 14, 16, 0
};

static const signed char _p16_cond_targs[] = {
	2, 3, 0, 6, 4, 3, 0, 6,
	3, 0, 6, 5, 3, 0, 3, 0,
	0, 0, 1, 2, 3, 4, 5, 6,
	0
};

static const int p16_start = 1;
static const int p16_first_final = 6;
static const int p16_error = 0;

static const int p16_en_main = 1;


#line 4 "16.rl"

int main(void){static const char *t[]={"abc\n","xyz\n","abcd\n","xbc\n","hello\n"};static const int e[]={0,1,1,1,1};int n=5,cs;const char *p,*pe,*eof;
	printf("Pattern 16: Subtraction (- and --)\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 45 "16.c"
	{
			cs = (int)p16_start;
		}
		
#line 8 "16.rl"


#line 50 "16.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p16_trans_keys + (_p16_key_offsets[cs]));
			_trans = (unsigned int)_p16_index_offsets[cs];
			
			_klen = (int)_p16_single_lengths[cs];
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
			
			_klen = (int)_p16_range_lengths[cs];
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
			cs = (int)_p16_cond_targs[_trans];
			
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
			_out: {}
		}
		
#line 9 "16.rl"

		int r=cs>=p16_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
