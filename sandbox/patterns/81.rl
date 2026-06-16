/* Pattern 81: CLI Arg Parser — parse space-delimited argv strings */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p81;
  action flag_h { printf("  flag: help\n"); }
  action flag_v { printf("  flag: version\n"); }
  action flag_o { printf("  flag: output\n"); }
  action arg    { printf("  arg: %.*s\n", (int)(p-buf), buf); }
  flag = '-' ( 'h' @flag_h | 'v' @flag_v | 'o' @flag_o );
  word = [a-zA-Z0-9._] >{buf=p;} [a-zA-Z0-9._\-]* %arg;
  ws = ' '+;
  item = flag | word;
  main := (item (ws item)*)? '\n';
  write data;
}%%
int main(void){int cs; const char *p,*pe,*eof;
  struct {const char *s; int ok;} t[]={
    {"-h -v -o\n",1},
    {"src.txt -o result.txt\n",1},
    {"-x\n",0},
    {"\n",1},
    {"-h file.txt -o out.dat\n",1},
    {"bad!arg\n",0},
    {"-h data.txt\n",1},
  }; int n=7; const char *buf;
  printf("Pattern 81: CLI Arg Parser\n\n");
  for(int i=0;i<n;i++){p=t[i].s;pe=p+strlen(p);eof=pe;
    printf("Input %d: %s",i,t[i].s);
    %% write init;
    %% write exec;
    int r=cs>=p81_first_final;
    printf("  -> %s (expect %s) %s\n",r?"ACCEPT":"FAIL",t[i].ok?"ACCEPT":"FAIL",r==t[i].ok?"ok":"FAIL");
    assert(r==t[i].ok);
  }
  printf("\nAll %d tests passed.\n",n);return 0;
}
