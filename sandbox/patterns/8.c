#line 1 "8.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "8.c"
static const signed char _p8_key_offsets[] = {
	0, 0, 8, 10, 13, 18, 0
};

static const char _p8_trans_keys[] = {
	10, 32, 48, 57, 65, 90, 97, 122,
	10, 32, 10, 48, 57, 10, 65, 90,
	97, 122, 0
};

static const signed char _p8_single_lengths[] = {
	0, 2, 2, 1, 1, 0, 0
};

static const signed char _p8_range_lengths[] = {
	0, 3, 0, 1, 2, 0, 0
};

static const signed char _p8_index_offsets[] = {
	0, 0, 6, 9, 12, 16, 0
};

static const signed char _p8_cond_targs[] = {
	5, 2, 3, 4, 4, 0, 5, 2,
	0, 5, 3, 0, 5, 4, 4, 0,
	0, 0, 1, 2, 3, 4, 5, 0
};

static const int p8_start = 1;
static const int p8_first_final = 5;
static const int p8_error = 0;

static const int p8_en_main = 1;


#line 4 "8.rl"

int main(void){static const char *t[]={"123\n","\n","abc\n","   \n","1a\n",""};static const int e[]={1,1,1,1,0,0};int n=6,cs;const char *p,*pe,*eof;
	printf("Pattern  8: Kleene Star/Plus\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 44 "8.c"
	{
			cs = (int)p8_start;
		}
		
#line 8 "8.rl"


#line 49 "8.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p8_trans_keys + (_p8_key_offsets[cs]));
			_trans = (unsigned int)_p8_index_offsets[cs];
			
			_klen = (int)_p8_single_lengths[cs];
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
			
			_klen = (int)_p8_range_lengths[cs];
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
			cs = (int)_p8_cond_targs[_trans];
			
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
			_out: {}
		}
		
#line 9 "8.rl"

		int r=cs>=p8_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
