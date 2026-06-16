#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/22.rl"
/* Pattern 22: Statechart Named States — atoi with start:/om_num:/more_nums: */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/22.c"
static const signed char _p22_actions[] = {
	0, 1, 0, 1, 1, 0
};

static const signed char _p22_key_offsets[] = {
	0, 0, 2, 5, 0
};

static const char _p22_trans_keys[] = {
	48, 57, 10, 48, 57, 0
};

static const signed char _p22_single_lengths[] = {
	0, 0, 1, 0, 0
};

static const signed char _p22_range_lengths[] = {
	0, 1, 1, 0, 0
};

static const signed char _p22_index_offsets[] = {
	0, 0, 2, 5, 0
};

static const signed char _p22_cond_targs[] = {
	2, 0, 3, 2, 0, 0, 0, 1,
	2, 3, 0
};

static const signed char _p22_cond_actions[] = {
	1, 0, 0, 3, 0, 0, 0, 0,
	0, 0, 0
};

static const int p22_start = 1;
static const int p22_first_final = 3;
static const int p22_error = 0;

static const int p22_en_main = 1;


#line 10 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/22.rl"

int main(void){int cs;const char *p,*pe,*eof;
	printf("Pattern 22: Statechart Named States\n\n");
	p="123\n";pe=p+4;eof=pe;

#line 51 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/22.c"
	{
		cs = (int)p22_start;
	}
	
#line 14 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/22.rl"


#line 56 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/22.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p22_trans_keys + (_p22_key_offsets[cs]));
		_trans = (unsigned int)_p22_index_offsets[cs];
		
		_klen = (int)_p22_single_lengths[cs];
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
		
		_klen = (int)_p22_range_lengths[cs];
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
		cs = (int)_p22_cond_targs[_trans];
		
		if ( _p22_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p22_actions + (_p22_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/22.rl"
							printf("%c",(( (*( p)))));}
						
#line 131 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/22.c"

						break; 
					}
					case 1:  {
							{
#line 8 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/22.rl"
							printf("%c",(( (*( p)))));}
						
#line 139 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/22.c"

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
	
#line 15 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/22.rl"

	printf(" -> %s\n",cs>=p22_first_final?"ACCEPT":"FAIL");
	return 0;}
