#line 1 "4.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "4.c"
static const signed char _p4_key_offsets[] = {
	0, 0, 1, 2, 3, 4, 0
};

static const char _p4_trans_keys[] = {
	47, 42, 42, 47, 0
};

static const signed char _p4_single_lengths[] = {
	0, 1, 1, 1, 1, 0, 0
};

static const signed char _p4_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0
};

static const signed char _p4_index_offsets[] = {
	0, 0, 2, 4, 6, 8, 0
};

static const signed char _p4_cond_targs[] = {
	2, 0, 3, 0, 4, 3, 5, 0,
	0, 0, 1, 2, 3, 4, 5, 0
};

static const int p4_start = 1;
static const int p4_first_final = 5;
static const int p4_error = 0;

static const int p4_en_main = 1;


#line 4 "4.rl"

int main(void){static const char *t[]={"/* comment */","/**/","/* x y */","/* unmatched","/ /","abc","/* a * b */"};static const int e[]={1,1,1,0,0,0,0};int n=7,cs;const char *p,*pe,*eof;
	printf("Pattern  4: Negated (C comments)\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 41 "4.c"
	{
			cs = (int)p4_start;
		}
		
#line 8 "4.rl"


#line 46 "4.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p4_trans_keys + (_p4_key_offsets[cs]));
			_trans = (unsigned int)_p4_index_offsets[cs];
			
			_klen = (int)_p4_single_lengths[cs];
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
			
			_klen = (int)_p4_range_lengths[cs];
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
			cs = (int)_p4_cond_targs[_trans];
			
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
			_out: {}
		}
		
#line 9 "4.rl"

		int r=cs>=p4_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
