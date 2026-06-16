/* Pattern 87: Mail Header Extractor — parse headers, extract From/To/Subject */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>
%%{ machine p87;
  action hdr_start { hdr = p; hstart = 1; }
  action hdr_key   { key = p; klen = 0; }
  action hdr_colon { klen = p - key; /* after colon skip space */ }
  action hdr_val   { val = p; }
  action hdr_end   {
    vlen = p - val;
    char kb[64]; int nk = klen<63?klen:63;
    memcpy(kb,key,nk); kb[nk]=0;
    if(strcmp(kb,"From")==0||strcmp(kb,"To")==0||strcmp(kb,"Subject")==0)
      printf("  %s: %.*s\n",kb,(int)vlen,val);
  }
  chars = (any-':' - '\n' - ' ');
  key_w = chars+ >hdr_key;
  val_w = (any-'\n')* >hdr_val %hdr_end;
  header = key_w ':' @hdr_colon ' '* val_w '\n';
  main := header (header '\n'?)**;
  write data;
}%%
int main(void){int cs; const char *p,*pe,*eof;
  const char *hdr,*key,*val; ptrdiff_t klen,vlen; int hstart;
  const char *data =
    "From: alice@example.com\n"
    "To: bob@test.org\n"
    "Subject: Hello World\n"
    "Date: Mon, 1 Jan 2025\n"
    "\n";
  printf("Pattern 87: Mail Header Extractor\n\n");
  printf("Headers:\n%s",data);
  p=data; pe=p+strlen(data); eof=pe;
  %% write init;
  %% write exec;
  int r=cs>=p87_first_final;
  printf("  -> %s\n",r?"ACCEPT":"FAIL"); assert(r);
  printf("\nAll tests passed.\n");return 0;
}
