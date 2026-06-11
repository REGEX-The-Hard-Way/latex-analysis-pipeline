# Benchmarks

This directory contains performance benchmarks for the LaTeX Analysis Pipeline.

## Running Benchmarks

```bash
cd scanner
make get_sample_data
make benchmark  # Add this target to Makefile if needed
```

## Sample Results

```
time ./scanner.out 2003 tex$ 2> sidecar.tok|./sent_split.out > clean.sent
real    0m5.738s
user    0m3.178s
sys     0m2.655s
```

The benchmark processes the HEp-TH 2003 corpus (thousands of LaTeX files)
through the full pipeline.

## Benchmark Targets

| Benchmark | Description |
|-----------|-------------|
| nesting-500 | 500 nested identical environments |
| nesting-unique | 500 unique nested environments |
| parens-500 | 500 levels of nested parentheses |
| corpus-hep-th | arXiv HEp-TH 2003 corpus |

## Notes

- Benchmarks use memory-mapped I/O for consistent results
- Run on a cold cache for reproducible timing
- System: Linux, AMD64, typical desktop hardware