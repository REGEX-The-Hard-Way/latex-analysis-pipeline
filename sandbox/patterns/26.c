#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/26.rl"
/* Pattern 26: Global Error Actions — >err $err %err on 'hello' parser */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/26.c"
static const signed char _p26_actions[] = {
	0, 1, 1, 2, 0, 1, 2, 1,
	2, 0
};

static const signed char _p26_key_offsets[] = {
	0, 0, 1, 2, 3, 4, 5, 6,
	0
};

static const char _p26_trans_keys[] = {
	104, 101, 108, 108, 111, 10, 0
};

static const signed char _p26_single_lengths[] = {
	0, 1, 1, 1, 1, 1, 1, 0,
	0
};

static const signed char _p26_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0
};

static const signed char _p26_index_offsets[] = {
	0, 0, 2, 4, 6, 8, 10, 12,
	0
};

static const signed char _p26_cond_targs[] = {
	2, 0, 3, 0, 4, 0, 5, 0,
	6, 0, 7, 0, 0, 0, 1, 2,
	3, 4, 5, 6, 7, 0
};

static const signed char _p26_cond_actions[] = {
	0, 3, 0, 1, 0, 1, 0, 1,
	0, 1, 0, 6, 0, 0, 3, 1,
	1, 1, 1, 6, 0, 0
};

static const signed char _p26_eof_trans[] = {
	14, 15, 16, 17, 18, 19, 20, 21,
	0
};

static const int p26_start = 1;
static const int p26_first_final = 7;
static const int p26_error = 0;

static const int p26_en_main = 1;


#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/26.rl"

int main(void){int cs;const char *p,*pe,*eof;
	printf("Pattern 26: Global Error Actions\n\n");
	p="helx\n";pe=p+5;eof=pe;

#line 63 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/26.c"
	{
		cs = (int)p26_start;
	}
	
#line 11 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/26.rl"


#line 68 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/26.c"
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
			if ( _p26_eof_trans[cs] > 0 ) {
				_trans = (unsigned int)_p26_eof_trans[cs] - 1;
			}
		}
		else {
			_keys = ( _p26_trans_keys + (_p26_key_offsets[cs]));
			_trans = (unsigned int)_p26_index_offsets[cs];
			
			_klen = (int)_p26_single_lengths[cs];
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
			
			_klen = (int)_p26_range_lengths[cs];
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
		cs = (int)_p26_cond_targs[_trans];
		
		if ( _p26_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p26_actions + (_p26_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/26.rl"
							printf(">ERR ");}
						
#line 150 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/26.c"

						break; 
					}
					case 1:  {
							{
#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/26.rl"
							printf("$ERR ");}
						
#line 158 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/26.c"

						break; 
					}
					case 2:  {
							{
#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/26.rl"
							printf("%%ERR ");}
						
#line 166 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/26.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
		}
		
		if ( p == eof ) {
			if ( cs >= 7 )
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
	
#line 12 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/26.rl"

	printf(" -> error state reached\n");
	return 0;}
