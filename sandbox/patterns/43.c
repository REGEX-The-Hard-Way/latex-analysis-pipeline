#line 1 "43.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "43.c"
static const int p43_start = 6;
static const int p43_first_final = 6;
static const int p43_error = -1;

static const int p43_en_main = 6;


#line 4 "43.rl"

int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
	static const char *t[]={"helloworld","helloXworld"};static const int e[]={1,1};int n=2;
	printf("Pattern 43: fhold Instruction\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 17 "43.c"
	{
			cs = (int)p43_start;
			ts = 0;
			te = 0;
		}
		
#line 9 "43.rl"


#line 24 "43.c"
{
			switch ( cs ) {
				case 6:
				goto st_case_6;
				case 7:
				goto st_case_7;
				case 0:
				goto st_case_0;
				case 1:
				goto st_case_1;
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
			}
			_ctr0:
				{
#line 4 "43.rl"
				{p = ((te))-1;
					{
#line 4 "43.rl"
						printf("? "); }
				}}
			
#line 54 "43.c"

			goto _st6;
			_ctr3:
				{
#line 4 "43.rl"
				{te = p+1;{
#line 4 "43.rl"
						printf("hello "); {p = p - 1; } }
				}}
			
#line 64 "43.c"

			goto _st6;
			_ctr6:
				{
#line 4 "43.rl"
				{te = p+1;{
#line 4 "43.rl"
						printf("world "); {p = p - 1; } }
				}}
			
#line 74 "43.c"

			goto _st6;
			_ctr8:
				{
#line 4 "43.rl"
				{te = p+1;{
#line 4 "43.rl"
						printf("? "); }
				}}
			
#line 84 "43.c"

			goto _st6;
			_ctr11:
				{
#line 4 "43.rl"
				{te = p;p = p - 1;{
#line 4 "43.rl"
						printf("? "); }
				}}
			
#line 94 "43.c"

			goto _st6;
			_st6:
			if ( p == eof )
				goto _out6;
				{
#line 1 "NONE"
				{ts = 0;}}
			
#line 103 "43.c"

			p+= 1;
			st_case_6:
			if ( p == pe && p != eof )
				goto _out6;
				{
#line 1 "NONE"
				{ts = p;}}
			
#line 112 "43.c"

			if ( p == eof ) {
				goto _st6;}
			else {
				switch( ( (*( p))) ) {
					case 104: {
						goto _ctr9;
					}
					case 119: {
						goto _ctr10;
					}
				}
				goto _ctr8;
			}
			_ctr9:
				{
#line 1 "NONE"
				{te = p+1;}}
			
#line 131 "43.c"

			goto _st7;
			_st7:
			if ( p == eof )
				goto _out7;
			p+= 1;
			st_case_7:
			if ( p == pe && p != eof )
				goto _out7;
			if ( p == eof ) {
				goto _ctr11;}
			else {
				if ( ( (*( p))) == 101 ) {
					goto _st0;
				}
				goto _ctr11;
			}
			_st0:
			if ( p == eof )
				goto _out0;
			p+= 1;
			st_case_0:
			if ( p == pe && p != eof )
				goto _out0;
			if ( p == eof ) {
				goto _ctr0;}
			else {
				if ( ( (*( p))) == 108 ) {
					goto _st1;
				}
				goto _ctr0;
			}
			_st1:
			if ( p == eof )
				goto _out1;
			p+= 1;
			st_case_1:
			if ( p == pe && p != eof )
				goto _out1;
			if ( p == eof ) {
				goto _ctr0;}
			else {
				if ( ( (*( p))) == 108 ) {
					goto _st2;
				}
				goto _ctr0;
			}
			_st2:
			if ( p == eof )
				goto _out2;
			p+= 1;
			st_case_2:
			if ( p == pe && p != eof )
				goto _out2;
			if ( p == eof ) {
				goto _ctr0;}
			else {
				if ( ( (*( p))) == 111 ) {
					goto _ctr3;
				}
				goto _ctr0;
			}
			_ctr10:
				{
#line 1 "NONE"
				{te = p+1;}}
			
#line 198 "43.c"

			goto _st8;
			_st8:
			if ( p == eof )
				goto _out8;
			p+= 1;
			st_case_8:
			if ( p == pe && p != eof )
				goto _out8;
			if ( p == eof ) {
				goto _ctr11;}
			else {
				if ( ( (*( p))) == 111 ) {
					goto _st3;
				}
				goto _ctr11;
			}
			_st3:
			if ( p == eof )
				goto _out3;
			p+= 1;
			st_case_3:
			if ( p == pe && p != eof )
				goto _out3;
			if ( p == eof ) {
				goto _ctr0;}
			else {
				if ( ( (*( p))) == 114 ) {
					goto _st4;
				}
				goto _ctr0;
			}
			_st4:
			if ( p == eof )
				goto _out4;
			p+= 1;
			st_case_4:
			if ( p == pe && p != eof )
				goto _out4;
			if ( p == eof ) {
				goto _ctr0;}
			else {
				if ( ( (*( p))) == 108 ) {
					goto _st5;
				}
				goto _ctr0;
			}
			_st5:
			if ( p == eof )
				goto _out5;
			p+= 1;
			st_case_5:
			if ( p == pe && p != eof )
				goto _out5;
			if ( p == eof ) {
				goto _ctr0;}
			else {
				if ( ( (*( p))) == 100 ) {
					goto _ctr6;
				}
				goto _ctr0;
			}
			_out6: cs = 6; goto _out; 
			_out7: cs = 7; goto _out; 
			_out0: cs = 0; goto _out; 
			_out1: cs = 1; goto _out; 
			_out2: cs = 2; goto _out; 
			_out8: cs = 8; goto _out; 
			_out3: cs = 3; goto _out; 
			_out4: cs = 4; goto _out; 
			_out5: cs = 5; goto _out; 
			_out: {}
		}
		
#line 10 "43.rl"

		printf("\n");
		int r=cs>=p43_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
