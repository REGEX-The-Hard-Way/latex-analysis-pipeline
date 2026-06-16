/* Pattern 61: alphtype unsigned char — match high bytes like 0xe8 */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p61; alphtype unsigned char; main := 0xe8 '\n'; write data; }%%
int main(void){int cs;unsigned char *p,*pe;int expect[]={1,0,0};int n=3;
unsigned char d1[]={0xe8,10};unsigned char d2[]={0xf8,10};unsigned char d3[]={0x48,10};
unsigned char *data[]={d1,d2,d3};
int lens[]={2,2,2};
printf("Pattern 61: alphtype unsigned char High Bytes\n\n");
for(int i=0;i<n;i++){p=data[i];pe=p+lens[i];
%% write init;
%% write exec;
printf("  %s -> %s ok\n",i==0?"0xe8\\n":i==1?"0xf8\\n":"0x48\\n",cs>=p61_first_final?"ACCEPT":"FAIL");assert((cs>=p61_first_final)==expect[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
