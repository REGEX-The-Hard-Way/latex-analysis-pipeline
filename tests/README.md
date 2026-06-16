# Tests

## Running Tests

```bash
# Python test suite (requires scanner.out)
cd scanner && make scanner && cd ..
python3 -m pytest tests/

# Cypher 200-test suite (from within scanner/cypher/)
cd scanner/cypher
make test
```

## Test Files

| File | Description |
|------|-------------|
| `test_issues.py` | Scanner + macro expander integration tests |
| `test_macro_expander.py` | Macro expander unit tests (20+ cases) |
| `validation_tests.py` | Cross-reference validation (`\ref` → `\label`) |
| `benchmark_runner.py` | Timing and accuracy benchmarks |

## Cypher Tests

The Cypher engine has its own 200-test suite in `scanner/cypher/run_200_final.sh`.
Run with `make test` from the `scanner/cypher/` directory.

## Adding Tests

```python
def test_new_feature(self):
    with tempfile.NamedTemporaryFile(mode='w', suffix='.tex', delete=False) as f:
        f.write(r'\begin{equation} x = 1 \end{equation}')
        tex_file = f.name
    try:
        result = subprocess.run(
            ['scanner/scanner.out', tex_file, 'tex'],
            capture_output=True, text=True
        )
        self.assertEqual(result.returncode, 0)
        self.assertIn('equation', result.stderr)
    finally:
        os.unlink(tex_file)
```
