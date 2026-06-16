#line 1 "61.rl"
/* Pattern 61: alphtype unsigned char — match high bytes like 0xe8 */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "61.c"
static const signed char _p61_key_offsets[] = {
	0, 0, 1, 2, 0
};

static const unsigned char _p61_trans_keys[] = {
	232u, 10u, 0u
};

static const signed char _p61_single_lengths[] = {
	0, 1, 1, 0, 0
};

static const signed char _p61_range_lengths[] = {
	0, 0, 0, 0, 0
};

static const signed char _p61_index_offsets[] = {
	0, 0, 2, 4, 0
};

static const signed char _p61_cond_targs[] = {
	2, 0, 3, 0, 0, 0, 1, 2,
	3, 0
};

static const int p61_start = 1;
static const int p61_first_final = 3;
static const int p61_error = 0;

static const int p61_en_main = 1;


#line 5 "61.rl"

int main(void){int cs;unsigned char *p,*pe;int expect[]={1,0,0};int n=3;
	unsigned char d1[]={0xe8,10};unsigned char d2[]={0xf8,10};unsigned char d3[]={0x48,10};
	unsigned char *data[]={d1,d2,d3};
	int lens[]={2,2,2};
	printf("Pattern 61: alphtype unsigned char High Bytes\n\n");
	for(int i=0;i<n;i++){p=data[i];pe=p+lens[i];

#line 45 "61.c"
	{
			cs = (int)p61_start;
		}
		
#line 12 "61.rl"


#line 50 "61.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const unsigned char * _keys;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p61_trans_keys + (_p61_key_offsets[cs]));
			_trans = (unsigned int)_p61_index_offsets[cs];
			
			_klen = (int)_p61_single_lengths[cs];
			if ( _klen > 0 ) {
				const unsigned char *_lower = _keys;
				const unsigned char *_upper = _keys + _klen - 1;
				const unsigned char *_mid;
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
			
			_klen = (int)_p61_range_lengths[cs];
			if ( _klen > 0 ) {
				const unsigned char *_lower = _keys;
				const unsigned char *_upper = _keys + (_klen<<1) - 2;
				const unsigned char *_mid;
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
			cs = (int)_p61_cond_targs[_trans];
			
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
			_out: {}
		}
		
#line 13 "61.rl"

		printf("  %s -> %s ok\n",i==0?"0xe8\\n":i==1?"0xf8\\n":"0x48\\n",cs>=p61_first_final?"ACCEPT":"FAIL");assert((cs>=p61_first_final)==expect[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
