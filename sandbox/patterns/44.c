#line 1 "44.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "44.c"
static const int p44_start = 1;
static const int p44_first_final = 7;
static const int p44_error = 0;

static const int p44_en_main = 1;


#line 4 "44.rl"

int main(void){int cs,ercount;const char *p,*pe,*eof;const char *save;
	static const char *t[]={"hello\n","helo\n"};static const int e[]={1,0};int n=2;
	printf("Pattern 44: fexec Instruction\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;save=p;ercount=0;

#line 17 "44.c"
	{
			cs = (int)p44_start;
		}
		
#line 9 "44.rl"


#line 22 "44.c"
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
				case 4:
				goto st_case_4;
				case 5:
				goto st_case_5;
				case 6:
				goto st_case_6;
				case 7:
				goto st_case_7;
			}
			_ctr1:
				{
#line 4 "44.rl"
				if(++ercount<4){{p = (( save))-1;}
					{goto _st1;}} }
			
#line 47 "44.c"

			goto _st1;
			_st1:
			if ( p == eof )
				goto _out1;
			p+= 1;
			st_case_1:
			if ( p == pe && p != eof )
				goto _out1;
			if ( p == eof ) {
				goto _ctr1;}
			else {
				if ( ( (*( p))) == 104 ) {
					goto _st2;
				}
				goto _ctr2;
			}
			_ctr2:
				{
#line 4 "44.rl"
				if(++ercount<4){{p = (( save))-1;}
					{goto _st1;}} }
			
#line 70 "44.c"

			goto _st0;
			_st0:
			if ( p == eof )
				goto _out0;
			st_case_0:
			goto _out0;
			_ctr4:
				{
#line 4 "44.rl"
				if(++ercount<4){{p = (( save))-1;}
					{goto _st1;}} }
			
#line 83 "44.c"

			goto _st2;
			_st2:
			if ( p == eof )
				goto _out2;
			p+= 1;
			st_case_2:
			if ( p == pe && p != eof )
				goto _out2;
			if ( p == eof ) {
				goto _ctr4;}
			else {
				if ( ( (*( p))) == 101 ) {
					goto _st3;
				}
				goto _ctr2;
			}
			_ctr6:
				{
#line 4 "44.rl"
				if(++ercount<4){{p = (( save))-1;}
					{goto _st1;}} }
			
#line 106 "44.c"

			goto _st3;
			_st3:
			if ( p == eof )
				goto _out3;
			p+= 1;
			st_case_3:
			if ( p == pe && p != eof )
				goto _out3;
			if ( p == eof ) {
				goto _ctr6;}
			else {
				if ( ( (*( p))) == 108 ) {
					goto _st4;
				}
				goto _ctr2;
			}
			_ctr8:
				{
#line 4 "44.rl"
				if(++ercount<4){{p = (( save))-1;}
					{goto _st1;}} }
			
#line 129 "44.c"

			goto _st4;
			_st4:
			if ( p == eof )
				goto _out4;
			p+= 1;
			st_case_4:
			if ( p == pe && p != eof )
				goto _out4;
			if ( p == eof ) {
				goto _ctr8;}
			else {
				if ( ( (*( p))) == 108 ) {
					goto _st5;
				}
				goto _ctr2;
			}
			_ctr10:
				{
#line 4 "44.rl"
				if(++ercount<4){{p = (( save))-1;}
					{goto _st1;}} }
			
#line 152 "44.c"

			goto _st5;
			_st5:
			if ( p == eof )
				goto _out5;
			p+= 1;
			st_case_5:
			if ( p == pe && p != eof )
				goto _out5;
			if ( p == eof ) {
				goto _ctr10;}
			else {
				if ( ( (*( p))) == 111 ) {
					goto _st6;
				}
				goto _ctr2;
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
				if ( ( (*( p))) == 10 ) {
					goto _st7;
				}
				goto _st0;
			}
			_st7:
			if ( p == eof )
				goto _out7;
			p+= 1;
			st_case_7:
			if ( p == pe && p != eof )
				goto _out7;
			if ( p == eof ) {
				goto _st7;}
			else {
				goto _st0;
			}
			_out1: cs = 1; goto _out; 
			_out0: cs = 0; goto _out; 
			_out2: cs = 2; goto _out; 
			_out3: cs = 3; goto _out; 
			_out4: cs = 4; goto _out; 
			_out5: cs = 5; goto _out; 
			_out6: cs = 6; goto _out; 
			_out7: cs = 7; goto _out; 
			_out: {}
		}
		
#line 10 "44.rl"

		int r=cs>=p44_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
