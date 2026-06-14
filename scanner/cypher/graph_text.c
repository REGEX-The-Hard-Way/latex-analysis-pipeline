#line 1 "graph_text.rl"
/*
* graph_text.rl — Ragel state machine for trigram extraction (Phase 5).
*
* Scans property strings and emits 3-byte trigrams as packed uint32 values.
* Replaces the hand-written sliding window loop at graph_store.c:340-395.
*
* Compiled with: ragel -m -G2 graph_text.rl
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph_store.h"


#line 15 "graph_text.c"
static const int trigram_extract_start = 1;
static const int trigram_extract_first_final = 1;
static const int trigram_extract_error = -1;

static const int trigram_extract_en_main = 1;


#line 17 "graph_text.rl"


/*
* Extract trigrams from a property string into a temporary hash map.
* Returns the number of unique trigrams found.
*/
int gs_extract_trigrams(const char *str, int len,
uint32_t *out, int max_out) {
	const char *p  = str;
	const char *pe = str + len;
	const char *eof = pe;
	int cs;
	const char *ts, *te;
	int act;
	int count = 0;
	
	
#line 48 "graph_text.rl"

	

#line 41 "graph_text.c"
	{
		cs = (int)trigram_extract_start;
		ts = 0;
		te = 0;
	}
	
#line 50 "graph_text.rl"


#line 48 "graph_text.c"
{
		switch ( cs ) {
			case 1:
			goto st_case_1;
			case 2:
			goto st_case_2;
			case 0:
			goto st_case_0;
		}
		_ctr0:
			{
#line 1 "-"
			{p = ((te))-1;
			}}
		
#line 63 "graph_text.c"

		goto _st1;
		_ctr1:
			{
#line 34 "graph_text.rl"
			{te = p+1;{
#line 34 "graph_text.rl"
					
					if (count < max_out) {
						uint32_t tg;
						tg  = ((unsigned int)(unsigned char)ts[0]) << 16;
						tg |= ((unsigned int)(unsigned char)ts[1]) << 8;
						tg |=  (unsigned int)(unsigned char)ts[2];
						out[count++] = tg;
					}
				}
			}}
		
#line 81 "graph_text.c"

		goto _st1;
		_ctr4:
			{
#line 1 "-"
			{te = p;p = p - 1;}}
		
#line 88 "graph_text.c"

		goto _st1;
		_st1:
		if ( p == eof )
			goto _out1;
			{
#line 1 "NONE"
			{ts = 0;}}
		
#line 97 "graph_text.c"

		p+= 1;
		st_case_1:
		if ( p == pe && p != eof )
			goto _out1;
			{
#line 1 "NONE"
			{ts = p;}}
		
#line 106 "graph_text.c"

		if ( p == eof ) {
			goto _st1;}
		else {
			goto _ctr3;
		}
		_ctr3:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 117 "graph_text.c"

		goto _st2;
		_st2:
		if ( p == eof )
			goto _out2;
		p+= 1;
		st_case_2:
		if ( p == pe && p != eof )
			goto _out2;
		if ( p == eof ) {
			goto _ctr4;}
		else {
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
			goto _ctr1;
		}
		_out1: cs = 1; goto _out; 
		_out2: cs = 2; goto _out; 
		_out0: cs = 0; goto _out; 
		_out: {}
	}
	
#line 51 "graph_text.rl"

	
	return count;
}
