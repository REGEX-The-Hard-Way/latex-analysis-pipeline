#line 1 "52_debug.c"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "52_debug_out.c"
static const int p52_start = 1;
static const int p52_first_final = 3;
static const int p52_error = 0;

static const int p52_en_main = 1;


#line 4 "52_debug.c"

int main(void){int cs,c;const char *p,*pe,*eof; setbuf(stdout,NULL);
	char *t[]={"1\n","12\n","123\n","1234\n"};int e[]={1,1,0,0};
	printf("52 debug\n");
	for(int i=0;i<4;i++){p=t[i];pe=p+strlen(p);eof=pe;c=-1;
		printf("[%d] in='%s'\n",i,t[i]);

#line 18 "52_debug_out.c"
	{
			cs = (int)p52_start;
		}
		
#line 10 "52_debug.c"


#line 23 "52_debug_out.c"
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
#line 4 "52_debug.c"
				c = 0; }
			
#line 46 "52_debug_out.c"

			if ( p == eof ) {
				goto _st1;}
			else {
				if ( ( (*( p))) == 10 ) {
					_ck = 0;
					if ( ( c >= 1 )
#line 53 "52_debug_out.c"
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
#line 64 "52_debug_out.c"
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
#line 4 "52_debug.c"
				c++; }
			
#line 96 "52_debug_out.c"

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
#line 111 "52_debug_out.c"
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
#line 122 "52_debug_out.c"
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
		
#line 11 "52_debug.c"

		int r=cs>=p52_first_final;
		printf("  result=%d c=%d cs=%d first=%d\n",r,c,cs,p52_first_final);
		assert(r==e[i]);}
	return 0;}
