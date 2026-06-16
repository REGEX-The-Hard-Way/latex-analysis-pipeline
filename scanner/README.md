# Scanner Build Instructions

## Main Scanner
```bash
make scanner           # → scanner.out
make sent_split        # → sent_split.out
```

## Cypher Query Engine
```bash
cd cypher
make                   # optimized build
make debug             # debug build
make test              # run 200-test suite
make valgrind          # memory safety check
make bench             # benchmark (needs ../sidecar.json)

# Usage
./cypher_repl.out                                 # interactive REPL
./cypher_repl.out --sidecar ../sidecar.json        # load token tree
./cypher_repl.out --scan ../sidecar.json            # direct mmap scan
echo "MATCH (n:equation) RETURN n;" | ./cypher_repl.out --sidecar ../sidecar.json  # pipe mode
```

## Multi-Analyzer
```bash
cd multi_analyzer
make all               # → multi_analyzer.out
```

## Vocab Scanner
```bash
make vocab             # → vocab_scan.out
```

## Tokenize a LaTeX file
```bash
./scanner.out sound1.tex tex > sidecar.tok
./scanner.out --json /path/to/docs '.*\.tex$' > sidecar.json
```
