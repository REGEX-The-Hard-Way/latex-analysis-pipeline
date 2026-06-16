/* Pattern 33: Scanner Modes — on-last/on-next/on-lag pattern matching */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p33;
  main := |*
    'a' => { printf("LAST "); };
    'b'+ => { printf("NEXT "); };
    'c1' 'dxxx'? => { printf("LAG "); };
    [d0-9]+ '.';
    '\n';
  *|;
  write data; }%%
int main(void){int cs,act;const char *p,*pe,*eof,*ts,*te;
printf("Pattern 33: Scanner Modes\n\n");
char buf[]="abbc1d1d2\n";
p=buf;pe=p+strlen(buf);eof=pe;
%% write init;
%% write exec;
printf("\n");
return 0;}
