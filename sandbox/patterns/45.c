#line 1 "45.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "45.c"
static const int p45_start = 1;
static const int p45_first_final = 1;
static const int p45_error = 0;

static const int p45_en_main = 1;


#line 4 "45.rl"

int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
	static const char *t[]={"abc 123","abc 123 xyz"};static const int e[]={1,1};int n=2;
	printf("Pattern 45: => vs Explicit ts/te\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 17 "45.c"
	{
			cs = (int)p45_start;
			ts = 0;
			te = 0;
		}
		
#line 9 "45.rl"


#line 24 "45.c"
{
			switch ( cs ) {
				case 1:
				goto st_case_1;
				case 0:
				goto st_case_0;
				case 2:
				goto st_case_2;
				case 3:
				goto st_case_3;
			}
			_ctr2:
				{
#line 4 "45.rl"
				{te = p+1;{
#line 4 "45.rl"
						printf("<space>\n"); }
				}}
			
#line 43 "45.c"

			goto _st1;
			_ctr5:
				{
#line 4 "45.rl"
				te=p;}
			
#line 50 "45.c"

				{
#line 4 "45.rl"
				{te = p;p = p - 1;{
#line 4 "45.rl"
						printf("ts/te num: %.*s\n", (int)(te-ts), ts); }
				}}
			
#line 58 "45.c"

			goto _st1;
			_ctr7:
				{
#line 4 "45.rl"
				{te = p;p = p - 1;{
#line 4 "45.rl"
						printf("=> word: %.*s\n", (int)(te-ts), ts); }
				}}
			
#line 68 "45.c"

			goto _st1;
			_st1:
			if ( p == eof )
				goto _out1;
				{
#line 1 "NONE"
				{ts = 0;}}
			
#line 77 "45.c"

			p+= 1;
			st_case_1:
			if ( p == pe && p != eof )
				goto _out1;
				{
#line 1 "NONE"
				{ts = p;}}
			
#line 86 "45.c"

			if ( p == eof ) {
				goto _st1;}
			else {
				if ( ( (*( p))) == 32 ) {
					goto _ctr2;
				}
				if ( ( (*( p))) < 48 ) {
					if ( 9 <= ( (*( p))) && ( (*( p))) <= 13 ) {
						goto _ctr2;
					}
				} else if ( ( (*( p))) > 57 ) {
					if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
						goto _st3;
					}
				} else {
					goto _ctr3;
				}
				goto _st0;
			}
			_st0:
			if ( p == eof )
				goto _out0;
			st_case_0:
			goto _out0;
			_ctr3:
				{
#line 4 "45.rl"
				ts=p;}
			
#line 116 "45.c"

			goto _st2;
			_st2:
			if ( p == eof )
				goto _out2;
			p+= 1;
			st_case_2:
			if ( p == pe && p != eof )
				goto _out2;
			if ( p == eof ) {
				goto _ctr5;}
			else {
				if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
					goto _st2;
				}
				goto _ctr5;
			}
			_st3:
			if ( p == eof )
				goto _out3;
			p+= 1;
			st_case_3:
			if ( p == pe && p != eof )
				goto _out3;
			if ( p == eof ) {
				goto _ctr7;}
			else {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _st3;
				}
				goto _ctr7;
			}
			_out1: cs = 1; goto _out; 
			_out0: cs = 0; goto _out; 
			_out2: cs = 2; goto _out; 
			_out3: cs = 3; goto _out; 
			_out: {}
		}
		
#line 10 "45.rl"

		int r=cs>=p45_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
