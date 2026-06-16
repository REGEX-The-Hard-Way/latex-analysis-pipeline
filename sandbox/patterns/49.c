#line 1 "49.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "49.c"
static const int p49_start = 1;
static const int p49_first_final = 5;
static const int p49_error = 0;

static const int p49_en_main = 1;


#line 4 "49.rl"

int main(void){int cs,n;const char *p,*pe,*eof;
	static const char *t[]={"()\n","(a)\n","(a(b)c)\n","((a)(b))\n","(()\n","())\n"};
	static const int e[]={1,1,1,1,0,0};int n2=6;
	printf("Pattern 49: Condition Balancer\n\n");
	for(int i=0;i<n2;i++){p=t[i];pe=p+strlen(p);eof=pe;n=0;

#line 18 "49.c"
	{
			cs = (int)p49_start;
		}
		
#line 10 "49.rl"


#line 23 "49.c"
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
				case 5:
				goto st_case_5;
				case 4:
				goto st_case_4;
			}
			p+= 1;
			st_case_1:
			if ( p == pe )
				goto _out1;
			if ( ( (*( p))) == 40 ) {
				goto _ctr2;
			}
			goto _st0;
			_st0:
			st_case_0:
			goto _out0;
			_ctr5:
				{
#line 4 "49.rl"
				n--;}
			
#line 55 "49.c"

			goto _st2;
			_ctr2:
				{
#line 4 "49.rl"
				n++;}
			
#line 62 "49.c"

			goto _st2;
			_ctr4:
				{
#line 4 "49.rl"
				n++;}
			
#line 69 "49.c"

			goto _st2;
			_st2:
			p+= 1;
			st_case_2:
			if ( p == pe )
				goto _out2;
			switch( ( (*( p))) ) {
				case 40: {
					goto _ctr4;
				}
				case 41: {
					_ck = 0;
					if ( (n>0)
#line 83 "49.c"
 )
					_ck += 1;
					if ( (n==1)
#line 86 "49.c"
 )
					_ck += 2;
					if ( _ck < 2 ) {
						if ( 1 <= _ck )
							{goto _ctr5;
						}
					} else if ( _ck > 2 ) {
						goto _ctr7;
					} else {
						goto _ctr6;
					}
					goto _st0;
					
				}
			}
			if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
				goto _st2;
			}
			goto _st0;
			_ctr6:
				{
#line 4 "49.rl"
				n--;}
			
#line 110 "49.c"

			goto _st3;
			_st3:
			p+= 1;
			st_case_3:
			if ( p == pe )
				goto _out3;
			if ( ( (*( p))) == 10 ) {
				goto _st5;
			}
			goto _st0;
			_st5:
			p+= 1;
			st_case_5:
			if ( p == pe )
				goto _out5;
			goto _st0;
			_ctr7:
				{
#line 4 "49.rl"
				n--;}
			
#line 132 "49.c"

				{
#line 4 "49.rl"
				n--;}
			
#line 137 "49.c"

			goto _st4;
			_st4:
			p+= 1;
			st_case_4:
			if ( p == pe )
				goto _out4;
			switch( ( (*( p))) ) {
				case 10: {
					goto _st5;
				}
				case 40: {
					goto _ctr4;
				}
				case 41: {
					_ck = 0;
					if ( (n>0)
#line 154 "49.c"
 )
					_ck += 1;
					if ( (n==1)
#line 157 "49.c"
 )
					_ck += 2;
					if ( _ck < 2 ) {
						if ( 1 <= _ck )
							{goto _ctr5;
						}
					} else if ( _ck > 2 ) {
						goto _ctr7;
					} else {
						goto _ctr6;
					}
					goto _st0;
					
				}
			}
			if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
				goto _st2;
			}
			goto _st0;
			_out1: cs = 1; goto _out; 
			_out0: cs = 0; goto _out; 
			_out2: cs = 2; goto _out; 
			_out3: cs = 3; goto _out; 
			_out5: cs = 5; goto _out; 
			_out4: cs = 4; goto _out; 
			_out: {}
		}
		
#line 11 "49.rl"

		int r=cs>=p49_first_final;printf("  Input %d -> %s ok\n",i,r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("\nAll %d tests passed.\n",n2);return 0;}
