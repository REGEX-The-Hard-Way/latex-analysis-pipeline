#line 1 "48.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "48.c"
static const int p48_start = 1;
static const int p48_first_final = 3;
static const int p48_error = 0;

static const int p48_en_main = 1;


#line 4 "48.rl"

int main(void){int cs,cnt;const char *p,*pe,*eof;
	static const char *t[]={"3123\n","21a\n","11\n","0\n"};static const int e[]={1,0,1,1};int n=4;
	printf("Pattern 48: outwhen Condition\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 17 "48.c"
	{
			cs = (int)p48_start;
		}
		
#line 9 "48.rl"


#line 22 "48.c"
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
				goto _ctr2;
			}
			goto _st0;
			_st0:
			st_case_0:
			goto _out0;
			_ctr6:
				{
#line 4 "48.rl"
				cnt--; }
			
#line 50 "48.c"

			goto _st2;
			_ctr2:
				{
#line 4 "48.rl"
				cnt = (( (*( p)))) - '0'; }
			
#line 57 "48.c"

			goto _st2;
			_st2:
			p+= 1;
			st_case_2:
			if ( p == pe )
				goto _out2;
			if ( ( (*( p))) == 10 ) {
				_ck = 0;
				if ( ( cnt > 0 )
#line 67 "48.c"
 )
				_ck += 1;
				if ( _ck <= 0 ) {
					goto _st3;
				}
				goto _st0;
				
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				_ck = 0;
				if ( ( cnt > 0 )
#line 78 "48.c"
 )
				_ck += 1;
				if ( 1 <= _ck )
					{goto _ctr6;
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
		
#line 10 "48.rl"

		int r=cs>=p48_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
