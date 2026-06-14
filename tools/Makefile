CC = gcc
CFLAGS = -O2

all: sidecar_mod cluster_find

sidecar_mod: sidecar_mod.c
	$(CC) $(CFLAGS) -o $@ $<

cluster_find: cluster_find.c
	$(CC) $(CFLAGS) -o $@ $<

test: all
	@echo "=== sidecar_mod test ===" && head -5 ../scanner/sidecar.json > /tmp/test_in.json && ./sidecar_mod /tmp/test_in.json strip_latex.rules /tmp/test_out.json /tmp/test_undo.log && wc -l /tmp/test_out.json && echo "ok"
	@echo "=== cluster_find test ===" && grep '"type":"label"' ../scanner/sidecar.json > /tmp/labels.json && ./cluster_find /tmp/labels.json label | head -3 && echo "ok"

clean:
	rm -f sidecar_mod cluster_find

.PHONY: all test clean
