/* Pattern 85: Email Address Parser — user@host and <user@host> formats */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>
%%{ machine p85;
  action usr_start { usr = p; }
  action usr_end   { ulen = p - usr; }
  action dom_start { dom = p; }
  action dom_end   { dlen = p - dom; }
  atom    = [a-zA-Z0-9._%+\-]+;
  bare    = atom >usr_start %usr_end '@' atom >dom_start %dom_end;
  angled  = '<' bare '>';
  main   := (bare | angled) '\n';
  write data;
}%%
int main(void){int cs; const char *p,*pe,*eof; const char *usr,*dom; ptrdiff_t ulen,dlen;
  struct {const char *s; int ok;} t[]={
    {"user@example.com\n",1},
    {"<John.Doe@mail.org>\n",1},
    {"a@b.c\n",1},
    {"no_at_sign\n",0},
    {"<bad@host\n",0},
    {"@missing_usr.com\n",0},
  }; int n=6;
  printf("Pattern 85: Email Address Parser\n\n");
  for(int i=0;i<n;i++){p=t[i].s;pe=p+strlen(p);eof=pe;
    printf("Input: %s",t[i].s);
    %% write init;
    %% write exec;
    int r=cs>=p85_first_final;
    printf("  -> %s (expect %s) %s\n",r?"ACCEPT":"FAIL",t[i].ok?"ACCEPT":"FAIL",r==t[i].ok?"ok":"FAIL");
    assert(r==t[i].ok);
  }
  printf("\nAll %d tests passed.\n",n);return 0;
}
