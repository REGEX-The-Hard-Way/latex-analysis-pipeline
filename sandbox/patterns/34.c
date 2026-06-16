#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.rl"
/* Pattern 34: Scanner Leave — % actions fire before => in scanner */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.c"
static const signed char _p34_actions[] = {
	0, 1, 0, 1, 2, 1, 3, 1,
	6, 2, 1, 5, 3, 0, 1, 4,
	0
};

static const signed char _p34_key_offsets[] = {
	0, 0, 6, 9, 11, 14, 17, 20,
	23, 0
};

static const char _p34_trans_keys[] = {
	32, 104, 9, 10, 97, 122, 32, 9,
	10, 97, 122, 101, 97, 122, 108, 97,
	122, 108, 97, 122, 111, 97, 122, 97,
	122, 0
};

static const signed char _p34_single_lengths[] = {
	0, 2, 1, 0, 1, 1, 1, 1,
	0, 0
};

static const signed char _p34_range_lengths[] = {
	0, 2, 1, 1, 1, 1, 1, 1,
	1, 0
};

static const signed char _p34_index_offsets[] = {
	0, 0, 5, 8, 10, 13, 16, 19,
	22, 0
};

static const signed char _p34_cond_targs[] = {
	2, 4, 2, 3, 0, 2, 2, 1,
	3, 1, 5, 3, 1, 6, 3, 1,
	7, 3, 1, 8, 3, 1, 3, 1,
	0, 1, 1, 1, 1, 1, 1, 1,
	1, 0
};

static const signed char _p34_cond_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 7,
	0, 9, 0, 0, 9, 0, 0, 9,
	0, 0, 9, 0, 0, 9, 1, 12,
	0, 0, 7, 9, 9, 9, 9, 9,
	12, 0
};

static const signed char _p34_to_state_actions[] = {
	0, 3, 0, 0, 0, 0, 0, 0,
	0, 0
};

static const signed char _p34_from_state_actions[] = {
	0, 5, 0, 0, 0, 0, 0, 0,
	0, 0
};

static const signed char _p34_eof_trans[] = {
	25, 26, 27, 28, 29, 30, 31, 32,
	33, 0
};

static const int p34_start = 1;
static const int p34_first_final = 1;
static const int p34_error = 0;

static const int p34_en_main = 1;


#line 11 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.rl"

int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
	printf("Pattern 34: Scanner Leave %% Actions\n\n");
	char buf[]="hello world\n";
	p=buf;pe=p+strlen(buf);eof=pe;

#line 82 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.c"
	{
		cs = (int)p34_start;
		ts = 0;
		te = 0;
	}
	
#line 16 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.rl"


#line 89 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe && p != eof )
			goto _out;
		_acts = ( _p34_actions + (_p34_from_state_actions[cs]));
		_nacts = (unsigned int)(*( _acts));
		_acts += 1;
		while ( _nacts > 0 ) {
			switch ( (*( _acts)) ) {
				case 3:  {
						{
#line 1 "NONE"
						{ts = p;}}
					
#line 108 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.c"

					break; 
				}
			}
			_nacts -= 1;
			_acts += 1;
		}
		
		if ( p == eof ) {
			if ( _p34_eof_trans[cs] > 0 ) {
				_trans = (unsigned int)_p34_eof_trans[cs] - 1;
			}
		}
		else {
			_keys = ( _p34_trans_keys + (_p34_key_offsets[cs]));
			_trans = (unsigned int)_p34_index_offsets[cs];
			
			_klen = (int)_p34_single_lengths[cs];
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
			
			_klen = (int)_p34_range_lengths[cs];
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
		cs = (int)_p34_cond_targs[_trans];
		
		if ( _p34_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p34_actions + (_p34_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.rl"
							printf("(leave_hello) ");}
						
#line 189 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.c"

						break; 
					}
					case 1:  {
							{
#line 8 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.rl"
							printf("(leave_word) ");}
						
#line 197 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.c"

						break; 
					}
					case 4:  {
							{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.rl"
							{te = p;p = p - 1;{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.rl"
									printf("TOK:hello\n"); }
							}}
						
#line 208 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.c"

						break; 
					}
					case 5:  {
							{
#line 8 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.rl"
							{te = p;p = p - 1;{
#line 8 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.rl"
									printf("TOK:word\n"); }
							}}
						
#line 219 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.c"

						break; 
					}
					case 6:  {
							{
#line 1 "-"
							{te = p;p = p - 1;}}
						
#line 227 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.c"

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
			_acts = ( _p34_actions + (_p34_to_state_actions[cs]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) ) {
					case 2:  {
							{
#line 1 "NONE"
							{ts = 0;}}
						
#line 252 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.c"

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
	
#line 17 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/34.rl"

	return 0;}
