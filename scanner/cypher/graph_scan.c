#line 1 "graph_scan.rl"
/*
* graph_scan.rl — Ragel state machine for direct mmap'd JSON queries.
*
* Scans "type":"LABEL" patterns in newline-delimited JSON using a
* single-pass Ragel scanner. Eliminates strstr() passes and stack
* buffer copies per line.
*
* Compiled with: ragel -m -G2 graph_scan.rl
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "cypher_parser.h"


#line 20 "graph_scan.c"
static const int json_line_scan_start = 15;
static const int json_line_scan_first_final = 15;
static const int json_line_scan_error = -1;

static const int json_line_scan_en_main = 15;


#line 22 "graph_scan.rl"


int cypher_scan_sidecar(const char *filename, const char *label,
const char *contains, int limit,
cypher_result_t *result) {
	int fd = open(filename, O_RDONLY);
	if (fd < 0) return -1;
		struct stat st;
	if (fstat(fd, &st) < 0) { close(fd); return -1; }
	const char *p  = (const char *)mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	if (p == MAP_FAILED) return -1;
		const char *pe = p + st.st_size;
	const char *eof = pe;
	
	int cs, act;
	const char *ts, *te;
	int count = 0;
	int limit_val = limit > 0 ? limit : MAX_ROWS;
	int type_match = 0;
	char text_buf[2048];
	
	
#line 86 "graph_scan.rl"

	

#line 52 "graph_scan.c"
	{
		cs = (int)json_line_scan_start;
		ts = 0;
		te = 0;
	}
	
#line 88 "graph_scan.rl"


#line 59 "graph_scan.c"
{
		switch ( cs ) {
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
			case 3:
			goto st_case_3;
			case 4:
			goto st_case_4;
			case 5:
			goto st_case_5;
			case 6:
			goto st_case_6;
			case 7:
			goto st_case_7;
			case 8:
			goto st_case_8;
			case 9:
			goto st_case_9;
			case 10:
			goto st_case_10;
			case 11:
			goto st_case_11;
			case 12:
			goto st_case_12;
			case 13:
			goto st_case_13;
			case 14:
			goto st_case_14;
		}
		_ctr0:
			{
#line 1 "-"
			{p = ((te))-1;
			}}
		
#line 102 "graph_scan.c"

		goto _st15;
		_ctr8:
			{
#line 52 "graph_scan.rl"
			{te = p+1;{
#line 52 "graph_scan.rl"
					
					if (type_match && count < limit_val) {
						const char *vs = ts + 8; /* skip "text":" */
						int vl = (int)(te - vs - 1);
						/* unescape and capture */
						int ti = 0;
						for (int i = 0; i < vl && ti < 2047; i++) {
							if (vs[i] == '\\' && i+1 < vl) {
								i++;
								if (vs[i] == 'n') text_buf[ti++] = '\n';
								else if (vs[i] == 't') text_buf[ti++] = '\t';
								else if (vs[i] == '\\') text_buf[ti++] = '\\';
								else text_buf[ti++] = vs[i];
							} else {
								text_buf[ti++] = vs[i];
							}
						}
						text_buf[ti] = '\0';
						if (!contains || strstr(text_buf, contains)) {
							cypher_result_add_row_empty(result);
							cypher_result_set_cell(result, count, 0, text_buf);
							count++;
						}
						type_match = 0;
					}
				}
			}}
		
#line 137 "graph_scan.c"

		goto _st15;
		_ctr16:
			{
#line 46 "graph_scan.rl"
			{te = p+1;{
#line 46 "graph_scan.rl"
					
					const char *vs = ts + 8; /* skip "type":" */
					int vl = (int)(te - vs - 1);
					type_match = (vl == (int)strlen(label)
					&& !strncmp(vs, label, vl));
				}
			}}
		
#line 152 "graph_scan.c"

		goto _st15;
		_ctr18:
			{
#line 1 "-"
			{te = p+1;}}
		
#line 159 "graph_scan.c"

		goto _st15;
		_ctr20:
			{
#line 45 "graph_scan.rl"
			{te = p+1;{
#line 45 "graph_scan.rl"
					type_match = 0; }
			}}
		
#line 169 "graph_scan.c"

		goto _st15;
		_ctr21:
			{
#line 1 "-"
			{te = p;p = p - 1;}}
		
#line 176 "graph_scan.c"

		goto _st15;
		_st15:
		if ( p == eof )
			goto _out15;
			{
#line 1 "NONE"
			{ts = 0;}}
		
#line 185 "graph_scan.c"

		p+= 1;
		st_case_15:
		if ( p == pe && p != eof )
			goto _out15;
			{
#line 1 "NONE"
			{ts = p;}}
		
#line 194 "graph_scan.c"

		if ( p == eof ) {
			goto _st15;}
		else {
			switch( ( (*( p))) ) {
				case 34: {
					goto _ctr19;
				}
				case 123: {
					goto _ctr20;
				}
			}
			goto _ctr18;
		}
		_ctr19:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 213 "graph_scan.c"

		goto _st16;
		_st16:
		if ( p == eof )
			goto _out16;
		p+= 1;
		st_case_16:
		if ( p == pe && p != eof )
			goto _out16;
		if ( p == eof ) {
			goto _ctr21;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st0;
			}
			goto _ctr21;
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
				case 101: {
					goto _st1;
				}
				case 121: {
					goto _st8;
				}
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
			if ( ( (*( p))) == 120 ) {
				goto _st2;
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
			if ( ( (*( p))) == 116 ) {
				goto _st3;
			}
			goto _ctr0;
		}
		_st3:
		if ( p == eof )
			goto _out3;
		p+= 1;
		st_case_3:
		if ( p == pe && p != eof )
			goto _out3;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 34 ) {
				goto _st4;
			}
			goto _ctr0;
		}
		_st4:
		if ( p == eof )
			goto _out4;
		p+= 1;
		st_case_4:
		if ( p == pe && p != eof )
			goto _out4;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 58 ) {
				goto _st5;
			}
			goto _ctr0;
		}
		_st5:
		if ( p == eof )
			goto _out5;
		p+= 1;
		st_case_5:
		if ( p == pe && p != eof )
			goto _out5;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 34 ) {
				goto _st6;
			}
			goto _ctr0;
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
			switch( ( (*( p))) ) {
				case 34: {
					goto _ctr8;
				}
				case 92: {
					goto _st7;
				}
			}
			goto _st6;
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
			goto _st6;
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
			if ( ( (*( p))) == 112 ) {
				goto _st9;
			}
			goto _ctr0;
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
			if ( ( (*( p))) == 101 ) {
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
			if ( ( (*( p))) == 34 ) {
				goto _st11;
			}
			goto _ctr0;
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
			if ( ( (*( p))) == 58 ) {
				goto _st12;
			}
			goto _ctr0;
		}
		_st12:
		if ( p == eof )
			goto _out12;
		p+= 1;
		st_case_12:
		if ( p == pe && p != eof )
			goto _out12;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 34 ) {
				goto _st13;
			}
			goto _ctr0;
		}
		_st13:
		if ( p == eof )
			goto _out13;
		p+= 1;
		st_case_13:
		if ( p == pe && p != eof )
			goto _out13;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 34 ) {
				goto _ctr0;
			}
			goto _st14;
		}
		_st14:
		if ( p == eof )
			goto _out14;
		p+= 1;
		st_case_14:
		if ( p == pe && p != eof )
			goto _out14;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 34 ) {
				goto _ctr16;
			}
			goto _st14;
		}
		_out15: cs = 15; goto _out; 
		_out16: cs = 16; goto _out; 
		_out0: cs = 0; goto _out; 
		_out1: cs = 1; goto _out; 
		_out2: cs = 2; goto _out; 
		_out3: cs = 3; goto _out; 
		_out4: cs = 4; goto _out; 
		_out5: cs = 5; goto _out; 
		_out6: cs = 6; goto _out; 
		_out7: cs = 7; goto _out; 
		_out8: cs = 8; goto _out; 
		_out9: cs = 9; goto _out; 
		_out10: cs = 10; goto _out; 
		_out11: cs = 11; goto _out; 
		_out12: cs = 12; goto _out; 
		_out13: cs = 13; goto _out; 
		_out14: cs = 14; goto _out; 
		_out: {}
	}
	
#line 89 "graph_scan.rl"

	
	munmap((void *)p, (size_t)st.st_size);
	return count;
}
