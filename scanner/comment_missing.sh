#!/bin/bash
# comment_missing.sh — Comment out \includegraphics, \include, \input,
# and \bibliography commands that reference non-existent files.
#
# Usage: bash comment_missing.sh <input.tex> [output.tex]

set -euo pipefail

INPUT="$1"
OUTPUT="${2:-${INPUT%.tex}_compilable.tex}"
DIR="$(dirname "$(realpath "$INPUT")")"

[ -f "$INPUT" ] || { echo "ERROR: $INPUT not found"; exit 1; }

# Read file into array for line-based editing
mapfile -t lines < "$INPUT"
changes=0

# ---- Missing images ----
echo "=== Checking \\includegraphics ===" >&2
for i in "${!lines[@]}"; do
    line="${lines[$i]}"
    [[ "$line" == *'\includegraphics'* ]] || continue

    # Extract filename: \includegraphics[...]{file}
    file=$(echo "$line" | grep -Po '\\includegraphics\*?(?:\[[^]]*\])?\{([^}]+)\}' | grep -Po '\{[^}]+\}' | tr -d '{}')
    [ -z "$file" ] && continue

    found=0
    for ext in '' '.eps' '.pdf' '.ps' '.jpg' '.png' '.jpeg'; do
        [ -f "$DIR/${file}${ext}" ] && found=1 && break
    done

    if [ $found -eq 0 ]; then
        lines[$i]="% MISSING_IMAGE: ${line}"
        echo "  COMMENTED: $file (line $((i+1)))" >&2
        changes=$((changes + 1))
    fi
done

# ---- Missing \include/\input ----
echo "=== Checking \\include / \\input ===" >&2
for i in "${!lines[@]}"; do
    line="${lines[$i]}"
    [[ "$line" == *'\include{'* || "$line" == *'\input{'* ]] || continue

    file=$(echo "$line" | grep -Po '\\(?:include|input)\{([^}]+)\}' | grep -Po '\{[^}]+\}' | tr -d '{}')
    [ -z "$file" ] && continue

    found=0
    for try in "$DIR/${file}.tex" "$DIR/$file"; do
        [ -f "$try" ] && found=1 && break
    done

    if [ $found -eq 0 ]; then
        lines[$i]="% MISSING_INPUT: ${line}"
        echo "  COMMENTED: $file (line $((i+1)))" >&2
        changes=$((changes + 1))
    fi
done

# ---- Missing \bibliography ----
echo "=== Checking \\bibliography ===" >&2
for i in "${!lines[@]}"; do
    line="${lines[$i]}"
    [[ "$line" == *'\bibliography{'* ]] || continue

    file=$(echo "$line" | grep -Po '\\bibliography\{([^}]+)\}' | grep -Po '\{[^}]+\}' | tr -d '{}')
    [ -z "$file" ] && continue

    found=1
    for bib in $(echo "$file" | tr ',' ' '); do
        [ -f "$DIR/${bib}.bib" ] || { found=0; break; }
    done

    if [ $found -eq 0 ]; then
        lines[$i]="% MISSING_BIB: ${line}"
        echo "  COMMENTED: $file (line $((i+1)))" >&2
        changes=$((changes + 1))
    fi
done

# Write output
printf '%s\n' "${lines[@]}" > "$OUTPUT"
echo "" >&2
echo "Changes: $changes lines commented → $OUTPUT" >&2
echo "$OUTPUT"
