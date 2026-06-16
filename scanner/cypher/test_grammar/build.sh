#!/usr/bin/env bash
# build.sh – compile the Ragel Cypher tokenizer and parser
#
# Requirements:
#   ragel >= 6.x  (http://www.colm.net/open-source/ragel/)
#   cc / gcc / clang
#
# Usage:
#   chmod +x build.sh && ./build.sh
#   ./cypher_parser 'MATCH (n:Person) WHERE n.age > 30 RETURN n.name'
#
set -euo pipefail

# ── Step 1: generate C from the lexer Ragel source ─────────────────
# -T1  → table-driven backend (compatible with all ragel versions)
echo "[1/4] ragel → cypher_lexer.c"
ragel -C -T1 cypher_lexer.rl -o cypher_lexer.c

# ── Step 2: generate C from the parser Ragel source ─────────────────
# -G2  → goto-driven backend (faster, fine here since the machine is tiny)
echo "[2/4] ragel → cypher_parser.c"
ragel -C -G2 cypher_parser.rl -o cypher_parser.c

# ── Step 3: compile the lexer (no main) ─────────────────────────────
echo "[3/4] cc cypher_lexer.c"
cc -O2 -Wall -Wextra -std=c11 \
   -DCYPHER_LEXER_MAIN=0 \
   -c cypher_lexer.c -o cypher_lexer.o

# ── Step 4: compile & link the parser (provides main) ───────────────
echo "[4/4] cc cypher_parser.c + link"
cc -O2 -Wall -Wextra -std=c11 \
   cypher_parser.c cypher_lexer.o \
   -o cypher_parser

echo ""
echo "Build successful!  Try:"
echo "  ./cypher_parser 'MATCH (n:Person)-[:KNOWS]->(m) RETURN n, m'"
echo "  ./cypher_parser 'CREATE (a:Actor {name:\"Neo\"})-[:ACTED_IN]->(b:Movie)'"
