#line 1 "17.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "17.c"
static const signed char _p17_key_offsets[] = {
	0, 0, 1, 3, 4, 6, 7, 8,
	0
};

static const char _p17_trans_keys[] = {
	47, 42, 47, 42, 42, 47, 10, 10,
	0
};

static const signed char _p17_single_lengths[] = {
	0, 1, 2, 1, 2, 1, 1, 0,
	0
};

static const signed char _p17_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0
};

static const signed char _p17_index_offsets[] = {
	0, 0, 2, 5, 7, 10, 12, 14,
	0
};

static const signed char _p17_cond_targs[] = {
	2, 0, 3, 6, 0, 4, 3, 4,
	5, 3, 7, 0, 7, 6, 0, 0,
	1, 2, 3, 4, 5, 6, 7, 0
};

static const int p17_start = 1;
static const int p17_first_final = 7;
static const int p17_error = 0;

static const int p17_en_main = 1;


#line 4 "17.rl"

int main(void){static const char *t[]={"/*comment*/\n","//comment\n","/*x*/ abc\n","/* no end\n","//"};static const int e[]={1,1,0,0,0};int n=5,cs;const char *p,*pe,*eof;
	printf("Pattern 17: Commit Operators (:>>)\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 47 "17.c"
	{
			cs = (int)p17_start;
		}
		
#line 8 "17.rl"


#line 52 "17.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p17_trans_keys + (_p17_key_offsets[cs]));
			_trans = (unsigned int)_p17_index_offsets[cs];
			
			_klen = (int)_p17_single_lengths[cs];
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
			
			_klen = (int)_p17_range_lengths[cs];
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
			cs = (int)_p17_cond_targs[_trans];
			
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
			_out: {}
		}
		
#line 9 "17.rl"

		int r=cs>=p17_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
