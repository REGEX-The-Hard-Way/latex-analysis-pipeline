#line 1 "11.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "11.c"
static const signed char _p11_key_offsets[] = {
	0, 0, 4, 9, 0
};

static const char _p11_trans_keys[] = {
	65, 90, 97, 122, 10, 65, 90, 97,
	122, 0
};

static const signed char _p11_single_lengths[] = {
	0, 0, 1, 0, 0
};

static const signed char _p11_range_lengths[] = {
	0, 2, 2, 0, 0
};

static const signed char _p11_index_offsets[] = {
	0, 0, 3, 7, 0
};

static const signed char _p11_cond_targs[] = {
	2, 2, 0, 3, 2, 2, 0, 0,
	0, 1, 2, 3, 0
};

static const int p11_start = 1;
static const int p11_first_final = 3;
static const int p11_error = 0;

static const int p11_en_main = 1;


#line 4 "11.rl"

int main(void){static const char *t[]={"hello\n","HELLO\n","HeLLo\n","123\n","\n"};static const int e[]={1,1,1,0,0};int n=5,cs;const char *p,*pe,*eof;
	printf("Pattern 11: Case-Insensitive\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 42 "11.c"
	{
			cs = (int)p11_start;
		}
		
#line 8 "11.rl"


#line 47 "11.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p11_trans_keys + (_p11_key_offsets[cs]));
			_trans = (unsigned int)_p11_index_offsets[cs];
			
			_klen = (int)_p11_single_lengths[cs];
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
			
			_klen = (int)_p11_range_lengths[cs];
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
			cs = (int)_p11_cond_targs[_trans];
			
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
			_out: {}
		}
		
#line 9 "11.rl"

		int r=cs>=p11_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
