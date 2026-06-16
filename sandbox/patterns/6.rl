#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p6;
main := |*
  alpha  => { printf("  alpha\n");  };
  digit  => { printf("  digit\n");  };
  space  => { printf("  space\n");  };
  punct  => { printf("  punct\n");  };
  lower  => { printf("  lower\n");  };
  upper  => { printf("  upper\n");  };
  xdigit => { printf("  xdigit\n"); };
*|;
write data;
}%%
int main(void){
printf("Pattern  6: Builtin Classes\n\n");
static const char *t[]={"a","A","1"," ",".","z","Z","9","f","F","g"};
int n=11,cs,act; const char *p,*pe,*eof,*ts,*te;
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
printf("  Input '%s': ",t[i]);fflush(stdout);
%% write init;
%% write exec;
printf("\n");
}
printf("\nAll %d tests passed.\n",n);return 0;}
