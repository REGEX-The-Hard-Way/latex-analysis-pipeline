#line 1 "47.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "47.c"
static const int p47_start = 1;
static const int p47_first_final = 2;
static const int p47_error = 0;

static const int p47_en_main = 1;


#line 4 "47.rl"

int main(void){int cs,flag;const char *p,*pe,*eof;
	static const char *t[]={"abc\n","\n"};static const int e[]={1,0};
	static const int f[]={1,0};int n=2;
	printf("Pattern 47: when Condition\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;flag=f[i];

#line 18 "47.c"
	{
			cs = (int)p47_start;
		}
		
#line 10 "47.rl"


#line 23 "47.c"
{
			int _ck;
			switch ( cs ) {
				case 1:
				goto st_case_1;
				case 0:
				goto st_case_0;
				case 2:
				goto st_case_2;
			}
			_st1:
			p+= 1;
			st_case_1:
			if ( p == pe )
				goto _out1;
			if ( ( (*( p))) == 10 ) {
				_ck = 0;
				if ( ( flag )
#line 41 "47.c"
 )
				_ck += 1;
				if ( 1 <= _ck )
					{goto _ctr2;
				}
				goto _st0;
				
			}
			if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
				goto _st1;
			}
			goto _st0;
			_st0:
			st_case_0:
			goto _out0;
			_ctr2:
				{
#line 4 "47.rl"
				printf("when c1 matched\n"); }
			
#line 61 "47.c"

			goto _st2;
			_st2:
			p+= 1;
			st_case_2:
			if ( p == pe )
				goto _out2;
			goto _st0;
			_out1: cs = 1; goto _out; 
			_out0: cs = 0; goto _out; 
			_out2: cs = 2; goto _out; 
			_out: {}
		}
		
#line 11 "47.rl"

		int r=cs>=p47_first_final;printf("  Input %d (%d) -> %s ok\n",i,flag,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
