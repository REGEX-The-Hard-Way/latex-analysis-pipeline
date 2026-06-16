#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.rl"
/* Pattern 30: Scanner Tokenization — |* patterns with => actions */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.c"
static const signed char _p30_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1,
	3, 1, 4, 1, 5, 0
};

static const signed char _p30_key_offsets[] = {
	0, 0, 14, 17, 19, 0
};

static const char _p30_trans_keys[] = {
	32, 45, 47, 95, 9, 10, 42, 43,
	48, 57, 65, 90, 97, 122, 32, 9,
	10, 48, 57, 95, 48, 57, 65, 90,
	97, 122, 0
};

static const signed char _p30_single_lengths[] = {
	0, 4, 1, 0, 1, 0
};

static const signed char _p30_range_lengths[] = {
	0, 5, 1, 1, 3, 0
};

static const signed char _p30_index_offsets[] = {
	0, 0, 10, 13, 15, 0
};

static const signed char _p30_cond_targs[] = {
	2, 1, 1, 4, 2, 1, 3, 4,
	4, 0, 2, 2, 1, 3, 1, 4,
	4, 4, 4, 1, 0, 1, 1, 1,
	1, 0
};

static const signed char _p30_cond_actions[] = {
	0, 5, 5, 0, 0, 5, 0, 0,
	0, 0, 0, 0, 11, 0, 9, 0,
	0, 0, 0, 7, 0, 0, 11, 9,
	7, 0
};

static const signed char _p30_to_state_actions[] = {
	0, 1, 0, 0, 0, 0
};

static const signed char _p30_from_state_actions[] = {
	0, 3, 0, 0, 0, 0
};

static const signed char _p30_eof_trans[] = {
	21, 22, 23, 24, 25, 0
};

static const int p30_start = 1;
static const int p30_first_final = 1;
static const int p30_error = 0;

static const int p30_en_main = 1;


#line 12 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.rl"

int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
	printf("Pattern 30: Scanner Tokenization\n\n");
	char buf[]="foo + 42 bar\n";
	p=buf;pe=p+strlen(buf);eof=pe;

#line 72 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.c"
	{
		cs = (int)p30_start;
		ts = 0;
		te = 0;
	}
	
#line 17 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.rl"


#line 79 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe && p != eof )
			goto _out;
		_acts = ( _p30_actions + (_p30_from_state_actions[cs]));
		_nacts = (unsigned int)(*( _acts));
		_acts += 1;
		while ( _nacts > 0 ) {
			switch ( (*( _acts)) ) {
				case 1:  {
						{
#line 1 "NONE"
						{ts = p;}}
					
#line 98 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.c"

					break; 
				}
			}
			_nacts -= 1;
			_acts += 1;
		}
		
		if ( p == eof ) {
			if ( _p30_eof_trans[cs] > 0 ) {
				_trans = (unsigned int)_p30_eof_trans[cs] - 1;
			}
		}
		else {
			_keys = ( _p30_trans_keys + (_p30_key_offsets[cs]));
			_trans = (unsigned int)_p30_index_offsets[cs];
			
			_klen = (int)_p30_single_lengths[cs];
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
			
			_klen = (int)_p30_range_lengths[cs];
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
		cs = (int)_p30_cond_targs[_trans];
		
		if ( _p30_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p30_actions + (_p30_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 2:  {
							{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.rl"
							{te = p+1;{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.rl"
									printf("SYM:%c ",*ts); }
							}}
						
#line 182 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.c"

						break; 
					}
					case 3:  {
							{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.rl"
							{te = p;p = p - 1;{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.rl"
									printf("IDENT:%.*s ",(int)(te-ts),ts); }
							}}
						
#line 193 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.c"

						break; 
					}
					case 4:  {
							{
#line 8 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.rl"
							{te = p;p = p - 1;{
#line 8 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.rl"
									printf("NUM:%.*s ",(int)(te-ts),ts); }
							}}
						
#line 204 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.c"

						break; 
					}
					case 5:  {
							{
#line 1 "-"
							{te = p;p = p - 1;}}
						
#line 212 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
		}
		
		if ( p == eof ) {
			if ( cs >= 1 )
				goto _out;
		}
		else {
			_acts = ( _p30_actions + (_p30_to_state_actions[cs]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) ) {
					case 0:  {
							{
#line 1 "NONE"
							{ts = 0;}}
						
#line 237 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.c"

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
	
#line 18 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/30.rl"

	printf("\n");
	return 0;}
