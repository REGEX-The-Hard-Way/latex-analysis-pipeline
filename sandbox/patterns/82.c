#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/82.rl"
/* Pattern 82: Awk Emulation — line-based word splitting with position tracking */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/82.c"
static const signed char _p82_actions[] = {
	0, 1, 0, 1, 1, 0
};

static const signed char _p82_key_offsets[] = {
	0, 0, 4, 8, 12, 0
};

static const char _p82_trans_keys[] = {
	10, 32, 9, 12, 10, 32, 9, 12,
	10, 32, 9, 12, 10, 32, 9, 12,
	0
};

static const signed char _p82_single_lengths[] = {
	0, 2, 2, 2, 2, 0
};

static const signed char _p82_range_lengths[] = {
	0, 1, 1, 1, 1, 0
};

static const signed char _p82_index_offsets[] = {
	0, 0, 4, 8, 12, 0
};

static const signed char _p82_cond_targs[] = {
	4, 2, 2, 1, 4, 2, 2, 0,
	4, 3, 3, 1, 4, 3, 3, 1,
	0, 1, 2, 3, 4, 0
};

static const signed char _p82_cond_actions[] = {
	3, 3, 3, 0, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0
};

static const int p82_start = 4;
static const int p82_first_final = 4;
static const int p82_error = 0;

static const int p82_en_main = 4;


#line 12 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/82.rl"

int main(void){int cs; const char *p,*pe,*eof; ptrdiff_t wstart,wend;
	const char *data = "hello world\nfoo  bar   baz\n\n last \n";
	printf("Pattern 82: Awk Emulation — line-based word splitting\n\n");
	printf("Input:\n%s\n",data);
	p=data; pe=p+strlen(data); eof=pe;

#line 57 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/82.c"
	{
		cs = (int)p82_start;
	}
	
#line 18 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/82.rl"


#line 62 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/82.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p82_trans_keys + (_p82_key_offsets[cs]));
		_trans = (unsigned int)_p82_index_offsets[cs];
		
		_klen = (int)_p82_single_lengths[cs];
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
		
		_klen = (int)_p82_range_lengths[cs];
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
		cs = (int)_p82_cond_targs[_trans];
		
		if ( _p82_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p82_actions + (_p82_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/82.rl"
							wstart = p - data; }
						
#line 137 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/82.c"

						break; 
					}
					case 1:  {
							{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/82.rl"
							wend = p - data; printf("  word [%d..%d): %.*s\n",(int)wstart,(int)wend,(int)(wend-wstart),data+wstart); }
						
#line 145 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/82.c"

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
	
#line 19 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/82.rl"

	int r=cs>=p82_first_final;
	printf("  -> %s\n\n",r?"ACCEPT":"FAIL"); assert(r);
	printf("All tests passed.\n");return 0;
}
