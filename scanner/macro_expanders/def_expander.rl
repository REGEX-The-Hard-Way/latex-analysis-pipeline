#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../cypher/vendor/klib/khashl.h"

#define PARAM_BASE 0x01
static int n;

typedef struct {
  char *body;
  int body_len;
  int argc;
} macro_val_t;

static kh_inline khint_t str_hash(const char *s) {
  khint_t h = 0;
  while (*s) h = (h << 5) - h + (unsigned char)*s++;
  return h;
}
static kh_inline int str_eq(const char *a, const char *b) {
  return strcmp(a, b) == 0;
}

KHASHL_MAP_INIT(static, macro_map_t, macro_map, const char*, macro_val_t, str_hash, str_eq);

static macro_map_t *macros = NULL;
static char *input = NULL;
static long input_len = 0;

static int macro_exists(macro_map_t *h, const char *key) {
  if (!h || !h->keys) return 0;
  khint_t k = macro_map_get(h, key);
  khint_t nb = (khint_t)1U << h->bits;
  return k < nb;
}

static void store_macro(const char *name, int name_len, const char *body_start, const char *body_end, int argc) {
  if (!macros) macros = macro_map_init();

  char *name_dup = (char*)malloc(name_len + 1);
  memcpy(name_dup, name, name_len);
  name_dup[name_len] = '\0';

  int body_raw_len = (int)(body_end - body_start);
  char *body_buf = (char*)malloc(body_raw_len + 1);
  int j = 0;
  for (int i = 0; i < body_raw_len; i++) {
    char c = body_start[i];
    if (c == '#' && i + 1 < body_raw_len) {
      if (body_start[i+1] == '#') {
        body_buf[j++] = '#';
        i++;
      } else if (body_start[i+1] >= '1' && body_start[i+1] <= '9') {
        body_buf[j++] = PARAM_BASE + (body_start[i+1] - '1');
        i++;
      } else {
        body_buf[j++] = c;
      }
    } else {
      body_buf[j++] = c;
    }
  }
  body_buf[j] = '\0';

  macro_val_t val = { body_buf, j, argc };
  int absent;
  khint_t k = macro_map_put(macros, name_dup, &absent);
  macros->keys[k].val = val;
}

static int body_has_at(const char *s, int len) {
  for (int i = 0; i < len - 1; i++) {
    if (s[i] == '\\' && s[i+1] == '@') return 1;
  }
  return 0;
}


%%{
  machine def_expander;
include latex "../latex.rl";

tex_name = alpha (alpha | '@')*;
sp = [ \t];

def_prefix = ('\\long' sp*)? ('\\outer' sp*)? ('\\protected' sp*)? ('\\global' sp*)?;
def_cmd = def_prefix ('\\def'|'\\gdef') sp* '\\' tex_name sp* ('#' digit)* sp* braces;

ncmd    = '\\newcommand';
rencmd  = '\\renewcommand';
provcmd = '\\providecommand';
edef    = '\\edef';
xdef    = '\\xdef';
letcmd  = '\\let';

main :=|*

  def_cmd => {
    fwrite(ts, 1, te - ts, stdout);
    const char *p = ts;
    const char *end = te;

    while (p < end) {
      if (*p == '\\' && end - p >= 4) {
        if ((p[1]=='d'||p[1]=='D') && (p[2]=='e'||p[2]=='E') && (p[3]=='f'||p[3]=='F')) { p += 4; break; }
        if ((p[1]=='g'||p[1]=='G') && (p[2]=='d'||p[2]=='D') && (p[3]=='e'||p[3]=='E') && (p[4]=='f'||p[4]=='F')) { p += 5; break; }
      }
      p++;
    }

    while (p < end && (*p == ' ' || *p == '\t')) p++;
    if (p < end && *p == '\\') p++;
    const char *ns = p;
    while (p < end && (isalpha(*p) || *p == '@')) p++;
    const char *ne = p;

    while (p < end && (*p == ' ' || *p == '\t')) p++;
    int argc = 0;
    while (p + 1 < end && *p == '#' && isdigit(p[1])) { argc++; p += 2; }

    while (p < end && (*p == ' ' || *p == '\t')) p++;
    if (p < end && *p == '{') {
      const char *bs = p + 1;
      const char *be = end - 1;
      if (!body_has_at(bs, (int)(be - bs))) store_macro(ns, (int)(ne - ns), bs, be, argc);
    }
  };

  ncmd => { fwrite(ts, 1, te - ts, stdout); };
  rencmd => { fwrite(ts, 1, te - ts, stdout); };
  provcmd => { fwrite(ts, 1, te - ts, stdout); };
  edef => { fwrite(ts, 1, te - ts, stdout); };
  xdef => { fwrite(ts, 1, te - ts, stdout); };
  letcmd => { fwrite(ts, 1, te - ts, stdout); };

  '\\' tex_name => {
    int nl = (int)(te - ts - 1);
    int did_expand = 0;
    if (nl > 0 && nl < 256 && macros && macros->keys) {
      char nb[256];
      memcpy(nb, ts + 1, nl);
      nb[nl] = '\0';
      khint_t k = macro_map_get(macros, nb);
      khint_t nbk = (khint_t)1U << macros->bits;
      if (k < nbk) {
        macro_val_t mv = macros->keys[k].val;
        const char *sc = te;
        char *args[9] = {0};
        int ga = 0;
        for (int i = 0; i < mv.argc; i++) {
          while (*sc == ' ' || *sc == '\t' || *sc == '\n') sc++;
          if (*sc == '{') {
            sc++;
            const char *as = sc;
            int d = 1;
            while (*sc && d > 0) {
              if (*sc == '{') d++;
              else if (*sc == '}') { d--; if (d == 0) break; }
              sc++;
            }
            int al = (int)(sc - as);
            args[i] = (char*)malloc(al + 1);
            memcpy(args[i], as, al);
            args[i][al] = '\0';
            if (*sc) sc++;
          } else {
            args[i] = (char*)malloc(2);
            args[i][0] = *sc;
            args[i][1] = '\0';
            if (*sc) sc++;
          }
          ga = i + 1;
        }
        for (int i = 0; i < mv.body_len; i++) {
          unsigned char c = (unsigned char)mv.body[i];
          if (c >= PARAM_BASE && c < PARAM_BASE + 9) {
            int pi = c - PARAM_BASE;
            if (pi < ga && args[pi]) fputs(args[pi], stdout);
          } else {
            fputc(c, stdout);
          }
        }
        for (int i = 0; i < ga; i++) free(args[i]);
        did_expand = 1;
        fexec sc;
      }
    }
    if (!did_expand) {
      fwrite(ts, 1, te - ts, stdout);
    }
  };

  comment => { fwrite(ts, 1, te - ts, stdout); };

  any => { fputc(*ts, stdout); };
*|;

}%%

%% write data;
int main(int argc, char **argv) {
  int ch;
  long cap = 65536;
  input = (char*)malloc(cap);
  input_len = 0;
  while ((ch = fgetc(stdin)) != EOF) {
    if (input_len >= cap - 1) {
      cap *= 2;
      input = (char*)realloc(input, cap);
    }
    input[input_len++] = (char)ch;
  }
  input[input_len] = '\0';

  int cs = 0;
  const char *p = input;
  const char *pe = input + input_len;
  const char *eof = pe;
  const char *ts = NULL, *te = NULL;

  %% write init;
  %% write exec;

  if (cs == def_expander_error) {
    fprintf(stderr, "Scan error at position %ld\n", p - input);
    return 1;
  }

  if (macros) macro_map_destroy(macros);
  free(input);
  return 0;
}
