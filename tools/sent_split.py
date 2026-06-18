#!/usr/bin/env python3
"""
Sentence splitter for LaTeX body text.

Reads a .tex file and sidecar.tok, replaces inline math with hash IDs,
strips LaTeX commands, and outputs one sentence per line.

Usage:
    python tools/sent_split.py scanner/sound1.tex sidecar.tok > sentences.txt

The output is ready to pipe into meta/profile for constituency parsing.
"""
import re
import sys


def load_math_ids(sidecar_path):
    """Build mapping: math token text -> hash ID."""
    lookup = {}
    math_types = {'math', 'display_math', 'display_2_math',
                  'equation', 'align', 'eqnarray'}
    with open(sidecar_path) as f:
        for line in f:
            m = re.search(r'token_id:(\d+).*?type:([^,]+).*?<tok:(.*?)>', line)
            if m and m.group(2) in math_types:
                lookup[m.group(3).strip()] = m.group(1)
    return lookup


def extract_body(tex):
    """Return the document body (between \\begin{document} and bibliography)."""
    start = tex.find(r'\begin{document}')
    if start < 0:
        start = 0
    end = tex.find(r'\begin{thebibliography}')
    if end < 0:
        end = len(tex)
    return tex[start:end]


def replace_math(body, lookup):
    """Replace math blocks with their hash IDs (longest first)."""
    for text, tid in sorted(lookup.items(), key=lambda x: -len(x[0])):
        body = body.replace(text, f' {tid} ')
    return body


def strip_tex(body):
    """Remove remaining LaTeX commands and braces."""
    # Strip common commands with arguments
    body = re.sub(
        r'\\(?:title|author|affiliation|email|thanks|date)\{[^}]*\}',
        ' ', body)
    body = re.sub(r'\\(?:begin|end)\{document\}', ' ', body)
    # Strip citation wrappers: (see, e.g., \cite{...}) -> empty
    body = re.sub(r'\(?\s*see,?\s*e\.g\.,?\s*\\[a-zA-Z]+\{[^}]*\}\s*\)?', ' ', body)
    body = re.sub(r'\(?\s*see,?\s*Refs?\.?\s*\\[a-zA-Z]+\{[^}]*\}\s*\)?', ' ', body)
    # Strip \ref{...} wrappers including surrounding parentheses
    body = re.sub(r'\(\s*\\ref\{[^}]*\}\s*\)', ' ', body)
    body = re.sub(r'\\ref\{[^}]*\}', ' ', body)
    # Strip any remaining \command or \command* or \command[opt]{arg}
    body = re.sub(r'\\[a-zA-Z]+\*?(\[[^\]]*\])?(\{[^}]*\})?', ' ', body)
    # Strip braces
    body = body.replace('{', ' ').replace('}', ' ')
    # Strip residual math/TeX characters
    body = re.sub(r'[$\\%&#]', ' ', body)
    # Clean up orphaned parentheses and commas
    body = re.sub(r'\(\s*\)', ' ', body)
    body = re.sub(r'\s,\s*\)', ' ', body)
    # Collapse whitespace
    body = re.sub(r'\s+', ' ', body).strip()
    return body


def split_sentences(text):
    """Split text into sentences, respecting abbreviations."""
    abbrevs = {
        'e.g', 'i.e', 'cf', 'vs', 'etc', 'Fig', 'Eq', 'Ref', 'Vol', 'No',
        'Dr', 'Mr', 'Mrs', 'Ms', 'Prof', 'Sen', 'eds', 'approx', 'esp',
        'inc', 'est', 'al', 'et',
    }
    sentences = []
    start = 0
    i = 0

    while i < len(text):
        c = text[i]
        if c in '.!?':
            is_abbrev = False
            if c == '.':
                # Check known abbreviations (e.g., i.e., etc.)
                for ab in abbrevs:
                    alen = len(ab)
                    if i >= alen and text[i - alen:i] == ab:
                        is_abbrev = True
                        break
                # Single uppercase letter before period (initials like "J. K.")
                if i >= 2 and text[i - 1].isupper() and text[i - 2] == ' ':
                    is_abbrev = True
                # Digit before period (like "1." or "10.")
                if i >= 1 and text[i - 1].isdigit():
                    is_abbrev = True

            if not is_abbrev:
                sent = text[start:i + 1].strip()
                if sent and len(sent) > 10:  # skip fragments shorter than 10 chars
                    sentences.append(sent)
                i += 1
                while i < len(text) and text[i] in ' \t\n\r':
                    i += 1
                start = i
                continue
        i += 1

    if start < len(text):
        sent = text[start:].strip()
        if sent and len(sent) > 10:
            sentences.append(sent)

    return sentences


def main():
    if len(sys.argv) < 3:
        print("Usage: sent_split.py file.tex sidecar.tok", file=sys.stderr)
        sys.exit(1)

    with open(sys.argv[1]) as f:
        tex = f.read()

    lookup = load_math_ids(sys.argv[2])
    body = extract_body(tex)
    body = replace_math(body, lookup)
    body = strip_tex(body)

    for sent in split_sentences(body):
        print(sent)


if __name__ == '__main__':
    main()
