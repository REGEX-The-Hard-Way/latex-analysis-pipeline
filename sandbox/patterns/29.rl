/* Pattern 29: Fine-Grained Error — >! and %! on float parser */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p29;
  action exp_sgn { printf("EXP_SIGN "); }
  action exp_dig { printf("EXP_DIG "); }
  float = ( [+\-]? >!exp_sgn %!exp_dig ) ( digit+ ) ( '.' digit+ )?;
  main := float '\n';
  write data; }%%
int main(void){int cs;const char *p,*pe,*eof;
printf("Pattern 29: Fine-Grained Error (>! %%!)\n\n");
p="x\n";pe=p+2;eof=pe;
%% write init;
%% write exec;
printf(" -> %s\n",cs>=p29_first_final?"ok":"error");
return 0;}
