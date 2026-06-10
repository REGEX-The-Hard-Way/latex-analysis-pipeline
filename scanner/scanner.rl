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


#define EMIT_BLOCK(t,prefix_len,suffix_len) \
  uint32_t token_id = murmur3_seeded_v2(seed0, &in[ts - in], te - ts); \
  fprintf(stderr, "{<filepath:%s>,filepath_id:%lu,token_id:%lu,parent_id:%lu,offset:%d," \
         "length:%d,type:%s,<tok:%.*s>}\n", \
         filename, (unsigned long)filepath_id, (unsigned long)token_id, \
         (unsigned long)parent_id, prefix_len + (int)(ts - in), (int)(te - ts), \
         t, (int)(te - ts), &in[ts - in]); \
  printf("\n\n  %u  \n\n", token_id); \
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
  parens             => { EMIT_BLOCK("parens",1,1); };
  braces             => { EMIT("braces"); };
  author             => { EMIT("author"); };
  affiliation 		=> { EMIT("affiliation"); };
  usepackage             => { EMIT("usepackage"); };
  title => { EMIT("title"); };
  inline_math        => { EMIT("math"); };
  comment            => { EMIT("comment");};

  abstract => { EMIT_BLOCK("abstract",1,0); };
  ack => { EMIT_BLOCK("ack",1,0); };
  acknowledgement => { EMIT_BLOCK("acknowledgement",1,0); };
  acknowledgements => { EMIT_BLOCK("acknowledgements",1,0); };
  acknowledgments => { EMIT_BLOCK("acknowledgments",1,0); };
  acks => { EMIT_BLOCK("acks",1,0); };
  adjustbox => { EMIT_BLOCK("adjustbox",1,0); };
  algorithm => { EMIT_BLOCK("algorithm",1,0); };
  algorithmic => { EMIT_BLOCK("algorithmic",1,0); };
  alignat => { EMIT_BLOCK("alignat",1,0); };
  alignedat => { EMIT_BLOCK("alignedat",1,0); };
  aligned => { EMIT_BLOCK("aligned",1,0); };
  align => { EMIT_BLOCK("align",1,0); };
  alltt => { EMIT_BLOCK("alltt",1,0); };
  appendices => { EMIT_BLOCK("appendices",1,0); };
  appendix => { EMIT_BLOCK("appendix",1,0); };
  array => { EMIT_BLOCK("array",1,0); };
  article => { EMIT_BLOCK("article",1,0); };
  assumption => { EMIT_BLOCK("assumption",1,0); };
  aug => { EMIT_BLOCK("aug",1,0); };
  axis => { EMIT_BLOCK("axis",1,0); };
  barticle => { EMIT_BLOCK("barticle",1,0); };
  bbook => { EMIT_BLOCK("bbook",1,0); };
  block => { EMIT_BLOCK("block",1,0); };
  bmatrix => { EMIT_BLOCK("bmatrix",1,0); };
  bmisc => { EMIT_BLOCK("bmisc",1,0); };
  case => { EMIT_BLOCK("case",1,0); };
  cases => { EMIT_BLOCK("cases",1,0); };
  center => { EMIT_BLOCK("center",1,0); };
  centering => { EMIT_BLOCK("centering",1,0); };
  claim => { EMIT_BLOCK("claim",1,0); };
  code => { EMIT_BLOCK("code",1,0); };
  compactenum => { EMIT_BLOCK("compactenum",1,0); };
  compactitem => { EMIT_BLOCK("compactitem",1,0); };
  conjecture => { EMIT_BLOCK("conjecture",1,0); };
  conj => { EMIT_BLOCK("conj",1,0); };
  cor => { EMIT_BLOCK("cor",1,0); };
  coro => { EMIT_BLOCK("coro",1,0); };
  corollary => { EMIT_BLOCK("corollary",1,0); };
  dcases => { EMIT_BLOCK("dcases",1,0); };
  defi => { EMIT_BLOCK("defi",1,0); };
  defin => { EMIT_BLOCK("defin",1,0); };
  definition => { EMIT_BLOCK("definition",1,0); };
  defn => { EMIT_BLOCK("defn",1,0); };
  deluxetable => { EMIT_BLOCK("deluxetable",1,0); };
  demo => { EMIT_BLOCK("demo",1,0); };
  description => { EMIT_BLOCK("description",1,0); };
  df => { EMIT_BLOCK("df",1,0); };
  dfn => { EMIT_BLOCK("dfn",1,0); };
  diagram => { EMIT_BLOCK("diagram",1,0); };
  displaymath => { EMIT_BLOCK("displaymath",1,0); };
  document => { EMIT_BLOCK("document",1,0); };
  eg => { EMIT_BLOCK("eg",1,0); };
  enumerate => { EMIT_BLOCK("enumerate",1,0); };
  eqnarray => { EMIT_BLOCK("eqnarray",1,0); };
  equation => { EMIT_BLOCK("equation",1,0); };
  equ => { EMIT_BLOCK("equ",1,0); };
  equs => { EMIT_BLOCK("equs",1,0); };
  exa => { EMIT_BLOCK("exa",1,0); };
  exam => { EMIT_BLOCK("exam",1,0); };
  example => { EMIT_BLOCK("example",1,0); };
  ex => { EMIT_BLOCK("ex",1,0); };
  exercise => { EMIT_BLOCK("exercise",1,0); };
  exmp => { EMIT_BLOCK("exmp",1,0); };
  fact => { EMIT_BLOCK("fact",1,0); };
  figure => { EMIT_BLOCK("figure",1,0); };
  filecontents => { EMIT_BLOCK("filecontents",1,0); };
  flalign => { EMIT_BLOCK("flalign",1,0); };
  flushleft => { EMIT_BLOCK("flushleft",1,0); };
  flushright => { EMIT_BLOCK("flushright",1,0); };
  fmffile => { EMIT_BLOCK("fmffile",1,0); };
  fmfgraph => { EMIT_BLOCK("fmfgraph",1,0); };
  footnotesize => { EMIT_BLOCK("footnotesize",1,0); };
  framed => { EMIT_BLOCK("framed",1,0); };
  frame => { EMIT_BLOCK("frame",1,0); };
  frontmatter => { EMIT_BLOCK("frontmatter",1,0); };
  gathered => { EMIT_BLOCK("gathered",1,0); };
  gather => { EMIT_BLOCK("gather",1,0); };
  indented => { EMIT_BLOCK("indented",1,0); };
  inparaenum => { EMIT_BLOCK("inparaenum",1,0); };
  itemize => { EMIT_BLOCK("itemize",1,0); };
  keyword => { EMIT_BLOCK("keyword",1,0); };
  keywords => { EMIT_BLOCK("keywords",1,0); };
  landscape => { EMIT_BLOCK("landscape",1,0); };
  large => { EMIT_BLOCK("large",1,0); };
  lema => { EMIT_BLOCK("lema",1,0); };
  lem => { EMIT_BLOCK("lem",1,0); };
  lemma => { EMIT_BLOCK("lemma",1,0); };
  lemme => { EMIT_BLOCK("lemme",1,0); };
  lemm => { EMIT_BLOCK("lemm",1,0); };
  linenomath => { EMIT_BLOCK("linenomath",1,0); };
  linenumbers => { EMIT_BLOCK("linenumbers",1,0); };
  list => { EMIT_BLOCK("list",1,0); };
  lm => { EMIT_BLOCK("lm",1,0); };
  longlist => { EMIT_BLOCK("longlist",1,0); };
  longtable => { EMIT_BLOCK("longtable",1,0); };
  lrbox => { EMIT_BLOCK("lrbox",1,0); };
  lstlisting => { EMIT_BLOCK("lstlisting",1,0); };
  mapleinput => { EMIT_BLOCK("mapleinput",1,0); };
  maplelatex => { EMIT_BLOCK("maplelatex",1,0); };
  math => { EMIT_BLOCK("math",1,0); };
  mathletters => { EMIT_BLOCK("mathletters",1,0); };
  mathpar => { EMIT_BLOCK("mathpar",1,0); };
  matrix => { EMIT_BLOCK("matrix",1,0); };
  minipage => { EMIT_BLOCK("minipage",1,0); };
  multicols => { EMIT_BLOCK("multicols",1,0); };
  multlined => { EMIT_BLOCK("multlined",1,0); };
  multline => { EMIT_BLOCK("multline",1,0); };
  mydef => { EMIT_BLOCK("mydef",1,0); };
  notation => { EMIT_BLOCK("notation",1,0); };
  note => { EMIT_BLOCK("note",1,0); };
  obs => { EMIT_BLOCK("obs",1,0); };
  observation => { EMIT_BLOCK("observation",1,0); };
  opening => { EMIT_BLOCK("opening",1,0); };
  overpic => { EMIT_BLOCK("overpic",1,0); };
  pf => { EMIT_BLOCK("pf",1,0); };
  pgfonlayer => { EMIT_BLOCK("pgfonlayer",1,0); };
  pgfscope => { EMIT_BLOCK("pgfscope",1,0); };
  picture => { EMIT_BLOCK("picture",1,0); };
  pmatrix => { EMIT_BLOCK("pmatrix",1,0); };
  pr => { EMIT_BLOCK("pr",1,0); };
  prf => { EMIT_BLOCK("prf",1,0); };
  prob => { EMIT_BLOCK("prob",1,0); };
  problem => { EMIT_BLOCK("problem",1,0); };
  pro => { EMIT_BLOCK("pro",1,0); };
  proof => { EMIT_BLOCK("proof",1,0); };
  proofof => { EMIT_BLOCK("proofof",1,0); };
  prooftree => { EMIT_BLOCK("prooftree",1,0); };
  prop => { EMIT_BLOCK("prop",1,0); };
  property => { EMIT_BLOCK("property",1,0); };
  propo => { EMIT_BLOCK("propo",1,0); };
  proposition => { EMIT_BLOCK("proposition",1,0); };
  prp => { EMIT_BLOCK("prp",1,0); };
  psfrags => { EMIT_BLOCK("psfrags",1,0); };
  pspicture => { EMIT_BLOCK("pspicture",1,0); };
  question => { EMIT_BLOCK("question",1,0); };
  quotation => { EMIT_BLOCK("quotation",1,0); };
  quote => { EMIT_BLOCK("quote",1,0); };
  references => { EMIT_BLOCK("references",1,0); };
  rema => { EMIT_BLOCK("rema",1,0); };
  remark => { EMIT_BLOCK("remark",1,0); };
  rem => { EMIT_BLOCK("rem",1,0); };
  restatable => { EMIT_BLOCK("restatable",1,0); };
  rmk => { EMIT_BLOCK("rmk",1,0); };
  rotate => { EMIT_BLOCK("rotate",1,0); };
  ruledtabular => { EMIT_BLOCK("ruledtabular",1,0); };
  sc => { EMIT_BLOCK("sc",1,0); };
  scope => { EMIT_BLOCK("scope",1,0); };
  scriptsize => { EMIT_BLOCK("scriptsize",1,0); };
  section => { EMIT_BLOCK("section",1,0); };
  sideways => { EMIT_BLOCK("sideways",1,0); };
  sidewaystable => { EMIT_BLOCK("sidewaystable",1,0); };
  sloppypar => { EMIT_BLOCK("sloppypar",1,0); };
  small => { EMIT_BLOCK("small",1,0); };
  smallmatrix => { EMIT_BLOCK("smallmatrix",1,0); };
  spacing => { EMIT_BLOCK("spacing",1,0); };
  split => { EMIT_BLOCK("split",1,0); };
  subarray => { EMIT_BLOCK("subarray",1,0); };
  subeqnarray => { EMIT_BLOCK("subeqnarray",1,0); };
  subequations => { EMIT_BLOCK("subequations",1,0); };
  subfigure => { EMIT_BLOCK("subfigure",1,0); };
  subsection => { EMIT_BLOCK("subsection",1,0); };
  subtable => { EMIT_BLOCK("subtable",1,0); };
  tabbing => { EMIT_BLOCK("tabbing",1,0); };
  table => { EMIT_BLOCK("table",1,0); };
  tablenotes => { EMIT_BLOCK("tablenotes",1,0); };
  tabular => { EMIT_BLOCK("tabular",1,0); };
  tabularx => { EMIT_BLOCK("tabularx",1,0); };
  tcolorbox => { EMIT_BLOCK("tcolorbox",1,0); };
  teo => { EMIT_BLOCK("teo",1,0); };
  theacknowledgments => { EMIT_BLOCK("theacknowledgments",1,0); };
  thebibliography => { EMIT_BLOCK("thebibliography",1,0); };
  theo => { EMIT_BLOCK("theo",1,0); };
  theorem => { EMIT_BLOCK("theorem",1,0); };
  thm => { EMIT_BLOCK("thm",1,0); };
  threeparttable => { EMIT_BLOCK("threeparttable",1,0); };
  tikzcd => { EMIT_BLOCK("tikzcd",1,0); };
  tikzpicture => { EMIT_BLOCK("tikzpicture",1,0); };
  tiny => { EMIT_BLOCK("tiny",1,0); };
  titlepage => { EMIT_BLOCK("titlepage",1,0); };
  trivlist => { EMIT_BLOCK("trivlist",1,0); };
  turn => { EMIT_BLOCK("turn",1,0); };
  turnpage => { EMIT_BLOCK("turnpage",1,0); };
  verbatim => { EMIT_BLOCK("verbatim",1,0); };
  vmatrix => { EMIT_BLOCK("vmatrix",1,0); };
  widetext => { EMIT_BLOCK("widetext",1,0); };
  wrapfigure => { EMIT_BLOCK("wrapfigure",1,0); };


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

