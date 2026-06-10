#line 1 "sent_split.rl"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char sbuf[65536]; static int slen;
static void add(char c) { if (slen < 65530) sbuf[slen++] = c; }
static void flush(void) {
	sbuf[slen] = '\0';
	char *s = sbuf;
	while (*s==' '||*s=='\t'||*s=='\n') s++;
	char *e = s+strlen(s);
	while (e>s&&(e[-1]==' '||e[-1]=='\t'||e[-1]=='\n')) e--;
	*e = '\0';
	if (*s) printf("%s\n", s);
		slen = 0;
}


#line 46 "sent_split.rl"


#line 22 "sent_split.c"
static const int sent_split_start = 13;
static const int sent_split_first_final = 13;
static const int sent_split_error = -1;

static const int sent_split_en_main = 13;


#line 47 "sent_split.rl"


int sent_split_run(const char *in, int length) {
	slen = 0;
	int cs = 0, act = 0, i;
	const char *p = in, *pe = in + length;
	const char *ts = NULL, *te = NULL;
	const char *eof = pe;

#line 37 "sent_split.c"
	{
		cs = (int)sent_split_start;
		ts = 0;
		te = 0;
	}
	
#line 55 "sent_split.rl"


#line 44 "sent_split.c"
{
		switch ( cs ) {
			case 13:
			goto st_case_13;
			case 14:
			goto st_case_14;
			case 15:
			goto st_case_15;
			case 16:
			goto st_case_16;
			case 0:
			goto st_case_0;
			case 1:
			goto st_case_1;
			case 2:
			goto st_case_2;
			case 17:
			goto st_case_17;
			case 3:
			goto st_case_3;
			case 18:
			goto st_case_18;
			case 19:
			goto st_case_19;
			case 4:
			goto st_case_4;
			case 20:
			goto st_case_20;
			case 21:
			goto st_case_21;
			case 22:
			goto st_case_22;
			case 23:
			goto st_case_23;
			case 24:
			goto st_case_24;
			case 25:
			goto st_case_25;
			case 26:
			goto st_case_26;
			case 27:
			goto st_case_27;
			case 28:
			goto st_case_28;
			case 29:
			goto st_case_29;
			case 30:
			goto st_case_30;
			case 31:
			goto st_case_31;
			case 32:
			goto st_case_32;
			case 33:
			goto st_case_33;
			case 34:
			goto st_case_34;
			case 35:
			goto st_case_35;
			case 36:
			goto st_case_36;
			case 37:
			goto st_case_37;
			case 38:
			goto st_case_38;
			case 39:
			goto st_case_39;
			case 40:
			goto st_case_40;
			case 41:
			goto st_case_41;
			case 42:
			goto st_case_42;
			case 5:
			goto st_case_5;
			case 6:
			goto st_case_6;
			case 43:
			goto st_case_43;
			case 44:
			goto st_case_44;
			case 45:
			goto st_case_45;
			case 7:
			goto st_case_7;
			case 8:
			goto st_case_8;
			case 46:
			goto st_case_46;
			case 9:
			goto st_case_9;
			case 47:
			goto st_case_47;
			case 48:
			goto st_case_48;
			case 49:
			goto st_case_49;
			case 10:
			goto st_case_10;
			case 11:
			goto st_case_11;
			case 12:
			goto st_case_12;
		}
		_ctr0:
			{
#line 42 "sent_split.rl"
			{p = ((te))-1;
				{
#line 42 "sent_split.rl"
					add(' '); }
			}}
		
#line 156 "sent_split.c"

		goto _st13;
		_ctr3:
			{
#line 34 "sent_split.rl"
			{te = p+1;{
#line 34 "sent_split.rl"
					int i; for(i=0;i<te-ts;i++) add(ts[i]); }
			}}
		
#line 166 "sent_split.c"

		goto _st13;
		_ctr4:
			{
#line 44 "sent_split.rl"
			{p = ((te))-1;
				{
#line 44 "sent_split.rl"
					add(ts[0]); }
			}}
		
#line 177 "sent_split.c"

		goto _st13;
		_ctr6:
			{
#line 39 "sent_split.rl"
			{te = p+1;{
#line 39 "sent_split.rl"
					int i; for(i=0;i<te-ts;i++) add(ts[i]); flush(); }
			}}
		
#line 187 "sent_split.c"

		goto _st13;
		_ctr7:
			{
#line 40 "sent_split.rl"
			{te = p+1;{
#line 40 "sent_split.rl"
					int i; for(i=0;i<te-ts-1;i++) add(ts[i]); flush(); add(*(te-1)); }
			}}
		
#line 197 "sent_split.c"

		goto _st13;
		_ctr8:
			{
#line 37 "sent_split.rl"
			{p = ((te))-1;
				{
#line 37 "sent_split.rl"
					int i; for(i=0;i<te-ts;i++) add(ts[i]); }
			}}
		
#line 208 "sent_split.c"

		goto _st13;
		_ctr11:
			{
#line 35 "sent_split.rl"
			{te = p+1;{
#line 35 "sent_split.rl"
					int i; for(i=0;i<te-ts;i++) add(ts[i]); }
			}}
		
#line 218 "sent_split.c"

		goto _st13;
		_ctr16:
			{
#line 44 "sent_split.rl"
			{te = p+1;{
#line 44 "sent_split.rl"
					add(ts[0]); }
			}}
		
#line 228 "sent_split.c"

		goto _st13;
		_ctr17:
			{
#line 42 "sent_split.rl"
			{te = p+1;{
#line 42 "sent_split.rl"
					add(' '); }
			}}
		
#line 238 "sent_split.c"

		goto _st13;
		_ctr39:
			{
#line 43 "sent_split.rl"
			{te = p;p = p - 1;{
#line 43 "sent_split.rl"
					add(' '); }
			}}
		
#line 248 "sent_split.c"

		goto _st13;
		_ctr41:
			{
#line 41 "sent_split.rl"
			{te = p;p = p - 1;{
#line 41 "sent_split.rl"
					flush(); }
			}}
		
#line 258 "sent_split.c"

		goto _st13;
		_ctr42:
			{
#line 42 "sent_split.rl"
			{te = p;p = p - 1;{
#line 42 "sent_split.rl"
					add(' '); }
			}}
		
#line 268 "sent_split.c"

		goto _st13;
		_ctr44:
			{
#line 44 "sent_split.rl"
			{te = p;p = p - 1;{
#line 44 "sent_split.rl"
					add(ts[0]); }
			}}
		
#line 278 "sent_split.c"

		goto _st13;
		_ctr45:
			{
#line 38 "sent_split.rl"
			{te = p;p = p - 1;{
#line 38 "sent_split.rl"
					int i; for(i=0;i<te-ts;i++) add(ts[i]); }
			}}
		
#line 288 "sent_split.c"

		goto _st13;
		_ctr46:
			{
#line 37 "sent_split.rl"
			{te = p;p = p - 1;{
#line 37 "sent_split.rl"
					int i; for(i=0;i<te-ts;i++) add(ts[i]); }
			}}
		
#line 298 "sent_split.c"

		goto _st13;
		_ctr48:
			{
#line 36 "sent_split.rl"
			{te = p+1;{
#line 36 "sent_split.rl"
					int i; for(i=0;i<te-ts;i++) add(ts[i]); }
			}}
		
#line 308 "sent_split.c"

		goto _st13;
		_st13:
		if ( p == eof )
			goto _out13;
			{
#line 1 "NONE"
			{ts = 0;}}
		
#line 317 "sent_split.c"

		p+= 1;
		st_case_13:
		if ( p == pe && p != eof )
			goto _out13;
			{
#line 1 "NONE"
			{ts = p;}}
		
#line 326 "sent_split.c"

		if ( p == eof ) {
			goto _st13;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _ctr17;
				}
				case 10: {
					goto _st14;
				}
				case 32: {
					goto _ctr19;
				}
				case 33: {
					goto _ctr20;
				}
				case 46: {
					goto _ctr20;
				}
				case 63: {
					goto _ctr20;
				}
				case 68: {
					goto _ctr23;
				}
				case 69: {
					goto _ctr24;
				}
				case 70: {
					goto _ctr25;
				}
				case 77: {
					goto _ctr26;
				}
				case 78: {
					goto _ctr27;
				}
				case 80: {
					goto _ctr28;
				}
				case 82: {
					goto _ctr29;
				}
				case 83: {
					goto _ctr30;
				}
				case 86: {
					goto _ctr31;
				}
				case 97: {
					goto _ctr32;
				}
				case 99: {
					goto _ctr33;
				}
				case 101: {
					goto _ctr34;
				}
				case 105: {
					goto _ctr35;
				}
				case 112: {
					goto _ctr36;
				}
				case 118: {
					goto _ctr37;
				}
				case 119: {
					goto _ctr38;
				}
			}
			if ( ( (*( p))) < 65 ) {
				if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
					goto _st18;
				}
			} else if ( ( (*( p))) > 90 ) {
				if ( 98 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else {
				goto _ctr22;
			}
			goto _ctr16;
		}
		_st14:
		if ( p == eof )
			goto _out14;
		p+= 1;
		st_case_14:
		if ( p == pe && p != eof )
			goto _out14;
		if ( p == eof ) {
			goto _ctr39;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _st15;
			}
			goto _ctr39;
		}
		_st15:
		if ( p == eof )
			goto _out15;
		p+= 1;
		st_case_15:
		if ( p == pe && p != eof )
			goto _out15;
		if ( p == eof ) {
			goto _ctr41;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _st15;
			}
			goto _ctr41;
		}
		_ctr19:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 446 "sent_split.c"

		goto _st16;
		_st16:
		if ( p == eof )
			goto _out16;
		p+= 1;
		st_case_16:
		if ( p == pe && p != eof )
			goto _out16;
		if ( p == eof ) {
			goto _ctr42;}
		else {
			if ( ( (*( p))) == 32 ) {
				goto _st0;
			}
			goto _ctr42;
		}
		_st0:
		if ( p == eof )
			goto _out0;
		p+= 1;
		st_case_0:
		if ( p == pe && p != eof )
			goto _out0;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st1;
			}
			goto _ctr0;
		}
		_st1:
		if ( p == eof )
			goto _out1;
		p+= 1;
		st_case_1:
		if ( p == pe && p != eof )
			goto _out1;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 32 ) {
				goto _st2;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st1;
			}
			goto _ctr0;
		}
		_st2:
		if ( p == eof )
			goto _out2;
		p+= 1;
		st_case_2:
		if ( p == pe && p != eof )
			goto _out2;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 32 ) {
				goto _ctr3;
			}
			goto _ctr0;
		}
		_ctr20:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 516 "sent_split.c"

		goto _st17;
		_st17:
		if ( p == eof )
			goto _out17;
		p+= 1;
		st_case_17:
		if ( p == pe && p != eof )
			goto _out17;
		if ( p == eof ) {
			goto _ctr44;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st3;
				}
				case 10: {
					goto _ctr6;
				}
				case 32: {
					goto _st3;
				}
			}
			goto _ctr44;
		}
		_st3:
		if ( p == eof )
			goto _out3;
		p+= 1;
		st_case_3:
		if ( p == pe && p != eof )
			goto _out3;
		if ( p == eof ) {
			goto _ctr4;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st3;
				}
				case 10: {
					goto _ctr6;
				}
				case 32: {
					goto _st3;
				}
			}
			if ( 65 <= ( (*( p))) && ( (*( p))) <= 90 ) {
				goto _ctr7;
			}
			goto _ctr4;
		}
		_st18:
		if ( p == eof )
			goto _out18;
		p+= 1;
		st_case_18:
		if ( p == pe && p != eof )
			goto _out18;
		if ( p == eof ) {
			goto _ctr45;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st18;
			}
			goto _ctr45;
		}
		_ctr22:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 587 "sent_split.c"

		goto _st19;
		_st19:
		if ( p == eof )
			goto _out19;
		p+= 1;
		st_case_19:
		if ( p == pe && p != eof )
			goto _out19;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 46: {
					goto _ctr48;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_st4:
		if ( p == eof )
			goto _out4;
		p+= 1;
		st_case_4:
		if ( p == pe && p != eof )
			goto _out4;
		if ( p == eof ) {
			goto _ctr8;}
		else {
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr8;
		}
		_ctr9:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 643 "sent_split.c"

		goto _st20;
		_st20:
		if ( p == eof )
			goto _out20;
		p+= 1;
		st_case_20:
		if ( p == pe && p != eof )
			goto _out20;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr23:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 677 "sent_split.c"

		goto _st21;
		_st21:
		if ( p == eof )
			goto _out21;
		p+= 1;
		st_case_21:
		if ( p == pe && p != eof )
			goto _out21;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 46: {
					goto _ctr48;
				}
				case 114: {
					goto _ctr49;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr49:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 717 "sent_split.c"

		goto _st22;
		_st22:
		if ( p == eof )
			goto _out22;
		p+= 1;
		st_case_22:
		if ( p == pe && p != eof )
			goto _out22;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 46: {
					goto _ctr11;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr24:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 754 "sent_split.c"

		goto _st23;
		_st23:
		if ( p == eof )
			goto _out23;
		p+= 1;
		st_case_23:
		if ( p == pe && p != eof )
			goto _out23;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 46: {
					goto _ctr48;
				}
				case 113: {
					goto _ctr49;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr25:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 794 "sent_split.c"

		goto _st24;
		_st24:
		if ( p == eof )
			goto _out24;
		p+= 1;
		st_case_24:
		if ( p == pe && p != eof )
			goto _out24;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 46: {
					goto _ctr48;
				}
				case 105: {
					goto _ctr50;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr50:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 834 "sent_split.c"

		goto _st25;
		_st25:
		if ( p == eof )
			goto _out25;
		p+= 1;
		st_case_25:
		if ( p == pe && p != eof )
			goto _out25;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 103: {
					goto _ctr49;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr26:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 871 "sent_split.c"

		goto _st26;
		_st26:
		if ( p == eof )
			goto _out26;
		p+= 1;
		st_case_26:
		if ( p == pe && p != eof )
			goto _out26;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 46: {
					goto _ctr48;
				}
				case 114: {
					goto _ctr51;
				}
				case 115: {
					goto _ctr49;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr51:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 914 "sent_split.c"

		goto _st27;
		_st27:
		if ( p == eof )
			goto _out27;
		p+= 1;
		st_case_27:
		if ( p == pe && p != eof )
			goto _out27;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 46: {
					goto _ctr11;
				}
				case 115: {
					goto _ctr49;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr27:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 954 "sent_split.c"

		goto _st28;
		_st28:
		if ( p == eof )
			goto _out28;
		p+= 1;
		st_case_28:
		if ( p == pe && p != eof )
			goto _out28;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 46: {
					goto _ctr48;
				}
				case 111: {
					goto _ctr49;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr28:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 994 "sent_split.c"

		goto _st29;
		_st29:
		if ( p == eof )
			goto _out29;
		p+= 1;
		st_case_29:
		if ( p == pe && p != eof )
			goto _out29;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 46: {
					goto _ctr48;
				}
				case 114: {
					goto _ctr52;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr52:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1034 "sent_split.c"

		goto _st30;
		_st30:
		if ( p == eof )
			goto _out30;
		p+= 1;
		st_case_30:
		if ( p == pe && p != eof )
			goto _out30;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 111: {
					goto _ctr33;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr33:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1071 "sent_split.c"

		goto _st31;
		_st31:
		if ( p == eof )
			goto _out31;
		p+= 1;
		st_case_31:
		if ( p == pe && p != eof )
			goto _out31;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 102: {
					goto _ctr49;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr29:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1108 "sent_split.c"

		goto _st32;
		_st32:
		if ( p == eof )
			goto _out32;
		p+= 1;
		st_case_32:
		if ( p == pe && p != eof )
			goto _out32;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 46: {
					goto _ctr48;
				}
				case 101: {
					goto _ctr33;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr30:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1148 "sent_split.c"

		goto _st33;
		_st33:
		if ( p == eof )
			goto _out33;
		p+= 1;
		st_case_33:
		if ( p == pe && p != eof )
			goto _out33;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 46: {
					goto _ctr48;
				}
				case 101: {
					goto _ctr53;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr53:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1188 "sent_split.c"

		goto _st34;
		_st34:
		if ( p == eof )
			goto _out34;
		p+= 1;
		st_case_34:
		if ( p == pe && p != eof )
			goto _out34;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 99: {
					goto _ctr49;
				}
				case 110: {
					goto _ctr49;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr31:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1228 "sent_split.c"

		goto _st35;
		_st35:
		if ( p == eof )
			goto _out35;
		p+= 1;
		st_case_35:
		if ( p == pe && p != eof )
			goto _out35;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 46: {
					goto _ctr48;
				}
				case 111: {
					goto _ctr54;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr54:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1268 "sent_split.c"

		goto _st36;
		_st36:
		if ( p == eof )
			goto _out36;
		p+= 1;
		st_case_36:
		if ( p == pe && p != eof )
			goto _out36;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 108: {
					goto _ctr49;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr32:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1305 "sent_split.c"

		goto _st37;
		_st37:
		if ( p == eof )
			goto _out37;
		p+= 1;
		st_case_37:
		if ( p == pe && p != eof )
			goto _out37;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 112: {
					goto _ctr55;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr55:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1342 "sent_split.c"

		goto _st38;
		_st38:
		if ( p == eof )
			goto _out38;
		p+= 1;
		st_case_38:
		if ( p == pe && p != eof )
			goto _out38;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 112: {
					goto _ctr56;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr56:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1379 "sent_split.c"

		goto _st39;
		_st39:
		if ( p == eof )
			goto _out39;
		p+= 1;
		st_case_39:
		if ( p == pe && p != eof )
			goto _out39;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 114: {
					goto _ctr57;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr57:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1416 "sent_split.c"

		goto _st40;
		_st40:
		if ( p == eof )
			goto _out40;
		p+= 1;
		st_case_40:
		if ( p == pe && p != eof )
			goto _out40;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 111: {
					goto _ctr58;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr58:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1453 "sent_split.c"

		goto _st41;
		_st41:
		if ( p == eof )
			goto _out41;
		p+= 1;
		st_case_41:
		if ( p == pe && p != eof )
			goto _out41;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 120: {
					goto _ctr49;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr34:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1490 "sent_split.c"

		goto _st42;
		_st42:
		if ( p == eof )
			goto _out42;
		p+= 1;
		st_case_42:
		if ( p == pe && p != eof )
			goto _out42;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 46: {
					goto _st5;
				}
				case 100: {
					goto _ctr37;
				}
				case 115: {
					goto _ctr60;
				}
				case 116: {
					goto _ctr61;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_st5:
		if ( p == eof )
			goto _out5;
		p+= 1;
		st_case_5:
		if ( p == pe && p != eof )
			goto _out5;
		if ( p == eof ) {
			goto _ctr8;}
		else {
			if ( ( (*( p))) == 103 ) {
				goto _st6;
			}
			goto _ctr8;
		}
		_st6:
		if ( p == eof )
			goto _out6;
		p+= 1;
		st_case_6:
		if ( p == pe && p != eof )
			goto _out6;
		if ( p == eof ) {
			goto _ctr8;}
		else {
			if ( ( (*( p))) == 46 ) {
				goto _ctr11;
			}
			goto _ctr8;
		}
		_ctr37:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1566 "sent_split.c"

		goto _st43;
		_st43:
		if ( p == eof )
			goto _out43;
		p+= 1;
		st_case_43:
		if ( p == pe && p != eof )
			goto _out43;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 115: {
					goto _ctr49;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr60:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1603 "sent_split.c"

		goto _st44;
		_st44:
		if ( p == eof )
			goto _out44;
		p+= 1;
		st_case_44:
		if ( p == pe && p != eof )
			goto _out44;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 112: {
					goto _ctr49;
				}
				case 116: {
					goto _ctr49;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr61:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1643 "sent_split.c"

		goto _st45;
		_st45:
		if ( p == eof )
			goto _out45;
		p+= 1;
		st_case_45:
		if ( p == pe && p != eof )
			goto _out45;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 32: {
					goto _st7;
				}
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_st7:
		if ( p == eof )
			goto _out7;
		p+= 1;
		st_case_7:
		if ( p == pe && p != eof )
			goto _out7;
		if ( p == eof ) {
			goto _ctr8;}
		else {
			if ( ( (*( p))) == 97 ) {
				goto _st8;
			}
			goto _ctr8;
		}
		_st8:
		if ( p == eof )
			goto _out8;
		p+= 1;
		st_case_8:
		if ( p == pe && p != eof )
			goto _out8;
		if ( p == eof ) {
			goto _ctr8;}
		else {
			if ( ( (*( p))) == 108 ) {
				goto _st6;
			}
			goto _ctr8;
		}
		_ctr35:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1710 "sent_split.c"

		goto _st46;
		_st46:
		if ( p == eof )
			goto _out46;
		p+= 1;
		st_case_46:
		if ( p == pe && p != eof )
			goto _out46;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 46: {
					goto _st9;
				}
				case 110: {
					goto _ctr64;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_st9:
		if ( p == eof )
			goto _out9;
		p+= 1;
		st_case_9:
		if ( p == pe && p != eof )
			goto _out9;
		if ( p == eof ) {
			goto _ctr8;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st6;
			}
			goto _ctr8;
		}
		_ctr64:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1765 "sent_split.c"

		goto _st47;
		_st47:
		if ( p == eof )
			goto _out47;
		p+= 1;
		st_case_47:
		if ( p == pe && p != eof )
			goto _out47;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 99: {
					goto _ctr49;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr36:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1802 "sent_split.c"

		goto _st48;
		_st48:
		if ( p == eof )
			goto _out48;
		p+= 1;
		st_case_48:
		if ( p == pe && p != eof )
			goto _out48;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 112: {
					goto _ctr49;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_ctr38:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1839 "sent_split.c"

		goto _st49;
		_st49:
		if ( p == eof )
			goto _out49;
		p+= 1;
		st_case_49:
		if ( p == pe && p != eof )
			goto _out49;
		if ( p == eof ) {
			goto _ctr46;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st4;
				}
				case 45: {
					goto _st4;
				}
				case 46: {
					goto _st10;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr9;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr9;
			}
			goto _ctr46;
		}
		_st10:
		if ( p == eof )
			goto _out10;
		p+= 1;
		st_case_10:
		if ( p == pe && p != eof )
			goto _out10;
		if ( p == eof ) {
			goto _ctr8;}
		else {
			if ( ( (*( p))) == 114 ) {
				goto _st11;
			}
			goto _ctr8;
		}
		_st11:
		if ( p == eof )
			goto _out11;
		p+= 1;
		st_case_11:
		if ( p == pe && p != eof )
			goto _out11;
		if ( p == eof ) {
			goto _ctr8;}
		else {
			if ( ( (*( p))) == 46 ) {
				goto _st12;
			}
			goto _ctr8;
		}
		_st12:
		if ( p == eof )
			goto _out12;
		p+= 1;
		st_case_12:
		if ( p == pe && p != eof )
			goto _out12;
		if ( p == eof ) {
			goto _ctr8;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st6;
			}
			goto _ctr8;
		}
		_out13: cs = 13; goto _out; 
		_out14: cs = 14; goto _out; 
		_out15: cs = 15; goto _out; 
		_out16: cs = 16; goto _out; 
		_out0: cs = 0; goto _out; 
		_out1: cs = 1; goto _out; 
		_out2: cs = 2; goto _out; 
		_out17: cs = 17; goto _out; 
		_out3: cs = 3; goto _out; 
		_out18: cs = 18; goto _out; 
		_out19: cs = 19; goto _out; 
		_out4: cs = 4; goto _out; 
		_out20: cs = 20; goto _out; 
		_out21: cs = 21; goto _out; 
		_out22: cs = 22; goto _out; 
		_out23: cs = 23; goto _out; 
		_out24: cs = 24; goto _out; 
		_out25: cs = 25; goto _out; 
		_out26: cs = 26; goto _out; 
		_out27: cs = 27; goto _out; 
		_out28: cs = 28; goto _out; 
		_out29: cs = 29; goto _out; 
		_out30: cs = 30; goto _out; 
		_out31: cs = 31; goto _out; 
		_out32: cs = 32; goto _out; 
		_out33: cs = 33; goto _out; 
		_out34: cs = 34; goto _out; 
		_out35: cs = 35; goto _out; 
		_out36: cs = 36; goto _out; 
		_out37: cs = 37; goto _out; 
		_out38: cs = 38; goto _out; 
		_out39: cs = 39; goto _out; 
		_out40: cs = 40; goto _out; 
		_out41: cs = 41; goto _out; 
		_out42: cs = 42; goto _out; 
		_out5: cs = 5; goto _out; 
		_out6: cs = 6; goto _out; 
		_out43: cs = 43; goto _out; 
		_out44: cs = 44; goto _out; 
		_out45: cs = 45; goto _out; 
		_out7: cs = 7; goto _out; 
		_out8: cs = 8; goto _out; 
		_out46: cs = 46; goto _out; 
		_out9: cs = 9; goto _out; 
		_out47: cs = 47; goto _out; 
		_out48: cs = 48; goto _out; 
		_out49: cs = 49; goto _out; 
		_out10: cs = 10; goto _out; 
		_out11: cs = 11; goto _out; 
		_out12: cs = 12; goto _out; 
		_out: {}
	}
	
#line 56 "sent_split.rl"

	flush();
	return (cs == sent_split_error) ? 1 : 0;
}

#ifdef TEST
int main(void) {
	size_t cap = 1<<20, len = 0;
	char *data = malloc(cap); int n;
	while ((n = fread(data+len, 1, cap-len-1, stdin)) > 0) {
		len += (size_t)n;
		if (len >= cap-1) { cap *= 2; data = realloc(data, cap); }
	}
	data[len] = '\0';
	int rc = sent_split_run(data, (int)len);
	free(data);
	return rc;
}
#endif
