CC       = gcc
CFLAGS   = -O2 -I. -Ivendor/klib
RAGEL    = ragel
SRCS     = cypher_repl.c cypher_parser.c cypher_graph.c graph_store.c graph_exec.c graph_jit.c graph_scan.c graph_text.c cypher_lexer.c
HDRS     = cypher_parser.h graph_store.h graph_exec.h graph_jit.h
TARGET   = cypher_repl.out
LDLIBS   = -lm -ldl -lreadline

all: $(TARGET)

cypher_lexer.c: cypher_lexer.rl
	$(RAGEL) -m -G2 cypher_lexer.rl


graph_scan.c: graph_scan.rl
	$(RAGEL) -m -G2 graph_scan.rl

graph_text.c: graph_text.rl
	$(RAGEL) -m -G2 graph_text.rl

$(TARGET): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $(SRCS) -o $@ $(LDLIBS)

# debug build (no optimization, symbols)
debug: CFLAGS = -O0 -g -I. -Ivendor/klib
debug: clean all

# AddressSanitizer build
asan: CFLAGS = -O2 -g -fsanitize=address -I. -Ivendor/klib
asan: clean all

# run 200-test suite
test: $(TARGET)
	@bash run_200_final.sh

# valgrind memcheck on key query types
valgrind: debug
	@echo "=== Valgrind checks ==="; \
	e=0; \
	echo "CREATE (a:A {v:10}) CREATE (b:A {v:20}) CREATE (c:A {v:30}) MATCH (n:A) RETURN COUNT(*), SUM(n.v), AVG(n.v), MIN(n.v), MAX(n.v);" | valgrind --leak-check=full --error-exitcode=1 --quiet ./$(TARGET) 2>/dev/null && echo "  PASS: aggregation" || { echo "  FAIL: aggregation"; e=1; }; \
	echo "CREATE (a:V {n:'A'}) CREATE (b:V {n:'B'}) CREATE (c:V {n:'C'}) CREATE (a)-[:X]->(b) CREATE (b)-[:X]->(c) MATCH (s:V)-[:X*2]->(t:V) WHERE s.n='A' RETURN t.n;" | valgrind --leak-check=full --error-exitcode=1 --quiet ./$(TARGET) 2>/dev/null && echo "  PASS: var-len path" || { echo "  FAIL: var-len path"; e=1; }; \
	echo "CREATE (n:I {val:10}) MATCH (n:I) RETURN CASE WHEN n.val > 5 THEN 1 ELSE 0 END;" | valgrind --leak-check=full --error-exitcode=1 --quiet ./$(TARGET) 2>/dev/null && echo "  PASS: CASE" || { echo "  FAIL: CASE"; e=1; }; \
	echo "CREATE (a:Item {name:'S'})-[:LINK]->(b:Item {name:'T'}) MATCH (src:Item)-[:LINK]->(tgt:Item) RETURN src.name;" | valgrind --leak-check=full --error-exitcode=1 --quiet ./$(TARGET) 2>/dev/null && echo "  PASS: edge expand" || { echo "  FAIL: edge expand"; e=1; }; \
	echo "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) SET a.name = 'X' DETACH DELETE b MATCH (n:Item) RETURN n.name;" | valgrind --leak-check=full --error-exitcode=1 --quiet ./$(TARGET) 2>/dev/null && echo "  PASS: SET + DETACH DELETE" || { echo "  FAIL: SET + DETACH DELETE"; e=1; }; \
	test $$e -eq 0 || { echo "Valgrind: FAILURES"; exit 1; }; \
	echo "Valgrind: ALL PASS"

# build the benchmark runner
bench_exec.out: bench_exec.c $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) bench_exec.c $(SRCS) -o $@ $(LDLIBS)

# run benchmark (needs sidecar.json in ../)
bench: bench_exec.out
	./bench_exec.out ../sidecar.json

clean:
	rm -f $(TARGET) cypher_lexer.c graph_scan.c graph_text.c *.o bench_exec.out

# build data manipulation tools
tools:
	$(MAKE) -C ../../tools all

.PHONY: all debug asan test valgrind bench tools clean
