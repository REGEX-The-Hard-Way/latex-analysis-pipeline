#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/27.rl"
/* Pattern 27: Local Error — two sub-machines with different $lerr handlers */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/27.c"
static const signed char _p27_actions[] = {
	0, 1, 0, 1, 1, 2, 0, 1,
	0
};

static const signed char _p27_key_offsets[] = {
	0, 0, 6, 8, 0
};

static const char _p27_trans_keys[] = {
	48, 57, 65, 90, 97, 122, 48, 57,
	65, 90, 97, 122, 0
};

static const signed char _p27_single_lengths[] = {
	0, 0, 0, 0, 0
};

static const signed char _p27_range_lengths[] = {
	0, 3, 1, 2, 0
};

static const signed char _p27_index_offsets[] = {
	0, 0, 4, 6, 0
};

static const signed char _p27_cond_targs[] = {
	2, 3, 3, 0, 2, 0, 3, 3,
	0, 0, 1, 2, 3, 0
};

static const signed char _p27_cond_actions[] = {
	0, 0, 0, 5, 0, 3, 0, 0,
	1, 0, 5, 0, 0, 0
};

static const signed char _p27_eof_trans[] = {
	10, 11, 12, 13, 0
};

static const int p27_start = 1;
static const int p27_first_final = 2;
static const int p27_error = 0;

static const int p27_en_main = 1;


#line 11 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/27.rl"

int main(void){int cs;const char *p,*pe,*eof;
	printf("Pattern 27: Local Error ($lerr)\n\n");
	p="a1";pe=p+2;eof=pe;

#line 57 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/27.c"
	{
		cs = (int)p27_start;
	}
	
#line 15 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/27.rl"


#line 62 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/27.c"
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
			if ( _p27_eof_trans[cs] > 0 ) {
				_trans = (unsigned int)_p27_eof_trans[cs] - 1;
			}
		}
		else {
			_keys = ( _p27_trans_keys + (_p27_key_offsets[cs]));
			_trans = (unsigned int)_p27_index_offsets[cs];
			
			_klen = (int)_p27_single_lengths[cs];
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
			
			_klen = (int)_p27_range_lengths[cs];
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
		cs = (int)_p27_cond_targs[_trans];
		
		if ( _p27_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p27_actions + (_p27_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/27.rl"
							printf("AlphaErr "); }
						
#line 144 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/27.c"

						break; 
					}
					case 1:  {
							{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/27.rl"
							printf("DigitErr "); }
						
#line 152 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/27.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
		}
		
		if ( p == eof ) {
			if ( cs >= 2 )
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
	
#line 16 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/27.rl"

	printf("\n");
	return 0;}
