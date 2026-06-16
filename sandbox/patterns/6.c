#line 1 "6.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "6.c"
static const signed char _p6_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1,
	3, 1, 4, 1, 5, 0
};

static const signed char _p6_key_offsets[] = {
	0, 0, 0
};

static const char _p6_trans_keys[] = {
	32, 9, 13, 33, 47, 48, 57, 58,
	64, 65, 90, 91, 96, 97, 122, 123,
	126, 0
};

static const signed char _p6_single_lengths[] = {
	0, 1, 0
};

static const signed char _p6_range_lengths[] = {
	0, 8, 0
};

static const signed char _p6_index_offsets[] = {
	0, 0, 0
};

static const signed char _p6_cond_targs[] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 1, 0
};

static const signed char _p6_cond_actions[] = {
	9, 9, 11, 7, 11, 5, 11, 5,
	11, 0, 0, 0, 0
};

static const signed char _p6_to_state_actions[] = {
	0, 1, 0
};

static const signed char _p6_from_state_actions[] = {
	0, 3, 0
};

static const signed char _p6_eof_trans[] = {
	11, 12, 0
};

static const int p6_start = 1;
static const int p6_first_final = 1;
static const int p6_error = 0;

static const int p6_en_main = 1;


#line 15 "6.rl"

int main(void){
	printf("Pattern  6: Builtin Classes\n\n");
	static const char *t[]={"a","A","1"," ",".","z","Z","9","f","F","g"};
	int n=11,cs,act; const char *p,*pe,*eof,*ts,*te;
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
		printf("  Input '%s': ",t[i]);fflush(stdout);

#line 68 "6.c"
	{
			cs = (int)p6_start;
			ts = 0;
			te = 0;
		}
		
#line 22 "6.rl"


#line 75 "6.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			const signed char * _acts;
			unsigned int _nacts;
			_resume: {}
			if ( p == pe && p != eof )
				goto _out;
			_acts = ( _p6_actions + (_p6_from_state_actions[cs]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) ) {
					case 1:  {
							{
#line 1 "NONE"
							{ts = p;}}
						
#line 94 "6.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
			if ( p == eof ) {
				if ( _p6_eof_trans[cs] > 0 ) {
					_trans = (unsigned int)_p6_eof_trans[cs] - 1;
				}
			}
			else {
				_keys = ( _p6_trans_keys + (_p6_key_offsets[cs]));
				_trans = (unsigned int)_p6_index_offsets[cs];
				
				_klen = (int)_p6_single_lengths[cs];
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
				
				_klen = (int)_p6_range_lengths[cs];
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
			cs = (int)_p6_cond_targs[_trans];
			
			if ( _p6_cond_actions[_trans] != 0 ) {
				
				_acts = ( _p6_actions + (_p6_cond_actions[_trans]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 ) {
					switch ( (*( _acts)) )
					{
						case 2:  {
								{
#line 6 "6.rl"
								{te = p+1;{
#line 6 "6.rl"
										printf("  alpha\n");  }
								}}
							
#line 178 "6.c"

							break; 
						}
						case 3:  {
								{
#line 7 "6.rl"
								{te = p+1;{
#line 7 "6.rl"
										printf("  digit\n");  }
								}}
							
#line 189 "6.c"

							break; 
						}
						case 4:  {
								{
#line 8 "6.rl"
								{te = p+1;{
#line 8 "6.rl"
										printf("  space\n");  }
								}}
							
#line 200 "6.c"

							break; 
						}
						case 5:  {
								{
#line 9 "6.rl"
								{te = p+1;{
#line 9 "6.rl"
										printf("  punct\n");  }
								}}
							
#line 211 "6.c"

							break; 
						}
					}
					_nacts -= 1;
					_acts += 1;
				}
				
			}
			
			if ( p == eof ) {
				if ( cs >= 1 )
					goto _out;
			}
			else {
				_acts = ( _p6_actions + (_p6_to_state_actions[cs]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 ) {
					switch ( (*( _acts)) ) {
						case 0:  {
								{
#line 1 "NONE"
								{ts = 0;}}
							
#line 236 "6.c"

							break; 
						}
					}
					_nacts -= 1;
					_acts += 1;
				}
				
				if ( cs != 0 ) {
					p += 1;
					goto _resume;
				}
			}
			_out: {}
		}
		
#line 23 "6.rl"

		printf("\n");
	}
	printf("\nAll %d tests passed.\n",n);return 0;}
