#line 1 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.rl"
/* Pattern 81: CLI Arg Parser — parse NUL-delimited argv strings */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.c"
static const signed char _p81_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1,
	3, 1, 4, 2, 4, 3, 0
};

static const signed char _p81_key_offsets[] = {
	0, 0, 3, 5, 7, 10, 0
};

static const char _p81_trans_keys[] = {
	0, 32, 45, 0, 32, 0, 32, 0,
	32, 45, 0, 32, 104, 111, 118, 0
};

static const signed char _p81_single_lengths[] = {
	0, 3, 2, 2, 3, 5, 0
};

static const signed char _p81_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0
};

static const signed char _p81_index_offsets[] = {
	0, 0, 4, 7, 10, 14, 0
};

static const signed char _p81_cond_targs[] = {
	4, 4, 5, 2, 3, 4, 2, 3,
	4, 0, 3, 4, 5, 2, 3, 4,
	2, 2, 2, 2, 0, 1, 2, 3,
	4, 5, 0
};

static const signed char _p81_cond_actions[] = {
	11, 11, 9, 9, 7, 7, 0, 0,
	0, 0, 11, 11, 9, 9, 7, 7,
	1, 5, 3, 0, 0, 11, 7, 0,
	11, 7, 0
};

static const signed char _p81_eof_trans[] = {
	21, 22, 23, 24, 25, 26, 0
};

static const int p81_start = 1;
static const int p81_first_final = 1;
static const int p81_error = 0;

static const int p81_en_main = 1;


#line 16 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.rl"

int main(void){int cs; const char *p,*pe,*eof;
	struct {const char *s; int ok;} t[]={
		{"-h\0-v\0-o\0",1},
		{"src.txt\0-o\0result.txt\0",1},
		{"-x\0",0},
		{"\0",1},
		{"-h\0-v\0-o\0file.txt\0",1},
	}; int n=5; const char *buf;
	printf("Pattern 81: CLI Arg Parser\n\n");
	for(int i=0;i<n;i++){p=t[i].s;pe=p+strlen(p)+1;eof=pe-1;
		printf("Input %d:",i);for(const char *c=p;c<eof;c++)printf(" %c",*c);printf("\n");

#line 69 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.c"
	{
			cs = (int)p81_start;
		}
		
#line 28 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.rl"


#line 74 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.c"
	{
			int _klen;
			unsigned int _trans = 0;
			const char * _keys;
			const signed char * _acts;
			unsigned int _nacts;
			_resume: {}
			if ( p == pe && p != eof )
				goto _out;
			if ( p == eof ) {
				if ( _p81_eof_trans[cs] > 0 ) {
					_trans = (unsigned int)_p81_eof_trans[cs] - 1;
				}
			}
			else {
				_keys = ( _p81_trans_keys + (_p81_key_offsets[cs]));
				_trans = (unsigned int)_p81_index_offsets[cs];
				
				_klen = (int)_p81_single_lengths[cs];
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
				
				_klen = (int)_p81_range_lengths[cs];
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
			}
			cs = (int)_p81_cond_targs[_trans];
			
			if ( _p81_cond_actions[_trans] != 0 ) {
				
				_acts = ( _p81_actions + (_p81_cond_actions[_trans]));
				_nacts = (unsigned int)(*( _acts));
				_acts += 1;
				while ( _nacts > 0 ) {
					switch ( (*( _acts)) )
					{
						case 0:  {
								{
#line 6 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.rl"
								printf("  flag: help\n"); }
							
#line 156 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.c"

							break; 
						}
						case 1:  {
								{
#line 7 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.rl"
								printf("  flag: version\n"); }
							
#line 164 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.c"

							break; 
						}
						case 2:  {
								{
#line 8 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.rl"
								printf("  flag: output\n"); }
							
#line 172 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.c"

							break; 
						}
						case 3:  {
								{
#line 9 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.rl"
								printf("  arg: %.*s\n", (int)(p-buf), buf); }
							
#line 180 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.c"

							break; 
						}
						case 4:  {
								{
#line 12 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.rl"
								buf=p;}
							
#line 188 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.c"

							break; 
						}
					}
					_nacts -= 1;
					_acts += 1;
				}
				
			}
			
			if ( p == eof ) {
				if ( cs >= 1 )
					goto _out;
			}
			else {
				if ( cs != 0 ) {
					p += 1;
					goto _resume;
				}
			}
			_out: {}
		}
		
#line 29 "/mnt/x/home/user/DEV/latex-analysis-pipeline/ragel/patterns/81.rl"

		int r=cs>=p81_first_final;
		printf("  -> %s (expect %s) %s\n",r?"ACCEPT":"FAIL",t[i].ok?"ACCEPT":"FAIL",r==t[i].ok?"ok":"FAIL");
		assert(r==t[i].ok);
	}
	printf("\nAll %d tests passed.\n",n);return 0;
}
