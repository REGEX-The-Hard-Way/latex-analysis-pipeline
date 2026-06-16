#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/25.rl"
/* Pattern 25: Action-Only Patterns — zlen >action fires without consuming input */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/25.c"
static const signed char _p25_actions[] = {
	0, 1, 0, 0
};

static const signed char _p25_key_offsets[] = {
	0, 0, 1, 2, 3, 4, 5, 6,
	0
};

static const char _p25_trans_keys[] = {
	104, 101, 108, 108, 111, 10, 0
};

static const signed char _p25_single_lengths[] = {
	0, 1, 1, 1, 1, 1, 1, 0,
	0
};

static const signed char _p25_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0
};

static const signed char _p25_index_offsets[] = {
	0, 0, 2, 4, 6, 8, 10, 12,
	0
};

static const signed char _p25_cond_targs[] = {
	2, 0, 3, 0, 4, 0, 5, 0,
	6, 0, 7, 0, 0, 0, 1, 2,
	3, 4, 5, 6, 7, 0
};

static const signed char _p25_cond_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0
};

static const int p25_start = 1;
static const int p25_first_final = 7;
static const int p25_error = 0;

static const int p25_en_main = 1;


#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/25.rl"

int main(void){int cs;const char *p,*pe,*eof;
	printf("Pattern 25: Action-Only Patterns\n\n");
	p="hello\n";pe=p+6;eof=pe;

#line 57 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/25.c"
	{
		cs = (int)p25_start;
	}
	
#line 11 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/25.rl"


#line 62 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/25.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p25_trans_keys + (_p25_key_offsets[cs]));
		_trans = (unsigned int)_p25_index_offsets[cs];
		
		_klen = (int)_p25_single_lengths[cs];
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
		
		_klen = (int)_p25_range_lengths[cs];
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
		cs = (int)_p25_cond_targs[_trans];
		
		if ( _p25_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p25_actions + (_p25_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/25.rl"
							printf("MARKER ");}
						
#line 137 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/25.c"

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
	
#line 12 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/25.rl"

	printf("\n");
	return 0;}
