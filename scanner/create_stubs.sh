#!/bin/bash
# create_stubs.sh — Generate minimal .sty and .cls stubs for missing TeX packages
set -euo pipefail
STUBS="$(dirname "$0")/stubs"
mkdir -p "$STUBS"

# ============================================================
# CLASS STUBS
# ============================================================

# revtex4 — APS journal class (135 files)
cat > "$STUBS/revtex4.cls" << 'EOF'
\ProvidesClass{revtex4}
\LoadClass[twocolumn,10pt]{article}
\RequirePackage{cite}
\def\affiliation#1{\thanks{#1}}
\def\email#1{\texttt{#1}}
\def\pacs#1{}
\def\keywords#1{}
\def\preprint#1{}
\def\altaffilmark#1{}
\def\altaffiltext#1#2{}
\def\collaboration#1{}
\def\title#1{\gdef\@title{#1}}
\def\author#1{\gdef\@author{#1}}
\def\date#1{\gdef\@date{#1}}
\renewcommand\maketitle{\twocolumn[\begin{center}\LARGE\bf\@title\vskip1em\normalsize\@author\vskip0.5em\@date\end{center}]\global\let\@title\relax\global\let\@author\relax}
\def\and{\par}
\def\tabularx{\table*}
\endinput
EOF

# revtex — older APS class (5 files)
cat > "$STUBS/revtex.cls" << 'EOF'
\ProvidesClass{revtex}
\LoadClass[twocolumn]{article}
\def\preprint#1{}
\def\pacs#1{}
\def\email#1{\texttt{#1}}
\def\homepage#1{\texttt{#1}}
\endinput
EOF

# JHEP3 — JHEP class (63 files)
cat > "$STUBS/JHEP3.cls" << 'EOF'
\ProvidesClass{JHEP3}
\LoadClass[11pt]{article}
\RequirePackage{amsmath,amssymb}
\def\a{\alpha}
\def\b{\beta}
\def\d{\partial}
\def\e{\epsilon}
\def\g{\gamma}
\def\l{\lambda}
\def\m{\mu}
\def\n{\nu}
\def\w{\omega}
\def\s{\sigma}
\def\t{\theta}
\def\z{\zeta}
\def\pd{\partial}
\def\so{\mathcal{O}}
\def\sa{\mathcal{A}}
\def\se{\mathcal{E}}
\def\ver{\text{ver}}
\def\hor{\text{hor}}
\def\wt{\widetilde}
\def\wh{\widehat}
\def\vev{\langle}
\def\diag{\text{diag}}
\def\Tr{\text{Tr}}
\def\dd{\text{d}}
\def\ii{\text{i}}
\def\ee{\text{e}}
\def\iso{\cong}
\def\beq{\begin{equation}}
\def\eeq{\end{equation}}
\def\bea{\begin{eqnarray}}
\def\eea{\end{eqnarray}}
\def\bal{\begin{aligned}}
\def\eal{\end{aligned}}
\endinput
EOF

# JHEP — older JHEP class (17 files)
cat > "$STUBS/JHEP.cls" << 'EOF'
\ProvidesClass{JHEP}
\LoadClass[11pt]{article}
\RequirePackage{amsmath,amssymb}
\endinput
EOF

# iopart — IOP journal class (12 files)
cat > "$STUBS/iopart.cls" << 'EOF'
\ProvidesClass{iopart}
\LoadClass[11pt]{article}
\def\i{\text{i}}
\def\e{\text{e}}
\def\rd{\text{d}}
\def\baselinestretch{1}
\def\address#1{\thanks{#1}}
\def\author#1{\gdef\@author{#1}}
\def\title#1{\gdef\@title{#1}}
\endinput
EOF

# ws — World Scientific class (2 files)
cat > "$STUBS/ws.cls" << 'EOF'
\ProvidesClass{ws}
\LoadClass[11pt]{article}
\endinput
EOF
ln -sf ws.cls "$STUBS/ws-procs9x6.cls" 2>/dev/null || true

# elsart — Elsevier class (2 files)
cat > "$STUBS/elsart.cls" << 'EOF'
\ProvidesClass{elsart}
\LoadClass[11pt]{article}
\def\fnmark#1{}
\def\fntext#1{}
\def\corresp#1{}
\def\cormark#1{}
\def\address#1{\thanks{#1}}
\def\ead#1{\texttt{#1}}
\endinput
EOF

# svmult — Springer (2 files)
cat > "$STUBS/svmult.cls" << 'EOF'
\ProvidesClass{svmult}
\LoadClass[11pt]{article}
\endinput
EOF

# cargese — conference (3 files)
cat > "$STUBS/cargese.cls" << 'EOF'
\ProvidesClass{cargese}
\LoadClass[11pt]{article}
\endinput
EOF

# siggraph (1 file)
cat > "$STUBS/siggraph.cls" << 'EOF'
\ProvidesClass{siggraph}
\LoadClass[11pt]{article}
\endinput
EOF

# ptptex (2 files)
cat > "$STUBS/ptptex.cls" << 'EOF'
\ProvidesClass{ptptex}
\LoadClass[11pt]{article}
\endinput
EOF

# amsppt (1 file)
cat > "$STUBS/amsppt.cls" << 'EOF'
\ProvidesClass{amsppt}
\LoadClass{amsart}
\endinput
EOF

# smfart (1 file)
cat > "$STUBS/smfart.cls" << 'EOF'
\ProvidesClass{smfart}
\LoadClass[11pt]{article}
\endinput
EOF

# conm-p-l (1 file)
cat > "$STUBS/conm-p-l.cls" << 'EOF'
\ProvidesClass{conm-p-l}
\LoadClass[11pt]{article}
\endinput
EOF

# jpsj2 (1 file)
cat > "$STUBS/jpsj2.cls" << 'EOF'
\ProvidesClass{jpsj2}
\LoadClass[11pt]{article}
\endinput
EOF

# jnmp (1 file)
cat > "$STUBS/jnmp.cls" << 'EOF'
\ProvidesClass{jnmp}
\LoadClass[11pt]{article}
\endinput
EOF

# pacmem (1 file)
cat > "$STUBS/pacmem.cls" << 'EOF'
\ProvidesClass{pacmem}
\LoadClass[11pt]{article}
\endinput
EOF

echo "Created $(ls "$STUBS"/*.cls 2>/dev/null | wc -l) class stubs"

# ============================================================
# PACKAGE STUBS
# ============================================================

# epsf — EPS inclusion (55 files)
cat > "$STUBS/epsf.sty" << 'EOF'
\ProvidesPackage{epsf}
\newdimen\epsfxsize
\newdimen\epsfysize
\epsfxsize=0pt
\epsfysize=0pt
\def\epsfxsize#1{\epsfxsize=#1}
\def\epsfysize#1{\epsfysize=#1}
\def\epsfbox#1{\includegraphics{#1}}
\def\epsffile#1{\includegraphics{#1}}
\def\epsfverbose{}
\def\epsftype{0}
\endinput
EOF

# a4wide — wider A4 pages (19 files)
cat > "$STUBS/a4wide.sty" << 'EOF'
\ProvidesPackage{a4wide}
\setlength{\textwidth}{17cm}
\setlength{\textheight}{25cm}
\setlength{\oddsidemargin}{-0.5cm}
\setlength{\evensidemargin}{-0.5cm}
\setlength{\topmargin}{-1.5cm}
\endinput
EOF

# psfig — EPS inclusion (13 files)
cat > "$STUBS/psfig.sty" << 'EOF'
\ProvidesPackage{psfig}
\RequirePackage{graphicx}
\def\psfig{\@ifnextchar[{\@psfig}{\@psfig}}
\def\@psfig[#1]{\includegraphics{#1}}
\endinput
EOF

# bbm — blackboard bold (11 files)
cat > "$STUBS/bbm.sty" << 'EOF'
\ProvidesPackage{bbm}
\RequirePackage{amsfonts}
\DeclareFontFamily{U}{bbm}{}
\DeclareFontShape{U}{bbm}{m}{n}{<-> s*[0.95] bbm10}{}
\DeclareSymbolFont{bbm}{U}{bbm}{m}{n}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`A}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`B}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`C}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`D}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`E}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`F}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`G}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`H}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`I}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`J}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`K}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`L}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`M}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`N}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`O}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`P}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`Q}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`R}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`S}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`T}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`U}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`V}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`W}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`X}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`Y}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`Z}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`0}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`1}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`2}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`3}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`4}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`5}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`6}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`7}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`8}
\DeclareMathSymbol{\mathbbm}{1}{bbm}{`9}
\endinput
EOF

# bbold (11 files)
cat > "$STUBS/bbold.sty" << 'EOF'
\ProvidesPackage{bbold}
\RequirePackage{amsfonts}
\endinput
EOF

# epic — enhanced picture mode (10 files)
cat > "$STUBS/epic.sty" << 'EOF'
\ProvidesPackage{epic}
\newdimen\dashline
\newdimen\dottedline
\def\drawline{\@ifnextchar(\@drawline\@drawline}
\def\@drawline(#1,#2)(#3,#4)(#5,#6){\put(#1,#2){\line(#3-#1,#4-#2){1}}}
\def\dashline{\@ifnextchar[{\@dashline}{\@dashline}}
\def\@dashline[#1]#2(#3,#4)(#5,#6){\put(#3,#4){\line(#5-#3,#6-#4){1}}}
\def\dottedline#1(#2,#3)(#4,#5){\put(#2,#3){\circle*{1}}}
\def\dashlinestretch#1{}
\def\ellipse#1#2{\circle*{1}}
\def\arc#1#2#3{\circle*{1}}
\def\path{\@ifnextchar(\@path\@path}
\def\@path(#1,#2){\put(#1,#2){\circle*{0.5}}}
\endinput
EOF

# eepic (5 files)
cat > "$STUBS/eepic.sty" << 'EOF'
\ProvidesPackage{eepic}
\RequirePackage{epic}
\endinput
EOF

# axodraw — Feynman diagrams (9 files)
cat > "$STUBS/axodraw.sty" << 'EOF'
\ProvidesPackage{axodraw}
\RequirePackage{graphicx}
\unitlength=1pt
\def\Line(#1,#2)(#3,#4){\put(#1,#2){\line(#3-#1,#4-#2){1}}}
\def\ArrowLine(#1,#2)(#3,#4){\put(#1,#2){\vector(#3-#1,#4-#2){1}}}
\def\DashLine(#1,#2)(#3,#4)#5{\put(#1,#2){\line(#3-#1,#4-#2){1}}}
\def\DashArrowLine(#1,#2)(#3,#4)#5{\put(#1,#2){\vector(#3-#1,#4-#2){1}}}
\def\Photon(#1,#2)(#3,#4)#5#6{\put(#1,#2){\line(#3-#1,#4-#2){1}}}
\def\ArrowArc(#1,#2)(#3,#4,#5){\put(#1,#2){\circle*{1}}}
\def\ArrowArcn(#1,#2)(#3,#4,#5){\put(#1,#2){\circle*{1}}}
\def\Vertex(#1,#2)#3{\put(#1,#2){\circle*{#3}}}
\def\BCirc(#1,#2)#3{\put(#1,#2){\circle{#3}}}
\def\Text(#1,#2)#3{\put(#1,#2){\makebox(0,0){#3}}}
\def\LongArrow(#1,#2)(#3,#4){\put(#1,#2){\vector(#3-#1,#4-#2){1}}}
\endinput
EOF

# xy — Xy-pic diagrams (8 files)
cat > "$STUBS/xy.sty" << 'EOF'
\ProvidesPackage{xy}
\def\xymatrix#1{\begin{equation}\begin{array}{c}#1\end{array}\end{equation}}
\def\ar{\@ifnextchar[{\@ar}{\@ar}}
\def\@ar[#1]{}
\def\save{}
\def\restore{}
\endinput
EOF

# fortschritte (7 files)
cat > "$STUBS/fortschritte.sty" << 'EOF'
\ProvidesPackage{fortschritte}
\def\Journal#1{}
\def\Volume#1{}
\def\Year#1{}
\endinput
EOF

# showlabels (8 files)
cat > "$STUBS/showlabels.sty" << 'EOF'
\ProvidesPackage{showlabels}
\endinput
EOF

# wrapfig (5 files)
cat > "$STUBS/wrapfig.sty" << 'EOF'
\ProvidesPackage{wrapfig}
\newenvironment{wrapfigure}[2]{}{}
\newenvironment{wraptable}[2]{}{}
\endinput
EOF

# subfigure (5 files)
cat > "$STUBS/subfigure.sty" << 'EOF'
\ProvidesPackage{subfigure}
\newenvironment{subfigure}[1]{\begin{minipage}{#1}}{\end{minipage}}
\def\subcaption#1{}
\def\subfloat#1{#1}
\endinput
EOF

# pstricks (4 files)
cat > "$STUBS/pstricks.sty" << 'EOF'
\ProvidesPackage{pstricks}
\def\psline{\@ifnextchar[{\@gobble}{\@gobble}}
\def\psframe{\@ifnextchar[{\@gobble}{\@gobble}}
\def\pscircle{\@ifnextchar[{\@gobble}{\@gobble}}
\def\psoval{\@ifnextchar[{\@gobble}{\@gobble}}
\def\pscurve{\@ifnextchar[{\@gobble}{\@gobble}}
\def\rput(#1,#2)#3{#3}
\def\uput(#1,#2)#3{}
\def\psdots{\@ifnextchar[{\@gobble}{\@gobble}}
\def\psgrid{}
\newdimen\psunit
\psunit=1cm
\endinput
EOF

# dsfont (4 files)
cat > "$STUBS/dsfont.sty" << 'EOF'
\ProvidesPackage{dsfont}
\RequirePackage{amsfonts}
\def\mathds#1{\mathbb{#1}}
\endinput
EOF

# citesort (4 files)
cat > "$STUBS/citesort.sty" << 'EOF'
\ProvidesPackage{citesort}
\endinput
EOF

# feynmf (6 files)
cat > "$STUBS/feynmf.sty" << 'EOF'
\ProvidesPackage{feynmf}
\def\fmfcmd#1{}
\def\fmfstraight{}
\def\fmfleft#1{}
\def\fmfright#1{}
\def\fmf#1#2#3{}
\def\fmflabel#1#2{}
\def\fmfv#1#2{}
\def\fmffreeze{}
\def\fmfdot#1{}
\newenvironment{fmffile}[1]{\begin{center}}{\end{center}}
\endinput
EOF
ln -sf feynmf.sty "$STUBS/feynmp.sty" 2>/dev/null || true

# type1cm (2 files)
cat > "$STUBS/type1cm.sty" << 'EOF'
\ProvidesPackage{type1cm}
\endinput
EOF

# ulem (2 files)
cat > "$STUBS/ulem.sty" << 'EOF'
\ProvidesPackage{ulem}
\def\uline#1{#1}
\def\uwave#1{#1}
\def\sout#1{#1}
\def\xout#1{#1}
\def\dashuline#1{#1}
\def\dotuline#1{#1}
\endinput
EOF

# doublespace (2 files)
cat > "$STUBS/doublespace.sty" << 'EOF'
\ProvidesPackage{doublespace}
\def\doublespacing{}
\def\singlespacing{}
\endinput
EOF

# fullpage (2 files)
cat > "$STUBS/fullpage.sty" << 'EOF'
\ProvidesPackage{fullpage}
\setlength{\textwidth}{6.5in}
\setlength{\textheight}{9in}
\setlength{\oddsidemargin}{0pt}
\setlength{\evensidemargin}{0pt}
\setlength{\topmargin}{0pt}
\endinput
EOF

# german (2 files)
cat > "$STUBS/german.sty" << 'EOF'
\ProvidesPackage{german}
\def\dq{"}
\def\glqq{\char`\"}
\def\grqq{\char`\"}
\def\glq{\char`\"}
\def\grq{\char`\"}
\endinput
EOF

# isolatin1 (2 files) — legacy input encoding
cat > "$STUBS/isolatin1.sty" << 'EOF'
\ProvidesPackage{isolatin1}
\RequirePackage[utf8]{inputenc}
\endinput
EOF

# iopams — IOP additional math (6 files)
cat > "$STUBS/iopams.sty" << 'EOF'
\ProvidesPackage{iopams}
\RequirePackage{amsmath,amssymb}
\let\Bbb=\mathbb
\endinput
EOF

# subeqn (2 files)
cat > "$STUBS/subeqn.sty" << 'EOF'
\ProvidesPackage{subeqn}
\endinput
EOF

# subeqnarray (2 files)
cat > "$STUBS/subeqnarray.sty" << 'EOF'
\ProvidesPackage{subeqnarray}
\endinput
EOF

# subequation (1 file)
cat > "$STUBS/subequation.sty" << 'EOF'
\ProvidesPackage{subequations}
\endinput
EOF

# subeqnar (1 file)
cat > "$STUBS/subeqnar.sty" << 'EOF'
\ProvidesPackage{subeqnar}
\endinput
EOF

# supercite (1 file)
cat > "$STUBS/supercite.sty" << 'EOF'
\ProvidesPackage{supercite}
\endinput
EOF

# pstcol (1 file)
cat > "$STUBS/pstcol.sty" << 'EOF'
\ProvidesPackage{pstcol}
\RequirePackage{color}
\endinput
EOF

# espcrc2 (1 file)
cat > "$STUBS/espcrc2.sty" << 'EOF'
\ProvidesPackage{espcrc2}
\endinput
EOF

# bozhlogo (2 files)
cat > "$STUBS/bozhlogo.sty" << 'EOF'
\ProvidesPackage{bozhlogo}
\endinput
EOF
ln -sf bozhlogo.sty "$STUBS/bozhomac.sty" 2>/dev/null || true

# alpheqn (1 file)
cat > "$STUBS/alpheqn.sty" << 'EOF'
\ProvidesPackage{alpheqn}
\endinput
EOF

# curves (1 file)
cat > "$STUBS/curves.sty" << 'EOF'
\ProvidesPackage{curves}
\endinput
EOF

# emlines (1 file)
cat > "$STUBS/emlines.sty" << 'EOF'
\ProvidesPackage{emlines}
\endinput
EOF
ln -sf emlines.sty "$STUBS/emlines2.sty" 2>/dev/null || true

# eqsection (1 file)
cat > "$STUBS/eqsection.sty" << 'EOF'
\ProvidesPackage{eqsection}
\endinput
EOF

# equations (1 file)
cat > "$STUBS/equations.sty" << 'EOF'
\ProvidesPackage{equations}
\endinput
EOF

# physprbb (1 file)
cat > "$STUBS/physprbb.sty" << 'EOF'
\ProvidesPackage{physprbb}
\endinput
EOF

# rsfs (1 file)
cat > "$STUBS/rsfs.sty" << 'EOF'
\ProvidesPackage{rsfs}
\RequirePackage{mathrsfs}
\endinput
EOF

# sc3conf (1 file)
cat > "$STUBS/sc3conf.sty" << 'EOF'
\ProvidesPackage{sc3conf}
\endinput
EOF

# simplemargins (1 file)
cat > "$STUBS/simplemargins.sty" << 'EOF'
\ProvidesPackage{simplemargins}
\endinput
EOF

# ujp (1 file)
cat > "$STUBS/ujp.sty" << 'EOF'
\ProvidesPackage{ujp}
\endinput
EOF

# yfonts (1 file)
cat > "$STUBS/yfonts.sty" << 'EOF'
\ProvidesPackage{yfonts}
\RequirePackage{amsfonts}
\endinput
EOF

# multibox (1 file)
cat > "$STUBS/multibox.sty" << 'EOF'
\ProvidesPackage{multibox}
\endinput
EOF

# multirow (1 file)
cat > "$STUBS/multirow.sty" << 'EOF'
\ProvidesPackage{multirow}
\def\multirow#1#2#3{#3}
\endinput
EOF

# footmisc (1 file)
cat > "$STUBS/footmisc.sty" << 'EOF'
\ProvidesPackage{footmisc}
\endinput
EOF

# showlabels (8 files)
cat > "$STUBS/showlabels.sty" << 'EOF'
\ProvidesPackage{showlabels}
\endinput
EOF

# paper2e (1 file)
cat > "$STUBS/paper2e.sty" << 'EOF'
\ProvidesPackage{paper2e}
\endinput
EOF

# mydefs2e (1 file)
cat > "$STUBS/mydefs2e.sty" << 'EOF'
\ProvidesPackage{mydefs2e}
\endinput
EOF

# myref (1 file)
cat > "$STUBS/myref.sty" << 'EOF'
\ProvidesPackage{myref}
\endinput
EOF

# mysymb (1 file)
cat > "$STUBS/mysymb.sty" << 'EOF'
\ProvidesPackage{mysymb}
\endinput
EOF

# psfig (13 files)
cat > "$STUBS/psfig.sty" << 'EOF'
\ProvidesPackage{psfig}
\RequirePackage{graphicx,keyval}
\define@key{psfig}{figure}{\def\psfig@file{#1}}
\define@key{psfig}{file}{\def\psfig@file{#1}}
\define@key{psfig}{width}{\def\psfig@width{#1}}
\define@key{psfig}{height}{\def\psfig@height{#1}}
\define@key{psfig}{angle}{\def\psfig@angle{#1}}
\def\psfig{\@ifnextchar[{\@psfig}{\@psfig}}
\def\@psfig[#1]{\setkeys{psfig}{#1}\includegraphics[width=\psfig@width,height=\psfig@height,angle=\psfig@angle]{\psfig@file}}
\endinput
EOF

echo "Created $(ls "$STUBS"/*.sty 2>/dev/null | wc -l) package stubs"
echo "Stubs directory: $STUBS"
