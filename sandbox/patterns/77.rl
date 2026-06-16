/* Pattern 77: URL Parser — scheme://authority/path?query#fragment */
#include <stdio.h>
#include <string.h>
#include <assert.h>

char scheme[64], host[128], path[256], query[256], frag[256];
char *s=scheme,*h=host,*pa=path,*q=query,*fr=frag;

%%{ machine p77;

action schm_start { s=scheme; *s=0; }
action schm_char  { *s++=fc; }
action schm_done  { *s=0; printf("scheme=%s\n",scheme); }

action host_start { h=host; *h=0; }
action host_char  { *h++=fc; }
action host_done  { *h=0; printf("host=%s\n",host); }

action path_start { pa=path; *pa=0; }
action path_char  { *pa++=fc; }
action path_done  { *pa=0; printf("path=%s\n",path); }

action query_start{ q=query; *q=0; }
action query_char { *q++=fc; }
action query_done { *q=0; printf("query=%s\n",query); }

action frag_start { fr=frag; *fr=0; }
action frag_char  { *fr++=fc; }
action frag_done  { *fr=0; printf("frag=%s\n",frag); }

action literal { printf("LIT: %c\n", fc); }

schm    = (alpha (alpha|digit|[+\-.])*) >schm_start $schm_char %schm_done;
host_p  = (any - [/?#: \t\n])+ >host_start $host_char %host_done;
port_p  = ':' digit+;
auth    = '://' host_p port_p?;
path_p  = '/' (any - [?#\n])* >path_start $path_char %path_done;
query_p = '?' (any - [#\n])* >query_start $query_char %query_done;
frag_p  = '#' (any - [\n])* >frag_start $frag_char %frag_done;

url = schm auth? path_p? query_p? frag_p? '\n';

main := url;

write data;
}%%
int main(void){int cs;const char *p,*pe,*eof;
printf("Pattern 77: URL Parser\n\n");
const char *test_url = "https://example.com/path/to/file?key=val#section\n";
p=test_url;pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
printf("-> %s ok\n",cs>=p77_first_final?"ACCEPT":"FAIL");assert(cs>=p77_first_final);
printf("\nAll tests passed.\n");return 0;}
