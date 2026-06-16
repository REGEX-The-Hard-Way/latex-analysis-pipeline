#line 1 "46.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "46.c"
static const int p46_start = 1;
static const int p46_first_final = 3;
static const int p46_error = 0;

static const int p46_en_main = 1;


#line 4 "46.rl"

int main(void){int cs;const char *p,*pe,*eof,*buf;
	static const char *t[]={"hello world\n","abc\n"};static const int e[]={1,1};int n=2;
	printf("Pattern 46: String Accumulation $buf %%emit\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 17 "46.c"
	{
			cs = (int)p46_start;
		}
		
#line 9 "46.rl"


#line 22 "46.c"
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
			_ctr5:
				{
#line 4 "46.rl"
				printf(" %.*s", (int)(p-buf), buf); }
			
#line 38 "46.c"

			goto _st1;
			_st1:
			p+= 1;
			st_case_1:
			if ( p == pe )
				goto _out1;
			if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
				goto _ctr2;
			}
			goto _st0;
			_st0:
			st_case_0:
			goto _out0;
			_ctr2:
				{
#line 4 "46.rl"
				buf = p; }
			
#line 57 "46.c"

			goto _st2;
			_st2:
			p+= 1;
			st_case_2:
			if ( p == pe )
				goto _out2;
			switch( ( (*( p))) ) {
				case 10: {
					goto _ctr4;
				}
				case 32: {
					goto _ctr5;
				}
			}
			if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
				goto _st2;
			}
			goto _st0;
			_ctr4:
				{
#line 4 "46.rl"
				printf(" %.*s", (int)(p-buf), buf); }
			
#line 81 "46.c"

			goto _st3;
			_st3:
			p+= 1;
			st_case_3:
			if ( p == pe )
				goto _out3;
			goto _st0;
			_out1: cs = 1; goto _out; 
			_out0: cs = 0; goto _out; 
			_out2: cs = 2; goto _out; 
			_out3: cs = 3; goto _out; 
			_out: {}
		}
		
#line 10 "46.rl"

		int r=cs>=p46_first_final;printf("\n  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
