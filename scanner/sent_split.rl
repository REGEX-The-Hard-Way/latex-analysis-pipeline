#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char sbuf[65536]; static int slen;
static void add(char c) { if (slen < 65530) sbuf[slen++] = c; }
static void flush(void) {
    sbuf[slen] = '\0';
    char *s = sbuf;
    while (*s==' '||*s=='\t'||*s=='\n') s++;
    char *e = s+strlen(s);
    while (e>s&&(e[-1]==' '||e[-1]=='\t'||e[-1]=='\n')) e--;
    *e = '\0';
    if (*s) printf("%s\n", s);
    slen = 0;
}

%%{
  machine sent_split;
  hash_id = '  ' digit+ '  ';
  abbrev  = 'e.g' | 'i.e' | 'cf' | 'vs' | 'w.r.t' | 'et al'
          | 'Eq' | 'Ref' | 'Fig' | 'Sec' | 'Vol' | 'No' | 'pp'
          | 'Dr' | 'Mr' | 'Mrs' | 'Ms' | 'Prof' | 'Sen'
          | 'eds' | 'approx' | 'esp' | 'inc' | 'est';
  initial = [A-Z] '.';
  word    = alpha+ ([\-'] alpha+)*;
  number  = digit+;
  sent_nl  = [.!?] [ \t]* '\n';
  sent_cap = [.!?] [ \t]+ [A-Z];
  blank    = '\n' '\n'+;

  main := |*
    hash_id       => { int i; for(i=0;i<te-ts;i++) add(ts[i]); };
    abbrev '.'    => { int i; for(i=0;i<te-ts;i++) add(ts[i]); };
    initial       => { int i; for(i=0;i<te-ts;i++) add(ts[i]); };
    word          => { int i; for(i=0;i<te-ts;i++) add(ts[i]); };
    number        => { int i; for(i=0;i<te-ts;i++) add(ts[i]); };
    sent_nl       => { int i; for(i=0;i<te-ts;i++) add(ts[i]); flush(); };
    sent_cap      => { int i; for(i=0;i<te-ts-1;i++) add(ts[i]); flush(); add(*(te-1)); };
    blank         => { flush(); };
    [ \t]         => { add(' '); };
    '\n'          => { add(' '); };
    any           => { add(ts[0]); };
  *|;
}%%
%% write data;

int sent_split_run(const char *in, int length) {
    slen = 0;
    int cs = 0, act = 0, i;
    const char *p = in, *pe = in + length;
    const char *ts = NULL, *te = NULL;
    const char *eof = pe;
    %% write init;
    %% write exec;
    flush();
    return (cs == sent_split_error) ? 1 : 0;
}

#ifdef TEST
int main(void) {
    size_t cap = 1<<20, len = 0;
    char *data = malloc(cap); int n;
    while ((n = fread(data+len, 1, cap-len-1, stdin)) > 0) {
        len += (size_t)n;
        if (len >= cap-1) { cap *= 2; data = realloc(data, cap); }
    }
    data[len] = '\0';
    int rc = sent_split_run(data, (int)len);
    free(data);
    return rc;
}
#endif
