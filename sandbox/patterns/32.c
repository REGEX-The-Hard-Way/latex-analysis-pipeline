#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.rl"
/* Pattern 32: Scanner Priority — keyword before identifier (longest match) */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.c"
static const signed char _p32_actions[] = {
	0, 1, 0, 1, 1, 1, 6, 1,
	7, 1, 8, 1, 9, 2, 2, 3,
	2, 2, 4, 2, 2, 5, 0
};

static const signed char _p32_key_offsets[] = {
	0, 0, 11, 14, 16, 23, 31, 39,
	47, 55, 0
};

static const char _p32_trans_keys[] = {
	32, 105, 119, 9, 10, 48, 57, 65,
	90, 97, 122, 32, 9, 10, 48, 57,
	95, 48, 57, 65, 90, 97, 122, 95,
	102, 48, 57, 65, 90, 97, 122, 95,
	104, 48, 57, 65, 90, 97, 122, 95,
	105, 48, 57, 65, 90, 97, 122, 95,
	108, 48, 57, 65, 90, 97, 122, 95,
	101, 48, 57, 65, 90, 97, 122, 0
};

static const signed char _p32_single_lengths[] = {
	0, 3, 1, 0, 1, 2, 2, 2,
	2, 2, 0
};

static const signed char _p32_range_lengths[] = {
	0, 4, 1, 1, 3, 3, 3, 3,
	3, 3, 0
};

static const signed char _p32_index_offsets[] = {
	0, 0, 8, 11, 13, 18, 24, 30,
	36, 42, 0
};

static const signed char _p32_cond_targs[] = {
	2, 5, 6, 2, 3, 4, 4, 0,
	2, 2, 1, 3, 1, 4, 4, 4,
	4, 1, 4, 4, 4, 4, 4, 1,
	4, 7, 4, 4, 4, 1, 4, 8,
	4, 4, 4, 1, 4, 9, 4, 4,
	4, 1, 4, 4, 4, 4, 4, 1,
	0, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 0
};

static const signed char _p32_cond_actions[] = {
	0, 0, 0, 0, 0, 19, 19, 0,
	0, 0, 9, 0, 7, 19, 19, 19,
	19, 11, 19, 16, 19, 19, 19, 5,
	19, 0, 19, 19, 19, 5, 19, 0,
	19, 19, 19, 5, 19, 0, 19, 19,
	19, 5, 19, 13, 19, 19, 19, 5,
	0, 0, 9, 7, 11, 5, 5, 5,
	5, 5, 0
};

static const signed char _p32_to_state_actions[] = {
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 0, 0
};

static const signed char _p32_from_state_actions[] = {
	0, 3, 0, 0, 0, 0, 0, 0,
	0, 0, 0
};

static const signed char _p32_eof_trans[] = {
	49, 50, 51, 52, 53, 54, 55, 56,
	57, 58, 0
};

static const int p32_start = 1;
static const int p32_first_final = 1;
static const int p32_error = 0;

static const int p32_en_main = 1;


#line 13 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.rl"

int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
	printf("Pattern 32: Scanner Priority\n\n");
	char buf[]="while if while1 foo\n";
	p=buf;pe=p+strlen(buf);eof=pe;

#line 92 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.c"
	{
		cs = (int)p32_start;
		ts = 0;
		te = 0;
		act = 0;
	}
	
#line 18 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.rl"


#line 100 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe && p != eof )
			goto _out;
		_acts = ( _p32_actions + (_p32_from_state_actions[cs]));
		_nacts = (unsigned int)(*( _acts));
		_acts += 1;
		while ( _nacts > 0 ) {
			switch ( (*( _acts)) ) {
				case 1:  {
						{
#line 1 "NONE"
						{ts = p;}}
					
#line 119 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.c"

					break; 
				}
			}
			_nacts -= 1;
			_acts += 1;
		}
		
		if ( p == eof ) {
			if ( _p32_eof_trans[cs] > 0 ) {
				_trans = (unsigned int)_p32_eof_trans[cs] - 1;
			}
		}
		else {
			_keys = ( _p32_trans_keys + (_p32_key_offsets[cs]));
			_trans = (unsigned int)_p32_index_offsets[cs];
			
			_klen = (int)_p32_single_lengths[cs];
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
			
			_klen = (int)_p32_range_lengths[cs];
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
		cs = (int)_p32_cond_targs[_trans];
		
		if ( _p32_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p32_actions + (_p32_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 2:  {
							{
#line 1 "NONE"
							{te = p+1;}}
						
#line 200 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.c"

						break; 
					}
					case 3:  {
							{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.rl"
							{act = 1;}}
						
#line 208 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.c"

						break; 
					}
					case 4:  {
							{
#line 8 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.rl"
							{act = 2;}}
						
#line 216 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.c"

						break; 
					}
					case 5:  {
							{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.rl"
							{act = 3;}}
						
#line 224 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.c"

						break; 
					}
					case 6:  {
							{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.rl"
							{te = p;p = p - 1;{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.rl"
									printf("ID:%.*s ",(int)(te-ts),ts); }
							}}
						
#line 235 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.c"

						break; 
					}
					case 7:  {
							{
#line 10 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.rl"
							{te = p;p = p - 1;{
#line 10 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.rl"
									printf("NUM:%.*s ",(int)(te-ts),ts); }
							}}
						
#line 246 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.c"

						break; 
					}
					case 8:  {
							{
#line 1 "-"
							{te = p;p = p - 1;}}
						
#line 254 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.c"

						break; 
					}
					case 9:  {
							{
#line 1 "NONE"
							{switch( act ) {
									case 1:  {
										p = ((te))-1;
										{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.rl"
											printf("KW:while "); }
										break; 
									}
									case 2:  {
										p = ((te))-1;
										{
#line 8 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.rl"
											printf("KW:if "); }
										break; 
									}
									case 3:  {
										p = ((te))-1;
										{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.rl"
											printf("ID:%.*s ",(int)(te-ts),ts); }
										break; 
									}
								}}
						}
						
#line 285 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.c"

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
			_acts = ( _p32_actions + (_p32_to_state_actions[cs]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) ) {
					case 0:  {
							{
#line 1 "NONE"
							{ts = 0;}}
						
#line 310 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.c"

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
	
#line 19 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/32.rl"

	printf("\n");
	return 0;}
