#line 1 "72.rl"
/* Pattern 72: EOF Actions — >eof $eof %eof <>eof on 'hello' machine */
#include <stdio.h>
#include <string.h>
#include <assert.h>


#line 7 "72.c"
static const signed char _p72_actions[] = {
	0, 2, 0, 1, 2, 1, 2, 2,
	1, 3, 0
};

static const signed char _p72_key_offsets[] = {
	0, 0, 1, 2, 3, 4, 5, 0
};

static const char _p72_trans_keys[] = {
	104, 101, 108, 108, 111, 0
};

static const signed char _p72_single_lengths[] = {
	0, 1, 1, 1, 1, 1, 0, 0
};

static const signed char _p72_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0
};

static const signed char _p72_index_offsets[] = {
	0, 0, 2, 4, 6, 8, 10, 0
};

static const signed char _p72_cond_targs[] = {
	2, 0, 3, 0, 4, 0, 5, 0,
	6, 0, 0, 0, 1, 2, 3, 4,
	5, 6, 0
};

static const signed char _p72_cond_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 7, 7, 7,
	7, 4, 0
};

static const signed char _p72_eof_trans[] = {
	12, 13, 14, 15, 16, 17, 18, 0
};

static const int p72_start = 1;
static const int p72_first_final = 6;
static const int p72_error = 0;

static const int p72_en_main = 1;


#line 18 "72.rl"


void test(const char *s, int expect) {
	int cs; const char *p,*pe,*eof;
	p=s; pe=p+strlen(s); eof=pe;
	printf("  \"%s\" -> ", s);

#line 61 "72.c"
	{
		cs = (int)p72_start;
	}
	
#line 24 "72.rl"


#line 66 "72.c"
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
			if ( _p72_eof_trans[cs] > 0 ) {
				_trans = (unsigned int)_p72_eof_trans[cs] - 1;
			}
		}
		else {
			_keys = ( _p72_trans_keys + (_p72_key_offsets[cs]));
			_trans = (unsigned int)_p72_index_offsets[cs];
			
			_klen = (int)_p72_single_lengths[cs];
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
			
			_klen = (int)_p72_range_lengths[cs];
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
		cs = (int)_p72_cond_targs[_trans];
		
		if ( _p72_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p72_actions + (_p72_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 8 "72.rl"
							printf(" >eof"); }
						
#line 148 "72.c"

						break; 
					}
					case 1:  {
							{
#line 9 "72.rl"
							printf(" $eof"); }
						
#line 156 "72.c"

						break; 
					}
					case 2:  {
							{
#line 10 "72.rl"
							printf(" %%eof"); }
						
#line 164 "72.c"

						break; 
					}
					case 3:  {
							{
#line 11 "72.rl"
							printf(" <>eof"); }
						
#line 172 "72.c"

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
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
		}
		_out: {}
	}
	
#line 25 "72.rl"

	int r = cs >= p72_first_final;
	printf(" %s ok\n", r?"ACCEPT":"FAIL");
	assert(r == expect);
}

int main(void){
	printf("Pattern 72: EOF Actions (eof=pe)\n\n");
	test("hello", 1);
	test("hell",  0);
	test("h",     0);
	test("",      0);
	test("hello!",0);
	printf("\nAll 5 tests passed.\n");return 0;}
