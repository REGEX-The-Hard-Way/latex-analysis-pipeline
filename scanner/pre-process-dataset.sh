#!/bin/bash
# pre-process-dataset.sh — Prepare 2003 arXiv dataset for analysis.
#
# Steps:
#   0. Build required binaries (strip_non_ascii, macro_expanders, scanner)
#   1. Comment out missing includes/images/bibs
#   2. Strip non-ASCII characters from all .tex files
#   3. Compare original vs cleaned — move divergent files to 2003_errors/
#   4. Expand macros using the macro pipeline
#   5. Tokenize with the scanner
#
# Usage: bash pre-process-dataset.sh

set -euo pipefail
cd "$(dirname "$0")/2003" || exit 1

echo "=== Step 0: Build required binaries ==="
gcc -O2 ../strip_non_ascii.c -o ../strip_non_ascii.out
make -C .. -s macro_expanders 2>&1
make -C .. -s scanner 2>&1 || echo "  WARNING: scanner build failed (step 5 will skip)"
echo "  Done"

echo "=== Step 1: Comment out missing includes/images/bibs ==="
for f in *.tex; do
    [ -f "$f" ] || continue
    bash ../comment_missing.sh "$f" "$f" > /dev/null 2>&1 || true
done
echo "  Done"

echo "=== Step 2: Strip non-ASCII ==="
while read f; do
    echo "  strip: $f"
    ../strip_non_ascii.out "$f" > "${f%.tex}_cleaned.tex"
done < <(find . -type f -regex "./03[0-9]+\.tex")

echo ""
echo "=== Step 3: Compare checksums ==="
mkdir -p ../2003_errors
while read f; do
    cleaned_f="${f%.tex}_cleaned.tex"
    original_sha=$(sha256sum "$f" | cut -d' ' -f1)
    cleaned_sha=$(sha256sum "$cleaned_f" | cut -d' ' -f1)
    if [ "$original_sha" = "$cleaned_sha" ]; then
        rm "$cleaned_f"
    else
        mv "$f" ../2003_errors/
    fi
done < <(find . -type f -regex "./03[0-9]+\.tex" 2>/dev/null || true)

echo ""
echo "=== Step 4: Expand macros (macro_pipeline) ==="
expanded_count=0
for f in ./*_cleaned.tex; do
    [ -f "$f" ] || continue
    base="${f%.tex}"
    echo "  expand: $f → ${base}_expanded.tex"
    ../macro_pipeline.sh < "$f" > "${base}_expanded.tex" 2>/dev/null || {
        echo "    WARNING: macro expansion failed for $f"
        continue
    }
    expanded_count=$((expanded_count + 1))
done
echo "  Expanded $expanded_count files"

echo ""
echo "=== Step 5: Tokenize with scanner ==="
for f in ./*_expanded.tex; do
    [ -f "$f" ] || continue
    base=$(basename "$f" _expanded.tex)
    echo "  scan: $base"
    ../scanner.out "$f" tex > "../${base}.tok" 2>/dev/null || {
        echo "    WARNING: scan failed for $f"
    }
done

echo ""
echo "=== Done ==="
echo "  Token files: ../*.tok"
echo "  Expanded files: 2003/*_expanded.tex"
echo "  Original divergent: 2003_errors/"
