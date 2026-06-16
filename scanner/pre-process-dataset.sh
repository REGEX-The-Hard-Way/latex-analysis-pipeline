#!/bin/bash
# pre-process-dataset.sh — Prepare 2003 arXiv dataset for analysis.
#
# Steps:
#   0. Build required binaries (strip_non_ascii, newcommand)
#   1. Strip non-ASCII characters from all .tex files
#   2. Compare original vs cleaned — move divergent files to 2003_errors/
#   3. Expand macros on cleaned files
#   4. Tokenize with the scanner
#
# Usage: bash pre-process-dataset.sh

set -euo pipefail
cd "$(dirname "$0")/2003" || exit 1

echo "=== Step 0: Build required binaries ==="
gcc -O2 ../strip_non_ascii.c -o ../strip_non_ascii.out
gcc -O2 ../newcommand.c -o ../newcommand.out
echo "  Done"

echo "=== Step 1: Comment out missing includes/images/bibs ==="
for f in *.tex ./03*.tex; do
    [ -f "$f" ] || continue
    bash ../comment_missing.sh "$f" "$f" > /dev/null 2>&1 || true
done
echo "  Done"

echo "=== Step 2: Strip non-ASCII ==="
while read f; do
    echo "  strip: $f"
    ../strip_non_ascii.out "$f" > "${f}_cleaned"
done < <(find . -type f -regex "./03[0-9]+\.tex")

echo ""
echo "=== Step 3: Compare checksums ==="
while read f; do
    original=$(sha256sum "$f" | cut -d' ' -f1)
    cleaned=$(sha256sum "${f}_cleaned" | cut -d' ' -f1)
    if [ "$original" = "$cleaned" ]; then
        rm "${f}_cleaned"
    else
        mv "$f" ../2003_errors/
        mv "${f}_cleaned" "${f}_cleaned.tex"
    fi
done < <(find . -type f -regex "./03[0-9]+\.tex" 2>/dev/null || true)

echo ""
echo "=== Step 4: Expand macros (newcommand/def) ==="
expanded_count=0
for f in ./*_cleaned.tex; do
    [ -f "$f" ] || continue
    base="${f%.tex}"
    echo "  expand: $f → ${base}_expanded.tex"
    ../newcommand.out < "$f" > "${base}_expanded.tex" 2>/dev/null || {
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
