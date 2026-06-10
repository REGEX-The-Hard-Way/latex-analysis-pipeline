#include "globals.h"

int n;
uint32_t seed0 = 0;
#define EMIT(t) \
  uint32_t token_id = murmur3_seeded_v2(seed0, &in[ts - in], te - ts); \
  fprintf(stderr, "{<filepath:%s>,filepath_id:%lu,token_id:%lu,parent_id:%lu,offset:%d," \
         "length:%d,type:%s,<tok:%.*s>}\n", \
         filename, (unsigned long)filepath_id, (unsigned long)token_id, \
         (unsigned long)parent_id, prefix_len + (int)(ts - in), (int)(te - ts), \
         t, (int)(te - ts), &in[ts - in]); \
  printf("  %u  ", token_id);


#define EMIT_BLOCK(t) \
  uint32_t token_id = murmur3_seeded_v2(seed0, &in[ts - in], te - ts); \
  fprintf(stderr, "{<filepath:%s>,filepath_id:%lu,token_id:%lu,parent_id:%lu,offset:%d," \
         "length:%d,type:%s,<tok:%.*s>}\n", \
         filename, (unsigned long)filepath_id, (unsigned long)token_id, \
         (unsigned long)parent_id, prefix_len + (int)(ts - in), (int)(te - ts), \
         t, (int)(te - ts), &in[ts - in]); \
  printf("\n\n  %u  \n\n", token_id);


%%{
  machine strings;
  include latex "latex.rl";

  sc_prose = alpha+ ([\-'] alpha+)*;

main :=|*

  begin_doc => { EMIT("begin_doc"); };
  end_doc => { EMIT("end_doc"); };
  frac               => { EMIT("frac"); };
  cite               => { EMIT("cite"); };
  ref               => { EMIT("ref"); };
  parens             => { EMIT("parens"); };
  braces             => { EMIT("braces"); };
  author             => { EMIT("author"); };
  affiliation 		=> { EMIT("affiliation"); };
  usepackage             => { EMIT("usepackage"); };
  title => { EMIT("title"); };
  
  abstract           => { EMIT_BLOCK("abstract"); };
  section            => { EMIT_BLOCK("section"); };
  equation           => { EMIT_BLOCK("equation"); };
  align              => { EMIT_BLOCK("align"); };
  eqnarray           => { EMIT_BLOCK("eqnarray"); };
  figure             => { EMIT_BLOCK("figure"); };
  table              => { EMIT_BLOCK("table"); };
  thebibliography    => { EMIT_BLOCK("bibliography"); };
  bibitem            => { EMIT_BLOCK("bibitem"); };
  verbatim           => { EMIT_BLOCK("verbatim"); };
  inline_math        => { EMIT("math"); };
  comment            => { EMIT("comment");};

  sc_prose           => { printf("%.*s", (int)(te-ts), ts); };

  [ \t]+             => { printf(" "); };
  '\n'               => { printf("\n"); };
  '\r' '\n'?         => { printf("\n"); };

  any                => { printf("%c", *ts); };
*| ;
}%%

    %% write data;
int scanner(const char *in, int length, char filename[], uint32_t filepath_id,
            uint32_t parent_id,int prefix_len,int suffix_len) {
  int in_size = length;
  int cs = 0, act = 0;
  const char *p = in;
  const char *pe = in + length;
  const char *ts = NULL, *te = NULL;
  const char *eof = pe;

  %% write init;
  %% write exec;

  if (cs == strings_error)
    printf("Error near %zd\n", p - in);
}

