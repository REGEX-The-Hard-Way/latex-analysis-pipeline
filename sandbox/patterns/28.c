#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/28.rl"
/* Pattern 28: Error Recovery — $! fhold+fgoto garble_line to skip bad lines */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/28.c"
static const signed char _p28_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 0
};

static const signed char _p28_key_offsets[] = {
	0, 0, 1, 2, 3, 4, 5, 6,
	7, 7, 0
};

static const char _p28_trans_keys[] = {
	104, 101, 108, 108, 111, 10, 10, 0
};

static const signed char _p28_single_lengths[] = {
	0, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0
};

static const signed char _p28_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0
};

static const signed char _p28_index_offsets[] = {
	0, 0, 2, 4, 6, 8, 10, 12,
	14, 15, 0
};

static const signed char _p28_cond_targs[] = {
	2, 0, 3, 0, 4, 0, 5, 0,
	6, 0, 8, 0, 9, 7, 0, 0,
	0, 1, 2, 3, 4, 5, 6, 7,
	8, 9, 0
};

static const signed char _p28_cond_actions[] = {
	0, 5, 0, 5, 0, 5, 0, 5,
	3, 5, 0, 5, 1, 0, 0, 0,
	0, 5, 5, 5, 5, 5, 5, 0,
	0, 0, 0
};

static const signed char _p28_eof_trans[] = {
	17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 0
};

static const int p28_start = 1;
static const int p28_first_final = 8;
static const int p28_error = 0;

static const int p28_en_garble_line = 7;
static const int p28_en_main = 1;


#line 8 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/28.rl"

int main(void){int cs,stack[8],top=0;const char *p,*pe,*eof;
	printf("Pattern 28: Error Recovery ($!)\n\n");
	p="bad\nhello\n";pe=p+11;eof=pe;

#line 65 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/28.c"
	{
		cs = (int)p28_start;
	}
	
#line 12 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/28.rl"


#line 70 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/28.c"
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
			if ( _p28_eof_trans[cs] > 0 ) {
				_trans = (unsigned int)_p28_eof_trans[cs] - 1;
			}
		}
		else {
			_keys = ( _p28_trans_keys + (_p28_key_offsets[cs]));
			_trans = (unsigned int)_p28_index_offsets[cs];
			
			_klen = (int)_p28_single_lengths[cs];
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
			
			_klen = (int)_p28_range_lengths[cs];
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
		cs = (int)_p28_cond_targs[_trans];
		
		if ( _p28_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p28_actions + (_p28_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/28.rl"
							printf("SKIP "); {cs = 1;goto _again;}}
						
#line 152 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/28.c"

						break; 
					}
					case 1:  {
							{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/28.rl"
							printf("OK\n");}
						
#line 160 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/28.c"

						break; 
					}
					case 2:  {
							{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/28.rl"
							{p = p - 1; }{cs = 7;goto _again;}}
						
#line 168 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/28.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
		}
		
		_again: {}
		if ( p == eof ) {
			if ( cs >= 8 )
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
	
#line 13 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/28.rl"

	printf("\n");
	return 0;}
