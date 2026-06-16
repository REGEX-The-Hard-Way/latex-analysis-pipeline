#line 1 "50.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "50.c"
static const int p50_start = 1;
static const int p50_first_final = 3;
static const int p50_error = 0;

static const int p50_en_main = 1;


#line 4 "50.rl"

int main(void){int cs,c;const char *p,*pe,*eof;
	static const char *t[]={"1234\n","12345\n","1234567\n"};static const int e[]={1,1,0};int n=3;
	printf("Pattern 50: Inline Conditions\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;c=0;

#line 17 "50.c"
	{
			cs = (int)p50_start;
		}
		
#line 9 "50.rl"


#line 22 "50.c"
{
			int _ck;
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
			p+= 1;
			st_case_1:
			if ( p == pe )
				goto _out1;
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				_ck = 0;
				if ( ( c++ < 5 )
#line 41 "50.c"
 )
				_ck += 1;
				if ( 1 <= _ck )
					{goto _st2;
				}
				goto _st0;
				
			}
			goto _st0;
			_st0:
			st_case_0:
			goto _out0;
			_st2:
			p+= 1;
			st_case_2:
			if ( p == pe )
				goto _out2;
			if ( ( (*( p))) == 10 ) {
				goto _st3;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				_ck = 0;
				if ( ( c++ < 5 )
#line 64 "50.c"
 )
				_ck += 1;
				if ( 1 <= _ck )
					{goto _st2;
				}
				goto _st0;
				
			}
			goto _st0;
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
		
#line 10 "50.rl"

		int r=cs>=p50_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
