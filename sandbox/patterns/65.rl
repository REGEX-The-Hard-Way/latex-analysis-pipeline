/* Pattern 65: Export Constants — demo write exports and constant usage */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p65;
export c1 = 'c';
export c2 = 'z';

action c1_action { printf("got c1\n"); }
action c2_action { printf("got c2\n"); }
action other     { printf("got other\n"); }

main := (
    c1 @c1_action |
    c2 @c2_action |
    any @other
)*;

write data;
}%%

%% write exports;

int main(void){int cs;const char *p,*pe,*eof;int expect[]={1,1};int n=2;
char d1[]={'c','x','z','y'};char d2[]={'a','b'};
char *data[]={d1,d2};int lens[]={4,2};
printf("Pattern 65: Export Constants\n\n");
for(int i=0;i<n;i++){p=data[i];pe=p+lens[i];eof=pe;
%% write init;
%% write exec;
printf("  %s -> %s ok\n",i==0?"cxyz":"ab",cs>=p65_first_final?"ACCEPT":"FAIL");assert((cs>=p65_first_final)==expect[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
