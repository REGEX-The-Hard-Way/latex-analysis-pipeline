#!/bin/bash
# Pipeline driver: iterates all macro expanders until fixpoint
# Usage: ./macro_pipeline.sh < input.tex > output.tex

DIR="$(cd "$(dirname "$0")" && pwd)"
EXPANDERS="$DIR/macro_expanders"

MAX_ITER=10
TMP=$(mktemp -d)
trap "rm -rf $TMP" EXIT

cat > "$TMP/input.tex"
prev_size=$(wc -c < "$TMP/input.tex")

for i in $(seq 1 $MAX_ITER); do
  < "$TMP/input.tex" \
    "$EXPANDERS/def_expander.out" |
    "$EXPANDERS/newcommand_expander.out" |
    "$EXPANDERS/renewcommand_expander.out" |
    "$EXPANDERS/providecommand_expander.out" |
    "$EXPANDERS/edef_expander.out" > "$TMP/output.tex"

  cur_size=$(wc -c < "$TMP/output.tex")
  if [ "$cur_size" -eq "$prev_size" ] && cmp -s "$TMP/input.tex" "$TMP/output.tex"; then
    cat "$TMP/output.tex"
    exit 0
  fi
  prev_size=$cur_size
  mv "$TMP/output.tex" "$TMP/input.tex"
done

cat "$TMP/input.tex"
