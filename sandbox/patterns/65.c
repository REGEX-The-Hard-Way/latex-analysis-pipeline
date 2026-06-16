#line 1 "65.rl"
/* Pattern 65: Export Constants — demo write exports and constant usage */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "65.c"
static const signed char _p65_actions[] = {
	0, 1, 2, 2, 0, 2, 2, 1,
	2, 0
};

static const signed char _p65_key_offsets[] = {
	0, 0
};

static const char _p65_trans_keys[] = {
	99, 122, 0
};

static const signed char _p65_single_lengths[] = {
	2, 0
};

static const signed char _p65_range_lengths[] = {
	0, 0
};

static const signed char _p65_index_offsets[] = {
	0, 0
};

static const signed char _p65_cond_targs[] = {
	0, 0, 0, 0, 0
};

static const signed char _p65_cond_actions[] = {
	3, 6, 1, 0, 0
};

static const int p65_start = 0;
static const int p65_first_final = 0;
static const int p65_error = -1;

static const int p65_en_main = 0;


#line 20 "65.rl"



#line 48 "65.c"
#define p65_ex_c1 99
#define p65_ex_c2 122


#line 22 "65.rl"


int main(void){int cs;const char *p,*pe,*eof;int expect[]={1,1};int n=2;
	char d1[]={'c','x','z','y'};char d2[]={'a','b'};
	char *data[]={d1,d2};int lens[]={4,2};
	printf("Pattern 65: Export Constants\n\n");
	for(int i=0;i<n;i++){p=data[i];pe=p+lens[i];eof=pe;

#line 59 "65.c"
	{
			cs = (int)p65_start;
		}
		
#line 29 "65.rl"


#line 64 "65.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			const signed char * _acts;
			unsigned int _nacts;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p65_trans_keys + (_p65_key_offsets[cs]));
			_trans = (unsigned int)_p65_index_offsets[cs];
			
			_klen = (int)_p65_single_lengths[cs];
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
			
			_klen = (int)_p65_range_lengths[cs];
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
			cs = (int)_p65_cond_targs[_trans];
			
			if ( _p65_cond_actions[_trans] != 0 ) {
				
				_acts = ( _p65_actions + (_p65_cond_actions[_trans]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 ) {
					switch ( (*( _acts)) )
					{
						case 0:  {
								{
#line 9 "65.rl"
								printf("got c1\n"); }
							
#line 139 "65.c"

							break; 
						}
						case 1:  {
								{
#line 10 "65.rl"
								printf("got c2\n"); }
							
#line 147 "65.c"

							break; 
						}
						case 2:  {
								{
#line 11 "65.rl"
								printf("got other\n"); }
							
#line 155 "65.c"

							break; 
						}
					}
					_nacts -= 1;
					_acts += 1;
				}
				
			}
			
			p += 1;
			goto _resume;
			_out: {}
		}
		
#line 30 "65.rl"

		printf("  %s -> %s ok\n",i==0?"cxyz":"ab",cs>=p65_first_final?"ACCEPT":"FAIL");assert((cs>=p65_first_final)==expect[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
