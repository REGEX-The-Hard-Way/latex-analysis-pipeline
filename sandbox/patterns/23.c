#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/23.rl"
/* Pattern 23: Named Patterns — hexdigit/letter patterns combined */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/23.c"
static const signed char _p23_key_offsets[] = {
	0, 0, 5, 6, 12, 19, 26, 0
};

static const char _p23_trans_keys[] = {
	48, 65, 90, 97, 122, 120, 48, 57,
	65, 70, 97, 102, 10, 48, 57, 65,
	70, 97, 102, 10, 48, 57, 65, 90,
	97, 122, 0
};

static const signed char _p23_single_lengths[] = {
	0, 1, 1, 0, 1, 1, 0, 0
};

static const signed char _p23_range_lengths[] = {
	0, 2, 0, 3, 3, 3, 0, 0
};

static const signed char _p23_index_offsets[] = {
	0, 0, 4, 6, 10, 15, 20, 0
};

static const signed char _p23_cond_targs[] = {
	2, 5, 5, 0, 3, 0, 4, 4,
	4, 0, 6, 4, 4, 4, 0, 6,
	5, 5, 5, 0, 0, 0, 1, 2,
	3, 4, 5, 6, 0
};

static const int p23_start = 1;
static const int p23_first_final = 6;
static const int p23_error = 0;

static const int p23_en_main = 1;


#line 11 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/23.rl"

int main(void){int cs;const char *p,*pe,*eof;
	printf("Pattern 23: Named Patterns\n\n");
	p="myVar\n";pe=p+6;eof=pe;

#line 47 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/23.c"
	{
		cs = (int)p23_start;
	}
	
#line 15 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/23.rl"


#line 52 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/23.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p23_trans_keys + (_p23_key_offsets[cs]));
		_trans = (unsigned int)_p23_index_offsets[cs];
		
		_klen = (int)_p23_single_lengths[cs];
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
		
		_klen = (int)_p23_range_lengths[cs];
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
		cs = (int)_p23_cond_targs[_trans];
		
		
		if ( cs != 0 ) {
			p += 1;
			goto _resume;
		}
		_out: {}
	}
	
#line 16 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/23.rl"

	printf("  myVar -> %s\n",cs>=p23_first_final?"ACCEPT":"FAIL");
	return 0;}
