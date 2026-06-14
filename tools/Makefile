CC = gcc
CFLAGS = -O2 -I.

all: json_scan.o sidecar_mod cluster_find expr_tree lean4_gen def_extract.o

expr_tree: expr_tree.c json_scan.o
	$(CC) $(CFLAGS) -o $@ expr_tree.c json_scan.o

lean4_gen: lean4_gen.c
	$(CC) $(CFLAGS) -o $@ lean4_gen.c

def_extract.o: def_extract.c def_extract.h
	$(CC) $(CFLAGS) -c def_extract.c -o $@

json_scan.c: json_scan.rl json_scan.h
	ragel -m -G2 json_scan.rl

json_scan.o: json_scan.c json_scan.h
	$(CC) $(CFLAGS) -c json_scan.c -o $@

sidecar_mod: sidecar_mod.c json_scan.o
	$(CC) $(CFLAGS) -o $@ sidecar_mod.c json_scan.o

cluster_find: cluster_find.c json_scan.o
	$(CC) $(CFLAGS) -o $@ cluster_find.c json_scan.o

test: all
	bash test_tools.sh

clean:
	rm -f sidecar_mod cluster_find json_scan.o json_scan.c

.PHONY: all test clean
