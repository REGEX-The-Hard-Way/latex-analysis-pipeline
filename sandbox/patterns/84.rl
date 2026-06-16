/* Pattern 84: Statechart Protocol — named states, explicit transitions */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p84;
  action state_idle    { printf("  -> idle\n"); }
  action state_cmd_char { printf("  -> cmd_char\n"); }
  action state_cmd_end  { printf("  -> cmd_end\n"); }
  action state_args    { printf("  -> args\n"); }
  action got_arg       { printf("    arg: %c\n", *p); }
  cmd = [a-zA-Z]+ '@'? >state_idle @state_cmd_end;
  arg = [a-zA-Z0-9] >state_args $got_arg;
  main := cmd ' '* (arg ' '*)* '\n';
  write data;
}%%
int main(void){int cs; const char *p,*pe,*eof;
  struct {const char *s; int ok;} t[]={
    {"SET a1 b2\n",1},
    {"GET@ x y z\n",1},
    {"BAD! args\n",0},
    {"QUIT\n",1},
    {"READ\n",1},
  }; int n=5;
  printf("Pattern 84: Statechart Protocol\n\n");
  for(int i=0;i<n;i++){p=t[i].s;pe=p+strlen(p);eof=pe;
    printf("Input: %s",t[i].s);
    %% write init;
    %% write exec;
    int r=cs>=p84_first_final;
    printf("  -> %s (expect %s) %s\n",r?"ACCEPT":"FAIL",t[i].ok?"ACCEPT":"FAIL",r==t[i].ok?"ok":"FAIL");
    assert(r==t[i].ok);
  }
  printf("\nAll %d tests passed.\n",n);return 0;
}
