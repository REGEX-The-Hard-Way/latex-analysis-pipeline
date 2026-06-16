#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/40.rl"
/* Pattern 40: fnext*return_to — ftargs captures return context for non-local return */
#include <stdio.h>
#include <string.h>
#include <assert.h>
int return_to;

#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/40.c"
static const signed char _p40_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1,
	3, 0
};

static const signed char _p40_key_offsets[] = {
	0, 0, 1, 2, 3, 4, 5, 6,
	9, 9, 0
};

static const char _p40_trans_keys[] = {
	111, 110, 101, 116, 119, 111, 10, 49,
	50, 0
};

static const signed char _p40_single_lengths[] = {
	0, 1, 1, 1, 1, 1, 1, 3,
	0, 0, 0
};

static const signed char _p40_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0
};

static const signed char _p40_index_offsets[] = {
	0, 0, 2, 4, 6, 8, 10, 12,
	16, 17, 0
};

static const signed char _p40_cond_targs[] = {
	2, 0, 3, 0, 8, 0, 5, 0,
	6, 0, 9, 0, 7, 7, 7, 0,
	0, 0, 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 0
};

static const signed char _p40_cond_actions[] = {
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 3, 0, 0, 5, 7, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0
};

static const int p40_start = 7;
static const int p40_first_final = 7;
static const int p40_error = 0;

static const int p40_en_one = 1;
static const int p40_en_two = 4;
static const int p40_en_main = 7;


#line 10 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/40.rl"

int main(void){int cs,stack[8],top=0;const char *p,*pe,*eof;
	printf("Pattern 40: fnext with ftargs Return Context\n\n");
	p="1one2two\n";pe=p+9;eof=pe;

#line 64 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/40.c"
	{
		cs = (int)p40_start;
	}
	
#line 14 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/40.rl"


#line 69 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/40.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p40_trans_keys + (_p40_key_offsets[cs]));
		_trans = (unsigned int)_p40_index_offsets[cs];
		
		_klen = (int)_p40_single_lengths[cs];
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
		
		_klen = (int)_p40_range_lengths[cs];
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
		cs = (int)_p40_cond_targs[_trans];
		
		if ( _p40_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p40_actions + (_p40_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/40.rl"
							printf("ONE "); {cs = (return_to);}}
						
#line 144 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/40.c"

						break; 
					}
					case 1:  {
							{
#line 8 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/40.rl"
							printf("TWO "); {cs = (return_to);}}
						
#line 152 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/40.c"

						break; 
					}
					case 2:  {
							{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/40.rl"
							return_to = (cs); {cs = 1;}}
						
#line 160 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/40.c"

						break; 
					}
					case 3:  {
							{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/40.rl"
							return_to = (cs); {cs = 4;}}
						
#line 168 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/40.c"

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
	
#line 15 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/40.rl"

	printf("-> %s\n",cs>=p40_first_final?"ACCEPT":"FAIL");
	return 0;}
