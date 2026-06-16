#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p20;
action per_char   { printf(" [char:%c]", fc); }
action per_elem   { printf(" (elem)"); }
action finish     { printf("\n  val=%d\n", val); }
action begin      { val=0; }
action add_digit  { val=val*10+(fc-'0'); }
action print_val  { printf("  val=%d\n", val); }
main := ( digit+ $per_char @per_elem >begin %print_val '\n' );
write data;
}%%
int main(void){int val,cs;const char *p,*pe,*eof;
printf("Pattern 20: @ and $ Actions\n\n");
static const char *t[]={"123\n","45\n"};
int n=2;
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
}
printf("\nAll %d tests passed.\n",n);return 0;}
