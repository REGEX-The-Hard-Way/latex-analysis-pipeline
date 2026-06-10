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
  printf("\n\n  %u  \n\n", token_id); \
  int prefix_len = 1; \
  int suffix_len = 0; \
  scanner(&in[ts + prefix_len - in], te - (prefix_len + suffix_len) - ts,filename, filepath_id, token_id, prefix_len, suffix_len);

%%{
  machine strings;
  include latex "latex.rl";

  sc_prose = alpha+ ([\-'] alpha+)*;

main :=|*

  begin_doc => { EMIT("begin_doc"); };
  end_doc => { EMIT("end_doc"); };
  label               => { EMIT("label"); };
  frac               => { EMIT("frac"); };
  cite               => { EMIT("cite"); };
  ref               => { EMIT("ref"); };
  parens             => { EMIT("parens"); };
  braces             => { EMIT("braces"); };
  author             => { EMIT("author"); };
  affiliation 		=> { EMIT("affiliation"); };
  usepackage             => { EMIT("usepackage"); };
  title => { EMIT("title"); };
  inline_math        => { EMIT("math"); };
  comment            => { EMIT("comment");};

  abstract => { EMIT_BLOCK("abstract"); };
  ack => { EMIT_BLOCK("ack"); };
  acknowledgement => { EMIT_BLOCK("acknowledgement"); };
  acknowledgements => { EMIT_BLOCK("acknowledgements"); };
  acknowledgments => { EMIT_BLOCK("acknowledgments"); };
  acks => { EMIT_BLOCK("acks"); };
  adjustbox => { EMIT_BLOCK("adjustbox"); };
  algorithm => { EMIT_BLOCK("algorithm"); };
  algorithmic => { EMIT_BLOCK("algorithmic"); };
  alignat => { EMIT_BLOCK("alignat"); };
  alignedat => { EMIT_BLOCK("alignedat"); };
  aligned => { EMIT_BLOCK("aligned"); };
  align => { EMIT_BLOCK("align"); };
  alltt => { EMIT_BLOCK("alltt"); };
  appendices => { EMIT_BLOCK("appendices"); };
  appendix => { EMIT_BLOCK("appendix"); };
  array => { EMIT_BLOCK("array"); };
  article => { EMIT_BLOCK("article"); };
  assumption => { EMIT_BLOCK("assumption"); };
  aug => { EMIT_BLOCK("aug"); };
  axis => { EMIT_BLOCK("axis"); };
  barticle => { EMIT_BLOCK("barticle"); };
  bbook => { EMIT_BLOCK("bbook"); };
  block => { EMIT_BLOCK("block"); };
  bmatrix => { EMIT_BLOCK("bmatrix"); };
  bmisc => { EMIT_BLOCK("bmisc"); };
  case => { EMIT_BLOCK("case"); };
  cases => { EMIT_BLOCK("cases"); };
  center => { EMIT_BLOCK("center"); };
  centering => { EMIT_BLOCK("centering"); };
  claim => { EMIT_BLOCK("claim"); };
  code => { EMIT_BLOCK("code"); };
  compactenum => { EMIT_BLOCK("compactenum"); };
  compactitem => { EMIT_BLOCK("compactitem"); };
  conjecture => { EMIT_BLOCK("conjecture"); };
  conj => { EMIT_BLOCK("conj"); };
  cor => { EMIT_BLOCK("cor"); };
  coro => { EMIT_BLOCK("coro"); };
  corollary => { EMIT_BLOCK("corollary"); };
  dcases => { EMIT_BLOCK("dcases"); };
  defi => { EMIT_BLOCK("defi"); };
  defin => { EMIT_BLOCK("defin"); };
  definition => { EMIT_BLOCK("definition"); };
  defn => { EMIT_BLOCK("defn"); };
  deluxetable => { EMIT_BLOCK("deluxetable"); };
  demo => { EMIT_BLOCK("demo"); };
  description => { EMIT_BLOCK("description"); };
  df => { EMIT_BLOCK("df"); };
  dfn => { EMIT_BLOCK("dfn"); };
  diagram => { EMIT_BLOCK("diagram"); };
  displaymath => { EMIT_BLOCK("displaymath"); };
  document => { EMIT_BLOCK("document"); };
  eg => { EMIT_BLOCK("eg"); };
  enumerate => { EMIT_BLOCK("enumerate"); };
  eqnarray => { EMIT_BLOCK("eqnarray"); };
  equation => { EMIT_BLOCK("equation"); };
  equ => { EMIT_BLOCK("equ"); };
  equs => { EMIT_BLOCK("equs"); };
  exa => { EMIT_BLOCK("exa"); };
  exam => { EMIT_BLOCK("exam"); };
  example => { EMIT_BLOCK("example"); };
  ex => { EMIT_BLOCK("ex"); };
  exercise => { EMIT_BLOCK("exercise"); };
  exmp => { EMIT_BLOCK("exmp"); };
  fact => { EMIT_BLOCK("fact"); };
  figure => { EMIT_BLOCK("figure"); };
  filecontents => { EMIT_BLOCK("filecontents"); };
  flalign => { EMIT_BLOCK("flalign"); };
  flushleft => { EMIT_BLOCK("flushleft"); };
  flushright => { EMIT_BLOCK("flushright"); };
  fmffile => { EMIT_BLOCK("fmffile"); };
  fmfgraph => { EMIT_BLOCK("fmfgraph"); };
  footnotesize => { EMIT_BLOCK("footnotesize"); };
  framed => { EMIT_BLOCK("framed"); };
  frame => { EMIT_BLOCK("frame"); };
  frontmatter => { EMIT_BLOCK("frontmatter"); };
  gathered => { EMIT_BLOCK("gathered"); };
  gather => { EMIT_BLOCK("gather"); };
  indented => { EMIT_BLOCK("indented"); };
  inparaenum => { EMIT_BLOCK("inparaenum"); };
  itemize => { EMIT_BLOCK("itemize"); };
  keyword => { EMIT_BLOCK("keyword"); };
  keywords => { EMIT_BLOCK("keywords"); };
  landscape => { EMIT_BLOCK("landscape"); };
  large => { EMIT_BLOCK("large"); };
  lema => { EMIT_BLOCK("lema"); };
  lem => { EMIT_BLOCK("lem"); };
  lemma => { EMIT_BLOCK("lemma"); };
  lemme => { EMIT_BLOCK("lemme"); };
  lemm => { EMIT_BLOCK("lemm"); };
  linenomath => { EMIT_BLOCK("linenomath"); };
  linenumbers => { EMIT_BLOCK("linenumbers"); };
  list => { EMIT_BLOCK("list"); };
  lm => { EMIT_BLOCK("lm"); };
  longlist => { EMIT_BLOCK("longlist"); };
  longtable => { EMIT_BLOCK("longtable"); };
  lrbox => { EMIT_BLOCK("lrbox"); };
  lstlisting => { EMIT_BLOCK("lstlisting"); };
  mapleinput => { EMIT_BLOCK("mapleinput"); };
  maplelatex => { EMIT_BLOCK("maplelatex"); };
  math => { EMIT_BLOCK("math"); };
  mathletters => { EMIT_BLOCK("mathletters"); };
  mathpar => { EMIT_BLOCK("mathpar"); };
  matrix => { EMIT_BLOCK("matrix"); };
  minipage => { EMIT_BLOCK("minipage"); };
  multicols => { EMIT_BLOCK("multicols"); };
  multlined => { EMIT_BLOCK("multlined"); };
  multline => { EMIT_BLOCK("multline"); };
  mydef => { EMIT_BLOCK("mydef"); };
  notation => { EMIT_BLOCK("notation"); };
  note => { EMIT_BLOCK("note"); };
  obs => { EMIT_BLOCK("obs"); };
  observation => { EMIT_BLOCK("observation"); };
  opening => { EMIT_BLOCK("opening"); };
  overpic => { EMIT_BLOCK("overpic"); };
  pf => { EMIT_BLOCK("pf"); };
  pgfonlayer => { EMIT_BLOCK("pgfonlayer"); };
  pgfscope => { EMIT_BLOCK("pgfscope"); };
  picture => { EMIT_BLOCK("picture"); };
  pmatrix => { EMIT_BLOCK("pmatrix"); };
  pr => { EMIT_BLOCK("pr"); };
  prf => { EMIT_BLOCK("prf"); };
  prob => { EMIT_BLOCK("prob"); };
  problem => { EMIT_BLOCK("problem"); };
  pro => { EMIT_BLOCK("pro"); };
  proof => { EMIT_BLOCK("proof"); };
  proofof => { EMIT_BLOCK("proofof"); };
  prooftree => { EMIT_BLOCK("prooftree"); };
  prop => { EMIT_BLOCK("prop"); };
  property => { EMIT_BLOCK("property"); };
  propo => { EMIT_BLOCK("propo"); };
  proposition => { EMIT_BLOCK("proposition"); };
  prp => { EMIT_BLOCK("prp"); };
  psfrags => { EMIT_BLOCK("psfrags"); };
  pspicture => { EMIT_BLOCK("pspicture"); };
  question => { EMIT_BLOCK("question"); };
  quotation => { EMIT_BLOCK("quotation"); };
  quote => { EMIT_BLOCK("quote"); };
  references => { EMIT_BLOCK("references"); };
  rema => { EMIT_BLOCK("rema"); };
  remark => { EMIT_BLOCK("remark"); };
  rem => { EMIT_BLOCK("rem"); };
  restatable => { EMIT_BLOCK("restatable"); };
  rmk => { EMIT_BLOCK("rmk"); };
  rotate => { EMIT_BLOCK("rotate"); };
  ruledtabular => { EMIT_BLOCK("ruledtabular"); };
  sc => { EMIT_BLOCK("sc"); };
  scope => { EMIT_BLOCK("scope"); };
  scriptsize => { EMIT_BLOCK("scriptsize"); };
  section => { EMIT_BLOCK("section"); };
  sideways => { EMIT_BLOCK("sideways"); };
  sidewaystable => { EMIT_BLOCK("sidewaystable"); };
  sloppypar => { EMIT_BLOCK("sloppypar"); };
  small => { EMIT_BLOCK("small"); };
  smallmatrix => { EMIT_BLOCK("smallmatrix"); };
  spacing => { EMIT_BLOCK("spacing"); };
  split => { EMIT_BLOCK("split"); };
  subarray => { EMIT_BLOCK("subarray"); };
  subeqnarray => { EMIT_BLOCK("subeqnarray"); };
  subequations => { EMIT_BLOCK("subequations"); };
  subfigure => { EMIT_BLOCK("subfigure"); };
  subsection => { EMIT_BLOCK("subsection"); };
  subtable => { EMIT_BLOCK("subtable"); };
  tabbing => { EMIT_BLOCK("tabbing"); };
  table => { EMIT_BLOCK("table"); };
  tablenotes => { EMIT_BLOCK("tablenotes"); };
  tabular => { EMIT_BLOCK("tabular"); };
  tabularx => { EMIT_BLOCK("tabularx"); };
  tcolorbox => { EMIT_BLOCK("tcolorbox"); };
  teo => { EMIT_BLOCK("teo"); };
  theacknowledgments => { EMIT_BLOCK("theacknowledgments"); };
  thebibliography => { EMIT_BLOCK("thebibliography"); };
  theo => { EMIT_BLOCK("theo"); };
  theorem => { EMIT_BLOCK("theorem"); };
  thm => { EMIT_BLOCK("thm"); };
  threeparttable => { EMIT_BLOCK("threeparttable"); };
  tikzcd => { EMIT_BLOCK("tikzcd"); };
  tikzpicture => { EMIT_BLOCK("tikzpicture"); };
  tiny => { EMIT_BLOCK("tiny"); };
  titlepage => { EMIT_BLOCK("titlepage"); };
  trivlist => { EMIT_BLOCK("trivlist"); };
  turn => { EMIT_BLOCK("turn"); };
  turnpage => { EMIT_BLOCK("turnpage"); };
  verbatim => { EMIT_BLOCK("verbatim"); };
  vmatrix => { EMIT_BLOCK("vmatrix"); };
  widetext => { EMIT_BLOCK("widetext"); };
  wrapfigure => { EMIT_BLOCK("wrapfigure"); };
  xy => { EMIT_BLOCK("xy"); };


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

