#line 1 "12.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "12.c"
static const signed char _p12_key_offsets[] = {
	0, 0, 3, 4, 5, 6, 7, 8,
	9, 10, 11, 12, 13, 14, 15, 0
};

static const char _p12_trans_keys[] = {
	72, 104, 119, 101, 108, 108, 10, 101,
	108, 108, 111, 111, 114, 108, 100, 0
};

static const signed char _p12_single_lengths[] = {
	0, 3, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 0, 0
};

static const signed char _p12_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

static const signed char _p12_index_offsets[] = {
	0, 0, 4, 6, 8, 10, 12, 14,
	16, 18, 20, 22, 24, 26, 28, 0
};

static const signed char _p12_cond_targs[] = {
	2, 6, 10, 0, 3, 0, 4, 0,
	5, 0, 14, 0, 7, 0, 8, 0,
	9, 0, 5, 0, 11, 0, 12, 0,
	13, 0, 5, 0, 0, 0, 1, 2,
	3, 4, 5, 6, 7, 8, 9, 10,
	11, 12, 13, 14, 0
};

static const int p12_start = 1;
static const int p12_first_final = 14;
static const int p12_error = 0;

static const int p12_en_main = 1;


#line 4 "12.rl"

int main(void){static const char *t[]={"hello\n","world\n","Hell\n","foo\n","HELL\n"};static const int e[]={1,1,1,0,0};int n=5,cs;const char *p,*pe,*eof;
	printf("Pattern 12: String Literals\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 50 "12.c"
	{
			cs = (int)p12_start;
		}
		
#line 8 "12.rl"


#line 55 "12.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p12_trans_keys + (_p12_key_offsets[cs]));
			_trans = (unsigned int)_p12_index_offsets[cs];
			
			_klen = (int)_p12_single_lengths[cs];
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
			
			_klen = (int)_p12_range_lengths[cs];
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
			cs = (int)_p12_cond_targs[_trans];
			
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
			_out: {}
		}
		
#line 9 "12.rl"

		int r=cs>=p12_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
