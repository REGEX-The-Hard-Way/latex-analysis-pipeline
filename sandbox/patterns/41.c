#line 1 "41.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "41.c"
static const int p41_start = 1;
static const int p41_first_final = 7;
static const int p41_error = 0;

static const int p41_en_main = 1;
static const int p41_en_other = 2;


#line 4 "41.rl"

int main(void){int cs,return_to=0;const char *p,*pe,*eof;
	static const char *t[]={"1other\n","1otherX\n"};static const int e[]={1,0};int n=2;
	printf("Pattern 41: fcurs Variable\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 18 "41.c"
	{
			cs = (int)p41_start;
		}
		
#line 9 "41.rl"


#line 23 "41.c"
{
			int _ps;
			goto _resume;
			
			_again: {}
			switch ( cs ) {
				case 1: goto _st1;
				case 0: goto _st0;
				case 7: goto _st7;
				case 2: goto _st2;
				case 3: goto _st3;
				case 4: goto _st4;
				case 5: goto _st5;
				case 6: goto _st6;
				case 8: goto _st8;
			}
			
			_resume: {}
			switch ( cs ) {
				case 1:
				goto st_case_1;
				case 0:
				goto st_case_0;
				case 7:
				goto st_case_7;
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
				case 8:
				goto st_case_8;
			}
			_st1:
			p+= 1;
			st_case_1:
			if ( p == pe )
				goto _out1;
			_ps = 1;
			switch( ( (*( p))) ) {
				case 10: {
					goto _st7;
				}
				case 49: {
					goto _ctr3;
				}
			}
			goto _st0;
			_st0:
			st_case_0:
			goto _out0;
			_ctr3:
				{
#line 4 "41.rl"
				return_to = (_ps); {goto _st2;}}
			
#line 84 "41.c"

			goto _st7;
			_st7:
			p+= 1;
			st_case_7:
			if ( p == pe )
				goto _out7;
			goto _st0;
			_st2:
			p+= 1;
			st_case_2:
			if ( p == pe )
				goto _out2;
			if ( ( (*( p))) == 111 ) {
				goto _st3;
			}
			goto _st0;
			_st3:
			p+= 1;
			st_case_3:
			if ( p == pe )
				goto _out3;
			if ( ( (*( p))) == 116 ) {
				goto _st4;
			}
			goto _st0;
			_st4:
			p+= 1;
			st_case_4:
			if ( p == pe )
				goto _out4;
			if ( ( (*( p))) == 104 ) {
				goto _st5;
			}
			goto _st0;
			_st5:
			p+= 1;
			st_case_5:
			if ( p == pe )
				goto _out5;
			if ( ( (*( p))) == 101 ) {
				goto _st6;
			}
			goto _st0;
			_st6:
			p+= 1;
			st_case_6:
			if ( p == pe )
				goto _out6;
			if ( ( (*( p))) == 114 ) {
				goto _ctr9;
			}
			goto _st0;
			_ctr9:
				{
#line 4 "41.rl"
				printf("other\n"); {cs = (return_to); goto _again;} }
			
#line 142 "41.c"

			goto _st8;
			_st8:
			p+= 1;
			st_case_8:
			if ( p == pe )
				goto _out8;
			goto _st0;
			_out1: cs = 1; goto _out; 
			_out0: cs = 0; goto _out; 
			_out7: cs = 7; goto _out; 
			_out2: cs = 2; goto _out; 
			_out3: cs = 3; goto _out; 
			_out4: cs = 4; goto _out; 
			_out5: cs = 5; goto _out; 
			_out6: cs = 6; goto _out; 
			_out8: cs = 8; goto _out; 
			_out: {}
		}
		
#line 10 "41.rl"

		int r=cs>=p41_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
