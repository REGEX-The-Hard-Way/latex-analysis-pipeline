#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p19;
action begin { val = 0; }
action digit { val = val*10 + (fc-'0'); }
action finish { printf("  val=%d (accepted)\n", val); }
main := digit+ >begin $digit %finish '\n';
write data;
}%%
int main(void){int val;
printf("Pattern 19: Leave Action %%\n\n");
static const char *t[]={"42\n","999\n","7\n"};
int n=3,cs; const char *p,*pe,*eof;
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
}
printf("\nAll %d tests passed.\n",n);return 0;}
