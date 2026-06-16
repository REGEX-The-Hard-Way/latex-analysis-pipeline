#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/83.rl"
/* Pattern 83: Incremental Line Processing — backward newline search + partial */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/83.c"
static const signed char _p83_actions[] = {
	0, 2, 0, 1, 0
};

static const signed char _p83_key_offsets[] = {
	0, 0
};

static const char _p83_trans_keys[] = {
	10, 0
};

static const signed char _p83_single_lengths[] = {
	1, 0
};

static const signed char _p83_range_lengths[] = {
	0, 0
};

static const signed char _p83_index_offsets[] = {
	0, 0
};

static const signed char _p83_cond_targs[] = {
	0, 0, 0, 0
};

static const signed char _p83_cond_actions[] = {
	1, 0, 0, 0
};

static const int p83_start = 0;
static const int p83_first_final = 0;
static const int p83_error = -1;

static const int p83_en_main = 0;


#line 11 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/83.rl"

int main(void){int cs; const char *p,*pe,*eof; const char *bol=0;
	struct chunk { const char *data; int len; int eof; } chunks[] = {
		{"hello\nwor",8,0},
		{"ld\nfoo\n",8,0},
		{"partial",7,1},
	}; int nc=3;
	printf("Pattern 83: Incremental Line Processing\n\n");
	for(int i=0;i<nc;i++){
		p=chunks[i].data; pe=p+chunks[i].len; eof=chunks[i].eof?pe:0;
		if(i==0){

#line 56 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/83.c"
	{
				cs = (int)p83_start;
			}
			
#line 22 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/83.rl"

		}

#line 62 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/83.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			const signed char * _acts;
			unsigned int _nacts;
			_resume: {}
			if ( p == pe )
				goto _out;
			_keys = ( _p83_trans_keys + (_p83_key_offsets[cs]));
			_trans = (unsigned int)_p83_index_offsets[cs];
			
			_klen = (int)_p83_single_lengths[cs];
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
			
			_klen = (int)_p83_range_lengths[cs];
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
			cs = (int)_p83_cond_targs[_trans];
			
			if ( _p83_cond_actions[_trans] != 0 ) {
				
				_acts = ( _p83_actions + (_p83_cond_actions[_trans]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 ) {
					switch ( (*( _acts)) )
					{
						case 0:  {
								{
#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/83.rl"
								bol = p; }
							
#line 137 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/83.c"

							break; 
						}
						case 1:  {
								{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/83.rl"
								printf("  complete: %.*s\n", (int)(p - bol - 1), bol); }
							
#line 145 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/83.c"

							break; 
						}
					}
					_nacts -= 1;
					_acts += 1;
				}
				
			}
			
			p += 1;
			goto _resume;
			_out: {}
		}
		
#line 24 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/83.rl"

		if(chunks[i].eof && p<pe){
			printf("  partial remainder: %.*s\n",(int)(pe-p),p);
		}
	}
	int r=cs>=p83_first_final;
	printf("\n  -> %s\n",r?"ACCEPT":"FAIL"); assert(r);
	printf("All tests passed.\n");return 0;
}
