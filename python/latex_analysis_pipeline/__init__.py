"""
LaTeX Analysis Pipeline — Python bindings.

Provides programmatic access to the LaTeX scanner, macro expander,
and sentence splitter via subprocess wrappers.

Usage:
    from latex_analysis_pipeline import MacroExpander, LaTeXScanner

    expander = MacroExpander()
    result = expander.expand(r'\\newcommand{\\foo}{bar}\\foo')
    print(result)  # 'bar'

    scanner = LaTeXScanner()
    tokens = list(scanner.scan_file('paper.tex'))
    for tok in tokens:
        print(f'{tok["type"]}: {tok["text"][:50]}')
"""

import json
import os
import subprocess
import tempfile
from typing import Iterator, Optional

_PACKAGE_DIR = os.path.dirname(os.path.abspath(__file__))
_PROJECT_ROOT = os.path.abspath(os.path.join(_PACKAGE_DIR, '..', '..'))


def _find_binary(name: str) -> str:
    """Locate a binary: check project root, scanner/, then PATH."""
    candidates = [
        os.path.join(_PROJECT_ROOT, name),
        os.path.join(_PROJECT_ROOT, 'scanner', name),
        name,
    ]
    for path in candidates:
        if os.path.isfile(path) and os.access(path, os.X_OK):
            return path
    raise FileNotFoundError(
        f"Cannot find {name}. Build it first or install the package."
    )


class MacroExpander:
    """Wrapper around the macro_expander binary."""

    def __init__(self, binary_path: Optional[str] = None):
        self._binary = binary_path or _find_binary('macro_expander.out')

    def expand(self, tex: str) -> str:
        """Expand LaTeX macros in `tex`. Returns the expanded text."""
        r = subprocess.run(
            [self._binary],
            input=tex,
            capture_output=True,
            text=True,
        )
        r.check_returncode()
        return r.stdout

    def report(self, *filepaths: str) -> str:
        """Run in --report mode: list macro definitions found in files."""
        abs_paths = [os.path.abspath(f) for f in filepaths]
        r = subprocess.run(
            [self._binary, '--report'] + abs_paths,
            capture_output=True,
            text=True,
        )
        r.check_returncode()
        return r.stdout


class LaTeXScanner:
    """Wrapper around the scanner binary."""

    def __init__(self, binary_path: Optional[str] = None):
        self._binary = binary_path or _find_binary('scanner.out')
        self._scanner_dir = os.path.dirname(self._binary)

    def scan_file(self, filepath: str, json_mode: bool = True) -> Iterator[dict]:
        """
        Tokenize a LaTeX file. Yields dicts with keys:
        filepath, filepath_id, token_id, parent_id, offset, length, type, text.
        """
        abs_path = os.path.abspath(filepath)
        args = [self._binary]
        if json_mode:
            args.append('--json')
        args.extend([abs_path, 'tex'])

        subprocess.run(
            args,
            capture_output=True,
            text=True,
            cwd=self._scanner_dir,
        )

        if json_mode:
            json_path = os.path.join(self._scanner_dir, 'sidecar.json')
            if os.path.exists(json_path):
                with open(json_path) as f:
                    for line in f:
                        line = line.strip()
                        if line:
                            yield json.loads(line)
                os.unlink(json_path)
        else:
            tok_path = os.path.join(self._scanner_dir, 'sidecar.tok')
            if os.path.exists(tok_path):
                with open(tok_path) as f:
                    yield from _parse_tok_lines(f)
                os.unlink(tok_path)

    def scan_text(self, tex: str) -> Iterator[dict]:
        """Tokenize LaTeX from a string. Writes to temp file first."""
        with tempfile.NamedTemporaryFile(
            mode='w', suffix='.tex', delete=False
        ) as f:
            f.write(tex)
            tmp_path = f.name
        try:
            yield from self.scan_file(tmp_path)
        finally:
            os.unlink(tmp_path)


class SentenceSplitter:
    """Wrapper around the sent_split binary."""

    def __init__(self, binary_path: Optional[str] = None):
        self._binary = binary_path or _find_binary('sent_split.out')

    def split(self, text: str) -> list[str]:
        """Split text into sentences. Returns list of sentence strings."""
        r = subprocess.run(
            [self._binary],
            input=text,
            capture_output=True,
            text=True,
        )
        r.check_returncode()
        return [s.strip() for s in r.stdout.strip().split('\n') if s.strip()]


def _parse_tok_lines(lines) -> Iterator[dict]:
    """Parse the custom .tok format into dicts."""
    import re
    pattern = re.compile(
        r'\{<filepath:(.*?)>,filepath_id:(\d+),token_id:(\d+),'
        r'parent_id:(\d+),offset:(\d+),length:(\d+),'
        r'type:([^,]+),<tok:(.*?)>\}'
    )
    for line in lines:
        line = line.strip()
        if not line:
            continue
        m = pattern.match(line)
        if m:
            yield {
                'filepath': m.group(1),
                'filepath_id': int(m.group(2)),
                'token_id': int(m.group(3)),
                'parent_id': int(m.group(4)),
                'offset': int(m.group(5)),
                'length': int(m.group(6)),
                'type': m.group(7),
                'text': m.group(8),
            }
