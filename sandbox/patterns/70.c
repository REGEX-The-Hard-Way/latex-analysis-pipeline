#line 1 "70.rl"
/* Pattern 70: Buffer Streaming — shift+refill buffer size 16 */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define BUFSIZE 16
char buffer[BUFSIZE];
int have = 0;


#line 11 "70.c"
static const signed char _p70_actions[] = {
	0, 1, 0, 0
};

static const signed char _p70_key_offsets[] = {
	0, 0, 0
};

static const char _p70_trans_keys[] = {
	65, 90, 97, 122, 0
};

static const signed char _p70_single_lengths[] = {
	0, 0, 0
};

static const signed char _p70_range_lengths[] = {
	0, 2, 0
};

static const signed char _p70_index_offsets[] = {
	0, 0, 0
};

static const signed char _p70_cond_targs[] = {
	1, 1, 0, 0, 1, 0
};

static const signed char _p70_cond_actions[] = {
	1, 1, 0, 0, 0, 0
};

static const int p70_start = 1;
static const int p70_first_final = 1;
static const int p70_error = 0;

static const int p70_en_main = 1;


#line 17 "70.rl"


int main(void){int cs;const char *p,*pe,*eof;
	const char *input = "abcdefghijklmnopqrstuvwxyz";
	int input_len = strlen(input);
	int offset = 0;
	
	printf("Pattern 70: Buffer Streaming (BS=%d)\n\n", BUFSIZE);
	

#line 59 "70.c"
	{
		cs = (int)p70_start;
	}
	
#line 26 "70.rl"

	
	while (offset < input_len) {
		if (have > 0) {
			memmove(buffer, p, have);
		}
		int space = BUFSIZE - have;
		int chunk = input_len - offset;
		if (chunk > space) chunk = space;
			memcpy(buffer + have, input + offset, chunk);
		offset += chunk;
		int len = have + chunk;
		
		p = buffer;
		pe = buffer + len;
		eof = (offset >= input_len) ? pe : NULL;
		

#line 80 "70.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			const signed char * _acts;
			unsigned int _nacts;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p70_trans_keys + (_p70_key_offsets[cs]));
			_trans = (unsigned int)_p70_index_offsets[cs];
			
			_klen = (int)_p70_single_lengths[cs];
			if ( _klen > 0 ) {
				const char *_lower = _keys;
				const char *_upper = _keys + _klen - 1;
				const char *_mid;
				while ( 1 ) {
					if ( _upper < _lower ) {
						_keys += _klen;
						_trans += (unsigned int)_klen;
						break;
					}
					
					_mid = _lower + ((_upper-_lower) >> 1);
					if ( ( (*( p))) < (*( _mid)) )
						_upper = _mid - 1;
					else if ( ( (*( p))) > (*( _mid)) )
						_lower = _mid + 1;
					else {
						_trans += (unsigned int)(_mid - _keys);
						goto _match;
					}
				}
			}
			
			_klen = (int)_p70_range_lengths[cs];
			if ( _klen > 0 ) {
				const char *_lower = _keys;
				const char *_upper = _keys + (_klen<<1) - 2;
				const char *_mid;
				while ( 1 ) {
					if ( _upper < _lower ) {
						_trans += (unsigned int)_klen;
						break;
					}
					
					_mid = _lower + (((_upper-_lower) >> 1) & ~1);
					if ( ( (*( p))) < (*( _mid)) )
						_upper = _mid - 2;
					else if ( ( (*( p))) > (*( _mid + 1)) )
						_lower = _mid + 2;
					else {
						_trans += (unsigned int)((_mid - _keys)>>1);
						break;
					}
				}
			}
			
			_match: {}
			cs = (int)_p70_cond_targs[_trans];
			
			if ( _p70_cond_actions[_trans] != 0 ) {
				
				_acts = ( _p70_actions + (_p70_cond_actions[_trans]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 ) {
					switch ( (*( _acts)) )
					{
						case 0:  {
								{
#line 12 "70.rl"
								printf("TOKEN(%c) ", (( (*( p))))); }
							
#line 155 "70.c"

							break; 
						}
					}
					_nacts -= 1;
					_acts += 1;
				}
				
			}
			
			if ( cs != 0 ) {
				p += 1;
				goto _resume;
			}
			_out: {}
		}
		
#line 43 "70.rl"

		
		have = pe - p;
		printf("chunk=%d have=%d\n", chunk, have);
	}
	
	if (cs >= p70_first_final)
		printf("\nACCEPT\n");
	else
		printf("\nFAIL\n");
	
	assert(cs >= p70_first_final);
	printf("\nAll tests passed.\n");return 0;}
