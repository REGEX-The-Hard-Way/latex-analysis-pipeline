/* Pattern 64: write Statement Family — data, init, exec, start, first_final, error, exports */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p64;
export f64_final = 'X';
main := ( 'hello' '\n' );
write data;
}%%

%% write exports;
/* No separate data — already inside machine block */

int main(void){int cs;const char *p,*pe,*eof;p="hello\n";pe=p+strlen(p);eof=pe;
printf("Pattern 64: write Statement Family\n\n");
printf("  Start state = %d\n", p64_start);
printf("  First final = %d\n", p64_first_final);
printf("  Error state = %d\n", p64_error);
printf("  Exported X  = %d\n", p64_ex_f64_final);
%% write init;
%% write exec;
printf("  Result      -> %s\n",cs>=p64_first_final?"ACCEPT":"FAIL");
assert(cs>=p64_first_final);
printf("\nAll tests passed.\n");return 0;}
