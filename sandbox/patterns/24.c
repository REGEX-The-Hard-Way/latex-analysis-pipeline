#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/24.rl"
/* Pattern 24: Epsilon Transitions — fnext/fgoto jumps between named states */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/24.c"
static const signed char _p24_actions[] = {
	0, 1, 0, 0
};

static const signed char _p24_key_offsets[] = {
	0, 0, 1, 2, 2, 0
};

static const char _p24_trans_keys[] = {
	97, 98, 0
};

static const signed char _p24_single_lengths[] = {
	0, 1, 1, 0, 0, 0
};

static const signed char _p24_range_lengths[] = {
	0, 0, 0, 0, 0, 0
};

static const signed char _p24_index_offsets[] = {
	0, 0, 2, 4, 5, 0
};

static const signed char _p24_cond_targs[] = {
	3, 0, 4, 0, 0, 0, 0, 1,
	2, 3, 4, 0
};

static const signed char _p24_cond_actions[] = {
	1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0
};

static const int p24_start = 1;
static const int p24_first_final = 3;
static const int p24_error = 0;

static const int p24_en_sub = 2;
static const int p24_en_main = 1;


#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/24.rl"

int main(void){int cs,stack[8],top=0;const char *p,*pe,*eof;
	printf("Pattern 24: Epsilon Transitions\n\n");
	p="ab";pe=p+2;eof=pe;

#line 52 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/24.c"
	{
		cs = (int)p24_start;
	}
	
#line 13 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/24.rl"


#line 57 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/24.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p24_trans_keys + (_p24_key_offsets[cs]));
		_trans = (unsigned int)_p24_index_offsets[cs];
		
		_klen = (int)_p24_single_lengths[cs];
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
		
		_klen = (int)_p24_range_lengths[cs];
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
		cs = (int)_p24_cond_targs[_trans];
		
		if ( _p24_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p24_actions + (_p24_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/24.rl"
							{cs = 2;goto _again;}}
						
#line 132 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/24.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
		}
		
		_again: {}
		if ( cs != 0 ) {
			p += 1;
			goto _resume;
		}
		_out: {}
	}
	
#line 14 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/24.rl"

	printf("  ab -> %s\n",cs>=p24_first_final?"ACCEPT":"FAIL");
	return 0;}
