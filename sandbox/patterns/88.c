#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/88.rl"
/* Pattern 88: Multi-Language Demo — C version showing Ruby/OCaml/ASM equivalents */
/* Ruby:   %% write data;  -> same directive, generates Ruby code
* OCaml:  %% write data;  -> generates OCaml code
* ASM:    %% write data;  -> ragel -G2 gives x86-64 ASM output
*/
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 10 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/88.c"
static const signed char _p88_actions[] = {
	0, 1, 0, 0
};

static const signed char _p88_key_offsets[] = {
	0, 0, 5, 11, 0
};

static const char _p88_trans_keys[] = {
	33, 48, 57, 97, 122, 10, 33, 48,
	57, 97, 122, 0
};

static const signed char _p88_single_lengths[] = {
	0, 1, 2, 0, 0
};

static const signed char _p88_range_lengths[] = {
	0, 2, 2, 0, 0
};

static const signed char _p88_index_offsets[] = {
	0, 0, 4, 9, 0
};

static const signed char _p88_cond_targs[] = {
	2, 2, 2, 0, 3, 2, 2, 2,
	0, 0, 0, 1, 2, 3, 0
};

static const signed char _p88_cond_actions[] = {
	1, 1, 1, 0, 0, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0
};

static const int p88_start = 1;
static const int p88_first_final = 3;
static const int p88_error = 0;

static const int p88_en_main = 1;


#line 13 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/88.rl"

int main(void){int cs; const char *p,*pe,*eof;
	struct {const char *s; int ok;} t[]={
		{"abc123!\n",1},
		{"hello987world!\n",1},
		{"ABC\n",0},
		{"\n",0},
	}; int n=4;
	printf("Pattern 88: Multi-Language Demo\n\n");
	printf("C version compiled. Ruby/OCaml/ASM would use same .rl with:\n");
	printf("  Ruby: ragel -R input.rl\n");
	printf("  OCaml: ragel -O input.rl\n");
	printf("  ASM x86-64: ragel -G2 input.rl\n\n");
	for(int i=0;i<n;i++){p=t[i].s;pe=p+strlen(p);eof=pe;
		printf("Input: %s",t[i].s);

#line 67 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/88.c"
	{
			cs = (int)p88_start;
		}
		
#line 28 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/88.rl"


#line 72 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/88.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			const signed char * _acts;
			unsigned int _nacts;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p88_trans_keys + (_p88_key_offsets[cs]));
			_trans = (unsigned int)_p88_index_offsets[cs];
			
			_klen = (int)_p88_single_lengths[cs];
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
			
			_klen = (int)_p88_range_lengths[cs];
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
			cs = (int)_p88_cond_targs[_trans];
			
			if ( _p88_cond_actions[_trans] != 0 ) {
				
				_acts = ( _p88_actions + (_p88_cond_actions[_trans]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 ) {
					switch ( (*( _acts)) )
					{
						case 0:  {
								{
#line 10 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/88.rl"
								printf("  char: %c\n", *p); }
							
#line 147 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/88.c"

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
		
#line 29 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/88.rl"

		int r=cs>=p88_first_final;
		printf("  -> %s (expect %s) %s\n",r?"ACCEPT":"FAIL",t[i].ok?"ACCEPT":"FAIL",r==t[i].ok?"ok":"FAIL");
		assert(r==t[i].ok);
	}
	printf("\nAll %d tests passed.\n",n);return 0;
}
