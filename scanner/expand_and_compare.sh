#!/bin/bash
# expand_and_compare.sh — Macro-expand files and compare pdflatex outputs.
# For each .tex file in 2003/:
#   1. Comment out missing includes/images/bibs
#   2. pdflatex original → original.pdf
#   3. Macro-expand → expanded.tex
#   4. pdflatex expanded → expanded.pdf
#   5. Compare PDFs (page count, text extraction)
#
# Usage: bash expand_and_compare.sh [count] [start_pattern]
#   count: number of files to process (default: 10, 0 = all)
#   start_pattern: file glob to start from (default: 0301)

set -euo pipefail

COUNT="${1:-10}"
PATTERN="${2:-0301}"
DIR="$(dirname "$0")/2003"
OUTDIR="/tmp/pdflatex_compare"
mkdir -p "$OUTDIR"
WORKDIR="$OUTDIR/work"
mkdir -p "$WORKDIR"

cd "$(dirname "$0")"

processed=0
expanded_ok=0
compile_orig_ok=0
compile_exp_ok=0
compare_ok=0

echo "=== Macro Expansion + pdflatex Comparison ==="
echo "Files to process: $COUNT (pattern: $PATTERN*)"
echo "Working directory: $WORKDIR"
echo ""

for f in "$DIR/${PATTERN}"*.tex; do
    [ -f "$f" ] || continue
    [ "$COUNT" -gt 0 ] && [ "$processed" -ge "$COUNT" ] && break
    processed=$((processed + 1))
    base=$(basename "$f" .tex)
    echo "[$processed] $base"

    # Copy to workdir
    cp "$f" "$WORKDIR/${base}_orig.tex"

    # Comment out missing deps on copy
    bash comment_missing.sh "$WORKDIR/${base}_orig.tex" "$WORKDIR/${base}_orig.tex" > /dev/null 2>&1 || true

    # ---- pdflatex on original ----
    if pdflatex -interaction=nonstopmode -output-directory "$WORKDIR" "$WORKDIR/${base}_orig.tex" > "$WORKDIR/${base}_orig.log" 2>&1; then
        if [ -f "$WORKDIR/${base}_orig.pdf" ]; then
            compile_orig_ok=$((compile_orig_ok + 1))
            orig_pages=$(pdfinfo "$WORKDIR/${base}_orig.pdf" 2>/dev/null | grep 'Pages:' | awk '{print $2}')
            echo "  orig:     ✓ PDF ($orig_pages pages)"
        else
            echo "  orig:     ✗ no PDF produced"
        fi
    else
        err=$(grep -c 'Error\|Fatal' "$WORKDIR/${base}_orig.log" 2>/dev/null || echo 0)
        echo "  orig:     ✗ pdflatex errors: $err"
    fi

    # ---- Macro expansion ----
    if ./macro_expander.out < "$WORKDIR/${base}_orig.tex" > "$WORKDIR/${base}_exp.tex" 2>/dev/null; then
        macros_before=$(grep -c '\\newcommand\|\\def\|\\renewcommand' "$WORKDIR/${base}_orig.tex" 2>/dev/null || echo 0)
        macros_after=$(grep -c '\\newcommand\|\\def\|\\renewcommand' "$WORKDIR/${base}_exp.tex" 2>/dev/null || echo 0)
        expanded_ok=$((expanded_ok + 1))
        echo "  expand:   ✓ macros: $macros_before → $macros_after"
    else
        echo "  expand:   ✗ failed"
        continue
    fi

    # ---- pdflatex on expanded ----
    if pdflatex -interaction=nonstopmode -output-directory "$WORKDIR" "$WORKDIR/${base}_exp.tex" > "$WORKDIR/${base}_exp.log" 2>&1; then
        if [ -f "$WORKDIR/${base}_exp.pdf" ]; then
            compile_exp_ok=$((compile_exp_ok + 1))
            exp_pages=$(pdfinfo "$WORKDIR/${base}_exp.pdf" 2>/dev/null | grep 'Pages:' | awk '{print $2}')
            echo "  expanded: ✓ PDF ($exp_pages pages)"

            # ---- Compare ----
            if [ -f "$WORKDIR/${base}_orig.pdf" ] && [ "$orig_pages" = "$exp_pages" ]; then
                # Extract text and compare
                pdftotext "$WORKDIR/${base}_orig.pdf" "$WORKDIR/${base}_orig.txt" 2>/dev/null
                pdftotext "$WORKDIR/${base}_exp.pdf" "$WORKDIR/${base}_exp.txt" 2>/dev/null
                orig_lines=$(wc -l < "$WORKDIR/${base}_orig.txt" 2>/dev/null || echo 0)
                exp_lines=$(wc -l < "$WORKDIR/${base}_exp.txt" 2>/dev/null || echo 0)
                diff_lines=$(diff "$WORKDIR/${base}_orig.txt" "$WORKDIR/${base}_exp.txt" 2>/dev/null | wc -l || echo 0)

                if [ "$diff_lines" -eq 0 ]; then
                    compare_ok=$((compare_ok + 1))
                    echo "  compare:  ✓ IDENTICAL ($orig_lines lines)"
                elif [ "$diff_lines" -lt 100 ]; then
                    echo "  compare:  ~ similar ($diff_lines diff lines / $orig_lines total)"
                else
                    echo "  compare:  ✗ DIFFER ($diff_lines diff lines / $orig_lines total)"
                fi
            elif [ "$orig_pages" != "$exp_pages" ]; then
                echo "  compare:  ✗ page count mismatch ($orig_pages vs $exp_pages)"
            fi
        else
            echo "  expanded: ✗ no PDF produced"
        fi
    else
        err=$(grep -c 'Error\|Fatal' "$WORKDIR/${base}_exp.log" 2>/dev/null || echo 0)
        echo "  expanded: ✗ pdflatex errors: $err"
    fi

    echo ""
done

echo "========================================="
echo "  Results for $processed files"
echo "========================================="
echo "  Macro expansion successful:    $expanded_ok / $processed"
echo "  pdflatex original OK:          $compile_orig_ok / $processed"
echo "  pdflatex expanded OK:          $compile_exp_ok / $processed"
echo "  Identical output:              $compare_ok / $processed"
echo ""
echo "  Working directory: $WORKDIR"
echo "  PDFs: $WORKDIR/*.pdf"
echo "  Logs: $WORKDIR/*.log"
