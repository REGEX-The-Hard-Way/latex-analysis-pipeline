#!/bin/bash
# Expand macros in LaTeX files
# Usage: ./expand_macros.sh [-i] file.tex [output.tex]

if [ "$1" = "-i" ]; then
    # In-place mode
    if [ -z "$2" ]; then
        echo "Usage: $0 -i file.tex"
        exit 1
    fi
    INPUT="$2"
    TEMP="/tmp/expanded_$$$.tex"
    cat "$INPUT" | ./macro_expander.out > "$TEMP"
    mv "$TEMP" "$INPUT"
    echo "Updated: $INPUT"
else
    # Pipe mode (default)
    if [ -n "$1" ] && [ -n "$2" ]; then
        cat "$1" | ./macro_expander.out > "$2"
    else
        cat | ./macro_expander.out
    fi
fi
