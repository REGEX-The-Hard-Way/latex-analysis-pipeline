#line 1 "5.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "5.c"
static const signed char _p5_key_offsets[] = {
	0, 1, 0
};

static const char _p5_trans_keys[] = {
	10, 10, 0
};

static const signed char _p5_single_lengths[] = {
	1, 1, 0
};

static const signed char _p5_range_lengths[] = {
	0, 0, 0
};

static const signed char _p5_index_offsets[] = {
	0, 2, 0
};

static const signed char _p5_cond_targs[] = {
	1, 0, 1, 0, 0, 1, 0
};

static const int p5_start = 0;
static const int p5_first_final = 1;
static const int p5_error = -1;

static const int p5_en_main = 0;


#line 4 "5.rl"

int main(void){static const char *t[]={"A\n","hello world\n","\n","",".\n"};static const int e[]={1,1,1,0,1};int n=5,cs;const char *p,*pe,*eof;
	printf("Pattern  5: Dot/Any\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 40 "5.c"
	{
			cs = (int)p5_start;
		}
		
#line 8 "5.rl"


#line 45 "5.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p5_trans_keys + (_p5_key_offsets[cs]));
			_trans = (unsigned int)_p5_index_offsets[cs];
			
			_klen = (int)_p5_single_lengths[cs];
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
			
			_klen = (int)_p5_range_lengths[cs];
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
			cs = (int)_p5_cond_targs[_trans];
			
			
			p += 1;
			goto _resume;
			_out: {}
		}
		
#line 9 "5.rl"

		int r=cs>=p5_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
