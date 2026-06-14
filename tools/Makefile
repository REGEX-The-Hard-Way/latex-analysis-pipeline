CC = gcc
CFLAGS = -O2 -I.

all: json_scan.o sidecar_mod cluster_find

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
