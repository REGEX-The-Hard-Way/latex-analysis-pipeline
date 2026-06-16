#line 1 "20.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "20.c"
static const signed char _p20_actions[] = {
	0, 1, 3, 2, 0, 1, 3, 2,
	0, 1, 0
};

static const signed char _p20_key_offsets[] = {
	0, 0, 2, 5, 0
};

static const char _p20_trans_keys[] = {
	48, 57, 10, 48, 57, 0
};

static const signed char _p20_single_lengths[] = {
	0, 0, 1, 0, 0
};

static const signed char _p20_range_lengths[] = {
	0, 1, 1, 0, 0
};

static const signed char _p20_index_offsets[] = {
	0, 0, 2, 5, 0
};

static const signed char _p20_cond_targs[] = {
	2, 0, 3, 2, 0, 0, 0, 1,
	2, 3, 0
};

static const signed char _p20_cond_actions[] = {
	6, 0, 1, 3, 0, 0, 0, 0,
	0, 0, 0
};

static const int p20_start = 1;
static const int p20_first_final = 3;
static const int p20_error = 0;

static const int p20_en_main = 1;


#line 13 "20.rl"

int main(void){int val,cs;const char *p,*pe,*eof;
	printf("Pattern 20: @ and $ Actions\n\n");
	static const char *t[]={"123\n","45\n"};
	int n=2;
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 53 "20.c"
	{
			cs = (int)p20_start;
		}
		
#line 19 "20.rl"


#line 58 "20.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			const signed char * _acts;
			unsigned int _nacts;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p20_trans_keys + (_p20_key_offsets[cs]));
			_trans = (unsigned int)_p20_index_offsets[cs];
			
			_klen = (int)_p20_single_lengths[cs];
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
			
			_klen = (int)_p20_range_lengths[cs];
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
			cs = (int)_p20_cond_targs[_trans];
			
			if ( _p20_cond_actions[_trans] != 0 ) {
				
				_acts = ( _p20_actions + (_p20_cond_actions[_trans]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 ) {
					switch ( (*( _acts)) )
					{
						case 0:  {
								{
#line 5 "20.rl"
								printf(" [char:%c]", (( (*( p))))); }
							
#line 133 "20.c"

							break; 
						}
						case 1:  {
								{
#line 6 "20.rl"
								printf(" (elem)"); }
							
#line 141 "20.c"

							break; 
						}
						case 2:  {
								{
#line 8 "20.rl"
								val=0; }
							
#line 149 "20.c"

							break; 
						}
						case 3:  {
								{
#line 10 "20.rl"
								printf("  val=%d\n", val); }
							
#line 157 "20.c"

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
		
#line 20 "20.rl"

	}
	printf("\nAll %d tests passed.\n",n);return 0;}
