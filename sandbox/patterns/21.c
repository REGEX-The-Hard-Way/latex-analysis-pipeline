#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/21.rl"
/* Pattern 21: Action Chaining — >begin @accum %finish on word parser */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/21.c"
static const signed char _p21_actions[] = {
	0, 1, 1, 1, 2, 2, 0, 1,
	0
};

static const signed char _p21_key_offsets[] = {
	0, 0, 4, 9, 0
};

static const char _p21_trans_keys[] = {
	65, 90, 97, 122, 10, 65, 90, 97,
	122, 0
};

static const signed char _p21_single_lengths[] = {
	0, 0, 1, 0, 0
};

static const signed char _p21_range_lengths[] = {
	0, 2, 2, 0, 0
};

static const signed char _p21_index_offsets[] = {
	0, 0, 3, 7, 0
};

static const signed char _p21_cond_targs[] = {
	2, 2, 0, 3, 2, 2, 0, 0,
	0, 1, 2, 3, 0
};

static const signed char _p21_cond_actions[] = {
	5, 5, 0, 3, 1, 1, 0, 0,
	0, 0, 0, 0, 0
};

static const int p21_start = 1;
static const int p21_first_final = 3;
static const int p21_error = 0;

static const int p21_en_main = 1;


#line 8 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/21.rl"

int main(void){int cs;const char *p,*pe,*eof;
	printf("Pattern 21: Action Chaining\n\n");
	p="hello\n";pe=p+6;eof=pe;

#line 53 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/21.c"
	{
		cs = (int)p21_start;
	}
	
#line 12 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/21.rl"


#line 58 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/21.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p21_trans_keys + (_p21_key_offsets[cs]));
		_trans = (unsigned int)_p21_index_offsets[cs];
		
		_klen = (int)_p21_single_lengths[cs];
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
		
		_klen = (int)_p21_range_lengths[cs];
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
		cs = (int)_p21_cond_targs[_trans];
		
		if ( _p21_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p21_actions + (_p21_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/21.rl"
							printf("B ");}
						
#line 133 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/21.c"

						break; 
					}
					case 1:  {
							{
#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/21.rl"
							printf("%c",(( (*( p)))));}
						
#line 141 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/21.c"

						break; 
					}
					case 2:  {
							{
#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/21.rl"
							printf(" E");}
						
#line 149 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/21.c"

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
	
#line 13 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/21.rl"

	printf("\n");
	return 0;}
