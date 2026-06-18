"""
Phase F: Lean4 Code Generation

Recursively walks math block token trees and generates Lean4 theorem statements,
integrating type annotations from Phase E.
"""
import sqlite3
import os
import sys
import re

DB_PATH = "/mnt/x/home/user/my_data.db"
OUTPUT_DIR = "/home/user/latex-analysis-pipeline/output/lean4"

TOKEN_TO_LEAN = {
    '+': '+', '-': '-', '\\times': '*', '\\cdot': '*',
    '\\pm': '±', '\\mp': '∓',
    '/': '/', '\\div': '/',
    '=': '=', '\\equiv': '≡', '\\approx': '≈',
    '\\leq': '≤', '\\le': '≤', '\\geq': '≥', '\\ge': '≥',
    '\\neq': '≠', '\\ne': '≠',
    '<': '<', '>': '>',
    '\\in': '∈', '\\subset': '⊂', '\\subseteq': '⊆',
    '\\supset': '⊃', '\\supseteq': '⊇',
    '\\to': '→', '\\rightarrow': '→', '\\Rightarrow': '⇒',
    '\\mapsto': '↦', '\\longrightarrow': '⟶',
    '\\sin': 'Real.sin', '\\cos': 'Real.cos', '\\tan': 'Real.tan',
    '\\log': 'Real.log', '\\ln': 'Real.log', '\\exp': 'Real.exp',
    '\\sqrt': 'Real.sqrt',
    '\\sinh': 'Real.sinh', '\\cosh': 'Real.cosh', '\\tanh': 'Real.tanh',
    '\\floor': 'Int.floor', '\\ceil': 'Int.ceil',
    '\\max': 'max', '\\min': 'min',
    '\\partial': '∂', '\\infty': '∞', '\\nabla': '∇',
    '\\forall': '∀', '\\exists': '∃',
    '\\sum': '∑', '\\prod': '∏', '\\int': '∫', '\\lim': 'lim',
}

GREEK_TO_UNICODE = {
    '\\alpha': 'α', '\\beta': 'β', '\\gamma': 'γ', '\\delta': 'δ',
    '\\epsilon': 'ε', '\\varepsilon': 'ε', '\\zeta': 'ζ', '\\eta': 'η',
    '\\theta': 'θ', '\\vartheta': 'ϑ', '\\iota': 'ι', '\\kappa': 'κ',
    '\\lambda': 'λ', '\\mu': 'μ', '\\nu': 'ν', '\\xi': 'ξ',
    '\\pi': 'π', '\\varpi': 'ϖ', '\\rho': 'ρ', '\\varrho': 'ϱ',
    '\\sigma': 'σ', '\\varsigma': 'ς', '\\tau': 'τ', '\\upsilon': 'υ',
    '\\phi': 'φ', '\\varphi': 'ϕ', '\\chi': 'χ', '\\psi': 'ψ',
    '\\omega': 'ω', '\\omicron': 'ο',
    '\\Gamma': 'Γ', '\\Delta': 'Δ', '\\Theta': 'Θ', '\\Lambda': 'Λ',
    '\\Xi': 'Ξ', '\\Pi': 'Π', '\\Sigma': 'Σ', '\\Upsilon': 'Υ',
    '\\Phi': 'Φ', '\\Psi': 'Ψ', '\\Omega': 'Ω',
    '\\hbar': 'ℏ', '\\ell': 'ℓ', '\\emptyset': '∅', '\\nabla': '∇',
    '\\partial': '∂', '\\infty': '∞',
}

BLOCK_TYPES = {'parens', 'braces', 'frac', 'sqrt', 'sum', 'prod', 'lim', 'int'}
FN_TYPES = {'math_fn'}
OP_TYPES = {'math_op'}
REL_TYPES = {'math_rel'}
VAR_TYPES = {'math_var', 'math_greek'}
SUP_TYPES = {'math_sup'}
SUB_TYPES = {'math_sub'}
NUM_TYPES = {'math_num', 'scientific'}
SYM_TYPES = {'math_sym'}
ACCENT_TYPES = {'accent'}

IMPLICIT_MUL_BEFORE = VAR_TYPES | NUM_TYPES | SYM_TYPES | ACCENT_TYPES | \
                      {'parens', 'braces'}
IMPLICIT_MUL_AFTER = VAR_TYPES | NUM_TYPES | SYM_TYPES | ACCENT_TYPES | \
                     {'parens', 'braces', 'frac', 'sqrt', 'sum', 'prod'}


def atom_text(tok_type, tok_text):
    """Convert a single atom token to Lean4 text."""
    if tok_type in VAR_TYPES:
        v = tok_text.strip()
        if v in GREEK_TO_UNICODE:
            return GREEK_TO_UNICODE[v]
        return v

    if tok_type in NUM_TYPES:
        return tok_text.strip().replace(',', '')

    if tok_type in SYM_TYPES:
        return TOKEN_TO_LEAN.get(tok_text, tok_text.replace('\\', ''))

    if tok_type in FN_TYPES:
        return TOKEN_TO_LEAN.get(tok_text, tok_text.replace('\\', ''))

    if tok_type in ACCENT_TYPES:
        if tok_text.startswith('\\mathbb{R}') or tok_text.startswith('{\\mathbb R}'):
            return 'ℝ'
        if tok_text.startswith('\\mathbb{Z}') or tok_text.startswith('{\\mathbb Z}'):
            return 'ℤ'
        if tok_text.startswith('\\mathbb{N}'):
            return 'ℕ'
        if tok_text.startswith('\\mathbb{C}') or tok_text.startswith('{\\mathbb C}'):
            return 'ℂ'
        if tok_text.startswith('\\boldsymbol{') or tok_text.startswith('\\mathbf{'):
            inner = tok_text.split('{', 1)[1].rstrip('}')
            return atom_text('math_var', inner)
        return tok_text[:40]

    return tok_text


def build_lean_expr(tokens, start, end, annotations):
    """
    Convert a slice of tokens [start:end] into a Lean4 expression string.
    tokens: list of (token_id, type, text)
    annotations: dict of token_id -> type string
    Returns (lean_string, has_type)
    """
    parts = []
    i = start

    while i < end:
        tid, ttype, text = tokens[i]

        if ttype in VAR_TYPES:
            var = atom_text(ttype, text)
            ann = annotations.get(tid)
            if ann and ann in {'ℝ', 'ℤ', 'ℕ', 'ℂ'}:
                parts.append(f"({var} : {ann})")
            else:
                parts.append(var)

            if i + 1 < end and tokens[i + 1][1] in SUP_TYPES:
                sup_text = tokens[i + 1][2].lstrip('^').strip('{').rstrip('}')
                sup_text = sup_text.strip()
                if sup_text:
                    sup_lean = atom_text('math_var', sup_text) if sup_text.isalpha() or sup_text.startswith('\\') else sup_text
                    parts[-1] = f"{parts[-1]} ^ {sup_lean}"
                i += 1

            if i + 1 < end and tokens[i + 1][1] in SUB_TYPES:
                sub_text = tokens[i + 1][2].lstrip('_').strip('{').rstrip('}')
                sub_text = sub_text.strip()
                if sub_text:
                    sub_lean = atom_text('math_var', sub_text) if not sub_text.isdigit() else sub_text
                    parts[-1] = f"{parts[-1]}_{sub_lean}"
                i += 1

        elif ttype in NUM_TYPES:
            num = atom_text(ttype, text)
            parts.append(num)

        elif ttype in OP_TYPES:
            op = TOKEN_TO_LEAN.get(text, text)
            parts.append(f" {op} ")

        elif ttype in REL_TYPES:
            rel = TOKEN_TO_LEAN.get(text, text)
            parts.append(f" {rel} ")

        elif ttype in FN_TYPES:
            fn = TOKEN_TO_LEAN.get(text, text.replace('\\', ''))
            parts.append(fn)

        elif ttype in SYM_TYPES:
            sym = TOKEN_TO_LEAN.get(text, text.replace('\\', ''))
            parts.append(sym)

        elif ttype in ACCENT_TYPES:
            acc = atom_text(ttype, text)
            parts.append(acc)

        elif ttype in BLOCK_TYPES:
            block_children = _get_block_children(tokens, tid, i)
            if ttype == 'frac':
                num_children = [c for c in block_children if c[1] not in ('parens', 'braces')]
                if len(num_children) >= 2:
                    num_expr = build_lean_expr([num_children[0]], 0, 1, annotations)
                    den_expr = build_lean_expr([num_children[1]], 0, 1, annotations)
                    parts.append(f"({num_expr} / {den_expr})")
                else:
                    parts.append("(1/1)")
            elif ttype == 'sqrt':
                inner_children = [c for c in block_children if c[1] not in ('parens', 'braces')]
                if inner_children:
                    sqrt_expr = build_lean_expr(inner_children, 0, len(inner_children), annotations)
                    parts.append(f"Real.sqrt ({sqrt_expr})")
            elif ttype == 'parens':
                inner_expr = build_lean_expr(block_children, 0, len(block_children), annotations)
                parts.append(f"({inner_expr})")
            elif ttype in ('sum', 'prod'):
                op_sym = '∑' if ttype == 'sum' else '∏'
                inner = build_lean_expr(block_children, 0, len(block_children), annotations)
                parts.append(f"({op_sym}, {inner})")
            elif ttype in ('lim', 'int'):
                inner = build_lean_expr(block_children, 0, len(block_children), annotations)
                parts.append(f"({ttype}, {inner})")
            else:
                inner = build_lean_expr(block_children, 0, len(block_children), annotations)
                parts.append(f"({inner})")

        i += 1

    result = ''.join(parts).strip()
    result = re.sub(r'\s+', ' ', result)
    return result


def _get_block_children(tokens, block_tid, block_idx):
    """Get child tokens that belong to a block token (already in the stream)."""
    return []


def emit_theorem(filepath, filepath_id, block_tid, block_type,
                 children, annotations, constraints):
    """
    Generate a single Lean4 theorem from a math block.
    children: list of (token_id, type, text) sorted by offset
    """
    n = len(children)
    if n < 3:
        return None

    rel_idx = None
    for i, (tid, ttype, text) in enumerate(children):
        if ttype in REL_TYPES and text in {'=', '\\equiv', '\\approx',
                                            '\\leq', '\\geq', '\\le', '\\ge',
                                            '<', '>', '\\neq', '\\ne'}:
            rel_idx = i
            break

    if rel_idx is None:
        return None

    lhs = build_lean_expr(children, 0, rel_idx, annotations)
    rhs = build_lean_expr(children, rel_idx + 1, n, annotations)

    rel_text = children[rel_idx][2]
    rel_lean = TOKEN_TO_LEAN.get(rel_text, rel_text)

    short_fn = os.path.basename(filepath).replace('.tex', '').replace('-', '_')
    safe_lhs = re.sub(r'[^a-zA-Z0-9_]', '_', lhs)[:30]
    theorem_name = f"{short_fn}_{safe_lhs}"

    block_tid_str = str(block_tid)[:8]

    var_annotations = []
    for tid, ttype, text in children:
        if tid in annotations:
            vname = atom_text(ttype, text)
            var_annotations.append(f"{vname} : {annotations[tid]}")

    type_sig = ""
    if var_annotations:
        type_sig = "(" + ") (".join(var_annotations) + ") : "

    filename = os.path.basename(filepath)
    lean_source = f"""import Mathlib

/-- Equation from {filename}, block {block_tid_str} -/
theorem {theorem_name} {type_sig}{lhs} {rel_lean} {rhs} := by
  sorry
"""

    return lean_source


def main(dry_run=True, limit=0, output=False):
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()

    math_block_types = (
        'math', 'display_math', 'display_2_math', 'equation',
        'align', 'gather', 'multline', 'split', 'eqnarray',
    )

    placeholders = ','.join(f"'{t}'" for t in math_block_types)
    cursor.execute(f"""
        SELECT token_id, type, filepath, filepath_id
        FROM authors
        WHERE type IN ({placeholders})
        ORDER BY filepath, CAST(offset AS INTEGER)
    """)
    all_blocks = cursor.fetchall()

    if limit:
        all_blocks = all_blocks[:limit]

    print(f"Processing {len(all_blocks)} math blocks")

    block_ids = [r[0] for r in all_blocks]
    block_info = {r[0]: (r[1], r[2], r[3]) for r in all_blocks}

    generated = 0

    for bidx in range(0, len(block_ids), 500):
        batch = block_ids[bidx:bidx + 500]
        bp = ','.join(f"'{b}'" for b in batch)

        cursor.execute(f"""
            SELECT t.token_id, t.type, t.token, t.parent_id, CAST(t.offset AS INTEGER)
            FROM authors t
            WHERE t.parent_id IN ({bp})
            AND t.type IN ('math_var','math_greek','math_sym','math_fn','math_op',
                           'math_rel','math_num','accent','parens','braces','frac',
                           'sqrt','math_sub','math_sup','sum','prod','lim','int',
                           'scientific','binom',
                           'matrix_col_delim','matrix_row_delim')
            ORDER BY t.parent_id, CAST(t.offset AS INTEGER)
        """)
        rows = cursor.fetchall()

        from collections import defaultdict
        groups = defaultdict(list)
        for tid, typ, tok, parent_id, offset in rows:
            groups[parent_id].append((tid, typ, tok))

        cursor.execute(f"""
            SELECT ta.token_id, ta.token, ta.parent_id
            FROM authors ta
            WHERE ta.parent_id IN ({bp})
            AND ta.type = 'type_annotation'
        """)
        ann_rows = cursor.fetchall()
        annotations = {}
        for tid, tok, parent_id in ann_rows:
            if 'type=' in tok:
                type_val = tok.split('type=')[1].split(' ')[0]
                annotations[parent_id] = type_val

        for parent_id, children in groups.items():
            if len(children) < 3:
                continue

            block_meta = block_info.get(parent_id)
            if not block_meta:
                continue
            block_type, filepath, fp_id = block_meta

            lean = emit_theorem(
                filepath, fp_id, parent_id, block_type,
                children, annotations, {}
            )

            if lean is None:
                continue

            generated += 1

            if output:
                short = os.path.basename(filepath).replace('.tex', '')
                out_dir = os.path.join(OUTPUT_DIR, short)
                os.makedirs(out_dir, exist_ok=True)
                out_file = os.path.join(out_dir, f"{parent_id}.lean")
                with open(out_file, 'w') as f:
                    f.write(lean)

            if generated <= 5 and not output:
                print(f"--- Example {generated} ---")
                print(lean)

        pct = 100 * (bidx + len(batch)) // len(block_ids)
        if bidx % 2500 == 0:
            print(f"  {pct}%: {generated} theorems generated")

    conn.close()
    print(f"\nDone. {generated} Lean4 theorems generated.")
    return generated


if __name__ == "__main__":
    dry_run = '--dry-run' in sys.argv
    output = '--output' in sys.argv
    limit = 0
    for arg in sys.argv[1:]:
        if arg.startswith('--limit='):
            limit = int(arg.split('=')[1])
    main(dry_run=dry_run, limit=limit, output=output)
