#line 1 "51.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "51.c"
static const int p51_start = 1;
static const int p51_first_final = 4;
static const int p51_error = 0;

static const int p51_en_main = 1;


#line 4 "51.rl"

int main(void){int cs,thresh;const char *p,*pe,*eof;
	static const char *t[]={"510\n","55\n","50\n","5\n"};static const int e[]={1,1,1,0};int n=4;
	printf("Pattern 51: Parameterized Conditions\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;thresh=0;

#line 17 "51.c"
	{
			cs = (int)p51_start;
		}
		
#line 9 "51.rl"


#line 22 "51.c"
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
				case 4:
				goto st_case_4;
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
			_ctr2:
				{
#line 4 "51.rl"
				thresh = (( (*( p)))) - '0'; }
			
#line 52 "51.c"

			goto _st2;
			_st2:
			p+= 1;
			st_case_2:
			if ( p == pe )
				goto _out2;
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				_ck = 0;
				if ( ( thresh )
#line 62 "51.c"
 )
				_ck += 1;
				if ( 1 <= _ck )
					{goto _st3;
				}
				goto _st0;
				
			}
			goto _st0;
			_st3:
			p+= 1;
			st_case_3:
			if ( p == pe )
				goto _out3;
			if ( ( (*( p))) == 10 ) {
				goto _st4;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				_ck = 0;
				if ( ( thresh )
#line 82 "51.c"
 )
				_ck += 1;
				if ( 1 <= _ck )
					{goto _st3;
				}
				goto _st0;
				
			}
			goto _st0;
			_st4:
			p+= 1;
			st_case_4:
			if ( p == pe )
				goto _out4;
			goto _st0;
			_out1: cs = 1; goto _out; 
			_out0: cs = 0; goto _out; 
			_out2: cs = 2; goto _out; 
			_out3: cs = 3; goto _out; 
			_out4: cs = 4; goto _out; 
			_out: {}
		}
		
#line 10 "51.rl"

		int r=cs>=p51_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
