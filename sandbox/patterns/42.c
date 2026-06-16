#line 1 "42.rl"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 5 "42.c"
static const int p42_start = 1;
static const int p42_first_final = 6;
static const int p42_error = 0;

static const int p42_en_main = 1;


#line 4 "42.rl"

int main(void){int cs;const char *p,*pe,*eof;
	static const char *t[]={"hello","helo"};static const int e[]={1,0};int n=2;
	printf("Pattern 42: fbreak/fnbreak\n\n");
	for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;

#line 17 "42.c"
	{
			cs = (int)p42_start;
		}
		
#line 9 "42.rl"

		unsigned nchars=0;
		while(p<pe){

#line 24 "42.c"
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
					case 4:
					goto st_case_4;
					case 5:
					goto st_case_5;
					case 6:
					goto st_case_6;
				}
				p+= 1;
				st_case_1:
				if ( p == pe )
					goto _out1;
				if ( ( (*( p))) == 104 ) {
					goto _ctr2;
				}
				goto _st0;
				_st0:
				st_case_0:
				goto _out0;
				_ctr2:
					{
#line 4 "42.rl"
					printf("h");{p+= 1; cs = 2; goto _out;}}
				
#line 57 "42.c"

				goto _st2;
				_st2:
				p+= 1;
				st_case_2:
				if ( p == pe )
					goto _out2;
				if ( ( (*( p))) == 101 ) {
					goto _ctr4;
				}
				goto _st0;
				_ctr4:
					{
#line 4 "42.rl"
					printf("e");{p+= 1; cs = 3; goto _out;}}
				
#line 73 "42.c"

				goto _st3;
				_st3:
				p+= 1;
				st_case_3:
				if ( p == pe )
					goto _out3;
				if ( ( (*( p))) == 108 ) {
					goto _ctr6;
				}
				goto _st0;
				_ctr6:
					{
#line 4 "42.rl"
					printf("l");{p+= 1; cs = 4; goto _out;}}
				
#line 89 "42.c"

				goto _st4;
				_st4:
				p+= 1;
				st_case_4:
				if ( p == pe )
					goto _out4;
				if ( ( (*( p))) == 108 ) {
					goto _ctr8;
				}
				goto _st0;
				_ctr8:
					{
#line 4 "42.rl"
					printf("l");{p+= 1; cs = 5; goto _out;}}
				
#line 105 "42.c"

				goto _st5;
				_st5:
				p+= 1;
				st_case_5:
				if ( p == pe )
					goto _out5;
				if ( ( (*( p))) == 111 ) {
					goto _ctr10;
				}
				goto _st0;
				_ctr10:
					{
#line 4 "42.rl"
					printf("o");{p+= 1; cs = 6; goto _out;}}
				
#line 121 "42.c"

				goto _st6;
				_st6:
				p+= 1;
				st_case_6:
				if ( p == pe )
					goto _out6;
				goto _st0;
				_out1: cs = 1; goto _out; 
				_out0: cs = 0; goto _out; 
				_out2: cs = 2; goto _out; 
				_out3: cs = 3; goto _out; 
				_out4: cs = 4; goto _out; 
				_out5: cs = 5; goto _out; 
				_out6: cs = 6; goto _out; 
				_out: {}
			}
			
#line 12 "42.rl"

			if(cs==p42_error)break;nchars++;}
		printf("\n");
		int ok=(nchars==5&&cs>=p42_first_final);
		printf("  Input %d -> %s ok\n",i,ok?"ACCEPT":"FAIL");assert(ok==e[i]);}
	printf("\nAll %d tests passed.\n",n);return 0;}
