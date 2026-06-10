#!/bin/bash

# Build script for Ragel-based multi_analyzer pipeline
# This script builds the chainable text processing pipeline independently

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/multi_analyzer"

cd "${BUILD_DIR}"

echo "=========================================="
echo "Building Ragel-based multi_analyzer pipeline"
echo "=========================================="
echo ""

# Check if ragel is installed
if ! command -v ragel &> /dev/null; then
    echo "ERROR: Ragel compiler not found."
    echo "Please install Ragel (e.g., 'sudo apt-get install ragel' on Ubuntu)"
    exit 1
fi

echo "Ragel compiler found: $(which ragel)"
echo ""

# Check if gcc is installed
if ! command -v gcc &> /dev/null; then
    echo "ERROR: GCC compiler not found."
    exit 1
fi

echo "GCC compiler found: $(which gcc)"
echo ""

# Build
echo "Building..."
make clean 2>/dev/null || true
make

echo ""
echo "=========================================="
echo "Build successful!"
echo "=========================================="
echo ""
echo "Executable: ${BUILD_DIR}/multi_analyzer.out"
echo ""
echo "Usage:"
echo "  cat input.txt | ./multi_analyzer.out --tokenize --lower --stem"
echo ""
echo "Options:"
echo "  --tokenize  Tokenize input (split on whitespace)"
echo "  --lower     Normalize (lowercase + strip punctuation)"
echo "  --stem      Apply Porter2 stemming"
echo ""
echo "Example:"
echo "  echo 'Running and jumping' | ./multi_analyzer.out --tokenize --lower --stem"
echo "  # Output: run jump"
echo ""
