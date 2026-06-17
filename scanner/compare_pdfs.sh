#!/bin/bash
# PDF comparison harness: expand, compile, compare
# Usage: ./compare_pdfs.sh input.tex [--keep-temps]

set -uo pipefail

FILE="$1"
BASENAME="${FILE%.tex}"
SCRIPTPATH="$(cd "$(dirname "$0")" && pwd)"

STUBS_DIR="$SCRIPTPATH/stubs"
export TEXINPUTS="${STUBS_DIR}:"

TMPDIR=$(mktemp -d)
trap "rm -rf $TMPDIR" EXIT

# Filter non-ASCII first
LC_ALL=C sed 's/[\x80-\xff]//g' "$TMPDIR/orig.tex" > "$TMPDIR/orig_clean.tex"
mv "$TMPDIR/orig_clean.tex" "$TMPDIR/orig.tex"

# Detect plain TeX (uses \magnification or no \documentclass/style)
is_plain=0
if grep -q '\\magnification' "$TMPDIR/orig.tex"; then is_plain=1; fi
if ! grep -q '\\documentclass\|\\documentstyle' "$TMPDIR/orig.tex"; then is_plain=1; fi

PDFLATEX=pdflatex
if [ "$is_plain" -eq 1 ]; then PDFLATEX=pdftex; fi

echo "  Engine: $PDFLATEX"

# Expand macros and prepend catcode fix for @-commands
{ printf '\\catcode`\\@=11\\relax\n'; "$SCRIPTPATH/macro_pipeline.sh" < "$TMPDIR/orig.tex" 2>/dev/null; } > "$TMPDIR/exp.tex"
if [ ! -s "$TMPDIR/exp.tex" ]; then
  echo "  EXPAND: FAILED"
  exit 1
fi

orig_bytes=$(wc -c < "$TMPDIR/orig.tex")
exp_bytes=$(wc -c < "$TMPDIR/exp.tex")
echo "  Size: orig=$orig_bytes exp=$exp_bytes"

cd "$TMPDIR"

export max_print_line=1000
export openout_any=a

if ! $PDFLATEX -interaction=nonstopmode -halt-on-error orig.tex >/dev/null 2>&1; then
  echo "  COMPILE ORIG: FAILED"
  exit 1
fi
$PDFLATEX -interaction=nonstopmode orig.tex >/dev/null 2>&1
$PDFLATEX -interaction=nonstopmode orig.tex >/dev/null 2>&1

if ! $PDFLATEX -interaction=nonstopmode -halt-on-error exp.tex >/dev/null 2>&1; then
  echo "  COMPILE EXP: FAILED"
  exit 1
fi
$PDFLATEX -interaction=nonstopmode exp.tex >/dev/null 2>&1
$PDFLATEX -interaction=nonstopmode exp.tex >/dev/null 2>&1

orig_pages=$(pdfinfo orig.pdf 2>/dev/null | grep Pages | awk '{print $2}') || true
exp_pages=$(pdfinfo exp.pdf 2>/dev/null | grep Pages | awk '{print $2}') || true

pdftotext orig.pdf orig.txt 2>/dev/null
pdftotext exp.pdf exp.txt 2>/dev/null

diff_lines=$(diff orig.txt exp.txt 2>/dev/null | wc -l) || true

if [ "$orig_pages" = "$exp_pages" ] && [ "$diff_lines" -eq 0 ]; then
  echo "  RESULT: IDENTICAL ($orig_pages pages, $orig_bytes→$exp_bytes bytes)"
elif [ "$orig_pages" = "$exp_pages" ] && [ "$diff_lines" -le 50 ]; then
  echo "  RESULT: NEARLY IDENTICAL ($orig_pages pages, $diff_lines diff lines)"
elif [ "$orig_pages" = "$exp_pages" ]; then
  echo "  RESULT: SAME PAGES, DIFFER ($orig_pages pages, $diff_lines diff lines)"
else
  echo "  RESULT: DIFFER (orig=$orig_pages pages, exp=$exp_pages pages, $diff_lines diff lines)"
fi

if echo "$*" | grep -q -- "--keep"; then
  rm "$TMPDIR/*.log" "$TMPDIR/*.aux" "$TMPDIR/*.out" 2>/dev/null
  echo "  Files kept in: $TMPDIR"
  trap "" EXIT
fi
