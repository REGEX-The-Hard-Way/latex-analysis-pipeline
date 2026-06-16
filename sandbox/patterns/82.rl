/* Pattern 82: Awk Emulation — line-based word splitting with position tracking */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>
%%{ machine p82;
  action mark_word  { wstart = p - data; }
  action end_word   { wend = p - data; printf("  word [%ld..%ld): %.*s\n",(long)wstart,(long)wend,(int)(wend-wstart),data+wstart); }
  ws = [ \t\v\f];
  word = (any-'\n' - ws)+ >mark_word %end_word;
  main := (ws* (word (ws+ word)*)? ws* '\n')+;
  write data;
}%%
int main(void){int cs; const char *p,*pe,*eof; ptrdiff_t wstart,wend;
  const char *data = "hello world\nfoo  bar   baz\n\n last \n";
  printf("Pattern 82: Awk Emulation — line-based word splitting\n\n");
  printf("Input:\n%s\n",data);
  p=data; pe=p+strlen(data); eof=pe;
  %% write init;
  %% write exec;
  int r=cs>=p82_first_final;
  printf("  -> %s\n\n",r?"ACCEPT":"FAIL"); assert(r);
  printf("All tests passed.\n");return 0;
}
