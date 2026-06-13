#include "globals.h"
#include "json_escape.h"
int g_json_mode = 0;
FILE *g_out_tok = NULL;
FILE *g_out_json = NULL;
int n;
uint32_t seed0 = 0;


#define EMIT(t) \
  uint32_t token_id = murmur3_seeded_v2(seed0, &in[ts - in], te - ts); \
  char emit_buf[(int)(te-ts)+250]; \
  memset(emit_buf,'\0',(int)(te-ts)+250); \
  int emit_len = snprintf(emit_buf, sizeof(emit_buf), \
         "{<filepath:%s>,filepath_id:%lu,token_id:%lu,parent_id:%lu,offset:%d," \
         "length:%d,type:%s,<tok:%.*s>}\n", \
         filename, (unsigned long)filepath_id, (unsigned long)token_id, \
         (unsigned long)parent_id, prefix_len + (int)(ts - in), (int)(te - ts), \
         t, (int)(te - ts), &in[ts - in]); \
  if (g_json_mode) { \
    char *js_esc = (char *)malloc((size_t)(te-ts) * 6 + 100); \
    if (js_esc) { \
      int js_el = json_escape(js_esc, (size_t)(te-ts) * 6 + 100, &in[ts - in], (int)(te-ts)); \
      if (js_el < 0) js_el = 0; \
      char *js_buf = (char *)malloc((size_t)js_el + 512); \
      if (js_buf) { \
        int js_bl = snprintf(js_buf, (size_t)js_el + 512, \
          "{\"filepath\":\"%s\",\"filepath_id\":%lu,\"token_id\":%lu," \
          "\"parent_id\":%lu,\"offset\":%d,\"length\":%d,\"type\":\"%s\"," \
          "\"text\":\"%.*s\"}\n", \
          filename, (unsigned long)filepath_id, (unsigned long)token_id, \
          (unsigned long)parent_id, prefix_len + (int)(ts - in), (int)(te-ts), \
          t, js_el, js_esc); \
        fwrite(js_buf, 1, js_bl, g_out_json); \
        free(js_buf); \
      } \
      free(js_esc); \
    } \
  } else { \
    fwrite(emit_buf, 1, emit_len, g_out_tok); \
  } \
  printf("  %u  ", token_id);

#define EMIT_BLOCK(t,prefix_len,suffix_len) \
  uint32_t token_id = murmur3_seeded_v2(seed0, &in[ts - in], te - ts); \
  char emit_buf[(int)(te-ts)+250]; \
  memset(emit_buf,'\0',(int)(te-ts)+250); \
  int emit_len = snprintf(emit_buf, sizeof(emit_buf), \
         "{<filepath:%s>,filepath_id:%lu,token_id:%lu,parent_id:%lu,offset:%d," \
         "length:%d,type:%s,<tok:%.*s>}\n", \
         filename, (unsigned long)filepath_id, (unsigned long)token_id, \
         (unsigned long)parent_id, prefix_len + (int)(ts - in), (int)(te - ts), \
         t, (int)(te - ts), &in[ts - in]); \
  if (g_json_mode) { \
    char *js_esc = (char *)malloc((size_t)(te-ts) * 6 + 100); \
    if (js_esc) { \
      int js_el = json_escape(js_esc, (size_t)(te-ts) * 6 + 100, &in[ts - in], (int)(te-ts)); \
      if (js_el < 0) js_el = 0; \
      char *js_buf = (char *)malloc((size_t)js_el + 512); \
      if (js_buf) { \
        int js_bl = snprintf(js_buf, (size_t)js_el + 512, \
          "{\"filepath\":\"%s\",\"filepath_id\":%lu,\"token_id\":%lu," \
          "\"parent_id\":%lu,\"offset\":%d,\"length\":%d,\"type\":\"%s\"," \
          "\"text\":\"%.*s\"}\n", \
          filename, (unsigned long)filepath_id, (unsigned long)token_id, \
          (unsigned long)parent_id, prefix_len + (int)(ts - in), (int)(te-ts), \
          t, js_el, js_esc); \
        fwrite(js_buf, 1, js_bl, g_out_json); \
        free(js_buf); \
      } \
      free(js_esc); \
    } \
  } else { \
    fwrite(emit_buf, 1, emit_len, g_out_tok); \
  } \
  printf("\n\n  %u  \n\n", token_id); \
	if((int)(te-(prefix_len+suffix_len)-ts)>0){ \
  	scanner(&in[ts + prefix_len - in], te - (prefix_len + suffix_len) - ts,filename, filepath_id, token_id, prefix_len, suffix_len); \
  }
%%{
  machine strings;
  include latex "latex.rl";

  ## Extended citation patterns (natbib + biblatex) ##
  citep      = '\\citep' (braces|brackets braces| braces braces) ;
  citet      = '\\citet' (braces|brackets braces| braces braces) ;
  citeauthor = '\\citeauthor' (braces|brackets braces) ;
  citeyear   = '\\citeyear' (braces|brackets braces) ;
  citealp    = '\\citealp' (braces|brackets braces| braces braces) ;
  autocite   = '\\autocite' (braces|brackets braces| braces braces) ;
  textcite   = '\\textcite' (braces|brackets braces| braces braces) ;
  parencite  = '\\parencite' (braces|brackets braces| braces braces) ;
  footcite   = '\\footcite' (braces|brackets braces) ;
  nocite     = '\\nocite' braces ;
  math_sub   = '_' (braces | alpha | digit) ;
  math_sup   = '^' (braces | alpha | digit) ;


  sc_prose = alpha+ ([\-'] alpha+)*;

main :=|*

  begin_doc => { EMIT("begin_doc"); };
  end_doc => { EMIT("end_doc"); };
  bibitem => { EMIT("bibitem"); };
  label               => { EMIT("label"); };
  frac               => { EMIT("frac"); };
  cite               => { EMIT("cite"); };
  citep              => { EMIT("citep"); };
  citet              => { EMIT("citet"); };
  citeauthor         => { EMIT("citeauthor"); };
  citeyear           => { EMIT("citeyear"); };
  citealp            => { EMIT("citealp"); };
  autocite           => { EMIT("autocite"); };
  textcite           => { EMIT("textcite"); };
  parencite          => { EMIT("parencite"); };
  footcite           => { EMIT("footcite"); };
  nocite             => { EMIT("nocite"); };
  ref               => { EMIT("ref"); };
  parens             => { EMIT_BLOCK("parens",1,1); };
  display_math             => { EMIT_BLOCK("display_math",2,2); };
  display_2_math             => { EMIT_BLOCK("display_2_math",2,2); };
  braces             => { EMIT("braces"); };
  author             => { EMIT_BLOCK("author",7,1); };
  affiliation 		=> { EMIT("affiliation"); };
  usepackage             => { EMIT("usepackage"); };
  title => { EMIT("title"); };
  inline_math        => { EMIT("math"); };
  math_sub           => { EMIT("math_sub"); };
  math_sup           => { EMIT("math_sup"); };
  comment            => { EMIT("comment");};

abstract => { EMIT_BLOCK("abstract", 16 , 14 ); };
ack => { EMIT_BLOCK("ack", 11 , 9 ); };
acknowledgement => { EMIT_BLOCK("acknowledgement", 23 , 21 ); };
acknowledgements => { EMIT_BLOCK("acknowledgements", 24 , 22 ); };
acknowledgments => { EMIT_BLOCK("acknowledgments", 23 , 21 ); };
acks => { EMIT_BLOCK("acks", 12 , 10 ); };
adjustbox => { EMIT_BLOCK("adjustbox", 17 , 15 ); };
algorithm => { EMIT_BLOCK("algorithm", 17 , 15 ); };
algorithmic => { EMIT_BLOCK("algorithmic", 19 , 17 ); };
alignat => { EMIT_BLOCK("alignat", 15 , 13 ); };
alignedat => { EMIT_BLOCK("alignedat", 17 , 15 ); };
aligned => { EMIT_BLOCK("aligned", 15 , 13 ); };
align => { EMIT_BLOCK("align", 13 , 11 ); };
alltt => { EMIT_BLOCK("alltt", 13 , 11 ); };
appendices => { EMIT_BLOCK("appendices", 18 , 16 ); };
appendix => { EMIT_BLOCK("appendix", 16 , 14 ); };
array => { EMIT_BLOCK("array", 13 , 11 ); };
article => { EMIT_BLOCK("article", 15 , 13 ); };
assumption => { EMIT_BLOCK("assumption", 18 , 16 ); };
aug => { EMIT_BLOCK("aug", 11 , 9 ); };
axis => { EMIT_BLOCK("axis", 12 , 10 ); };
barticle => { EMIT_BLOCK("barticle", 16 , 14 ); };
bbook => { EMIT_BLOCK("bbook", 13 , 11 ); };
block => { EMIT_BLOCK("block", 13 , 11 ); };
bmatrix => { EMIT_BLOCK("bmatrix", 15 , 13 ); };
bmisc => { EMIT_BLOCK("bmisc", 13 , 11 ); };
case => { EMIT_BLOCK("case", 12 , 10 ); };
cases => { EMIT_BLOCK("cases", 13 , 11 ); };
center => { EMIT_BLOCK("center", 14 , 12 ); };
centering => { EMIT_BLOCK("centering", 17 , 15 ); };
claim => { EMIT_BLOCK("claim", 13 , 11 ); };
code => { EMIT_BLOCK("code", 12 , 10 ); };
compactenum => { EMIT_BLOCK("compactenum", 19 , 17 ); };
compactitem => { EMIT_BLOCK("compactitem", 19 , 17 ); };
conjecture => { EMIT_BLOCK("conjecture", 18 , 16 ); };
conj => { EMIT_BLOCK("conj", 12 , 10 ); };
cor => { EMIT_BLOCK("cor",11,9); };
coro => { EMIT_BLOCK("coro", 12 , 10 ); };
corollary => { EMIT_BLOCK("corollary", 17 , 15 ); };
dcases => { EMIT_BLOCK("dcases", 14 , 12 ); };
defi => { EMIT_BLOCK("defi", 12 , 10 ); };
defin => { EMIT_BLOCK("defin", 13 , 11 ); };
definition => { EMIT_BLOCK("definition", 18 , 16 ); };
defn => { EMIT_BLOCK("defn", 12 , 10 ); };
deluxetable => { EMIT_BLOCK("deluxetable", 19 , 17 ); };
demo => { EMIT_BLOCK("demo", 12 , 10 ); };
description => { EMIT_BLOCK("description", 19 , 17 ); };
df => { EMIT_BLOCK("df", 10 , 8 ); };
dfn => { EMIT_BLOCK("dfn", 11 , 9 ); };
diagram => { EMIT_BLOCK("diagram", 15 , 13 ); };
eg => { EMIT_BLOCK("eg", 10 , 8 ); };
enumerate => { EMIT_BLOCK("enumerate", 17 , 15 ); };
eqnarray => { EMIT_BLOCK("eqnarray", 16 , 14 ); };
equation => { EMIT_BLOCK("equation", 16 , 14 ); };
equ => { EMIT_BLOCK("equ", 11 , 9 ); };
equs => { EMIT_BLOCK("equs", 12 , 10 ); };
exa => { EMIT_BLOCK("exa", 11 , 9 ); };
exam => { EMIT_BLOCK("exam", 12 , 10 ); };
example => { EMIT_BLOCK("example", 15 , 13 ); };
ex => { EMIT_BLOCK("ex", 10 , 8 ); };
exercise => { EMIT_BLOCK("exercise", 16 , 14 ); };
exmp => { EMIT_BLOCK("exmp", 12 , 10 ); };
fact => { EMIT_BLOCK("fact", 12 , 10 ); };
figure => { EMIT_BLOCK("figure", 14 , 12 ); };
filecontents => { EMIT_BLOCK("filecontents", 20 , 18 ); };
flalign => { EMIT_BLOCK("flalign", 15 , 13 ); };
flushleft => { EMIT_BLOCK("flushleft", 17 , 15 ); };
flushright => { EMIT_BLOCK("flushright", 18 , 16 ); };
fmffile => { EMIT_BLOCK("fmffile", 15 , 13 ); };
fmfgraph => { EMIT_BLOCK("fmfgraph", 16 , 14 ); };
footnotesize => { EMIT_BLOCK("footnotesize", 20 , 18 ); };
framed => { EMIT_BLOCK("framed", 14 , 12 ); };
frame => { EMIT_BLOCK("frame", 13 , 11 ); };
frontmatter => { EMIT_BLOCK("frontmatter", 19 , 17 ); };
gathered => { EMIT_BLOCK("gathered", 16 , 14 ); };
gather => { EMIT_BLOCK("gather", 14 , 12 ); };
indented => { EMIT_BLOCK("indented", 16 , 14 ); };
inparaenum => { EMIT_BLOCK("inparaenum", 18 , 16 ); };
itemize => { EMIT_BLOCK("itemize", 15 , 13 ); };
keyword => { EMIT_BLOCK("keyword", 15 , 13 ); };
keywords => { EMIT_BLOCK("keywords", 16 , 14 ); };
landscape => { EMIT_BLOCK("landscape", 17 , 15 ); };
large => { EMIT_BLOCK("large", 13 , 11 ); };
lema => { EMIT_BLOCK("lema", 12 , 10 ); };
lem => { EMIT_BLOCK("lem", 11 , 9 ); };
lemma => { EMIT_BLOCK("lemma", 13 , 11 ); };
lemme => { EMIT_BLOCK("lemme", 13 , 11 ); };
lemm => { EMIT_BLOCK("lemm", 12 , 10 ); };
linenomath => { EMIT_BLOCK("linenomath", 18 , 16 ); };
linenumbers => { EMIT_BLOCK("linenumbers", 19 , 17 ); };
list => { EMIT_BLOCK("list", 12 , 10 ); };
lm => { EMIT_BLOCK("lm", 10 , 8 ); };
longlist => { EMIT_BLOCK("longlist", 16 , 14 ); };
longtable => { EMIT_BLOCK("longtable", 17 , 15 ); };
lrbox => { EMIT_BLOCK("lrbox", 13 , 11 ); };
lstlisting => { EMIT_BLOCK("lstlisting", 18 , 16 ); };
mapleinput => { EMIT_BLOCK("mapleinput", 18 , 16 ); };
maplelatex => { EMIT_BLOCK("maplelatex", 18 , 16 ); };
math => { EMIT_BLOCK("math", 12 , 10 ); };
mathletters => { EMIT_BLOCK("mathletters", 19 , 17 ); };
mathpar => { EMIT_BLOCK("mathpar", 15 , 13 ); };
matrix => { EMIT_BLOCK("matrix", 14 , 12 ); };
minipage => { EMIT_BLOCK("minipage", 16 , 14 ); };
multicols => { EMIT_BLOCK("multicols", 17 , 15 ); };
multlined => { EMIT_BLOCK("multlined", 17 , 15 ); };
multline => { EMIT_BLOCK("multline", 16 , 14 ); };
mydef => { EMIT_BLOCK("mydef", 13 , 11 ); };
notation => { EMIT_BLOCK("notation", 16 , 14 ); };
note => { EMIT_BLOCK("note", 12 , 10 ); };
obs => { EMIT_BLOCK("obs", 11 , 9 ); };
observation => { EMIT_BLOCK("observation", 19 , 17 ); };
opening => { EMIT_BLOCK("opening", 15 , 13 ); };
overpic => { EMIT_BLOCK("overpic", 15 , 13 ); };
pf => { EMIT_BLOCK("pf", 10 , 8 ); };
pgfonlayer => { EMIT_BLOCK("pgfonlayer", 18 , 16 ); };
pgfscope => { EMIT_BLOCK("pgfscope", 16 , 14 ); };
picture => { EMIT_BLOCK("picture", 15 , 13 ); };
pmatrix => { EMIT_BLOCK("pmatrix", 15 , 13 ); };
pr => { EMIT_BLOCK("pr", 10 , 8 ); };
prf => { EMIT_BLOCK("prf", 11 , 9 ); };
prob => { EMIT_BLOCK("prob", 12 , 10 ); };
problem => { EMIT_BLOCK("problem", 15 , 13 ); };
pro => { EMIT_BLOCK("pro", 11 , 9 ); };
proof => { EMIT_BLOCK("proof", 13 , 11 ); };
proofof => { EMIT_BLOCK("proofof", 15 , 13 ); };
prooftree => { EMIT_BLOCK("prooftree", 17 , 15 ); };
prop => { EMIT_BLOCK("prop", 12 , 10 ); };
property => { EMIT_BLOCK("property", 16 , 14 ); };
propo => { EMIT_BLOCK("propo", 13 , 11 ); };
proposition => { EMIT_BLOCK("proposition", 19 , 17 ); };
prp => { EMIT_BLOCK("prp", 11 , 9 ); };
psfrags => { EMIT_BLOCK("psfrags", 15 , 13 ); };
pspicture => { EMIT_BLOCK("pspicture", 17 , 15 ); };
question => { EMIT_BLOCK("question", 16 , 14 ); };
quotation => { EMIT_BLOCK("quotation", 17 , 15 ); };
quote => { EMIT_BLOCK("quote", 13 , 11 ); };
references => { EMIT_BLOCK("references", 18 , 16 ); };
rema => { EMIT_BLOCK("rema", 12 , 10 ); };
remark => { EMIT_BLOCK("remark", 14 , 12 ); };
rem => { EMIT_BLOCK("rem", 11 , 9 ); };
restatable => { EMIT_BLOCK("restatable", 18 , 16 ); };
rmk => { EMIT_BLOCK("rmk", 11 , 9 ); };
rotate => { EMIT_BLOCK("rotate", 14 , 12 ); };
ruledtabular => { EMIT_BLOCK("ruledtabular", 20 , 18 ); };
sc => { EMIT_BLOCK("sc", 10 , 8 ); };
scope => { EMIT_BLOCK("scope", 13 , 11 ); };
scriptsize => { EMIT_BLOCK("scriptsize", 18 , 16 ); };
section => { EMIT_BLOCK("section", 15 , 13 ); };
sideways => { EMIT_BLOCK("sideways", 16 , 14 ); };
sidewaystable => { EMIT_BLOCK("sidewaystable", 21 , 19 ); };
sloppypar => { EMIT_BLOCK("sloppypar", 17 , 15 ); };
small => { EMIT_BLOCK("small", 13 , 11 ); };
smallmatrix => { EMIT_BLOCK("smallmatrix", 19 , 17 ); };
spacing => { EMIT_BLOCK("spacing", 15 , 13 ); };
split => { EMIT_BLOCK("split", 13 , 11 ); };
subarray => { EMIT_BLOCK("subarray", 16 , 14 ); };
subeqnarray => { EMIT_BLOCK("subeqnarray", 19 , 17 ); };
subequations => { EMIT_BLOCK("subequations", 20 , 18 ); };
subfigure => { EMIT_BLOCK("subfigure", 17 , 15 ); };
subsection => { EMIT_BLOCK("subsection", 18 , 16 ); };
subtable => { EMIT_BLOCK("subtable", 16 , 14 ); };
tabbing => { EMIT_BLOCK("tabbing", 15 , 13 ); };
table => { EMIT_BLOCK("table", 13 , 11 ); };
tablenotes => { EMIT_BLOCK("tablenotes", 18 , 16 ); };
tabular => { EMIT_BLOCK("tabular", 15 , 13 ); };
tabularx => { EMIT_BLOCK("tabularx", 16 , 14 ); };
tcolorbox => { EMIT_BLOCK("tcolorbox", 17 , 15 ); };
teo => { EMIT_BLOCK("teo", 11 , 9 ); };
theacknowledgments => { EMIT_BLOCK("theacknowledgments", 26 , 24 ); };
thebibliography => { EMIT_BLOCK("thebibliography", 23 , 21 ); };

theo => { EMIT_BLOCK("theo", 12 , 10 ); };
theorem => { EMIT_BLOCK("theorem", 15 , 13 ); };
thm => { EMIT_BLOCK("thm", 11 , 9 ); };
threeparttable => { EMIT_BLOCK("threeparttable", 22 , 20 ); };
tikzcd => { EMIT_BLOCK("tikzcd", 14 , 12 ); };
tikzpicture => { EMIT_BLOCK("tikzpicture", 19 , 17 ); };
tiny => { EMIT_BLOCK("tiny", 12 , 10 ); };
titlepage => { EMIT_BLOCK("titlepage", 17 , 15 ); };
trivlist => { EMIT_BLOCK("trivlist", 16 , 14 ); };
turn => { EMIT_BLOCK("turn", 12 , 10 ); };
turnpage => { EMIT_BLOCK("turnpage", 16 , 14 ); };
verbatim => { EMIT_BLOCK("verbatim", 16 , 14 ); };
vmatrix => { EMIT_BLOCK("vmatrix", 15 , 13 ); };
widetext => { EMIT_BLOCK("widetext", 16 , 14 ); };
wrapfigure => { EMIT_BLOCK("wrapfigure", 18 , 16 ); };


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

