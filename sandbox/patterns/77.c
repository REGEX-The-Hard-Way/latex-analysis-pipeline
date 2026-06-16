#line 1 "77.rl"
/* Pattern 77: URL Parser — scheme://authority/path?query#fragment */
#include <stdio.h>
#include <string.h>
#include <assert.h>

char scheme[64], host[128], path[256], query[256], frag[256];
char *s=scheme,*h=host,*pa=path,*q=query,*fr=frag;


#line 10 "77.c"
static const signed char _p77_actions[] = {
	0, 1, 1, 1, 2, 1, 4, 1,
	5, 1, 7, 1, 8, 1, 10, 1,
	11, 1, 13, 1, 14, 2, 0, 1,
	2, 3, 4, 2, 6, 7, 2, 6,
	8, 2, 9, 10, 2, 9, 11, 2,
	12, 13, 2, 12, 14, 0
};

static const signed char _p77_key_offsets[] = {
	0, 0, 4, 16, 17, 18, 21, 24,
	26, 28, 29, 30, 37, 44, 46, 52,
	0
};

static const char _p77_trans_keys[] = {
	65, 90, 97, 122, 10, 35, 43, 47,
	58, 63, 45, 57, 65, 90, 97, 122,
	10, 10, 10, 35, 63, 10, 35, 63,
	10, 35, 10, 35, 47, 47, 32, 35,
	47, 58, 63, 9, 10, 9, 10, 32,
	35, 47, 58, 63, 48, 57, 10, 35,
	47, 63, 48, 57, 0
};

static const signed char _p77_single_lengths[] = {
	0, 0, 6, 1, 1, 3, 3, 2,
	2, 1, 1, 5, 7, 0, 4, 0,
	0
};

static const signed char _p77_range_lengths[] = {
	0, 2, 3, 0, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 1, 1, 0,
	0
};

static const signed char _p77_index_offsets[] = {
	0, 0, 3, 13, 15, 17, 21, 25,
	28, 31, 33, 35, 42, 50, 52, 58,
	0
};

static const signed char _p77_cond_targs[] = {
	2, 2, 0, 15, 3, 2, 5, 9,
	7, 2, 2, 2, 0, 15, 4, 15,
	4, 15, 3, 7, 6, 15, 3, 7,
	6, 15, 3, 8, 15, 3, 8, 10,
	0, 11, 0, 0, 0, 0, 0, 0,
	0, 12, 0, 15, 0, 3, 5, 13,
	7, 12, 14, 0, 15, 3, 5, 7,
	14, 0, 0, 0, 1, 2, 3, 4,
	5, 6, 7, 8, 9, 10, 11, 12,
	13, 14, 15, 0
};

static const signed char _p77_cond_actions[] = {
	21, 21, 0, 3, 3, 1, 3, 3,
	3, 1, 1, 1, 0, 42, 39, 19,
	17, 30, 30, 30, 27, 11, 11, 11,
	9, 36, 36, 33, 15, 15, 13, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 24, 0, 7, 0, 7, 7, 7,
	7, 5, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0
};

static const int p77_start = 1;
static const int p77_first_final = 15;
static const int p77_error = 0;

static const int p77_en_main = 1;


#line 46 "77.rl"

int main(void){int cs;const char *p,*pe,*eof;
	printf("Pattern 77: URL Parser\n\n");
	const char *test_url = "https://example.com/path/to/file?key=val#section\n";
	p=test_url;pe=p+strlen(p);eof=pe;

#line 91 "77.c"
	{
		cs = (int)p77_start;
	}
	
#line 51 "77.rl"


#line 96 "77.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p77_trans_keys + (_p77_key_offsets[cs]));
		_trans = (unsigned int)_p77_index_offsets[cs];
		
		_klen = (int)_p77_single_lengths[cs];
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
		
		_klen = (int)_p77_range_lengths[cs];
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
		cs = (int)_p77_cond_targs[_trans];
		
		if ( _p77_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p77_actions + (_p77_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 11 "77.rl"
							s=scheme; *s=0; }
						
#line 171 "77.c"

						break; 
					}
					case 1:  {
							{
#line 12 "77.rl"
							*s++=(( (*( p)))); }
						
#line 179 "77.c"

						break; 
					}
					case 2:  {
							{
#line 13 "77.rl"
							*s=0; printf("scheme=%s\n",scheme); }
						
#line 187 "77.c"

						break; 
					}
					case 3:  {
							{
#line 15 "77.rl"
							h=host; *h=0; }
						
#line 195 "77.c"

						break; 
					}
					case 4:  {
							{
#line 16 "77.rl"
							*h++=(( (*( p)))); }
						
#line 203 "77.c"

						break; 
					}
					case 5:  {
							{
#line 17 "77.rl"
							*h=0; printf("host=%s\n",host); }
						
#line 211 "77.c"

						break; 
					}
					case 6:  {
							{
#line 19 "77.rl"
							pa=path; *pa=0; }
						
#line 219 "77.c"

						break; 
					}
					case 7:  {
							{
#line 20 "77.rl"
							*pa++=(( (*( p)))); }
						
#line 227 "77.c"

						break; 
					}
					case 8:  {
							{
#line 21 "77.rl"
							*pa=0; printf("path=%s\n",path); }
						
#line 235 "77.c"

						break; 
					}
					case 9:  {
							{
#line 23 "77.rl"
							q=query; *q=0; }
						
#line 243 "77.c"

						break; 
					}
					case 10:  {
							{
#line 24 "77.rl"
							*q++=(( (*( p)))); }
						
#line 251 "77.c"

						break; 
					}
					case 11:  {
							{
#line 25 "77.rl"
							*q=0; printf("query=%s\n",query); }
						
#line 259 "77.c"

						break; 
					}
					case 12:  {
							{
#line 27 "77.rl"
							fr=frag; *fr=0; }
						
#line 267 "77.c"

						break; 
					}
					case 13:  {
							{
#line 28 "77.rl"
							*fr++=(( (*( p)))); }
						
#line 275 "77.c"

						break; 
					}
					case 14:  {
							{
#line 29 "77.rl"
							*fr=0; printf("frag=%s\n",frag); }
						
#line 283 "77.c"

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
	
#line 52 "77.rl"

	printf("-> %s ok\n",cs>=p77_first_final?"ACCEPT":"FAIL");assert(cs>=p77_first_final);
	printf("\nAll tests passed.\n");return 0;}
