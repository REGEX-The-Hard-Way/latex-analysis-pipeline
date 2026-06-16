#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/29.rl"
/* Pattern 29: Fine-Grained Error — >! and %! on float parser */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/29.c"
static const signed char _p29_actions[] = {
	0, 1, 1, 2, 0, 1, 0
};

static const signed char _p29_key_offsets[] = {
	0, 0, 4, 6, 10, 12, 15, 0
};

static const char _p29_trans_keys[] = {
	43, 45, 48, 57, 48, 57, 10, 46,
	48, 57, 48, 57, 10, 48, 57, 0
};

static const signed char _p29_single_lengths[] = {
	0, 2, 0, 2, 0, 1, 0, 0
};

static const signed char _p29_range_lengths[] = {
	0, 1, 1, 1, 1, 1, 0, 0
};

static const signed char _p29_index_offsets[] = {
	0, 0, 4, 6, 10, 12, 15, 0
};

static const signed char _p29_cond_targs[] = {
	2, 2, 3, 0, 3, 0, 6, 4,
	3, 0, 5, 0, 6, 5, 0, 0,
	0, 1, 2, 3, 4, 5, 6, 0
};

static const signed char _p29_cond_actions[] = {
	0, 0, 0, 3, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 3, 1, 0, 0, 0, 0, 0
};

static const signed char _p29_eof_trans[] = {
	17, 18, 19, 20, 21, 22, 23, 0
};

static const int p29_start = 1;
static const int p29_first_final = 6;
static const int p29_error = 0;

static const int p29_en_main = 1;


#line 10 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/29.rl"

int main(void){int cs;const char *p,*pe,*eof;
	printf("Pattern 29: Fine-Grained Error (>! %%!)\n\n");
	p="x\n";pe=p+2;eof=pe;

#line 58 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/29.c"
	{
		cs = (int)p29_start;
	}
	
#line 14 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/29.rl"


#line 63 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/29.c"
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
			if ( _p29_eof_trans[cs] > 0 ) {
				_trans = (unsigned int)_p29_eof_trans[cs] - 1;
			}
		}
		else {
			_keys = ( _p29_trans_keys + (_p29_key_offsets[cs]));
			_trans = (unsigned int)_p29_index_offsets[cs];
			
			_klen = (int)_p29_single_lengths[cs];
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
			
			_klen = (int)_p29_range_lengths[cs];
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
		cs = (int)_p29_cond_targs[_trans];
		
		if ( _p29_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p29_actions + (_p29_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/29.rl"
							printf("EXP_SIGN "); }
						
#line 145 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/29.c"

						break; 
					}
					case 1:  {
							{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/29.rl"
							printf("EXP_DIG "); }
						
#line 153 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/29.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
		}
		
		if ( p == eof ) {
			if ( cs >= 6 )
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
	
#line 15 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/29.rl"

	printf(" -> %s\n",cs>=p29_first_final?"ok":"error");
	return 0;}
