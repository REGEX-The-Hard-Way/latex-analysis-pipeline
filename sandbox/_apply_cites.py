#!/usr/bin/env python3
"""Apply citation pattern updates to latex.rl."""

with open('scanner/latex.rl', 'r') as f:
    content = f.read()

old = ("cite = '\\\\cite' (braces|brackets braces| braces braces) ;\n"
       "ref = '\\\\ref' braces;\n"
       "bibitem_start = 'bibitem' ;\n"
       "bibitem_body = any+ - bibitem_start;\n"
       "bibitem = bibitem_start braces (bibitem_body)* :>> '\\n\\\\' ;")

new = ("cite = '\\\\cite' (braces|brackets braces| braces braces) ;\n"
       "\n"
       "/* natbib citation variants */\n"
       "citep      = '\\\\citep' (braces|brackets braces| braces braces) ;\n"
       "citet      = '\\\\citet' (braces|brackets braces| braces braces) ;\n"
       "citeauthor = '\\\\citeauthor' (braces|brackets braces) ;\n"
       "citeyear   = '\\\\citeyear' (braces|brackets braces) ;\n"
       "citealp    = '\\\\citealp' (braces|brackets braces| braces braces) ;\n"
       "\n"
       "/* biblatex citation variants */\n"
       "autocite  = '\\\\autocite' (braces|brackets braces| braces braces) ;\n"
       "textcite  = '\\\\textcite' (braces|brackets braces| braces braces) ;\n"
       "parencite = '\\\\parencite' (braces|brackets braces| braces braces) ;\n"
       "footcite  = '\\\\footcite' (braces|brackets braces) ;\n"
       "\n"
       "/* \\nocite and \\ref */\n"
       "nocite = '\\\\nocite' braces ;\n"
       "ref = '\\\\ref' braces;\n"
       "\n"
       "/* \\bibitem with optional argument */\n"
       "bibitem_opt = 'bibitem' brackets braces ;\n"
       "bibitem_start = 'bibitem' ;\n"
       "bibitem_body = any+ - bibitem_start;\n"
       "bibitem = (bibitem_opt | bibitem_start braces) (bibitem_body)* :>> '\\n\\\\' ;")

if old in content:
    content = content.replace(old, new)
    with open('scanner/latex.rl', 'w') as f:
        f.write(content)
    print("OK: citation patterns added")
else:
    print("ERROR: old pattern not found in latex.rl")
