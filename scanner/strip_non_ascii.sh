#!/bin/bash
# Filter: strip non-printable/non-ASCII bytes from stdin or file
# Usage: ./strip_non_ascii.sh < input.tex > output.tex
#        ./strip_non_ascii.sh file.tex > output.tex

if [ $# -ge 1 ]; then
  LC_ALL=C sed 's/[\x80-\xff]//g' "$@"
else
  LC_ALL=C sed 's/[\x80-\xff]//g'
fi
