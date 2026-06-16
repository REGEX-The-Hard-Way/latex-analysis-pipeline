#!/bin/bash
# compare_compile.sh — Batch-compare pdflatex output for original vs pre-processed.
#
# Pre-processing pipeline:
#   strip non-ASCII → comment missing includes/images/bibs → expand \\newcommand/\\def
#
# For each file in the list:
#   1. Copy original to a sandbox, compile with pdflatex (2 passes)
#   2. Pre-process copy, compile with pdflatex (2 passes)
#   3. Compare PDFs via pdftotext diff
#
# Usage: bash compare_compile.sh [--limit N] [/path/to/filelist]
# Default filelist: /tmp/compile

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SRC_DIR="$SCRIPT_DIR/2003"
WORK_DIR="/tmp/pdflatex_compare"

FILE_LIST="/tmp/compile"
LIMIT=0

while [[ $# -gt 0 ]]; do
    case "$1" in
        --limit) LIMIT="$2"; shift 2 ;;
        *) FILE_LIST="$1"; shift ;;
    esac
done

mkdir -p "$WORK_DIR"

# --- Build required tools ---
echo "=== Building tools ==="
gcc -O2 "$SCRIPT_DIR/strip_non_ascii.c" -o "$WORK_DIR/strip_non_ascii.out"
gcc -O2 "$SCRIPT_DIR/newcommand.c" -o "$WORK_DIR/newcommand.out"

SUMMARY="$WORK_DIR/summary.txt"
echo "# file | orig_ok | proc_ok | text_match | orig_pages | proc_pages | orig_size | proc_size" > "$SUMMARY"

count=0
skipped=0
failures=0

while IFS= read -r fname; do
    [ -z "$fname" ] && continue
    count=$((count + 1))
    [[ $LIMIT -gt 0 && $count -gt $LIMIT ]] && break

    # --- Resolve actual file on disk ---
    # List entries: 0301001.tex or 0301067_cleaned.tex
    # Disk files:    0301001.tex or 0301067.tex_cleaned.tex
    actual=""
    is_cleaned=0
    if [[ "$fname" == *_cleaned.tex ]]; then
        stem="${fname%_cleaned.tex}"
        for candidate in \
            "$SRC_DIR/$fname" \
            "$SRC_DIR/${stem}.tex_cleaned.tex" \
            ; do
            [ -f "$candidate" ] && { actual="$candidate"; is_cleaned=1; break; }
        done
    else
        for candidate in \
            "$SRC_DIR/$fname" \
            ; do
            [ -f "$candidate" ] && { actual="$candidate"; break; }
        done
    fi

    if [ -z "$actual" ]; then
        echo "SKIP: $fname (not found on disk)"
        skipped=$((skipped + 1))
        continue
    fi

    base="$(basename "$fname" .tex)"
    sandbox="$WORK_DIR/$base"
    rm -rf "$sandbox"
    mkdir -p "$sandbox"/{orig,proc}

    echo ""
    echo "=== [$count] $fname ==="

    # --- Copy original ---
    cp "$actual" "$sandbox/orig/${base}.tex"

    # --- Pre-process ---
    PROC_TEX="$sandbox/proc/${base}.tex"
    if [ "$is_cleaned" -eq 1 ]; then
        # Already cleaned: skip strip_non_ascii and comment_missing, just expand macros
        echo "  (already cleaned, macro-expand only)"
        "$WORK_DIR/newcommand.out" < "$actual" > "$PROC_TEX" 2>/dev/null || {
            cp "$actual" "$PROC_TEX"
        }
    else
        "$WORK_DIR/strip_non_ascii.out" "$actual" > "$sandbox/proc/_step1.tex" 2>/dev/null || true
        bash "$SCRIPT_DIR/comment_missing.sh" "$sandbox/proc/_step1.tex" "$sandbox/proc/_step2.tex" > /dev/null 2>&1 || true
        [ -f "$sandbox/proc/_step2.tex" ] || cp "$sandbox/proc/_step1.tex" "$sandbox/proc/_step2.tex"
        "$WORK_DIR/newcommand.out" < "$sandbox/proc/_step2.tex" > "$PROC_TEX" 2>/dev/null || {
            cp "$sandbox/proc/_step2.tex" "$PROC_TEX"
        }
    fi

    # --- Compile original ---
    echo -n "  orig compile: "
    orig_ok=0
    (
        cd "$sandbox/orig"
        pdflatex -interaction=nonstopmode "${base}.tex" > /dev/null 2>&1 && \
        pdflatex -interaction=nonstopmode "${base}.tex" > /dev/null 2>&1 && \
        orig_ok=1
    ) || true
    # Re-read from subshell
    [ -f "$sandbox/orig/${base}.pdf" ] && orig_ok=1
    if [ "$orig_ok" -eq 1 ]; then echo "OK"; else echo "FAIL"; fi

    # --- Compile pre-processed ---
    echo -n "  proc compile: "
    proc_ok=0
    (
        cd "$sandbox/proc"
        pdflatex -interaction=nonstopmode "${base}.tex" > /dev/null 2>&1 && \
        pdflatex -interaction=nonstopmode "${base}.tex" > /dev/null 2>&1 && \
        proc_ok=1
    ) || true
    [ -f "$sandbox/proc/${base}.pdf" ] && proc_ok=1
    if [ "$proc_ok" -eq 1 ]; then echo "OK"; else echo "FAIL"; fi

    # --- Compare ---
    text_match="N/A"
    orig_pages=0; proc_pages=0
    orig_size=0; proc_size=0

    if [ "$orig_ok" -eq 1 ] && [ "$proc_ok" -eq 1 ]; then
        pdftotext "$sandbox/orig/${base}.pdf" "$sandbox/orig/_text.txt" 2>/dev/null || true
        pdftotext "$sandbox/proc/${base}.pdf" "$sandbox/proc/_text.txt" 2>/dev/null || true

        if [ -f "$sandbox/orig/_text.txt" ] && [ -f "$sandbox/proc/_text.txt" ]; then
            if diff -q "$sandbox/orig/_text.txt" "$sandbox/proc/_text.txt" > /dev/null 2>&1; then
                text_match="yes"
            else
                text_match="DIFFER"
                failures=$((failures + 1))
            fi
        fi

        orig_pages=$(pdfinfo "$sandbox/orig/${base}.pdf" 2>/dev/null | grep '^Pages:' | awk '{print $2}' || echo 0)
        proc_pages=$(pdfinfo "$sandbox/proc/${base}.pdf" 2>/dev/null | grep '^Pages:' | awk '{print $2}' || echo 0)
        orig_size=$(stat -c%s "$sandbox/orig/${base}.pdf" 2>/dev/null || echo 0)
        proc_size=$(stat -c%s "$sandbox/proc/${base}.pdf" 2>/dev/null || echo 0)
    fi

    echo "  text_match=$text_match  orig_pages=$orig_pages  proc_pages=$proc_pages"
    echo "  orig_size=$orig_size  proc_size=$proc_size"

    echo "$base | $orig_ok | $proc_ok | $text_match | $orig_pages | $proc_pages | $orig_size | $proc_size" >> "$SUMMARY"

done < "$FILE_LIST"

echo ""
echo "============================================"
echo "Done.  $count files processed, $skipped skipped, $failures text diffs."
echo "Summary: $SUMMARY"
echo "Sandboxes: $WORK_DIR"
