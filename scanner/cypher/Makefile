CC       = gcc
CFLAGS   = -O2 -I. -Ivendor/klib
RAGEL    = ragel
SRCS     = cypher_repl.c cypher_parser.c cypher_graph.c graph_store.c graph_exec.c graph_jit.c graph_scan.c cypher_lexer.c
HDRS     = cypher_parser.h graph_store.h graph_exec.h graph_jit.h
TARGET   = cypher_repl.out
LDLIBS   = -lm -ldl -lreadline

all: $(TARGET)

cypher_lexer.c: cypher_lexer.rl
	$(RAGEL) -m -G2 cypher_lexer.rl

$(TARGET): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $(SRCS) -o $@ $(LDLIBS)

# debug build (no optimization, symbols)
debug: CFLAGS = -O0 -g -I. -Ivendor/klib
debug: clean all

# AddressSanitizer build  
asan: CFLAGS = -O2 -g -fsanitize=address -I. -Ivendor/klib
asan: clean all

# run smoke tests (15 queries)
test: $(TARGET)
	@bash smoke_tests.sh

# valgrind memcheck on smoke tests
valgrind: debug
	@echo "=== Valgrind smoke tests ==="; \
	r=0; \
	echo "CREATE (n:Paper {title:'Test', year:2024}) RETURN n.title, n.year;" | valgrind --leak-check=full --show-reachable=no --error-exitcode=1 --quiet ./$(TARGET) 2>&1 || r=1; \
	if [ $$r -eq 0 ]; then echo "  PASS: CREATE + RETURN"; else echo "  FAIL: CREATE + RETURN"; fi; \
	r=0; \
	echo "CREATE (a:Item {name:'C', val:3}) CREATE (b:Item {name:'A', val:1}) MATCH (n:Item) RETURN n.name, n.val ORDER BY n.val LIMIT 2;" | valgrind --leak-check=full --show-reachable=no --error-exitcode=1 --quiet ./$(TARGET) 2>&1 || r=1; \
	if [ $$r -eq 0 ]; then echo "  PASS: ORDER BY + LIMIT"; else echo "  FAIL: ORDER BY + LIMIT"; fi; \
	r=0; \
	echo "CREATE (a:Item {name:'S'}) CREATE (b:Item {name:'T'}) CREATE (a)-[:CITES]->(b) MATCH (src:Item)-[:CITES]->(tgt:Item) RETURN src.name, tgt.name;" | valgrind --leak-check=full --show-reachable=no --error-exitcode=1 --quiet ./$(TARGET) 2>&1 || r=1; \
	if [ $$r -eq 0 ]; then echo "  PASS: edge expand"; else echo "  FAIL: edge expand"; fi; \
	r=0; \
	echo "CREATE (a:Item {name:'A'}) CREATE (b:Item {name:'B'}) SET a.name = 'X' DETACH DELETE b MATCH (n:Item) RETURN n.name;" | valgrind --leak-check=full --show-reachable=no --error-exitcode=1 --quiet ./$(TARGET) 2>&1 || r=1; \
	if [ $$r -eq 0 ]; then echo "  PASS: SET + DETACH DELETE"; else echo "  FAIL: SET + DETACH DELETE"; fi

# build the benchmark runner
bench_exec.out: bench_exec.c $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) bench_exec.c $(SRCS) -o $@ $(LDLIBS)

# run benchmark (needs sidecar-small.json in ../)
bench: bench_exec.out
	./bench_exec.out ../sidecar-small.json

clean:
	rm -f $(TARGET) cypher_lexer.c *.o bench_exec.out

.PHONY: all debug asan test valgrind bench clean
