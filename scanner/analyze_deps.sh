#!/bin/bash
# analyze_deps.sh — Scan 2003 dataset for pdflatex compilation blockers.
# Reports: missing images, missing includes, missing bibliographies,
#           packages used, macro counts, reference counts.
#
# Usage: bash analyze_deps.sh [sample_size]
#   sample_size: number of files to scan (default: 50)

SAMPLE="${1:-50}"
DIR="scanner/2003"

count=0
total_newcmd=0; total_labels=0; total_refs=0; total_cites=0
total_images=0; missing_img=0; missing_in=0; missing_bib=0

declare -A pkg_counts
declare -A dc_counts
declare -a blockers

echo "=== Scanning $SAMPLE files from $DIR ===" >&2

for f in "$DIR"/0301*.tex "$DIR"/0302*.tex; do
    [ -f "$f" ] || continue
    count=$((count + 1))
    [ "$count" -gt "$SAMPLE" ] && break
    base=$(basename "$f")
    dir=$(dirname "$f")

    # Extract commands (strip newlines, collapse spaces)
    docclass=$(grep -Po '\\documentclass\*?\[?[^]]*\]?\{([^}]+)\}' "$f" 2>/dev/null | grep -Po '\{[^}]+\}' | tr -d '{}' | tr '\n' ',' | sed 's/,$//')
    packages=$(grep -Po '\\usepackage\*?\[?[^]]*\]?\{([^}]+)\}' "$f" 2>/dev/null | grep -Po '\{[^}]+\}' | tr -d '{}' | tr '\n,' ' ' | xargs | tr ' ' ',')
    newcmds=$(grep -c '\\newcommand\|\\renewcommand\|\\def\|\\newenvironment' "$f" 2>/dev/null || echo 0)
    labels=$(grep -c '\\label{' "$f" 2>/dev/null || echo 0)
    refs=$(grep -c '\\ref{' "$f" 2>/dev/null || echo 0)
    cites=$(grep -c '\\cite' "$f" 2>/dev/null || echo 0)
    imgs=$(grep -Po '\\includegraphics\*?\[?[^]]*\]?\{([^}]+)\}' "$f" 2>/dev/null | grep -Po '\{[^}]+\}' | tr -d '{}')
    inputs=$(grep -Po '\\(?:include|input)\{([^}]+)\}' "$f" 2>/dev/null | grep -Po '\{[^}]+\}' | tr -d '{}')
    bibs=$(grep -Po '\\bibliography\{([^}]+)\}' "$f" 2>/dev/null | grep -Po '\{[^}]+\}' | tr -d '{}')
    bibstyle=$(grep -Po '\\bibliographystyle\{([^}]+)\}' "$f" 2>/dev/null | grep -Po '\{[^}]+\}' | tr -d '{}')

    total_newcmd=$((total_newcmd + newcmds))
    total_labels=$((total_labels + labels))
    total_refs=$((total_refs + refs))
    total_cites=$((total_cites + cites))
    total_images=$((total_images + $(echo "$imgs" | grep -c . 2>/dev/null || echo 0)))

    mi=""; for img in $imgs; do
        found=0
        for try in "$dir/$img" "$dir/${img}.eps" "$dir/${img}.pdf" "$dir/${img}.ps" "$dir/${img}.jpg" "$dir/${img}.png"; do
            [ -f "$try" ] && found=1 && break
        done
        [ $found -eq 0 ] && mi="$mi $img"
    done
    [ -n "$mi" ] && missing_img=$((missing_img + 1)) && blockers+=("$base: MISSING_IMAGE: $mi")

    mn=""; for inc in $inputs; do
        found=0
        for try in "$dir/${inc}.tex" "$dir/$inc"; do
            [ -f "$try" ] && found=1 && break
        done
        [ $found -eq 0 ] && mn="$mn $inc"
    done
    [ -n "$mn" ] && missing_in=$((missing_in + 1)) && blockers+=("$base: MISSING_INPUT: $mn")

    mb=""; for bib in $bibs; do
        [ ! -f "$dir/${bib}.bib" ] && mb="$mb $bib"
    done
    [ -n "$mb" ] && missing_bib=$((missing_bib + 1)) && blockers+=("$base: MISSING_BIB: $mb")

    # Package counts
    for p in $(echo "$packages" | tr ',' ' '); do
        [ -n "$p" ] && pkg_counts[$p]=$((${pkg_counts[$p]:-0} + 1))
    done
    [ -n "$docclass" ] && dc_counts[$docclass]=$((${dc_counts[$docclass]:-0} + 1))
done

echo ""
echo "========================================="
echo "  Compilation Blocker Analysis"
echo "  $count files scanned"
echo "========================================="
echo ""
echo "=== MACRO COUNTS ==="
echo "  \\newcommand / \\def / \\newenvironment:  $total_newcmd"
echo "  \\label:                                 $total_labels"
echo "  \\ref:                                   $total_refs"
echo "  \\cite:                                  $total_cites"
echo "  \\includegraphics:                       $total_images"
echo ""
echo "=== COMPILATION BLOCKERS ==="
echo "  Files with missing images:              $missing_img / $count"
echo "  Files with missing \\include/\\input:     $missing_in / $count"
echo "  Files with missing .bib:                $missing_bib / $count"
echo ""
echo "=== TOP PACKAGES ==="
for p in $(for k in "${!pkg_counts[@]}"; do echo "${pkg_counts[$k]} $k"; done | sort -rn | head -20); do
    cnt="${p%% *}"; name="${p#* }"
    printf "  %-35s %s\n" "$name" "$cnt"
done
echo ""
echo "=== DOCUMENT CLASSES ==="
for d in $(for k in "${!dc_counts[@]}"; do echo "${dc_counts[$k]} $k"; done | sort -rn); do
    cnt="${d%% *}"; name="${d#* }"
    printf "  %-20s %s\n" "$name" "$cnt"
done
echo ""
echo "=== BLOCKERS (top 20) ==="
for b in "${blockers[@]:0:20}"; do
    echo "  $b"
done
echo ""
echo "=== IMPLICATIONS FOR MACRO EXPANSION ==="
echo "  Files to preprocess: $count"
echo "  Total macro definitions: $total_newcmd"
echo "  Files with \\bibliography (needs .bib): $(grep -rl '\\bibliography{' "$DIR"/0301*.tex "$DIR"/0302*.tex 2>/dev/null | head -"$count" | wc -l)"
echo "  Files with \\includegraphics (needs images): $(grep -rl '\\includegraphics' "$DIR"/0301*.tex "$DIR"/0302*.tex 2>/dev/null | head -"$count" | wc -l)"
echo ""
echo "NOTE: pdflatex not available — image/bib/include paths checked against filesystem."
echo "To verify macro expansion, compare scanner output vs expected expanded text."
