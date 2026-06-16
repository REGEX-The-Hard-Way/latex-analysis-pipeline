#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.rl"
/* Pattern 38: fncall/fnret — named call/return dispatch */
#include <stdio.h>
#include <string.h>
#include <assert.h>
char comm;

#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.c"
static const signed char _p38_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1,
	3, 1, 4, 1, 5, 0
};

static const signed char _p38_key_offsets[] = {
	0, 0, 2, 3, 4, 5, 6, 7,
	8, 9, 9, 10, 11, 12, 13, 14,
	14, 14, 14, 0
};

static const char _p38_trans_keys[] = {
	97, 100, 32, 10, 65, 76, 80, 72,
	65, 68, 73, 71, 73, 84, 0
};

static const signed char _p38_single_lengths[] = {
	0, 2, 1, 1, 1, 1, 1, 1,
	1, 0, 1, 1, 1, 1, 1, 0,
	0, 0, 0, 0
};

static const signed char _p38_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0
};

static const signed char _p38_index_offsets[] = {
	0, 0, 3, 5, 7, 9, 11, 13,
	15, 17, 18, 20, 22, 24, 26, 28,
	29, 30, 31, 0
};

static const signed char _p38_cond_targs[] = {
	2, 2, 0, 3, 0, 16, 0, 5,
	0, 6, 0, 7, 0, 8, 0, 9,
	0, 17, 11, 0, 12, 0, 13, 0,
	14, 0, 15, 0, 18, 0, 0, 0,
	0, 1, 2, 3, 4, 5, 6, 7,
	8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 0
};

static const signed char _p38_cond_actions[] = {
	11, 11, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 3,
	0, 5, 0, 0, 0, 0, 0, 0,
	0, 0, 7, 0, 9, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0
};

static const int p38_start = 1;
static const int p38_first_final = 16;
static const int p38_error = 0;

static const int p38_en_alpha_comm = 4;
static const int p38_en_digit_comm = 10;
static const int p38_en_main = 1;


#line 15 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.rl"

int main(void){int cs,stack[8],top=0;const char *p,*pe,*eof;
	printf("Pattern 38: fncall/fnret\n\n");
	p="d DIGIT\n";pe=p+8;eof=pe;

#line 74 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.c"
	{
		cs = (int)p38_start;
		top = 0;
	}
	
#line 19 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.rl"


#line 80 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p38_trans_keys + (_p38_key_offsets[cs]));
		_trans = (unsigned int)_p38_index_offsets[cs];
		
		_klen = (int)_p38_single_lengths[cs];
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
		
		_klen = (int)_p38_range_lengths[cs];
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
		cs = (int)_p38_cond_targs[_trans];
		
		if ( _p38_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p38_actions + (_p38_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.rl"
							
							if (comm == 'a') {stack[top] = cs; top += 1;cs = 4; }else if (comm == 'd') {stack[top] = cs; top += 1;cs = 10; }}
						
#line 156 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.c"

						break; 
					}
					case 1:  {
							{
#line 11 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.rl"
							printf("ALPHA ");}
						
#line 164 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.c"

						break; 
					}
					case 2:  {
							{
#line 11 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.rl"
							{p = p - 1; }{top -= 1;cs = stack[top];}}
						
#line 172 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.c"

						break; 
					}
					case 3:  {
							{
#line 12 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.rl"
							printf("DIGIT ");}
						
#line 180 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.c"

						break; 
					}
					case 4:  {
							{
#line 12 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.rl"
							{p = p - 1; }{top -= 1;cs = stack[top];}}
						
#line 188 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.c"

						break; 
					}
					case 5:  {
							{
#line 13 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.rl"
							comm=(( (*( p))));}
						
#line 196 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.c"

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
	
#line 20 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/38.rl"

	printf("-> %s\n",cs>=p38_first_final?"ACCEPT":"FAIL");
	return 0;}
