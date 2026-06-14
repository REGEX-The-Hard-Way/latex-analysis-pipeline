#line 1 "json_scan.rl"
/*
* json_scan.rl — Ragel JSON scanner with proper escape handling.
*
* Single-pass scanner over mmap'd JSON. Handles quoted strings with
* escapes, numbers, nested braces via depth tracking.
*
* Compiled with: ragel -m -G2 json_scan.rl
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "json_scan.h"


#line 19 "json_scan.c"
static const int json_scan_start = 12;
static const int json_scan_first_final = 12;
static const int json_scan_error = -1;

static const int json_scan_en_main = 12;


#line 21 "json_scan.rl"


int json_scan(const char *data, size_t len, json_field_cb cb, void *user) {
	const char *p  = data;
	const char *pe = data + len;
	const char *eof = pe;
	int cs, act;
	const char *ts, *te;
	
	char  key_buf[256];  int key_len = 0;
	char  val_buf[4096]; int val_len = 0;
	int   in_record = 0, key_done = 0;
	int   count = 0;
	int   last_was_colon = 0;
	
	
#line 121 "json_scan.rl"

	

#line 44 "json_scan.c"
	{
		cs = (int)json_scan_start;
		ts = 0;
		te = 0;
	}
	
#line 123 "json_scan.rl"


#line 51 "json_scan.c"
{
		switch ( cs ) {
			case 12:
			goto st_case_12;
			case 13:
			goto st_case_13;
			case 14:
			goto st_case_14;
			case 0:
			goto st_case_0;
			case 15:
			goto st_case_15;
			case 1:
			goto st_case_1;
			case 2:
			goto st_case_2;
			case 16:
			goto st_case_16;
			case 17:
			goto st_case_17;
			case 3:
			goto st_case_3;
			case 18:
			goto st_case_18;
			case 4:
			goto st_case_4;
			case 5:
			goto st_case_5;
			case 19:
			goto st_case_19;
			case 20:
			goto st_case_20;
			case 21:
			goto st_case_21;
			case 6:
			goto st_case_6;
			case 7:
			goto st_case_7;
			case 8:
			goto st_case_8;
			case 22:
			goto st_case_22;
			case 9:
			goto st_case_9;
			case 10:
			goto st_case_10;
			case 23:
			goto st_case_23;
			case 11:
			goto st_case_11;
		}
		_ctr0:
			{
#line 1 "-"
			{p = ((te))-1;
			}}
		
#line 108 "json_scan.c"

		goto _st12;
		_ctr4:
			{
#line 55 "json_scan.rl"
			{p = ((te))-1;
				{
#line 55 "json_scan.rl"
					
					if (in_record && key_done) {
						const char *vs = ts + 1;        /* skip opening " */
						/* find the closing " (te may point past it with wsp) */
						const char *ve = vs;
						while (ve < te && *ve != '"') {
							if (*ve == '\\' && ve[1]) ve++;
							ve++;
						}
						int vl = (int)(ve - vs);
						/* unescape */
						int o = 0;
						for (int i = 0; i < vl && o < 4095; i++) {
							if (vs[i] == '\\' && i+1 < vl) {
								i++;
								if (vs[i] == 'n') val_buf[o++] = '\n';
								else if (vs[i] == 't') val_buf[o++] = '\t';
								else if (vs[i] == '\\') val_buf[o++] = '\\';
								else if (vs[i] == '"') val_buf[o++] = '"';
								else val_buf[o++] = vs[i];
							} else {
								val_buf[o++] = vs[i];
							}
						}
						val_buf[o] = '\0';
						cb(key_buf, key_len, val_buf, o, 0, user);
						key_done = 0;
					}
				}
			}}
		
#line 147 "json_scan.c"

		goto _st12;
		_ctr6:
			{
#line 41 "json_scan.rl"
			{te = p+1;{
#line 41 "json_scan.rl"
					
					const char *ks = ts + 1;           /* skip opening " */
					const char *ke = ks;
					/* find the closing " — it's the last " before any : in the match */
					while (ke < te && *ke != '"') ke++;
					int kl = (int)(ke - ks);
					if (kl > 255) kl = 255;
					memcpy(key_buf, ks, kl);
					key_buf[kl] = '\0';
					key_len = kl;
					key_done = 1;
				}
			}}
		
#line 168 "json_scan.c"

		goto _st12;
		_ctr7:
			{
#line 86 "json_scan.rl"
			{p = ((te))-1;
				{
#line 86 "json_scan.rl"
					
					if (in_record && key_done) {
						int vl = (int)(te - ts);
						if (vl > 4095) vl = 4095;
						memcpy(val_buf, ts, vl);
						val_buf[vl] = '\0';
						cb(key_buf, key_len, val_buf, vl, 1, user);
						key_done = 0;
					}
				}
			}}
		
#line 188 "json_scan.c"

		goto _st12;
		_ctr13:
			{
#line 1 "-"
			{te = p+1;}}
		
#line 195 "json_scan.c"

		goto _st12;
		_ctr16:
			{
#line 1 "-"
			{te = p+1;}}
		
#line 202 "json_scan.c"

		goto _st12;
		_ctr20:
			{
#line 1 "-"
			{te = p+1;}}
		
#line 209 "json_scan.c"

		goto _st12;
		_ctr26:
			{
#line 37 "json_scan.rl"
			{te = p+1;{
#line 37 "json_scan.rl"
					in_record = 1; key_len = 0; key_done = 0; }
			}}
		
#line 219 "json_scan.c"

		goto _st12;
		_ctr27:
			{
#line 38 "json_scan.rl"
			{te = p+1;{
#line 38 "json_scan.rl"
					if (in_record) { in_record = 0; count++; cb(NULL, 0, NULL, 0, -1, user); } }
			}}
		
#line 229 "json_scan.c"

		goto _st12;
		_ctr28:
			{
#line 1 "-"
			{te = p;p = p - 1;}}
		
#line 236 "json_scan.c"

		goto _st12;
		_ctr29:
			{
#line 1 "-"
			{te = p;p = p - 1;}}
		
#line 243 "json_scan.c"

		goto _st12;
		_ctr30:
			{
#line 55 "json_scan.rl"
			{te = p;p = p - 1;{
#line 55 "json_scan.rl"
					
					if (in_record && key_done) {
						const char *vs = ts + 1;        /* skip opening " */
						/* find the closing " (te may point past it with wsp) */
						const char *ve = vs;
						while (ve < te && *ve != '"') {
							if (*ve == '\\' && ve[1]) ve++;
							ve++;
						}
						int vl = (int)(ve - vs);
						/* unescape */
						int o = 0;
						for (int i = 0; i < vl && o < 4095; i++) {
							if (vs[i] == '\\' && i+1 < vl) {
								i++;
								if (vs[i] == 'n') val_buf[o++] = '\n';
								else if (vs[i] == 't') val_buf[o++] = '\t';
								else if (vs[i] == '\\') val_buf[o++] = '\\';
								else if (vs[i] == '"') val_buf[o++] = '"';
								else val_buf[o++] = vs[i];
							} else {
								val_buf[o++] = vs[i];
							}
						}
						val_buf[o] = '\0';
						cb(key_buf, key_len, val_buf, o, 0, user);
						key_done = 0;
					}
				}
			}}
		
#line 281 "json_scan.c"

		goto _st12;
		_ctr31:
			{
#line 86 "json_scan.rl"
			{te = p;p = p - 1;{
#line 86 "json_scan.rl"
					
					if (in_record && key_done) {
						int vl = (int)(te - ts);
						if (vl > 4095) vl = 4095;
						memcpy(val_buf, ts, vl);
						val_buf[vl] = '\0';
						cb(key_buf, key_len, val_buf, vl, 1, user);
						key_done = 0;
					}
				}
			}}
		
#line 300 "json_scan.c"

		goto _st12;
		_ctr34:
			{
#line 1 "-"
			{te = p;p = p - 1;}}
		
#line 307 "json_scan.c"

		goto _st12;
		_st12:
		if ( p == eof )
			goto _out12;
			{
#line 1 "NONE"
			{ts = 0;}}
		
#line 316 "json_scan.c"

		p+= 1;
		st_case_12:
		if ( p == pe && p != eof )
			goto _out12;
			{
#line 1 "NONE"
			{ts = p;}}
		
#line 325 "json_scan.c"

		if ( p == eof ) {
			goto _st12;}
		else {
			switch( ( (*( p))) ) {
				case 13: {
					goto _st13;
				}
				case 32: {
					goto _st13;
				}
				case 34: {
					goto _ctr18;
				}
				case 44: {
					goto _ctr20;
				}
				case 58: {
					goto _st20;
				}
				case 102: {
					goto _ctr23;
				}
				case 110: {
					goto _ctr24;
				}
				case 116: {
					goto _ctr25;
				}
				case 123: {
					goto _ctr26;
				}
				case 125: {
					goto _ctr27;
				}
			}
			if ( ( (*( p))) < 43 ) {
				if ( 9 <= ( (*( p))) && ( (*( p))) <= 10 ) {
					goto _st13;
				}
			} else if ( ( (*( p))) > 45 ) {
				if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
					goto _ctr21;
				}
			} else {
				goto _st16;
			}
			goto _ctr16;
		}
		_st13:
		if ( p == eof )
			goto _out13;
		p+= 1;
		st_case_13:
		if ( p == pe && p != eof )
			goto _out13;
		if ( p == eof ) {
			goto _ctr28;}
		else {
			switch( ( (*( p))) ) {
				case 13: {
					goto _st13;
				}
				case 32: {
					goto _st13;
				}
			}
			if ( 9 <= ( (*( p))) && ( (*( p))) <= 10 ) {
				goto _st13;
			}
			goto _ctr28;
		}
		_ctr18:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 402 "json_scan.c"

		goto _st14;
		_st14:
		if ( p == eof )
			goto _out14;
		p+= 1;
		st_case_14:
		if ( p == pe && p != eof )
			goto _out14;
		if ( p == eof ) {
			goto _ctr29;}
		else {
			switch( ( (*( p))) ) {
				case 34: {
					goto _ctr2;
				}
				case 92: {
					goto _st2;
				}
			}
			goto _st0;
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
			switch( ( (*( p))) ) {
				case 34: {
					goto _ctr2;
				}
				case 92: {
					goto _st2;
				}
			}
			goto _st0;
		}
		_ctr2:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 449 "json_scan.c"

		goto _st15;
		_st15:
		if ( p == eof )
			goto _out15;
		p+= 1;
		st_case_15:
		if ( p == pe && p != eof )
			goto _out15;
		if ( p == eof ) {
			goto _ctr30;}
		else {
			switch( ( (*( p))) ) {
				case 13: {
					goto _st1;
				}
				case 32: {
					goto _st1;
				}
				case 58: {
					goto _ctr6;
				}
			}
			if ( 9 <= ( (*( p))) && ( (*( p))) <= 10 ) {
				goto _st1;
			}
			goto _ctr30;
		}
		_st1:
		if ( p == eof )
			goto _out1;
		p+= 1;
		st_case_1:
		if ( p == pe && p != eof )
			goto _out1;
		if ( p == eof ) {
			goto _ctr4;}
		else {
			switch( ( (*( p))) ) {
				case 13: {
					goto _st1;
				}
				case 32: {
					goto _st1;
				}
				case 58: {
					goto _ctr6;
				}
			}
			if ( 9 <= ( (*( p))) && ( (*( p))) <= 10 ) {
				goto _st1;
			}
			goto _ctr4;
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
			goto _st0;
		}
		_st16:
		if ( p == eof )
			goto _out16;
		p+= 1;
		st_case_16:
		if ( p == pe && p != eof )
			goto _out16;
		if ( p == eof ) {
			goto _ctr29;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr21;
			}
			goto _ctr29;
		}
		_ctr21:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 535 "json_scan.c"

		goto _st17;
		_st17:
		if ( p == eof )
			goto _out17;
		p+= 1;
		st_case_17:
		if ( p == pe && p != eof )
			goto _out17;
		if ( p == eof ) {
			goto _ctr31;}
		else {
			switch( ( (*( p))) ) {
				case 46: {
					goto _st3;
				}
				case 69: {
					goto _st4;
				}
				case 101: {
					goto _st4;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr21;
			}
			goto _ctr31;
		}
		_st3:
		if ( p == eof )
			goto _out3;
		p+= 1;
		st_case_3:
		if ( p == pe && p != eof )
			goto _out3;
		if ( p == eof ) {
			goto _ctr7;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr8;
			}
			goto _ctr7;
		}
		_ctr8:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 583 "json_scan.c"

		goto _st18;
		_st18:
		if ( p == eof )
			goto _out18;
		p+= 1;
		st_case_18:
		if ( p == pe && p != eof )
			goto _out18;
		if ( p == eof ) {
			goto _ctr31;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st4;
				}
				case 101: {
					goto _st4;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr8;
			}
			goto _ctr31;
		}
		_st4:
		if ( p == eof )
			goto _out4;
		p+= 1;
		st_case_4:
		if ( p == pe && p != eof )
			goto _out4;
		if ( p == eof ) {
			goto _ctr7;}
		else {
			switch( ( (*( p))) ) {
				case 43: {
					goto _st5;
				}
				case 45: {
					goto _st5;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st19;
			}
			goto _ctr7;
		}
		_st5:
		if ( p == eof )
			goto _out5;
		p+= 1;
		st_case_5:
		if ( p == pe && p != eof )
			goto _out5;
		if ( p == eof ) {
			goto _ctr7;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st19;
			}
			goto _ctr7;
		}
		_st19:
		if ( p == eof )
			goto _out19;
		p+= 1;
		st_case_19:
		if ( p == pe && p != eof )
			goto _out19;
		if ( p == eof ) {
			goto _ctr31;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st19;
			}
			goto _ctr31;
		}
		_st20:
		if ( p == eof )
			goto _out20;
		p+= 1;
		st_case_20:
		if ( p == pe && p != eof )
			goto _out20;
		if ( p == eof ) {
			goto _ctr34;}
		else {
			switch( ( (*( p))) ) {
				case 13: {
					goto _st20;
				}
				case 32: {
					goto _st20;
				}
			}
			if ( 9 <= ( (*( p))) && ( (*( p))) <= 10 ) {
				goto _st20;
			}
			goto _ctr34;
		}
		_ctr23:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 689 "json_scan.c"

		goto _st21;
		_st21:
		if ( p == eof )
			goto _out21;
		p+= 1;
		st_case_21:
		if ( p == pe && p != eof )
			goto _out21;
		if ( p == eof ) {
			goto _ctr29;}
		else {
			if ( ( (*( p))) == 97 ) {
				goto _st6;
			}
			goto _ctr29;
		}
		_st6:
		if ( p == eof )
			goto _out6;
		p+= 1;
		st_case_6:
		if ( p == pe && p != eof )
			goto _out6;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 108 ) {
				goto _st7;
			}
			goto _ctr0;
		}
		_st7:
		if ( p == eof )
			goto _out7;
		p+= 1;
		st_case_7:
		if ( p == pe && p != eof )
			goto _out7;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 115 ) {
				goto _st8;
			}
			goto _ctr0;
		}
		_st8:
		if ( p == eof )
			goto _out8;
		p+= 1;
		st_case_8:
		if ( p == pe && p != eof )
			goto _out8;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _ctr13;
			}
			goto _ctr0;
		}
		_ctr24:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 756 "json_scan.c"

		goto _st22;
		_st22:
		if ( p == eof )
			goto _out22;
		p+= 1;
		st_case_22:
		if ( p == pe && p != eof )
			goto _out22;
		if ( p == eof ) {
			goto _ctr29;}
		else {
			if ( ( (*( p))) == 117 ) {
				goto _st9;
			}
			goto _ctr29;
		}
		_st9:
		if ( p == eof )
			goto _out9;
		p+= 1;
		st_case_9:
		if ( p == pe && p != eof )
			goto _out9;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 108 ) {
				goto _st10;
			}
			goto _ctr0;
		}
		_st10:
		if ( p == eof )
			goto _out10;
		p+= 1;
		st_case_10:
		if ( p == pe && p != eof )
			goto _out10;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 108 ) {
				goto _ctr13;
			}
			goto _ctr0;
		}
		_ctr25:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 808 "json_scan.c"

		goto _st23;
		_st23:
		if ( p == eof )
			goto _out23;
		p+= 1;
		st_case_23:
		if ( p == pe && p != eof )
			goto _out23;
		if ( p == eof ) {
			goto _ctr29;}
		else {
			if ( ( (*( p))) == 114 ) {
				goto _st11;
			}
			goto _ctr29;
		}
		_st11:
		if ( p == eof )
			goto _out11;
		p+= 1;
		st_case_11:
		if ( p == pe && p != eof )
			goto _out11;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 117 ) {
				goto _st8;
			}
			goto _ctr0;
		}
		_out12: cs = 12; goto _out; 
		_out13: cs = 13; goto _out; 
		_out14: cs = 14; goto _out; 
		_out0: cs = 0; goto _out; 
		_out15: cs = 15; goto _out; 
		_out1: cs = 1; goto _out; 
		_out2: cs = 2; goto _out; 
		_out16: cs = 16; goto _out; 
		_out17: cs = 17; goto _out; 
		_out3: cs = 3; goto _out; 
		_out18: cs = 18; goto _out; 
		_out4: cs = 4; goto _out; 
		_out5: cs = 5; goto _out; 
		_out19: cs = 19; goto _out; 
		_out20: cs = 20; goto _out; 
		_out21: cs = 21; goto _out; 
		_out6: cs = 6; goto _out; 
		_out7: cs = 7; goto _out; 
		_out8: cs = 8; goto _out; 
		_out22: cs = 22; goto _out; 
		_out9: cs = 9; goto _out; 
		_out10: cs = 10; goto _out; 
		_out23: cs = 23; goto _out; 
		_out11: cs = 11; goto _out; 
		_out: {}
	}
	
#line 124 "json_scan.rl"

	
	return count;
}

/* scan file via mmap */
int json_scan_file(const char *filename, json_field_cb cb, void *user) {
	int fd = open(filename, O_RDONLY);
	if (fd < 0) return -1;
		struct stat st; fstat(fd, &st);
	char *d = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	if (d == MAP_FAILED) return -1;
		int n = json_scan(d, (size_t)st.st_size, cb, user);
	munmap(d, (size_t)st.st_size);
	return n;
}
