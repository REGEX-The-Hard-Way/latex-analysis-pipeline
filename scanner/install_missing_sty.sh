#!/bin/bash
# install_missing_sty.sh — Install missing .sty files for 2003 dataset.
# Phase 1: Install texlive collections via apt
# Phase 2: Create stub .sty files for journal-specific packages
# Phase 3: Create forwarding stubs (epsf→graphicx, psfig→graphicx, etc.)
#
# Usage: sudo bash install_missing_sty.sh

set -euo pipefail
TEXMF=$(kpsewhich -var-value=TEXMFHOME 2>/dev/null || echo "$HOME/texmf")
STYDIR="${TEXMF}/tex/latex/stubs"
mkdir -p "$STYDIR"

echo "=== Phase 1: Install texlive collections ==="
if command -v apt-get &>/dev/null; then
    apt-get install -y texlive-latex-recommended texlive-latex-extra 2>&1 | tail -5 || echo "  (may need sudo)"
fi

echo ""
echo "=== Phase 2: Create stub .sty files ==="
echo "Installing to: $STYDIR"

# ---- epsf → graphicx forwarding ----
cat > "$STYDIR/epsf.sty" << 'STYEOF'
% epsf.sty — stub forwarding to graphicx
\ProvidesPackage{epsf}[2024/01/01 stub]
\RequirePackage{graphicx}
\def\epsfbox#1{\includegraphics{#1}}
\def\epsffile#1{\includegraphics{#1}}
\def\epsfxsize{}
\def\epsfysize{}
\def\epsfverbosetrue{}
\endinput
STYEOF

# ---- psfig → graphicx forwarding ----
cat > "$STYDIR/psfig.sty" << 'STYEOF'
\ProvidesPackage{psfig}[2024/01/01 stub]
\RequirePackage{graphicx}
\def\psfig#1{\includegraphics{#1}}
\endinput
STYEOF

# ---- a4wide ----
cat > "$STYDIR/a4wide.sty" << 'STYEOF'
\ProvidesPackage{a4wide}[2024/01/01 stub]
\setlength{\paperwidth}{210mm}
\setlength{\paperheight}{297mm}
\setlength{\textwidth}{160mm}
\setlength{\textheight}{247mm}
\endinput
STYEOF

# ---- srcltx ----
cat > "$STYDIR/srcltx.sty" << 'STYEOF'
\ProvidesPackage{srcltx}[2024/01/01 stub]
% source specials — no-op for compilation
\endinput
STYEOF

# ---- draftcopy ----
cat > "$STYDIR/draftcopy.sty" << 'STYEOF'
\ProvidesPackage{draftcopy}[2024/01/01 stub]
% draft watermark — no-op
\endinput
STYEOF

# ---- showlabels ----
cat > "$STYDIR/showlabels.sty" << 'STYEOF'
\ProvidesPackage{showlabels}[2024/01/01 stub]
% show labels in margin — no-op
\endinput
STYEOF

# ---- subfigure ----
cat > "$STYDIR/subfigure.sty" << 'STYEOF'
\ProvidesPackage{subfigure}[2024/01/01 stub]
\RequirePackage{subcaption}
\endinput
STYEOF

# ---- doublespace ----
cat > "$STYDIR/doublespace.sty" << 'STYEOF'
\ProvidesPackage{doublespace}[2024/01/01 stub]
\RequirePackage{setspace}
\doublespacing
\endinput
STYEOF

# ---- fullpage ----
cat > "$STYDIR/fullpage.sty" << 'STYEOF'
\ProvidesPackage{fullpage}[2024/01/01 stub]
\setlength{\textwidth}{6.5in}
\setlength{\textheight}{9in}
\setlength{\oddsidemargin}{0in}
\setlength{\evensidemargin}{0in}
\setlength{\topmargin}{0in}
\endinput
STYEOF

# ---- german ----
cat > "$STYDIR/german.sty" << 'STYEOF'
\ProvidesPackage{german}[2024/01/01 stub]
% German language support stub
\def\germanhyphen{}
\endinput
STYEOF

# ---- isolatin1 ----
cat > "$STYDIR/isolatin1.sty" << 'STYEOF'
\ProvidesPackage{isolatin1}[2024/01/01 stub]
\RequirePackage{inputenc}
\inputencoding{latin1}
\endinput
STYEOF

# ---- citesort ----
cat > "$STYDIR/citesort.sty" << 'STYEOF'
\ProvidesPackage{citesort}[2024/01/01 stub]
% sorted citations — no-op stub
\endinput
STYEOF

# ---- subeqnarray / subeqnar / subeqn / subequation ----
for name in subeqnarray subeqnar subeqn subequation; do
cat > "$STYDIR/${name}.sty" << 'STYEOF'
\ProvidesPackage{XXX}[2024/01/01 stub]
% sub-equation numbering stub
\newenvironment{subequations}{}{}
\newenvironment{XXXenv}{}{}
\endinput
STYEOF
sed -i "s/XXX/${name}/g" "$STYDIR/${name}.sty"
done

# ---- amsmath2000 ----
cat > "$STYDIR/amsmath2000.sty" << 'STYEOF'
\ProvidesPackage{amsmath2000}[2024/01/01 stub]
\RequirePackage{amsmath}
\endinput
STYEOF

# ---- multirow ----
cat > "$STYDIR/multirow.sty" << 'STYEOF'
\ProvidesPackage{multirow}[2024/01/01 stub]
\newcommand{\multirow}[3]{#3}
\endinput
STYEOF

# ---- cropmark ----
cat > "$STYDIR/cropmark.sty" << 'STYEOF'
\ProvidesPackage{cropmark}[2024/01/01 stub]
% crop marks — no-op
\endinput
STYEOF

# ---- refcheck ----
cat > "$STYDIR/refcheck.sty" << 'STYEOF'
\ProvidesPackage{refcheck}[2024/01/01 stub]
% reference checking — no-op
\endinput
STYEOF

# ---- latexsym (stub — already in base) ----
rm -f "$STYDIR/latexsym.sty"  # included in latex base

echo ""
echo "=== Phase 3: Journal-specific no-op stubs ==="
declare -A STUBS=(
    [romp34]="Reviews of Modern Physics 3/4"
    [sc3conf]="Sc3Conf conference style"
    [ascmac]="ASCII Macros"
    [fortschritte]="Fortschritte der Physik"
    [fortschritte2]="Fortschritte der Physik v2"
    [iopams]="IOP AMS math"
    [ujp]="Ukrainian Journal of Physics"
    [physprbb]="Physical Review B"
    [sw20lart]="SW 2.0 LaTeX article"
    [axodraw]="Feynman diagram drawing"
    [axmsmath]="AMS math extension"
    [bozhomac]="Bozho macros"
    [bozhlogo]="Bozho logo"
    [theopaper1]="Theory paper style"
    [emlines]="EM lines"
    [emlines2]="EM lines v2"
    [epic]="Extended picture"
    [eepic]="Extended epic"
    [feynmf]="Feynman MetaFont"
    [feynmp]="Feynman MetaPost"
    [pstricks]="PSTricks"
    [pstcol]="PSTricks color"
    [bbm]="Blackboard math"
    [dsfont]="Double-struck font"
    [curves]="Curves package"
    [yfonts]="Yannis fonts"
    [youngtab]="Young tableaux"
    [xypic]="XY-Pic (stub for xy)"
    [xy]="XY-Pic"
    [ecltree]="ECL tree"
    [diagrams]="Diagrams"
    [tframe]="T frame"
    [vmargin]="V margin"
    [style]="Style"
    [mydefs2e]="My definitions"
    [mysymb]="My symbols"
    [myref]="My references"
    [paper2e]="Paper 2e"
    [alpheqn]="Alpha equation"
    [Colordvi]="Color DVI"
    [cmcyralt]="CMCyr alternate"
    [lnpi]="LNPI"
    [option_keys]="Option keys"
    [masse]="Masse"
    [espcrc2]="ESPRC2"
    [supercite]="Super cite"
    [rsfs]="RSFS"
    [draftfil]="Draft file"
    [gunter]="Gunter"
    [multibox]="Multi box"
    [wrapft]="Wrap figure table"
    [footmisc]="Footnote misc"
    [version]="Version"
    [simplemargins]="Simple margins"
    [mathbb]="Math blackboard"
    [graphics]="Graphics"
    [epsbox]="EPS box"
    [eqsection]="Equation section"
)

for pkg in "${!STUBS[@]}"; do
    desc="${STUBS[$pkg]}"
    cat > "$STYDIR/${pkg}.sty" << STYEOF
\ProvidesPackage{${pkg}}[2024/01/01 stub — ${desc}]
% This is a stub package — the original journal style file is not available.
% Compilation may produce different output but should not error.
\endinput
STYEOF
    echo "  $pkg → ${pkg}.sty ($desc)"
done

echo ""
echo "=== Summary ==="
echo "Stubs installed to: $STYDIR"
ls -1 "$STYDIR"/*.sty | wc -l | xargs echo "  Total stubs:"
echo ""
echo "Run 'texhash' or 'mktexlsr' to update the TeX filename database."
echo "Then re-run pdflatex on the 2003 dataset."
