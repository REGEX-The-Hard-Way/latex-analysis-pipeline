#line 1 "55.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>
struct nfa_bp_rec { long state; const char *p; long popTrans; };
struct nfa_bp_rec nfa_bp[1024];
long nfa_len = 0;
long nfa_count = 0;

#line 9 "55.c"
static const int p55_start = 1;
static const int p55_first_final = 8;
static const int p55_error = 0;

static const int p55_en_main = 1;

static const signed char _p55_nfa_offsets[] = {
	0, 0, 1, 3, 5, 7, 9, 11,
	0, 0, 0
};


#line 14 "55.rl"

int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
	static const char *t[]={"hello world all"};static const int e[]={1};int n=1;
	printf("Pattern 55: NFA Multi-Match\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;nfa_len=0;

#line 26 "55.c"
	{
			cs = (int)p55_start;
			nfa_len = 0;
			ts = 0;
			te = 0;
		}
		
#line 19 "55.rl"


#line 34 "55.c"
{
			_resume: {}
			switch ( cs ) {
				case 1:
				goto st_case_1;
				case 0:
				goto st_case_0;
				case 2:
				goto st_case_2;
				case 8:
				goto st_case_8;
				case 3:
				goto st_case_3;
				case 4:
				goto st_case_4;
				case 5:
				goto st_case_5;
				case 6:
				goto st_case_6;
				case 7:
				goto st_case_7;
				case 9:
				goto st_case_9;
			}
			_ctr2:
				{
#line 12 "55.rl"
				{te = p+1;nfa_len = 0; {
#line 12 "55.rl"
						printf("<sp> "); }
				}}
			
#line 66 "55.c"

			goto _st1;
			_ctr10:
				{
#line 11 "55.rl"
				{te = p;p = p - 1;nfa_len = 0; {
#line 11 "55.rl"
						printf("word "); }
				}}
			
#line 76 "55.c"

			goto _st1;
			_ctr12:
				{
#line 10 "55.rl"
				{te = p;p = p - 1;nfa_len = 0; {
#line 10 "55.rl"
						printf("hello "); }
				}}
			
#line 86 "55.c"

			goto _st1;
			_st1:
			if ( p == eof )
				goto _pop1;
				{
#line 1 "NONE"
				{ts = 0;}}
			
#line 95 "55.c"

			p+= 1;
			st_case_1:
			if ( p == pe && p != eof )
				goto _out1;
				{
#line 1 "NONE"
				{ts = p;}}
			
#line 104 "55.c"

			if ( p == eof ) {
				goto _st1;}
			else {
				switch( ( (*( p))) ) {
					case 32: {
						goto _ctr2;
					}
					case 104: {
						goto _st3;
					}
				}
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _st2;
				}
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
			if ( _p55_nfa_offsets[2] != 0 ) {
				nfa_bp[nfa_len].state = 8;
				nfa_bp[nfa_len].p = p;
				nfa_len += 1;
			}
			if ( p == eof ) {
				goto _st2;}
			else {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _st2;
				}
				goto _st0;
			}
			_ctr9:
				{
#line 11 "55.rl"
				{te = p;nfa_len = 0; {
#line 11 "55.rl"
						printf("word "); }
				}}
			
#line 154 "55.c"

			goto _st8;
			_st8:
			if ( p == eof )
				goto _out8;
			p+= 1;
			st_case_8:
			if ( p == pe && p != eof )
				goto _out8;
			if ( p == eof ) {
				goto _ctr9;}
			else {
				goto _ctr10;
			}
			_st3:
			if ( p == eof )
				goto _pop3;
			p+= 1;
			st_case_3:
			if ( p == pe && p != eof )
				goto _out3;
			if ( _p55_nfa_offsets[3] != 0 ) {
				nfa_bp[nfa_len].state = 8;
				nfa_bp[nfa_len].p = p;
				nfa_len += 1;
			}
			if ( p == eof ) {
				goto _st3;}
			else {
				if ( ( (*( p))) == 101 ) {
					goto _st4;
				}
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _st2;
				}
				goto _st0;
			}
			_st4:
			if ( p == eof )
				goto _pop4;
			p+= 1;
			st_case_4:
			if ( p == pe && p != eof )
				goto _out4;
			if ( _p55_nfa_offsets[4] != 0 ) {
				nfa_bp[nfa_len].state = 8;
				nfa_bp[nfa_len].p = p;
				nfa_len += 1;
			}
			if ( p == eof ) {
				goto _st4;}
			else {
				if ( ( (*( p))) == 108 ) {
					goto _st5;
				}
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _st2;
				}
				goto _st0;
			}
			_st5:
			if ( p == eof )
				goto _pop5;
			p+= 1;
			st_case_5:
			if ( p == pe && p != eof )
				goto _out5;
			if ( _p55_nfa_offsets[5] != 0 ) {
				nfa_bp[nfa_len].state = 8;
				nfa_bp[nfa_len].p = p;
				nfa_len += 1;
			}
			if ( p == eof ) {
				goto _st5;}
			else {
				if ( ( (*( p))) == 108 ) {
					goto _st6;
				}
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _st2;
				}
				goto _st0;
			}
			_st6:
			if ( p == eof )
				goto _pop6;
			p+= 1;
			st_case_6:
			if ( p == pe && p != eof )
				goto _out6;
			if ( _p55_nfa_offsets[6] != 0 ) {
				nfa_bp[nfa_len].state = 8;
				nfa_bp[nfa_len].p = p;
				nfa_len += 1;
			}
			if ( p == eof ) {
				goto _st6;}
			else {
				if ( ( (*( p))) == 111 ) {
					goto _st7;
				}
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _st2;
				}
				goto _st0;
			}
			_st7:
			if ( p == eof )
				goto _pop7;
			p+= 1;
			st_case_7:
			if ( p == pe && p != eof )
				goto _out7;
			if ( _p55_nfa_offsets[7] != 0 ) {
				nfa_bp[nfa_len].state = 9;
				nfa_bp[nfa_len].p = p;
				nfa_len += 1;
			}
			if ( p == eof ) {
				goto _st7;}
			else {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _st2;
				}
				goto _st0;
			}
			_ctr11:
				{
#line 10 "55.rl"
				{te = p;nfa_len = 0; {
#line 10 "55.rl"
						printf("hello "); }
				}}
			
#line 288 "55.c"

			goto _st9;
			_st9:
			if ( p == eof )
				goto _out9;
			p+= 1;
			st_case_9:
			if ( p == pe && p != eof )
				goto _out9;
			if ( p == eof ) {
				goto _ctr11;}
			else {
				goto _ctr12;
			}
			_out1: cs = 1; goto _out; 
			_pop1: cs = 1; goto _pop; 
			_pop0: cs = 0; goto _pop; 
			_out2: cs = 2; goto _out; 
			_pop2: cs = 2; goto _pop; 
			_out8: cs = 8; goto _out; 
			_out3: cs = 3; goto _out; 
			_pop3: cs = 3; goto _pop; 
			_out4: cs = 4; goto _out; 
			_pop4: cs = 4; goto _pop; 
			_out5: cs = 5; goto _out; 
			_pop5: cs = 5; goto _pop; 
			_out6: cs = 6; goto _out; 
			_pop6: cs = 6; goto _pop; 
			_out7: cs = 7; goto _out; 
			_pop7: cs = 7; goto _pop; 
			_out9: cs = 9; goto _out; 
			_pop: {}
			if ( nfa_len == 0 )
				goto _out;
			
			nfa_count += 1;
			nfa_len -= 1;
			p = nfa_bp[nfa_len].p;
			cs = nfa_bp[nfa_len].state;
			goto _resume;
			_out: {}
		}
		
#line 20 "55.rl"

		int r=cs>=p55_first_final;printf("\n  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
