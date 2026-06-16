#line 1 "78.rl"
/* Pattern 78: Unicode XML — alphtype unsigned short, BaseChar/Ideographic/Letter */
#include <stdio.h>
#include <string.h>
#include <assert.h>


#line 7 "78.c"
static const signed char _p78_actions[] = {
	0, 1, 2, 2, 0, 2, 2, 1,
	2, 0
};

static const signed char _p78_key_offsets[] = {
	0, 0
};

static const unsigned short _p78_trans_keys[] = {
	12295u, 48u, 57u, 65u, 90u, 97u, 122u, 192u,
	214u, 216u, 246u, 248u, 255u, 19968u, 19973u, 0u
};

static const signed char _p78_single_lengths[] = {
	1, 0
};

static const signed char _p78_range_lengths[] = {
	7, 0
};

static const signed char _p78_index_offsets[] = {
	0, 0
};

static const signed char _p78_cond_targs[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0
};

static const signed char _p78_cond_actions[] = {
	3, 6, 3, 3, 3, 3, 3, 3,
	1, 0, 0
};

static const int p78_start = 0;
static const int p78_first_final = 0;
static const int p78_error = -1;

static const int p78_en_main = 0;


#line 27 "78.rl"

int main(void){int cs;unsigned short *p,*pe,*eof;
	printf("Pattern 78: Unicode XML Classes\n\n");
	unsigned short input[] = {0x0041, 0x007A, 0x0030, 0x0039, 0x0023, 0x00E9, 0x0030, 0};
	p=input;pe=p+7;eof=pe;

#line 55 "78.c"
	{
		cs = (int)p78_start;
	}
	
#line 32 "78.rl"


#line 60 "78.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const unsigned short * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p78_trans_keys + (_p78_key_offsets[cs]));
		_trans = (unsigned int)_p78_index_offsets[cs];
		
		_klen = (int)_p78_single_lengths[cs];
		if ( _klen > 0 ) {
			const unsigned short *_lower = _keys;
			const unsigned short *_upper = _keys + _klen - 1;
			const unsigned short *_mid;
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
		
		_klen = (int)_p78_range_lengths[cs];
		if ( _klen > 0 ) {
			const unsigned short *_lower = _keys;
			const unsigned short *_upper = _keys + (_klen<<1) - 2;
			const unsigned short *_mid;
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
		cs = (int)_p78_cond_targs[_trans];
		
		if ( _p78_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p78_actions + (_p78_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 16 "78.rl"
							printf("Letter=U+%04X\n", (( (*( p))))); }
						
#line 135 "78.c"

						break; 
					}
					case 1:  {
							{
#line 17 "78.rl"
							printf("Digit=U+%04X\n", (( (*( p))))); }
						
#line 143 "78.c"

						break; 
					}
					case 2:  {
							{
#line 18 "78.rl"
							printf("Other=U+%04X\n", (( (*( p))))); }
						
#line 151 "78.c"

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
	
#line 33 "78.rl"

	printf("-> %s ok\n",cs>=p78_first_final?"ACCEPT":"FAIL");
	assert(cs>=p78_first_final);
	printf("\nAll tests passed.\n");return 0;}
