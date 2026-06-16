#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/87.rl"
/* Pattern 87: Mail Header Extractor — parse headers, extract From/To/Subject */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/87.c"
static const signed char _p87_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 2,
	1, 2, 0
};

static const signed char _p87_key_offsets[] = {
	0, 0, 3, 6, 8, 9, 12, 14,
	15, 18, 21, 24, 0
};

static const char _p87_trans_keys[] = {
	10, 32, 58, 10, 32, 58, 10, 32,
	10, 10, 32, 58, 10, 32, 10, 10,
	32, 58, 10, 32, 58, 10, 32, 58,
	0
};

static const signed char _p87_single_lengths[] = {
	0, 3, 3, 2, 1, 3, 2, 1,
	3, 3, 3, 0, 0
};

static const signed char _p87_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0
};

static const signed char _p87_index_offsets[] = {
	0, 0, 4, 8, 11, 13, 17, 20,
	22, 26, 30, 34, 0
};

static const signed char _p87_cond_targs[] = {
	0, 0, 0, 2, 0, 0, 3, 2,
	8, 3, 4, 8, 4, 0, 0, 6,
	5, 9, 6, 7, 9, 7, 11, 0,
	0, 5, 10, 0, 0, 5, 0, 0,
	0, 5, 0, 0, 1, 2, 3, 4,
	5, 6, 7, 8, 9, 10, 11, 0
};

static const signed char _p87_cond_actions[] = {
	0, 0, 0, 1, 0, 0, 0, 0,
	7, 3, 3, 5, 0, 0, 0, 0,
	0, 7, 3, 3, 5, 0, 0, 0,
	0, 1, 0, 0, 0, 1, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

static const int p87_start = 1;
static const int p87_first_final = 8;
static const int p87_error = 0;

static const int p87_en_main = 1;


#line 23 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/87.rl"

int main(void){int cs; const char *p,*pe,*eof;
	const char *hdr,*key,*val; ptrdiff_t klen,vlen; int hstart;
	const char *data =
	"From: alice@example.com\n"
	"To: bob@test.org\n"
	"Subject: Hello World\n"
	"Date: Mon, 1 Jan 2025\n"
	"\n";
	printf("Pattern 87: Mail Header Extractor\n\n");
	printf("Headers:\n%s",data);
	p=data; pe=p+strlen(data); eof=pe;

#line 75 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/87.c"
	{
		cs = (int)p87_start;
	}
	
#line 35 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/87.rl"


#line 80 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/87.c"
	{
		int _klen;
		unsigned int _trans = 0;
		const char * _keys;
		const signed char * _acts;
		unsigned int _nacts;
		_resume: {}
		if ( p == pe )
			goto _out;
		_keys = ( _p87_trans_keys + (_p87_key_offsets[cs]));
		_trans = (unsigned int)_p87_index_offsets[cs];
		
		_klen = (int)_p87_single_lengths[cs];
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
		
		_klen = (int)_p87_range_lengths[cs];
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
		cs = (int)_p87_cond_targs[_trans];
		
		if ( _p87_cond_actions[_trans] != 0 ) {
			
			_acts = ( _p87_actions + (_p87_cond_actions[_trans]));
			_nacts = (unsigned int)(*( _acts));
			_acts += 1;
			while ( _nacts > 0 ) {
				switch ( (*( _acts)) )
				{
					case 0:  {
							{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/87.rl"
							key = p; klen = 0; }
						
#line 155 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/87.c"

						break; 
					}
					case 1:  {
							{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/87.rl"
							val = p; }
						
#line 163 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/87.c"

						break; 
					}
					case 2:  {
							{
#line 10 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/87.rl"
							
							vlen = p - val;
							char kb[64]; int nk = klen<63?klen:63;
							memcpy(kb,key,nk); kb[nk]=0;
							if(strcmp(kb,"From")==0||strcmp(kb,"To")==0||strcmp(kb,"Subject")==0)
							printf("  %s: %.*s\n",kb,(int)vlen,val);
						}
						
#line 177 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/87.c"

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
	
#line 36 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/87.rl"

	int r=cs>=p87_first_final;
	printf("  -> %s\n",r?"ACCEPT":"FAIL"); assert(r);
	printf("\nAll tests passed.\n");return 0;
}
