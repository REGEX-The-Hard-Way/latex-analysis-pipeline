CC=gcc
CFLAGS=-O2 -I.
RAGEL=ragel

all: cypher_repl.out

cypher_lexer.c: cypher_lexer.rl
	$(RAGEL) -m -G2 cypher_lexer.rl

cypher_repl.out: cypher_repl.c cypher_parser.c cypher_graph.c graph_store.c cypher_lexer.c cypher_parser.h graph_store.h
	$(CC) $(CFLAGS) cypher_repl.c cypher_parser.c cypher_graph.c graph_store.c cypher_lexer.c -o cypher_repl.out -lm

test: cypher_repl.out
	@bash smoke_tests.sh

clean:
	rm -f cypher_repl.out cypher_lexer.c
