#line 1 "54.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>
struct nfa_bp_rec { long state; const char *p; long popTrans; long q; };
struct nfa_bp_rec nfa_bp[1024];
long nfa_len = 0;
long nfa_count = 0;

#line 9 "54.c"
static const int p54_start = 1;
static const int p54_first_final = 6;
static const int p54_error = 0;

static const int p54_en_main = 1;

static const signed char _p54_nfa_offsets[] = {
	0, 1, 0, 3, 0, 0, 0, 0
};


#line 16 "54.rl"

int main(void){int cs;long q=0;const char *p,*pe,*eof;
	static const char *t[]={"1\n","12\n","123\n","1234\n"};static const int e[]={1,0,0,0};int n=4;
	printf("Pattern 54: :nfa(...): NFA Repetition\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;nfa_len=0;

#line 25 "54.c"
	{
			cs = (int)p54_start;
			nfa_len = 0;
		}
		
#line 21 "54.rl"


#line 31 "54.c"
{
			int _pop_test;
			_resume: {}
			switch ( cs ) {
				case 1:
				goto st_case_1;
				case 0:
				goto st_case_0;
				case 2:
				goto st_case_2;
				case 3:
				goto st_case_3;
				case 5:
				goto st_case_5;
				case 6:
				goto st_case_6;
				case 4:
				goto st_case_4;
			}
			_st1:
			if ( p == eof )
				goto _pop1;
			p+= 1;
			st_case_1:
			if ( p == pe && p != eof )
				goto _out1;
			if ( _p54_nfa_offsets[1] != 0 ) {
				nfa_bp[nfa_len].state = 2;
				nfa_bp[nfa_len].p = p;
				nfa_bp[nfa_len].popTrans = 4;
					{
#line 9 "54.rl"
					nfa_bp[nfa_len].q = q; }
				
#line 65 "54.c"
	nfa_len += 1;
			}
			if ( p == eof ) {
				goto _st1;}
			else {
				goto _st0;
			}
			_st0:
			if ( p == eof )
				goto _pop0;
			st_case_0:
			goto _pop0;
			_st2:
			if ( p == eof )
				goto _pop2;
			p+= 1;
			st_case_2:
			if ( p == pe && p != eof )
				goto _out2;
			if ( p == eof ) {
				goto _st2;}
			else {
				if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
					goto _st3;
				}
				goto _st0;
			}
			_st3:
			if ( p == eof )
				goto _pop3;
			p+= 1;
			st_case_3:
			if ( p == pe && p != eof )
				goto _out3;
			if ( _p54_nfa_offsets[3] != 0 ) {
				nfa_bp[nfa_len].state = 5;
				nfa_bp[nfa_len].p = p;
				nfa_bp[nfa_len].popTrans = 7;
					{
#line 9 "54.rl"
					nfa_bp[nfa_len].q = q; }
				
#line 107 "54.c"
	nfa_len += 1;
				nfa_bp[nfa_len].state = 2;
				nfa_bp[nfa_len].p = p;
				nfa_bp[nfa_len].popTrans = 6;
					{
#line 9 "54.rl"
					nfa_bp[nfa_len].q = q; }
				
#line 115 "54.c"
	nfa_len += 1;
				nfa_bp[nfa_len].state = 4;
				nfa_bp[nfa_len].p = p;
				nfa_bp[nfa_len].popTrans = 5;
					{
#line 9 "54.rl"
					nfa_bp[nfa_len].q = q; }
				
#line 123 "54.c"
	nfa_len += 1;
			}
			if ( p == eof ) {
				goto _st3;}
			else {
				goto _st0;
			}
			_st5:
			if ( p == eof )
				goto _pop5;
			p+= 1;
			st_case_5:
			if ( p == pe && p != eof )
				goto _out5;
			if ( p == eof ) {
				goto _st5;}
			else {
				if ( ( (*( p))) == 10 ) {
					goto _st6;
				}
				goto _st0;
			}
			_st6:
			if ( p == eof )
				goto _out6;
			p+= 1;
			st_case_6:
			if ( p == pe && p != eof )
				goto _out6;
			if ( p == eof ) {
				goto _st6;}
			else {
				goto _st0;
			}
			_st4:
			if ( p == eof )
				goto _pop4;
			p+= 1;
			st_case_4:
			if ( p == pe && p != eof )
				goto _out4;
			if ( p == eof ) {
				goto _st4;}
			else {
				goto _st0;
			}
			_out1: cs = 1; goto _out; 
			_pop1: cs = 1; goto _pop; 
			_pop0: cs = 0; goto _pop; 
			_out2: cs = 2; goto _out; 
			_pop2: cs = 2; goto _pop; 
			_out3: cs = 3; goto _out; 
			_pop3: cs = 3; goto _pop; 
			_out5: cs = 5; goto _out; 
			_pop5: cs = 5; goto _pop; 
			_out6: cs = 6; goto _out; 
			_out4: cs = 4; goto _out; 
			_pop4: cs = 4; goto _pop; 
			_pop: {}
			if ( nfa_len == 0 )
				goto _out;
			
			nfa_count += 1;
			nfa_len -= 1;
			p = nfa_bp[nfa_len].p;
			_pop_test = 1;
			switch ( nfa_bp[nfa_len].popTrans ) {
				case 4:  {
					_pop_test = ( ({ q = nfa_bp[nfa_len].q; 1; }) )
#line 192 "54.c"
;
					if ( !_pop_test )
						break;
					_pop_test = ( ({ q = 0; 1; }) )
#line 196 "54.c"
;
					break; 
				}
				case 5:  {
					_pop_test = ( ({ q = nfa_bp[nfa_len].q; 1; }) )
#line 201 "54.c"
;
					if ( !_pop_test )
						break;
					_pop_test = ( ({ 1; }) )
#line 205 "54.c"
;
					break; 
				}
				case 6:  {
					_pop_test = ( ({ q = nfa_bp[nfa_len].q; 1; }) )
#line 210 "54.c"
;
					if ( !_pop_test )
						break;
					_pop_test = ( ({ ++q < 1; }) )
#line 214 "54.c"
;
					break; 
				}
				case 7:  {
					_pop_test = ( ({ q = nfa_bp[nfa_len].q; 1; }) )
#line 219 "54.c"
;
					if ( !_pop_test )
						break;
					_pop_test = ( ({ ++q >= 2; }) )
#line 223 "54.c"
;
					break; 
				}
			}
			
			if ( _pop_test )
				cs = nfa_bp[nfa_len].state;
			else
				cs = 0;
			goto _resume;
			_out: {}
		}
		
#line 22 "54.rl"

		int r=cs>=p54_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
