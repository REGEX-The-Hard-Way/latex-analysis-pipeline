#line 1 "79.rl"
/* Pattern 79: RPN Calculator — digit+ push, operators pop-push, print stack */
#include <stdio.h>
#include <string.h>
#include <assert.h>

int stack[32];
int sp = 0;
const char *num_start;

void push(int v) { stack[sp++] = v; }
int pop() { return stack[--sp]; }


#line 14 "79.c"
static const signed char _p79_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1,
	3, 1, 4, 1, 5, 0
};

static const signed char _p79_key_offsets[] = {
	0, 0, 3, 8, 0
};

static const char _p79_trans_keys[] = {
	32, 9, 13, 32, 9, 13, 48, 57,
	32, 42, 43, 45, 47, 9, 13, 48,
	57, 0
};

static const signed char _p79_single_lengths[] = {
	0, 1, 1, 5, 0
};

static const signed char _p79_range_lengths[] = {
	0, 1, 2, 2, 0
};

static const signed char _p79_index_offsets[] = {
	0, 0, 3, 7, 0
};

static const signed char _p79_cond_targs[] = {
	3, 3, 0, 3, 3, 2, 0, 3,
	1, 1, 1, 1, 3, 2, 0, 0,
	1, 2, 3, 0
};

static const signed char _p79_cond_actions[] = {
	0, 0, 0, 3, 3, 0, 0, 0,
	9, 5, 7, 11, 0, 1, 0, 0,
	0, 0, 0, 0
};

static const int p79_start = 3;
static const int p79_first_final = 3;
static const int p79_error = 0;

static const int p79_en_main = 3;


#line 38 "79.rl"

int main(void){int cs;const char *p,*pe,*eof;
	printf("Pattern 79: RPN Calculator\n\n");
	const char *expr = "3 4 + 2 * 5 /\n";
	p=expr;pe=p+strlen(p);eof=pe;

#line 65 "79.c"
	{
		cs = (int)p79_start;
	}
	
#line 43 "79.rl"


#line 70 "79.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p79_trans_keys + (_p79_key_offsets[cs]));
		_trans = (unsigned int)_p79_index_offsets[cs];
		
		_klen = (int)_p79_single_lengths[cs];
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
		
		_klen = (int)_p79_range_lengths[cs];
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
		cs = (int)_p79_cond_targs[_trans];
		
		if ( _p79_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p79_actions + (_p79_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 15 "79.rl"
							num_start = p; }
						
#line 145 "79.c"

						break; 
					}
					case 1:  {
							{
#line 16 "79.rl"
							
							int v = 0;
							const char *c = num_start;
							while (c < p && *c >= '0' && *c <= '9') { v = v*10 + (*c-'0'); c++; }
							push(v);
							printf("push %d\n", v);
						}
						
#line 159 "79.c"

						break; 
					}
					case 2:  {
							{
#line 23 "79.rl"
							int b=pop(); int a=pop(); push(a+b); printf("add -> %d\n", a+b); }
						
#line 167 "79.c"

						break; 
					}
					case 3:  {
							{
#line 24 "79.rl"
							int b=pop(); int a=pop(); push(a-b); printf("sub -> %d\n", a-b); }
						
#line 175 "79.c"

						break; 
					}
					case 4:  {
							{
#line 25 "79.rl"
							int b=pop(); int a=pop(); push(a*b); printf("mul -> %d\n", a*b); }
						
#line 183 "79.c"

						break; 
					}
					case 5:  {
							{
#line 26 "79.rl"
							int b=pop(); int a=pop(); push(a/b); printf("div -> %d\n", a/b); }
						
#line 191 "79.c"

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
	
#line 44 "79.rl"

	printf("Stack top = %d\n", stack[sp-1]);
	printf("-> %s ok\n",cs>=p79_first_final?"ACCEPT":"FAIL");
	assert(stack[sp-1]==2);
	printf("\nAll tests passed.\n");return 0;}
