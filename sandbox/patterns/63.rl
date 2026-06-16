/* Pattern 63: variable — rename p, pe, eof to custom names */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p63;
variable p data_p;
variable pe data_pe;
variable eof data_eof;

main := 'hello' '\n';

write data;
}%%
int main(void){int cs;const char *data_p,*data_pe,*data_eof;
const char *test[]={"hello\n","world\n"};int expect[]={1,0};int lens[]={6,6};int n=2;
printf("Pattern 63: variable p/pe/eof Renaming\n\n");
for(int i=0;i<n;i++){data_p=test[i];data_pe=data_p+lens[i];data_eof=data_pe;
%% write init;
%% write exec;
printf("  %-10s -> %s ok\n",test[i],cs>=p63_first_final?"ACCEPT":"FAIL");assert((cs>=p63_first_final)==expect[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
