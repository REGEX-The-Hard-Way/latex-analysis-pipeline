/* Pattern 22: Statechart Named States — atoi with start:/om_num:/more_nums: */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p22;
  atoi_mach =
      start: ( digit @{printf("%c",fc);} -> more_nums ),
      more_nums: ( digit @{printf("%c",fc);} -> more_nums | zlen -> final );
  main := atoi_mach '\n';
  write data; }%%
int main(void){int cs;const char *p,*pe,*eof;
printf("Pattern 22: Statechart Named States\n\n");
p="123\n";pe=p+4;eof=pe;
%% write init;
%% write exec;
printf(" -> %s\n",cs>=p22_first_final?"ACCEPT":"FAIL");
return 0;}
