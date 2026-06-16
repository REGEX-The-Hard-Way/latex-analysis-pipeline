#line 1 "74.rl"
/* Pattern 74: Complete Atoi — classic ('-'@see_neg | '+')? (digit@add_digit)+ >begin %finish */
#include <stdio.h>
#include <string.h>
#include <assert.h>

int neg, value;


#line 9 "74.c"
static const signed char _p74_actions[] = {
	0, 1, 0, 1, 2, 1, 3, 2,
	0, 1, 2, 0, 2, 0
};

static const signed char _p74_key_offsets[] = {
	0, 0, 4, 6, 9, 0
};

static const char _p74_trans_keys[] = {
	43, 45, 48, 57, 48, 57, 10, 48,
	57, 0
};

static const signed char _p74_single_lengths[] = {
	0, 2, 0, 1, 0, 0
};

static const signed char _p74_range_lengths[] = {
	0, 1, 1, 1, 0, 0
};

static const signed char _p74_index_offsets[] = {
	0, 0, 4, 6, 9, 0
};

static const signed char _p74_cond_targs[] = {
	2, 2, 3, 0, 3, 0, 4, 3,
	0, 0, 0, 1, 2, 3, 4, 0
};

static const signed char _p74_cond_actions[] = {
	1, 7, 10, 0, 3, 0, 5, 3,
	0, 0, 0, 0, 0, 0, 0, 0
};

static const int p74_start = 1;
static const int p74_first_final = 4;
static const int p74_error = 0;

static const int p74_en_main = 1;


#line 35 "74.rl"

int main(void){int cs;const char *p,*pe,*eof;
	printf("Pattern 74: Complete Atoi\n\n");
	const char *tests[] = {"1\n","12\n","222\n","+2123\n","-99\n","-12321\n","x\n"};
	int expect[] = {1,1,1,1,1,1,0};
	int n = 7;
	for(int i=0;i<n;i++){p=tests[i];pe=p+strlen(p);eof=pe;

#line 59 "74.c"
	{
			cs = (int)p74_start;
		}
		
#line 42 "74.rl"


#line 64 "74.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			const signed char * _acts;
			unsigned int _nacts;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p74_trans_keys + (_p74_key_offsets[cs]));
			_trans = (unsigned int)_p74_index_offsets[cs];
			
			_klen = (int)_p74_single_lengths[cs];
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
			
			_klen = (int)_p74_range_lengths[cs];
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
			cs = (int)_p74_cond_targs[_trans];
			
			if ( _p74_cond_actions[_trans] != 0 ) {
				
				_acts = ( _p74_actions + (_p74_cond_actions[_trans]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 ) {
					switch ( (*( _acts)) )
					{
						case 0:  {
								{
#line 10 "74.rl"
								
								neg = 0;
								value = 0;
							}
							
#line 142 "74.c"

							break; 
						}
						case 1:  {
								{
#line 15 "74.rl"
								
								neg = 1;
							}
							
#line 152 "74.c"

							break; 
						}
						case 2:  {
								{
#line 19 "74.rl"
								
								value = value * 10 + ((( (*( p)))) - '0');
							}
							
#line 162 "74.c"

							break; 
						}
						case 3:  {
								{
#line 23 "74.rl"
								
								if (neg) value = -value;
								printf("value=%d\n", value);
							}
							
#line 173 "74.c"

							break; 
						}
					}
					_nacts -= 1;
					_acts += 1;
				}
				
			}
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
			_out: {}
		}
		
#line 43 "74.rl"

		int r=cs>=p74_first_final;printf("  %-8s -> %s ok\n",tests[i],r?"ACCEPT":"FAIL");assert(r==expect[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
