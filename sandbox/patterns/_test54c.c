#line 1 "_test54c.rl"
#include <stdio.h>
#include <string.h>
struct nfa_bp_rec { long state; const char *p; long popTrans; long q; };
struct nfa_bp_rec nfa_bp[1024];
long nfa_len = 0;
long nfa_count = 0;

#line 8 "_test54c.c"
static const int p54_start = 1;
static const int p54_first_final = 6;
static const int p54_error = 0;

static const int p54_en_main = 1;

static const signed char _p54_nfa_offsets[] = {
	0, 1, 0, 3, 0, 0, 0, 0
};


#line 15 "_test54c.rl"

int main(void){int cs;long q=0;const char *p,*pe,*eof; setbuf(stdout,NULL);
	for(int len=0;len<=10;len++){char buf[32];memset(buf,'1',len);buf[len]='\n';buf[len+1]=0;
		p=buf;pe=p+strlen(buf);eof=pe;nfa_len=0;

#line 23 "_test54c.c"
	{
			cs = (int)p54_start;
			nfa_len = 0;
		}
		
#line 19 "_test54c.rl"


#line 29 "_test54c.c"
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
#line 8 "_test54c.rl"
					nfa_bp[nfa_len].q = q; }
				
#line 63 "_test54c.c"
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
#line 8 "_test54c.rl"
					nfa_bp[nfa_len].q = q; }
				
#line 105 "_test54c.c"
	nfa_len += 1;
				nfa_bp[nfa_len].state = 2;
				nfa_bp[nfa_len].p = p;
				nfa_bp[nfa_len].popTrans = 6;
					{
#line 8 "_test54c.rl"
					nfa_bp[nfa_len].q = q; }
				
#line 113 "_test54c.c"
	nfa_len += 1;
				nfa_bp[nfa_len].state = 4;
				nfa_bp[nfa_len].p = p;
				nfa_bp[nfa_len].popTrans = 5;
					{
#line 8 "_test54c.rl"
					nfa_bp[nfa_len].q = q; }
				
#line 121 "_test54c.c"
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
#line 190 "_test54c.c"
;
					if ( !_pop_test )
						break;
					_pop_test = ( ({ q = 0; 1; }) )
#line 194 "_test54c.c"
;
					break; 
				}
				case 5:  {
					_pop_test = ( ({ q = nfa_bp[nfa_len].q; 1; }) )
#line 199 "_test54c.c"
;
					if ( !_pop_test )
						break;
					_pop_test = ( ({ 1; }) )
#line 203 "_test54c.c"
;
					break; 
				}
				case 6:  {
					_pop_test = ( ({ q = nfa_bp[nfa_len].q; 1; }) )
#line 208 "_test54c.c"
;
					if ( !_pop_test )
						break;
					_pop_test = ( ({ ++q < 2; }) )
#line 212 "_test54c.c"
;
					break; 
				}
				case 7:  {
					_pop_test = ( ({ q = nfa_bp[nfa_len].q; 1; }) )
#line 217 "_test54c.c"
;
					if ( !_pop_test )
						break;
					_pop_test = ( ({ ++q >= 2; }) )
#line 221 "_test54c.c"
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
		
#line 20 "_test54c.rl"

		printf("len=%d -> cs=%d first=%d p-pe=%ld\n",len,cs,p54_first_final,p-pe);}
	return 0;}
