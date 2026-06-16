#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.rl"
/* Pattern 33: Scanner Modes — on-last/on-next/on-lag pattern matching */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.c"
static const signed char _p33_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1,
	3, 1, 4, 1, 5, 1, 6, 1,
	7, 1, 8, 1, 9, 0
};

static const signed char _p33_key_offsets[] = {
	0, 0, 4, 5, 6, 7, 8, 15,
	16, 0
};

static const char _p33_trans_keys[] = {
	46, 100, 48, 57, 49, 120, 120, 120,
	10, 97, 98, 99, 100, 48, 57, 98,
	100, 0
};

static const signed char _p33_single_lengths[] = {
	0, 2, 1, 1, 1, 1, 5, 1,
	1, 0
};

static const signed char _p33_range_lengths[] = {
	0, 1, 0, 0, 0, 0, 1, 0,
	0, 0
};

static const signed char _p33_index_offsets[] = {
	0, 0, 4, 6, 8, 10, 12, 19,
	21, 0
};

static const signed char _p33_cond_targs[] = {
	6, 1, 1, 0, 8, 0, 4, 6,
	5, 6, 6, 6, 6, 6, 7, 2,
	1, 1, 0, 7, 6, 3, 6, 0,
	1, 2, 6, 6, 6, 6, 6, 6,
	0
};

static const signed char _p33_cond_actions[] = {
	11, 0, 0, 0, 5, 0, 0, 19,
	0, 19, 9, 19, 13, 7, 0, 0,
	0, 0, 0, 0, 15, 0, 17, 0,
	0, 0, 19, 19, 19, 0, 15, 17,
	0
};

static const signed char _p33_to_state_actions[] = {
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0
};

static const signed char _p33_from_state_actions[] = {
	0, 0, 0, 0, 0, 0, 3, 0,
	0, 0
};

static const signed char _p33_eof_trans[] = {
	24, 25, 26, 27, 28, 29, 30, 31,
	32, 0
};

static const int p33_start = 6;
static const int p33_first_final = 6;
static const int p33_error = 0;

static const int p33_en_main = 6;


#line 13 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.rl"

int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
	printf("Pattern 33: Scanner Modes\n\n");
	char buf[]="abbc1d1d2\n";
	p=buf;pe=p+strlen(buf);eof=pe;

#line 81 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.c"
	{
		cs = (int)p33_start;
		ts = 0;
		te = 0;
	}
	
#line 18 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.rl"


#line 88 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe && p != eof )
			goto _out;
		_acts = ( _p33_actions + (_p33_from_state_actions[cs]));
		_nacts = (unsigned int)(*( _acts));
		_acts += 1;
		while ( _nacts > 0 ) {
			switch ( (*( _acts)) ) {
				case 1:  {
						{
#line 1 "NONE"
						{ts = p;}}
					
#line 107 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.c"

					break; 
				}
			}
			_nacts -= 1;
			_acts += 1;
		}
		
		if ( p == eof ) {
			if ( _p33_eof_trans[cs] > 0 ) {
				_trans = (unsigned int)_p33_eof_trans[cs] - 1;
			}
		}
		else {
			_keys = ( _p33_trans_keys + (_p33_key_offsets[cs]));
			_trans = (unsigned int)_p33_index_offsets[cs];
			
			_klen = (int)_p33_single_lengths[cs];
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
			
			_klen = (int)_p33_range_lengths[cs];
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
		cs = (int)_p33_cond_targs[_trans];
		
		if ( _p33_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p33_actions + (_p33_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 2:  {
							{
#line 1 "NONE"
							{te = p+1;}}
						
#line 188 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.c"

						break; 
					}
					case 3:  {
							{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.rl"
							{te = p+1;{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.rl"
									printf("LAST "); }
							}}
						
#line 199 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.c"

						break; 
					}
					case 4:  {
							{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.rl"
							{te = p+1;{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.rl"
									printf("LAG "); }
							}}
						
#line 210 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.c"

						break; 
					}
					case 5:  {
							{
#line 1 "-"
							{te = p+1;}}
						
#line 218 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.c"

						break; 
					}
					case 6:  {
							{
#line 1 "-"
							{te = p+1;}}
						
#line 226 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.c"

						break; 
					}
					case 7:  {
							{
#line 8 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.rl"
							{te = p;p = p - 1;{
#line 8 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.rl"
									printf("NEXT "); }
							}}
						
#line 237 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.c"

						break; 
					}
					case 8:  {
							{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.rl"
							{te = p;p = p - 1;{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.rl"
									printf("LAG "); }
							}}
						
#line 248 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.c"

						break; 
					}
					case 9:  {
							{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.rl"
							{p = ((te))-1;
								{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.rl"
									printf("LAG "); }
							}}
						
#line 260 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.c"

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
			_acts = ( _p33_actions + (_p33_to_state_actions[cs]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) ) {
					case 0:  {
							{
#line 1 "NONE"
							{ts = 0;}}
						
#line 285 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
		}
		_out: {}
	}
	
#line 19 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/33.rl"

	printf("\n");
	return 0;}
