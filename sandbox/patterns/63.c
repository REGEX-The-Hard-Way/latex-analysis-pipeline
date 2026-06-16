#line 1 "63.rl"
/* Pattern 63: variable — rename p, pe, eof to custom names */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "63.c"
static const signed char _p63_key_offsets[] = {
	0, 0, 1, 2, 3, 4, 5, 6,
	0
};

static const char _p63_trans_keys[] = {
	104, 101, 108, 108, 111, 10, 0
};

static const signed char _p63_single_lengths[] = {
	0, 1, 1, 1, 1, 1, 1, 0,
	0
};

static const signed char _p63_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0
};

static const signed char _p63_index_offsets[] = {
	0, 0, 2, 4, 6, 8, 10, 12,
	0
};

static const signed char _p63_cond_targs[] = {
	2, 0, 3, 0, 4, 0, 5, 0,
	6, 0, 7, 0, 0, 0, 1, 2,
	3, 4, 5, 6, 7, 0
};

static const int p63_start = 1;
static const int p63_first_final = 7;
static const int p63_error = 0;

static const int p63_en_main = 1;


#line 13 "63.rl"

int main(void){int cs;const char *data_p,*data_pe,*data_eof;
	const char *test[]={"hello\n","world\n"};int expect[]={1,0};int lens[]={6,6};int n=2;
	printf("Pattern 63: variable p/pe/eof Renaming\n\n");
	for(int i=0;i<n;i++){data_p=test[i];data_pe=data_p+lens[i];data_eof=data_pe;

#line 48 "63.c"
	{
			cs = (int)p63_start;
		}
		
#line 18 "63.rl"


#line 53 "63.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			_resume: {}
			if ( ( data_p) == ( data_pe) )
				goto _out;
			_keys = ( _p63_trans_keys + (_p63_key_offsets[cs]));
			_trans = (unsigned int)_p63_index_offsets[cs];
			
			_klen = (int)_p63_single_lengths[cs];
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
					if ( ( (*( ( data_p)))) < (*( _mid)) )
						_upper = _mid - 1;
					else if ( ( (*( ( data_p)))) > (*( _mid)) )
						_lower = _mid + 1;
					else {
						_trans += (unsigned int)(_mid - _keys);
						goto _match;
					}
				}
			}
			
			_klen = (int)_p63_range_lengths[cs];
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
					if ( ( (*( ( data_p)))) < (*( _mid)) )
						_upper = _mid - 2;
					else if ( ( (*( ( data_p)))) > (*( _mid + 1)) )
						_lower = _mid + 2;
					else {
						_trans += (unsigned int)((_mid - _keys)>>1);
						break;
					}
				}
			}
			
			_match: {}
			cs = (int)_p63_cond_targs[_trans];
			
			
			if ( cs != 0 ) {
				( data_p) += 1;
				goto _resume;
			}
			_out: {}
		}
		
#line 19 "63.rl"

		printf("  %-10s -> %s ok\n",test[i],cs>=p63_first_final?"ACCEPT":"FAIL");assert((cs>=p63_first_final)==expect[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
