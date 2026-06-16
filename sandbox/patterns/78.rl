/* Pattern 78: Unicode XML — alphtype unsigned short, BaseChar/Ideographic/Letter */
#include <stdio.h>
#include <string.h>
#include <assert.h>

%%{ machine p78;
alphtype unsigned short;

BaseChar = 0x0041..0x005A | 0x0061..0x007A |
           0x00C0..0x00D6 | 0x00D8..0x00F6 | 0x00F8..0x00FF;

Ideographic = 0x4E00..0x4E05 | 0x3007;

Letter = BaseChar | Ideographic;

action letter_found { printf("Letter=U+%04X\n", fc); }
action digit_found  { printf("Digit=U+%04X\n", fc); }
action other_found  { printf("Other=U+%04X\n", fc); }

main := (
    Letter @letter_found |
    digit  @digit_found  |
    any    @other_found
)*;

write data;
}%%
int main(void){int cs;unsigned short *p,*pe,*eof;
printf("Pattern 78: Unicode XML Classes\n\n");
unsigned short input[] = {0x0041, 0x007A, 0x0030, 0x0039, 0x0023, 0x00E9, 0x0030, 0};
p=input;pe=p+7;eof=pe;
%% write init;
%% write exec;
printf("-> %s ok\n",cs>=p78_first_final?"ACCEPT":"FAIL");
assert(cs>=p78_first_final);
printf("\nAll tests passed.\n");return 0;}
