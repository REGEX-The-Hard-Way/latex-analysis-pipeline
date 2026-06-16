#line 1 "_test53.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "_test53.c"
static const int p53_start = 1;
static const int p53_first_final = 3;
static const int p53_error = 0;

static const int p53_en_main = 1;


#line 4 "_test53.rl"

int main(void){int cs,c;const char *p,*pe,*eof; setbuf(stdout,NULL);
	static const char *t[]={"12\n","123\n","1\n","\n"};static const int e[]={1,0,0,0};int n=4;
	printf("53 final\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 17 "_test53.c"
	{
			cs = (int)p53_start;
		}
		
#line 9 "_test53.rl"


#line 22 "_test53.c"
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
			_st1:
			if ( p == eof )
				goto _out1;
			p+= 1;
			st_case_1:
			if ( p == pe && p != eof )
				goto _out1;
				{
#line 4 "_test53.rl"
				c = 0; }
			
#line 45 "_test53.c"

			if ( p == eof ) {
				goto _st1;}
			else {
				if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
					_ck = 0;
					if ( ( c < 3 )
#line 52 "_test53.c"
 )
					_ck += 1;
					if ( 1 <= _ck )
						{goto _ctr2;
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
			_ctr2:
				{
#line 4 "_test53.rl"
				c++; }
			
#line 72 "_test53.c"

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
					if ( ( c >= 2 )
#line 87 "_test53.c"
 )
					_ck += 1;
					if ( 1 <= _ck )
						{goto _st3;
					}
					goto _st0;
					
				}
				if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
					_ck = 0;
					if ( ( c < 3 )
#line 98 "_test53.c"
 )
					_ck += 1;
					if ( 1 <= _ck )
						{goto _ctr2;
					}
					goto _st0;
					
				}
				goto _st0;
			}
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
			_out1: cs = 1; goto _out; 
			_out0: cs = 0; goto _out; 
			_out2: cs = 2; goto _out; 
			_out3: cs = 3; goto _out; 
			_out: {}
		}
		
#line 10 "_test53.rl"

		int r=cs>=p53_first_final;printf("  %d '%s' -> %s ok\n",i,t[i],r?"ACCEPT":"FAIL");assert(r==e[i]);}
	printf("done\n");return 0;}
