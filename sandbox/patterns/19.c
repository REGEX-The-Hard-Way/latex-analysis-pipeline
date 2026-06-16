#line 1 "19.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "19.c"
static const signed char _p19_actions[] = {
	0, 1, 1, 1, 2, 2, 0, 1,
	0
};

static const signed char _p19_key_offsets[] = {
	0, 0, 2, 5, 0
};

static const char _p19_trans_keys[] = {
	48, 57, 10, 48, 57, 0
};

static const signed char _p19_single_lengths[] = {
	0, 0, 1, 0, 0
};

static const signed char _p19_range_lengths[] = {
	0, 1, 1, 0, 0
};

static const signed char _p19_index_offsets[] = {
	0, 0, 2, 5, 0
};

static const signed char _p19_cond_targs[] = {
	2, 0, 3, 2, 0, 0, 0, 1,
	2, 3, 0
};

static const signed char _p19_cond_actions[] = {
	5, 0, 3, 1, 0, 0, 0, 0,
	0, 0, 0
};

static const int p19_start = 1;
static const int p19_first_final = 3;
static const int p19_error = 0;

static const int p19_en_main = 1;


#line 10 "19.rl"

int main(void){int val;
	printf("Pattern 19: Leave Action %%\n\n");
	static const char *t[]={"42\n","999\n","7\n"};
	int n=3,cs; const char *p,*pe,*eof;
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 53 "19.c"
	{
			cs = (int)p19_start;
		}
		
#line 16 "19.rl"


#line 58 "19.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			const signed char * _acts;
			unsigned int _nacts;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p19_trans_keys + (_p19_key_offsets[cs]));
			_trans = (unsigned int)_p19_index_offsets[cs];
			
			_klen = (int)_p19_single_lengths[cs];
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
			
			_klen = (int)_p19_range_lengths[cs];
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
			cs = (int)_p19_cond_targs[_trans];
			
			if ( _p19_cond_actions[_trans] != 0 ) {
				
				_acts = ( _p19_actions + (_p19_cond_actions[_trans]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 ) {
					switch ( (*( _acts)) )
					{
						case 0:  {
								{
#line 5 "19.rl"
								val = 0; }
							
#line 133 "19.c"

							break; 
						}
						case 1:  {
								{
#line 6 "19.rl"
								val = val*10 + ((( (*( p))))-'0'); }
							
#line 141 "19.c"

							break; 
						}
						case 2:  {
								{
#line 7 "19.rl"
								printf("  val=%d (accepted)\n", val); }
							
#line 149 "19.c"

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
		
#line 17 "19.rl"

	}
	printf("\nAll %d tests passed.\n",n);return 0;}
