#line 1 "7.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "7.c"
static const signed char _p7_actions[] = {
	0, 1, 0, 0
};

static const signed char _p7_key_offsets[] = {
	0, 0, 1, 2, 3, 0
};

static const char _p7_trans_keys[] = {
	65, 66, 10, 0
};

static const signed char _p7_single_lengths[] = {
	0, 1, 1, 1, 0, 0
};

static const signed char _p7_range_lengths[] = {
	0, 0, 0, 0, 0, 0
};

static const signed char _p7_index_offsets[] = {
	0, 0, 2, 4, 6, 0
};

static const signed char _p7_cond_targs[] = {
	2, 0, 3, 0, 4, 0, 0, 0,
	1, 2, 3, 4, 0
};

static const signed char _p7_cond_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 1, 0
};

static const signed char _p7_eof_trans[] = {
	8, 9, 10, 11, 12, 0
};

static const int p7_start = 1;
static const int p7_first_final = 4;
static const int p7_error = 0;

static const int p7_en_main = 1;


#line 4 "7.rl"

int main(void){static const char *t[]={"AB\n","A\n","\n"};static const int e[]={1,0,0};int n=3,cs,matched;const char *p,*pe,*eof;
	printf("Pattern  7: Empty/Zlen\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;matched=0;

#line 54 "7.c"
	{
			cs = (int)p7_start;
		}
		
#line 8 "7.rl"


#line 59 "7.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			const signed char * _acts;
			unsigned int _nacts;
			_resume: {}
			if ( p == pe && p != eof )
				goto _out;
			if ( p == eof ) {
				if ( _p7_eof_trans[cs] > 0 ) {
					_trans = (unsigned int)_p7_eof_trans[cs] - 1;
				}
			}
			else {
				_keys = ( _p7_trans_keys + (_p7_key_offsets[cs]));
				_trans = (unsigned int)_p7_index_offsets[cs];
				
				_klen = (int)_p7_single_lengths[cs];
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
				
				_klen = (int)_p7_range_lengths[cs];
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
			}
			cs = (int)_p7_cond_targs[_trans];
			
			if ( _p7_cond_actions[_trans] != 0 ) {
				
				_acts = ( _p7_actions + (_p7_cond_actions[_trans]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 ) {
					switch ( (*( _acts)) )
					{
						case 0:  {
								{
#line 4 "7.rl"
								matched=1; }
							
#line 141 "7.c"

							break; 
						}
					}
					_nacts -= 1;
					_acts += 1;
				}
				
			}
			
			if ( p == eof ) {
				if ( cs >= 4 )
					goto _out;
			}
			else {
				if ( cs != 0 ) {
					p += 1;
					goto _resume;
				}
			}
			_out: {}
		}
		
#line 9 "7.rl"

		printf("  Input %d -> %s ok\n",i,matched?"ACCEPT":"FAIL");assert(matched==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
