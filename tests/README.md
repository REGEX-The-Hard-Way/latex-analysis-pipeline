# Tests

This directory contains test cases for the LaTeX Analysis Pipeline.

## Running Tests

```bash
# Build the scanner first
cd scanner
make scanner

# Run tests
python3 -m pytest tests/
# or
python3 tests/test_issues.py
```

## Test Categories

### test_issues.py
Tests derived from GitHub issues in `issues_list.json`:

| Test Class | Issue Reference | Description |
|------------|-----------------|-------------|
| `TestScannerBinary` | General | Basic scanner functionality |
| `TestMathDetection` | #8 | Math equation detection accuracy |
| `TestMacroExpansion` | #10 | LaTeX macro expansion |
| `TestBibliographyTokens` | #11 | Bibliography entry tokenization |
| `TestSentenceSplit` | General | Sentence splitting utility |

### benchmark_runner.py
Benchmarks for issue #28:
- Timing benchmarks for scanner performance
- Accuracy benchmarks for token detection

### validation_tests.py
Bibliography validation for issue #11:
- Reference/label matching
- Citation validation

## Adding New Tests

1. Create a new test file or add to existing file
2. Use temporary files for LaTeX input
3. Run scanner via subprocess
4. Assert expected output in stderr (for .tok format) or stdout

## Sample Test Template

```python
def test_new_feature(self):
    """Test description here."""
    with tempfile.NamedTemporaryFile(mode='w', suffix='.tex', delete=False) as f:
        f.write(r'Your LaTeX content here')
        tex_file = f.name
    
    try:
        result = subprocess.run(
            ['scanner/scanner.out', tex_file, 'tex'],
            capture_output=True,
            text=True
        )
        self.assertEqual(result.returncode, 0)
        self.assertIn('expected_token', result.stderr)
    finally:
        os.unlink(tex_file)
```

## Dependencies

Tests require:
- scanner/scanner.out binary (built via `make scanner`)
- scanner/sent_split.out binary (built via `make sent_split`)
- Python 3.x with unittest