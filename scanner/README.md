# Scanner Build & Usage

## Build

```bash
# Main LaTeX tokenizer
make scanner           # ragel -m -G2 scanner.rl → gcc -O2 → scanner.out

# Sentence segmenter
make sent_split        # ragel -m -G2 sent_split.rl → gcc -O2 → sent_split.out

# Macro expander (standalone C, no Ragel dependency)
make macro_expander    # gcc -O2 → macro_expander.out

# Cypher query engine
make cypher_repl        # invokes cypher/Makefile → cypher_repl.out
# or: cd cypher && make
```

## Tokenize

```bash
# Single file (text output to stderr)
./scanner.out sound1.tex tex > sidecar.tok

# JSON output (one object per line, written to sidecar.json in file's directory)
./scanner.out --json sound1.tex tex

# Directory with regex pattern
./scanner.out /path/to/docs '.*\.tex$' > sidecar.tok

# JSON directory mode
./scanner.out --json /path/to/docs '.*\.tex$'
```

## Macro Expansion

```bash
# Pipe mode (stdin → stdout)
cat input.tex | ./macro_expander.out > expanded.tex

# File mode
./macro_expander.out input.tex
```

## Sentence Segmentation

```bash
cat sidecar.tok | ./sent_split.out > clean.sent
```

## Cypher Query Engine

See `cypher/` directory. Quick start:

```bash
cd cypher
make                   # optimized build
make test              # 200-test suite
make valgrind          # memory safety check
./cypher_repl.out --sidecar ../sidecar.json
```

## Get Sample Data

```bash
make get_sample_data
# Downloads hep-th-2003.tar.gz (~200MB), extracts to 2003/,
# runs strip_non_ascii and pre-process-dataset.sh
```

## File Reference

| File | Purpose |
|------|---------|
| `scanner.rl` | Main LaTeX tokenizer (Ragel state machine) |
| `latex.rl` | 150+ LaTeX environment definitions (include machine) |
| `sent_split.rl` | Sentence segmentation (Ragel scanner) |
| `main.c` | File traversal, scanner orchestration, CLI |
| `globals.h` | Shared definitions, EMIT/EMIT_BLOCK macros, g_in_math |
| `murmur3.c` | MurmurHash3 for token/filename hashing |
| `json_escape.c/h` | JSON escaping for sidecar output |
| `file_mmap.c/h` | Memory-mapped file I/O |

| `sound1.tex` | Test fixture (single equation with label) |
| `test_context.tex` | Context-aware math test |
| `2003/` | 1019 arXiv hep-th papers for benchmarking |
| `2003_errors/` | Files that failed preprocessing |
| `cypher/` | Embedded Cypher graph query engine |
| `multi_analyzer/` | Chainable text processing pipeline |
| `vocab_scan.rl` | Vocabulary scanner |
| `vocab.rl` | Vocabulary definitions |
| `strip_non_ascii.c` | Non-ASCII stripping utility |
| `pre-process-dataset.sh` | arXiv corpus preparation script |
| `expand_macros.sh` | Shell wrapper for macro expansion |
