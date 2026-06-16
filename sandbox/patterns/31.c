#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.rl"
/* Pattern 31: Silent Scanner — whitespace/comments consumed silently */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.c"
static const signed char _p31_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1,
	3, 1, 4, 1, 5, 0
};

static const signed char _p31_key_offsets[] = {
	0, 0, 1, 2, 12, 15, 17, 0
};

static const char _p31_trans_keys[] = {
	47, 10, 32, 47, 9, 10, 48, 57,
	65, 90, 97, 122, 32, 9, 10, 48,
	57, 65, 90, 97, 122, 0
};

static const signed char _p31_single_lengths[] = {
	0, 1, 1, 2, 1, 0, 0, 0
};

static const signed char _p31_range_lengths[] = {
	0, 0, 0, 4, 1, 1, 2, 0
};

static const signed char _p31_index_offsets[] = {
	0, 0, 2, 4, 11, 14, 16, 0
};

static const signed char _p31_cond_targs[] = {
	2, 0, 3, 2, 4, 1, 4, 5,
	6, 6, 0, 4, 4, 3, 5, 3,
	6, 6, 3, 0, 1, 2, 3, 3,
	3, 3, 0
};

static const signed char _p31_cond_actions[] = {
	0, 0, 5, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 7, 0, 11,
	0, 0, 9, 0, 0, 0, 0, 7,
	11, 9, 0
};

static const signed char _p31_to_state_actions[] = {
	0, 0, 0, 1, 0, 0, 0, 0
};

static const signed char _p31_from_state_actions[] = {
	0, 0, 0, 3, 0, 0, 0, 0
};

static const signed char _p31_eof_trans[] = {
	20, 21, 22, 23, 24, 25, 26, 0
};

static const int p31_start = 3;
static const int p31_first_final = 3;
static const int p31_error = 0;

static const int p31_en_main = 3;


#line 12 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.rl"

int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
	printf("Pattern 31: Silent Scanner\n\n");
	char buf[]="hello // comment\n 123 world\n";
	p=buf;pe=p+strlen(buf);eof=pe;

#line 71 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.c"
	{
		cs = (int)p31_start;
		ts = 0;
		te = 0;
	}
	
#line 17 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.rl"


#line 78 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe && p != eof )
			goto _out;
		_acts = ( _p31_actions + (_p31_from_state_actions[cs]));
		_nacts = (unsigned int)(*( _acts));
		_acts += 1;
		while ( _nacts > 0 ) {
			switch ( (*( _acts)) ) {
				case 1:  {
						{
#line 1 "NONE"
						{ts = p;}}
					
#line 97 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.c"

					break; 
				}
			}
			_nacts -= 1;
			_acts += 1;
		}
		
		if ( p == eof ) {
			if ( _p31_eof_trans[cs] > 0 ) {
				_trans = (unsigned int)_p31_eof_trans[cs] - 1;
			}
		}
		else {
			_keys = ( _p31_trans_keys + (_p31_key_offsets[cs]));
			_trans = (unsigned int)_p31_index_offsets[cs];
			
			_klen = (int)_p31_single_lengths[cs];
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
			
			_klen = (int)_p31_range_lengths[cs];
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
		cs = (int)_p31_cond_targs[_trans];
		
		if ( _p31_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p31_actions + (_p31_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 2:  {
							{
#line 1 "-"
							{te = p+1;}}
						
#line 178 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.c"

						break; 
					}
					case 3:  {
							{
#line 1 "-"
							{te = p;p = p - 1;}}
						
#line 186 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.c"

						break; 
					}
					case 4:  {
							{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.rl"
							{te = p;p = p - 1;{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.rl"
									printf("WORD "); }
							}}
						
#line 197 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.c"

						break; 
					}
					case 5:  {
							{
#line 10 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.rl"
							{te = p;p = p - 1;{
#line 10 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.rl"
									printf("NUM "); }
							}}
						
#line 208 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.c"

						break; 
					}
				}
				_nacts -= 1;
				_acts += 1;
			}
			
		}
		
		if ( p == eof ) {
			if ( cs >= 3 )
				goto _out;
		}
		else {
			_acts = ( _p31_actions + (_p31_to_state_actions[cs]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) ) {
					case 0:  {
							{
#line 1 "NONE"
							{ts = 0;}}
						
#line 233 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.c"

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
	
#line 18 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/31.rl"

	printf("\n");
	return 0;}
