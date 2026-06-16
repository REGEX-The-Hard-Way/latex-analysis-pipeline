#line 1 "10.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "10.c"
static const signed char _p10_key_offsets[] = {
	0, 1, 3, 5, 7, 0
};

static const char _p10_trans_keys[] = {
	69, 69, 78, 68, 69, 10, 69, 69,
	0
};

static const signed char _p10_single_lengths[] = {
	1, 2, 2, 2, 1, 0
};

static const signed char _p10_range_lengths[] = {
	0, 0, 0, 0, 0, 0
};

static const signed char _p10_index_offsets[] = {
	0, 2, 5, 8, 11, 0
};

static const signed char _p10_cond_targs[] = {
	1, 0, 1, 2, 0, 3, 1, 0,
	4, 1, 0, 1, 0, 0, 1, 2,
	3, 4, 0
};

static const int p10_start = 0;
static const int p10_first_final = 4;
static const int p10_error = -1;

static const int p10_en_main = 0;


#line 4 "10.rl"

int main(void){static const char *t[]={"END\n","abcEND\n","abcEND\n","\n"};static const int e[]={1,1,1,0};int n=4,cs;const char *p,*pe,*eof;
	printf("Pattern 10: Greedy Star\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 43 "10.c"
	{
			cs = (int)p10_start;
		}
		
#line 8 "10.rl"


#line 48 "10.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p10_trans_keys + (_p10_key_offsets[cs]));
			_trans = (unsigned int)_p10_index_offsets[cs];
			
			_klen = (int)_p10_single_lengths[cs];
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
			
			_klen = (int)_p10_range_lengths[cs];
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
			cs = (int)_p10_cond_targs[_trans];
			
			
			p += 1;
			goto _resume;
			_out: {}
		}
		
#line 9 "10.rl"

		int r=cs>=p10_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
