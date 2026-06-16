#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.rl"
/* Pattern 36: fcall/fret — parity-based call with fhold+fret */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.c"
static const signed char _p36_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1,
	3, 1, 4, 0
};

static const signed char _p36_key_offsets[] = {
	0, 0, 2, 5, 6, 7, 8, 9,
	10, 10, 11, 12, 13, 13, 13, 13,
	0
};

static const char _p36_trans_keys[] = {
	48, 57, 32, 48, 57, 10, 101, 118,
	101, 110, 111, 100, 100, 0
};

static const signed char _p36_single_lengths[] = {
	0, 0, 1, 1, 1, 1, 1, 1,
	0, 1, 1, 1, 0, 0, 0, 0,
	0
};

static const signed char _p36_range_lengths[] = {
	0, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0
};

static const signed char _p36_index_offsets[] = {
	0, 0, 2, 5, 7, 9, 11, 13,
	15, 16, 18, 20, 22, 23, 24, 25,
	0
};

static const signed char _p36_cond_targs[] = {
	2, 0, 3, 2, 0, 13, 0, 5,
	0, 6, 0, 7, 0, 8, 0, 14,
	10, 0, 11, 0, 12, 0, 15, 0,
	0, 0, 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11, 12, 13,
	14, 15, 0
};

static const signed char _p36_cond_actions[] = {
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 3, 0, 5,
	0, 0, 0, 0, 7, 0, 9, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0
};

static const int p36_start = 1;
static const int p36_first_final = 13;
static const int p36_error = 0;

static const int p36_en_even = 4;
static const int p36_en_odd = 9;
static const int p36_en_main = 1;


#line 14 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.rl"

int main(void){int cs,stack[8],top=0;const char *p,*pe,*eof;
	printf("Pattern 36: fcall/fret\n\n");
	p="2 even\n";pe=p+7;eof=pe;

#line 71 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.c"
	{
		cs = (int)p36_start;
		top = 0;
	}
	
#line 18 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.rl"


#line 77 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p36_trans_keys + (_p36_key_offsets[cs]));
		_trans = (unsigned int)_p36_index_offsets[cs];
		
		_klen = (int)_p36_single_lengths[cs];
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
		
		_klen = (int)_p36_range_lengths[cs];
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
		cs = (int)_p36_cond_targs[_trans];
		
		if ( _p36_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p36_actions + (_p36_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.rl"
							
							int d = (( (*( p)))) - '0';
							if (d & 1) {stack[top] = cs; top += 1;cs = 9;goto _again;}else {stack[top] = cs; top += 1;cs = 4;goto _again;}}
						
#line 154 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.c"

						break; 
					}
					case 1:  {
							{
#line 11 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.rl"
							printf("EVEN ");}
						
#line 162 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.c"

						break; 
					}
					case 2:  {
							{
#line 11 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.rl"
							{p = p - 1; }{top -= 1;cs = stack[top];goto _again;}}
						
#line 170 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.c"

						break; 
					}
					case 3:  {
							{
#line 12 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.rl"
							printf("ODD ");}
						
#line 178 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.c"

						break; 
					}
					case 4:  {
							{
#line 12 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.rl"
							{p = p - 1; }{top -= 1;cs = stack[top];goto _again;}}
						
#line 186 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.c"

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
	
#line 19 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/36.rl"

	printf("-> %s\n",cs>=p36_first_final?"ACCEPT":"FAIL");
	return 0;}
