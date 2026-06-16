#line 1 "18.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "18.c"
static const signed char _p18_actions[] = {
	0, 1, 1, 1, 2, 2, 0, 1,
	0
};

static const signed char _p18_key_offsets[] = {
	0, 0, 2, 5, 0
};

static const char _p18_trans_keys[] = {
	48, 57, 10, 48, 57, 0
};

static const signed char _p18_single_lengths[] = {
	0, 0, 1, 0, 0
};

static const signed char _p18_range_lengths[] = {
	0, 1, 1, 0, 0
};

static const signed char _p18_index_offsets[] = {
	0, 0, 2, 5, 0
};

static const signed char _p18_cond_targs[] = {
	2, 0, 3, 2, 0, 0, 0, 1,
	2, 3, 0
};

static const signed char _p18_cond_actions[] = {
	5, 0, 0, 1, 0, 0, 0, 0,
	0, 3, 0
};

static const signed char _p18_eof_trans[] = {
	7, 8, 9, 10, 0
};

static const int p18_start = 1;
static const int p18_first_final = 3;
static const int p18_error = 0;

static const int p18_en_main = 1;


#line 10 "18.rl"

int main(void){int val;
	printf("Pattern 18: Enter Action >\n\n");
	static const char *t[]={"123\n","456\n","0\n"};
	int n=3,cs; const char *p,*pe,*eof;
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 57 "18.c"
	{
			cs = (int)p18_start;
		}
		
#line 16 "18.rl"


#line 62 "18.c"
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
				if ( _p18_eof_trans[cs] > 0 ) {
					_trans = (unsigned int)_p18_eof_trans[cs] - 1;
				}
			}
			else {
				_keys = ( _p18_trans_keys + (_p18_key_offsets[cs]));
				_trans = (unsigned int)_p18_index_offsets[cs];
				
				_klen = (int)_p18_single_lengths[cs];
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
				
				_klen = (int)_p18_range_lengths[cs];
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
			cs = (int)_p18_cond_targs[_trans];
			
			if ( _p18_cond_actions[_trans] != 0 ) {
				
				_acts = ( _p18_actions + (_p18_cond_actions[_trans]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 ) {
					switch ( (*( _acts)) )
					{
						case 0:  {
								{
#line 5 "18.rl"
								val = 0; }
							
#line 144 "18.c"

							break; 
						}
						case 1:  {
								{
#line 6 "18.rl"
								val = val*10 + ((( (*( p))))-'0'); }
							
#line 152 "18.c"

							break; 
						}
						case 2:  {
								{
#line 7 "18.rl"
								printf("  val=%d\n", val); }
							
#line 160 "18.c"

							break; 
						}
					}
					_nacts -= 1;
					_acts += 1;
				}
				
			}
			
			if ( p == eof ) {
				if ( cs >= 3 )
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
		
#line 17 "18.rl"

	}
	printf("\nAll %d tests passed.\n",n);return 0;}
