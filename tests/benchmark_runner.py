#!/usr/bin/env python3
"""
Benchmark runner for LaTeX Analysis Pipeline.

This script runs benchmarks for issues:
- #28: benchmark the timing and accuracy of each option

Usage:
    python benchmark_runner.py [options]

Options:
    --accuracy    Run accuracy benchmarks
    --timing      Run timing benchmarks
    --all         Run all benchmarks (default)
"""

import subprocess
import time
import sys
import os

def run_timing_benchmark(tex_file, iterations=5):
    """Run timing benchmark on a LaTeX file."""
    times = []
    for i in range(iterations):
        start = time.time()
        result = subprocess.run(
            ['scanner/scanner.out', tex_file, 'tex'],
            capture_output=True,
            text=True
        )
        elapsed = time.time() - start
        times.append(elapsed)
        print(f"  Iteration {i+1}: {elapsed:.4f}s")
    avg = sum(times) / len(times)
    return avg

def run_accuracy_benchmark():
    """Run accuracy validation benchmark."""
    # Test various LaTeX constructs
    test_cases = [
        (r"$E = mc^2$", "math"),
        (r"\begin{equation}E = mc^2\end{equation}", "equation"),
        (r"\cite{key1,key2}", "cite"),
        (r"\label{my_label}", "label"),
        (r"\frac{x}{y}", "frac"),
    ]
    
    passed = 0
    for tex, expected_type in test_cases:
        with open('/tmp/bench_test.tex', 'w') as f:
            f.write(tex)
        
        result = subprocess.run(
            ['scanner/scanner.out', '/tmp/bench_test.tex', 'tex'],
            capture_output=True,
            text=True
        )
        
        if expected_type in result.stderr:
            passed += 1
            print(f"  ✓ {expected_type}")
        else:
            print(f"  ✗ {expected_type} (FAILED)")
    
    return passed, len(test_cases)

def main():
    print("=== LaTeX Analysis Pipeline Benchmarks ===")
    print()
    
    # Check binaries exist
    if not os.path.exists('scanner/scanner.out'):
        print("ERROR: scanner.out not found. Run 'make scanner' first.")
        return 1
    
    print("Issue #28: Benchmark timing and accuracy")
    print("-" * 40)
    
    # Accuracy benchmark
    print("\nAccuracy Benchmark:")
    passed, total = run_accuracy_benchmark()
    print(f"\nResult: {passed}/{total} tests passed")
    
    # Timing benchmark (if test file exists)
    print("\nTiming Benchmark:")
    if os.path.exists('scanner/sound1.tex'):
        avg_time = run_timing_benchmark('scanner/sound1.tex')
        print(f"\nAverage time: {avg_time:.4f}s")
    else:
        print("  No test file available (sound1.tex)")
    
    print("\n✅ Benchmark complete")
    return 0

if __name__ == '__main__':
    sys.exit(main())