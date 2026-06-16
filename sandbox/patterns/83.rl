/* Pattern 83: Incremental Line Processing — backward newline search + partial */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p83;
  action mark_bol { bol = p; }
  action emit    { printf("  complete: %.*s\n", (int)(p - bol - 1), bol); }
  line = (any-'\n')* '\n' >mark_bol @emit;
  main := line* (any-'\n')*;
  write data;
}%%
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
    %% write init;
    }
    %% write exec;
    if(chunks[i].eof && p<pe){
      printf("  partial remainder: %.*s\n",(int)(pe-p),p);
    }
  }
  int r=cs>=p83_first_final;
  printf("\n  -> %s\n",r?"ACCEPT":"FAIL"); assert(r);
  printf("All tests passed.\n");return 0;
}
