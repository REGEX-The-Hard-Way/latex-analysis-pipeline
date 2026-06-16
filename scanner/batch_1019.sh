#!/bin/bash
# batch_1019.sh — Process all 1019 files: comment → expand → pdflatex → categorise.
# Outputs one JSON line per file to /tmp/pdfs/summary.jsonl

set -euo pipefail
DIR="$(dirname "$0")/2003"
OUT="/tmp/pdfs"
mkdir -p "$OUT"

MACRO="./macro_expander.out"
COMMENT="./comment_missing.sh"
SUMMARY="$OUT/summary.jsonl"
: > "$SUMMARY"

total=0
t0=$SECONDS

for f in "$DIR"/03*.tex; do
    base=$(basename "$f" .tex)
    total=$((total + 1))
    cat="" state="start" pdf_pages=0 macros_before=0 macros_after=0 errs=0

    # ---- Comment missing deps ----
    cleaned="$OUT/${base}.tex"
    bash "$COMMENT" "$f" "$cleaned" >/dev/null 2>&1 || true
    if [ ! -f "$cleaned" ]; then
        state="comment_failed"
        cat="comment_failed"
        printf '{"f":"%s","s":"%s","c":"%s","p":%d,"mb":%d,"ma":%d,"e":%d}\n' \
            "$base" "$state" "$cat" "$pdf_pages" "$macros_before" "$macros_after" "$errs" >> "$SUMMARY"
        continue
    fi

    # ---- Macro expand ----
    expanded="$OUT/${base}_exp.tex"
    macros_before=$(grep -c '\\newcommand\|\\renewcommand\|\\def\|\\newenvironment' "$cleaned" 2>/dev/null | tr '\n' ' ' | awk '{print $1}'); macros_before=${macros_before:-0}
    timeout 30 "$MACRO" < "$cleaned" > "$expanded" 2>/dev/null || true
    if [ ! -s "$expanded" ]; then
        state="expand_fail"
        cat="expand_fail"
        printf '{"f":"%s","s":"%s","c":"%s","p":%d,"mb":%d,"ma":%d,"e":%d}\n' \
            "$base" "$state" "$cat" "$pdf_pages" "$macros_before" 0 0 >> "$SUMMARY"
        continue
    fi
    macros_after=$(grep -c '\\newcommand\|\\renewcommand\|\\def\|\\newenvironment' "$expanded" 2>/dev/null | tr '\n' ' ' | awk '{print $1}'); macros_after=${macros_after:-0}

    # ---- pdflatex ----
    log="$OUT/${base}.log"
    timeout 45 pdflatex -interaction=nonstopmode -output-directory "$OUT" "$expanded" > "$log" 2>&1 || true

    pdf="$OUT/${base}_exp.pdf"
    if [ -f "$pdf" ]; then
        pdf_pages=$(pdfinfo "$pdf" 2>/dev/null | grep 'Pages:' | awk '{print $2}' || echo 0)
        errs=$(grep -c '^!' "$log" 2>/dev/null | tr '\n' ' ' | awk '{print $1}'); errs=${errs:-0}
    else
        errs=$(grep -c '^!' "$log" 2>/dev/null | tr '\n' ' ' | awk '{print $1}'); errs=${errs:-0}
    fi

    # ---- Categorise ----
    if [ -f "$pdf" ] && [ "$errs" -eq 0 ] 2>/dev/null; then
        cat="clean"
    elif [ -f "$pdf" ] && [ "${errs:-0}" -gt 0 ] 2>/dev/null; then
        cat="pdf_with_errors"
    elif grep -q 'Missing \\begin{document}' "$log" 2>/dev/null; then
        cat="missing_begin_document"
    elif grep -q 'Two \\documentclass' "$log" 2>/dev/null; then
        cat="duplicate_documentclass"  
    elif grep -qE "File \`.+\.sty' not found" "$log" 2>/dev/null; then
        cat="missing_sty"
    elif grep -qE "File \`.+' not found" "$log" 2>/dev/null; then
        cat="missing_file"
    elif grep -q 'Undefined control sequence' "$log" 2>/dev/null; then
        cat="undefined_command"
    elif grep -qE 'Missing [{}] inserted' "$log" 2>/dev/null; then
        cat="unmatched_braces"
    else
        cat="other_error"
    fi
    state="done"

    printf '{"f":"%s","s":"%s","c":"%s","p":%s,"mb":%s,"ma":%s,"e":%s}\n' \
        "$base" "$state" "$cat" "$pdf_pages" "$macros_before" "$macros_after" "$errs" \
        >> "$SUMMARY"

    if [ $((total % 100)) -eq 0 ]; then
        elapsed=$((SECONDS - t0))
        echo "  [$total/1019] ${elapsed}s  last: $base -> $cat (pdf=$pdf_pages mb=$macros_before->$macros_after)" 
    fi
done

elapsed=$((SECONDS - t0))
echo ""
echo "Done: $total files in ${elapsed}s"
echo "Summary: $SUMMARY"

# Quick stats
python3 -c "
import json
cats = {}; p_ok = 0; mb = 0; ma = 0
for line in open('$SUMMARY'):
    r = json.loads(line)
    c = r['c']; cats[c] = cats.get(c,0)+1
    if r['p'] > 0: p_ok += 1
    mb += r.get('mb',0); ma += r.get('ma',0)
print()
for c,n in sorted(cats.items(), key=lambda x:-x[1]):
    print(f'  {c:30s} {n:5d}')
print(f'  {\"PDF produced\":30s} {p_ok:5d}')
print(f'  {\"Macros before\":30s} {mb:5d}')
print(f'  {\"Macros after\":30s} {ma:5d}')
"
