#line 1 "69.rl"
/* Pattern 69: Concurrent Line/Column — scanner & position tracker */
#include <stdio.h>
#include <string.h>
#include <assert.h>

int line = 1, col = 1;
const char *ts, *te;
int act;


#line 11 "69.c"
static const signed char _p69_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1,
	3, 1, 4, 0
};

static const signed char _p69_key_offsets[] = {
	0, 5, 0
};

static const char _p69_trans_keys[] = {
	10, 65, 90, 97, 122, 65, 90, 97,
	122, 0
};

static const signed char _p69_single_lengths[] = {
	1, 0, 0
};

static const signed char _p69_range_lengths[] = {
	2, 2, 0
};

static const signed char _p69_index_offsets[] = {
	0, 4, 0
};

static const signed char _p69_cond_targs[] = {
	0, 1, 1, 0, 1, 1, 0, 0,
	0, 0
};

static const signed char _p69_cond_actions[] = {
	5, 0, 0, 7, 0, 0, 9, 0,
	9, 0
};

static const signed char _p69_to_state_actions[] = {
	1, 0, 0
};

static const signed char _p69_from_state_actions[] = {
	3, 0, 0
};

static const signed char _p69_eof_trans[] = {
	8, 9, 0
};

static const int p69_start = 0;
static const int p69_first_final = 0;
static const int p69_error = -1;

static const int p69_en_main = 0;


#line 23 "69.rl"

int main(void){int cs;const char *p,*pe,*eof;
	const char *d="abc def\nghi jkl";
	printf("Pattern 69: Concurrent Line/Column Tracker\n\n");
	p=d;pe=p+strlen(d);eof=pe;

#line 71 "69.c"
	{
		cs = (int)p69_start;
		ts = 0;
		te = 0;
	}
	
#line 28 "69.rl"


#line 78 "69.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe && p != eof )
			goto _out;
		_acts = ( _p69_actions + (_p69_from_state_actions[cs]));
		_nacts = (unsigned int)(*( _acts));
		_acts += 1;
		while ( _nacts > 0 ) {
			switch ( (*( _acts)) ) {
				case 1:  {
						{
#line 1 "NONE"
						{ts = p;}}
					
#line 97 "69.c"

					break; 
				}
			}
			_nacts -= 1;
			_acts += 1;
		}
		
		if ( p == eof ) {
			if ( _p69_eof_trans[cs] > 0 ) {
				_trans = (unsigned int)_p69_eof_trans[cs] - 1;
			}
		}
		else {
			_keys = ( _p69_trans_keys + (_p69_key_offsets[cs]));
			_trans = (unsigned int)_p69_index_offsets[cs];
			
			_klen = (int)_p69_single_lengths[cs];
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
			
			_klen = (int)_p69_range_lengths[cs];
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
		cs = (int)_p69_cond_targs[_trans];
		
		if ( _p69_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p69_actions + (_p69_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 2:  {
							{
#line 12 "69.rl"
							{te = p+1;{
#line 12 "69.rl"
									line++; col = 1; }
							}}
						
#line 181 "69.c"

						break; 
					}
					case 3:  {
							{
#line 13 "69.rl"
							{te = p+1;{
#line 13 "69.rl"
									col++; }
							}}
						
#line 192 "69.c"

						break; 
					}
					case 4:  {
							{
#line 14 "69.rl"
							{te = p;p = p - 1;{
#line 14 "69.rl"
									printf("tok@%d:%d ", line, col); }
							}}
						
#line 203 "69.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
		}
		
		if ( p == eof ) {
			if ( cs >= 0 )
				goto _out;
		}
		else {
			_acts = ( _p69_actions + (_p69_to_state_actions[cs]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) ) {
					case 0:  {
							{
#line 1 "NONE"
							{ts = 0;}}
						
#line 228 "69.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
			p += 1;
			goto _resume;
		}
		_out: {}
	}
	
#line 29 "69.rl"

	printf("-> %s ok\n",cs>=p69_first_final?"ACCEPT":"FAIL");
	printf("\nAll tests passed.\n");return 0;}
