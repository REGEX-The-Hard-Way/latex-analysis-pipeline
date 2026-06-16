/* Pattern 27: Local Error — two sub-machines with different $lerr handlers */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p27;
  action a_err { printf("AlphaErr "); }
  action d_err { printf("DigitErr "); }
  alpha_m = alpha+;
  digit_m = digit+;
  main := (alpha_m $lerr a_err) | (digit_m $lerr d_err);
  write data; }%%
int main(void){int cs;const char *p,*pe,*eof;
printf("Pattern 27: Local Error ($lerr)\n\n");
p="a1";pe=p+2;eof=pe;
%% write init;
%% write exec;
printf("\n");
return 0;}
