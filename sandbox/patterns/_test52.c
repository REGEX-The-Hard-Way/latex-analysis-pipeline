#line 1 "_test52.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "_test52.c"
static const int p52_start = 1;
static const int p52_first_final = 3;
static const int p52_error = 0;

static const int p52_en_main = 1;


#line 4 "_test52.rl"

int main(void){int cs,c;const char *p,*pe,*eof; setbuf(stdout,NULL);
	static const char *t[]={"\n","1\n","12\n"};static const int e[]={0,1,0};int n=3;
	printf("52 final\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 17 "_test52.c"
	{
			cs = (int)p52_start;
		}
		
#line 9 "_test52.rl"


#line 22 "_test52.c"
{
			int _ck;
			switch ( cs ) {
				case 1:
				goto st_case_1;
				case 0:
				goto st_case_0;
				case 3:
				goto st_case_3;
				case 2:
				goto st_case_2;
			}
			_st1:
			if ( p == eof )
				goto _out1;
			p+= 1;
			st_case_1:
			if ( p == pe && p != eof )
				goto _out1;
				{
#line 4 "_test52.rl"
				c = 0; }
			
#line 45 "_test52.c"

			if ( p == eof ) {
				goto _st1;}
			else {
				if ( ( (*( p))) == 10 ) {
					_ck = 0;
					if ( ( c >= 1 )
#line 52 "_test52.c"
 )
					_ck += 1;
					if ( 1 <= _ck )
						{goto _st3;
					}
					goto _st0;
					
				}
				if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
					_ck = 0;
					if ( ( c < 2 )
#line 63 "_test52.c"
 )
					_ck += 1;
					if ( 1 <= _ck )
						{goto _ctr4;
					}
					goto _st0;
					
				}
				goto _st0;
			}
			_st0:
			if ( p == eof )
				goto _out0;
			st_case_0:
			goto _out0;
			_st3:
			if ( p == eof )
				goto _out3;
			p+= 1;
			st_case_3:
			if ( p == pe && p != eof )
				goto _out3;
			if ( p == eof ) {
				goto _st3;}
			else {
				goto _st0;
			}
			_ctr4:
				{
#line 4 "_test52.rl"
				c++; }
			
#line 95 "_test52.c"

			goto _st2;
			_st2:
			if ( p == eof )
				goto _out2;
			p+= 1;
			st_case_2:
			if ( p == pe && p != eof )
				goto _out2;
			if ( p == eof ) {
				goto _st2;}
			else {
				if ( ( (*( p))) == 10 ) {
					_ck = 0;
					if ( ( c >= 1 )
#line 110 "_test52.c"
 )
					_ck += 1;
					if ( 1 <= _ck )
						{goto _st3;
					}
					goto _st0;
					
				}
				if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
					_ck = 0;
					if ( ( c < 2 )
#line 121 "_test52.c"
 )
					_ck += 1;
					if ( 1 <= _ck )
						{goto _ctr4;
					}
					goto _st0;
					
				}
				goto _st0;
			}
			_out1: cs = 1; goto _out; 
			_out0: cs = 0; goto _out; 
			_out3: cs = 3; goto _out; 
			_out2: cs = 2; goto _out; 
			_out: {}
		}
		
#line 10 "_test52.rl"

		int r=cs>=p52_first_final;printf("  %d '%s' -> %s ok\n",i,t[i],r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("done\n");return 0;}
