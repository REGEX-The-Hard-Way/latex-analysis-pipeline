#line 1 "3.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "3.c"
static const signed char _p3_key_offsets[] = {
	0, 0, 5, 13, 0
};

static const char _p3_trans_keys[] = {
	95, 65, 90, 97, 122, 10, 95, 48,
	57, 65, 90, 97, 122, 0
};

static const signed char _p3_single_lengths[] = {
	0, 1, 2, 0, 0
};

static const signed char _p3_range_lengths[] = {
	0, 2, 3, 0, 0
};

static const signed char _p3_index_offsets[] = {
	0, 0, 4, 10, 0
};

static const signed char _p3_cond_targs[] = {
	2, 2, 2, 0, 3, 2, 2, 2,
	2, 0, 0, 0, 1, 2, 3, 0
};

static const int p3_start = 1;
static const int p3_first_final = 3;
static const int p3_error = 0;

static const int p3_en_main = 1;


#line 4 "3.rl"

int main(void){static const char *t[]={"abc\n","_foo\n","X123\n","_123\n","123\n","\n"};static const int e[]={1,1,1,1,0,0};int n=6,cs;const char *p,*pe,*eof;
	printf("Pattern  3: Char Class\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 42 "3.c"
	{
			cs = (int)p3_start;
		}
		
#line 8 "3.rl"


#line 47 "3.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p3_trans_keys + (_p3_key_offsets[cs]));
			_trans = (unsigned int)_p3_index_offsets[cs];
			
			_klen = (int)_p3_single_lengths[cs];
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
			
			_klen = (int)_p3_range_lengths[cs];
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
			cs = (int)_p3_cond_targs[_trans];
			
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
			_out: {}
		}
		
#line 9 "3.rl"

		int r=cs>=p3_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
