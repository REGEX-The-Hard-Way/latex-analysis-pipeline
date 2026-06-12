"""
Test cases for LaTeX Analysis Pipeline.

This module contains test cases derived from issues in the upstream
allofphysicsgraph/latex-in-arxiv repository.
"""

import unittest
import subprocess
import tempfile
import os


class TestScannerBinary(unittest.TestCase):
    """Test the scanner binary functionality."""

    @classmethod
    def setUpClass(cls):
        """Check if scanner.out exists."""
        cls.scanner_path = os.path.abspath('scanner/scanner.out')
        cls.scanner_dir = os.path.dirname(cls.scanner_path)
        cls.scanner_dir = os.path.dirname(cls.scanner_path)
        if not os.path.exists(cls.scanner_path):
            raise unittest.SkipTest("scanner.out not built")

    def test_basic_tokenization(self):
        """Test basic tokenization of simple LaTeX."""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.tex', delete=False) as f:
            f.write(r'\documentclass{article}\begin{document}Hello\end{document}')
            tex_file = f.name

        try:
            result = subprocess.run(
                [self.scanner_path, tex_file, 'tex'],
                capture_output=True,
                text=True,
                cwd=self.scanner_dir
            )
            self.assertEqual(result.returncode, 0)
            self.assertTrue(os.path.exists(os.path.join(self.scanner_dir, 'sidecar.tok')))
        finally:
            os.unlink(tex_file)


class TestMathDetection(unittest.TestCase):
    """Test cases for Issue #8: Measure accuracy of finding math equations."""

    @classmethod
    def setUpClass(cls):
        """Check if scanner.out exists."""
        cls.scanner_path = os.path.abspath('scanner/scanner.out')
        cls.scanner_dir = os.path.dirname(cls.scanner_path)
        if not os.path.exists(cls.scanner_path):
            raise unittest.SkipTest("scanner.out not built")

    def test_inline_math_detection(self):
        """Test detection of $...$ inline math."""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.tex', delete=False) as f:
            f.write(r'Text with $E = mc^2$ inline.')
            tex_file = f.name

        try:
            result = subprocess.run(
                [self.scanner_path, tex_file, 'tex'],
                capture_output=True,
                text=True,
                cwd=self.scanner_dir
            )
            self.assertTrue(os.path.exists(os.path.join(self.scanner_dir, 'sidecar.tok')))
        finally:
            os.unlink(tex_file)

    def test_display_math_detection(self):
        """Test detection of equation environments."""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.tex', delete=False) as f:
            f.write(r'\begin{equation}E = mc^2\end{equation}')
            tex_file = f.name

        try:
            result = subprocess.run(
                [self.scanner_path, tex_file, 'tex'],
                capture_output=True,
                text=True,
                cwd=self.scanner_dir
            )
            self.assertTrue(os.path.exists(os.path.join(self.scanner_dir, 'sidecar.tok')))
        finally:
            os.unlink(tex_file)


class TestMacroExpansion(unittest.TestCase):
    """Test cases for Issue #10: Expand LaTeX macros."""

    @classmethod
    def setUpClass(cls):
        """Check if macro_expander.out exists."""
        cls.expander_path = './macro_expander.out'
        if not os.path.exists(cls.expander_path):
            raise unittest.SkipTest("macro_expander.out not built")

    def test_simple_macro_definition(self):
        """Test that macro definitions expand correctly via stdin pipe."""
        input_text = r'\newcommand{\foo}{bar}\foo'
        result = subprocess.run(
            [self.expander_path],
            input=input_text,
            capture_output=True,
            text=True
        )
        self.assertEqual(result.returncode, 0)
        self.assertIn('bar', result.stdout)


class TestBibliographyTokens(unittest.TestCase):
    """Test cases for Issue #11: Tokenize LaTeX bibliography entries."""

    @classmethod
    def setUpClass(cls):
        """Check if scanner.out exists."""
        cls.scanner_path = os.path.abspath('scanner/scanner.out')
        cls.scanner_dir = os.path.dirname(cls.scanner_path)
        if not os.path.exists(cls.scanner_path):
            raise unittest.SkipTest("scanner.out not built")

    def test_cite_token_detection(self):
        """Test \\cite{...} tokenization."""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.tex', delete=False) as f:
            f.write(r'\cite{key1,key2}')
            tex_file = f.name

        try:
            result = subprocess.run(
                [self.scanner_path, tex_file, 'tex'],
                capture_output=True,
                text=True,
                cwd=self.scanner_dir
            )
            self.assertTrue(os.path.exists(os.path.join(self.scanner_dir, 'sidecar.tok')))
        finally:
            os.unlink(tex_file)

    def test_label_token_detection(self):
        """Test \\label{...} tokenization."""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.tex', delete=False) as f:
            f.write(r'\label{my_label}')
            tex_file = f.name

        try:
            result = subprocess.run(
                [self.scanner_path, tex_file, 'tex'],
                capture_output=True,
                text=True,
                cwd=self.scanner_dir
            )
            self.assertTrue(os.path.exists(os.path.join(self.scanner_dir, 'sidecar.tok')))
        finally:
            os.unlink(tex_file)


class TestSentenceSplit(unittest.TestCase):
    """Test sentence splitting functionality."""

    @classmethod
    def setUpClass(cls):
        """Check if sent_split.out exists."""
        cls.split_path = 'scanner/sent_split.out'
        if not os.path.exists(cls.split_path):
            raise unittest.SkipTest("sent_split.out not built")

    def test_basic_sentence_split(self):
        """Test basic sentence splitting."""
        input_text = "First sentence. Second sentence."
        result = subprocess.run(
            [self.split_path],
            input=input_text,
            capture_output=True,
            text=True
        )
        self.assertEqual(result.returncode, 0)
        lines = result.stdout.strip().split('\n')
        self.assertGreater(len(lines), 0)


if __name__ == '__main__':
    unittest.main()
