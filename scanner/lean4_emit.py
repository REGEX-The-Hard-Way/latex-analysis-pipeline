"""
Phase F: Lean4 Code Generation

Recursively walks math block token trees and generates Lean4 theorem statements.
Handles nested blocks (frac, sqrt, parens, sum, int, lim).
"""
import sqlite3
import os
import sys
import re
import json

DB_PATH = "/mnt/x/home/user/my_data.db"
OUTPUT_DIR = "/home/user/latex-analysis-pipeline/output/lean4"

OP_MAP = {
    '+': '+', '-': '-', '\\times': '*', '\\cdot': '*',
    '/': '/', '\\div': '/',
    '=': '=', '\\equiv': '≡', '\\approx': '≈',
    '\\leq': '≤', '\\le': '≤', '\\geq': '≥', '\\ge': '≥',
    '\\neq': '≠', '\\ne': '≠', '<': '<', '>': '>',
    '\\in': '∈', '\\subset': '⊂', '\\subseteq': '⊆',
    '\\to': '→', '\\rightarrow': '→', '\\Rightarrow': '⇒',
    '\\sin': 'Real.sin', '\\cos': 'Real.cos', '\\tan': 'Real.tan',
    '\\log': 'Real.log', '\\ln': 'Real.log', '\\exp': 'Real.exp',
    '\\sinh': 'Real.sinh', '\\cosh': 'Real.cosh', '\\tanh': 'Real.tanh',
    '\\sqrt': 'Real.sqrt',
}

GREEK = {
    '\\alpha': 'α', '\\beta': 'β', '\\gamma': 'γ', '\\delta': 'δ',
    '\\epsilon': 'ε', '\\varepsilon': 'ε', '\\zeta': 'ζ', '\\eta': 'η',
    '\\theta': 'θ', '\\iota': 'ι', '\\kappa': 'κ', '\\lambda': 'λ',
    '\\mu': 'μ', '\\nu': 'ν', '\\xi': 'ξ', '\\pi': 'π',
    '\\rho': 'ρ', '\\sigma': 'σ', '\\tau': 'τ', '\\upsilon': 'υ',
    '\\phi': 'φ', '\\varphi': 'ϕ', '\\chi': 'χ', '\\psi': 'ψ',
    '\\omega': 'ω', '\\Gamma': 'Γ', '\\Delta': 'Δ', '\\Theta': 'Θ',
    '\\Lambda': 'Λ', '\\Xi': 'Ξ', '\\Pi': 'Π', '\\Sigma': 'Σ',
    '\\Phi': 'Φ', '\\Psi': 'Ψ', '\\Omega': 'Ω',
    '\\partial': '∂', '\\infty': '∞', '\\nabla': '∇',
    '\\hbar': 'ℏ', '\\emptyset': '∅',
}


def leaf_text(typ, tok):
    if typ in {'math_var'}:
        return tok.strip()
    if typ == 'math_greek':
        return GREEK.get(tok, tok.replace('\\', ''))
    if typ in {'math_num', 'scientific'}:
        return tok.strip().replace(',', '')
    if typ == 'math_fn':
        return OP_MAP.get(tok, tok.replace('\\', ''))
    if typ in {'math_op', 'math_rel'}:
        return OP_MAP.get(tok, tok)
    if typ == 'math_sym':
        return OP_MAP.get(tok, GREEK.get(tok, tok.replace('\\', '')))
    if typ == 'accent':
        if '\\mathbb{R}' in tok or '{\\mathbb R}' in tok:
            return 'ℝ'
        if '\\mathbb{Z}' in tok or '{\\mathbb Z}' in tok:
            return 'ℤ'
        if '\\mathbb{N}' in tok:
            return 'ℕ'
        if '\\mathbb{C}' in tok or '{\\mathbb C}' in tok:
            return 'ℂ'
        if '\\mathbf{' in tok or '\\boldsymbol{' in tok:
            inner = tok.split('{', 1)[1].rstrip('}').rstrip('%')
            return leaf_text('math_var', inner)
        if '\\widetilde{' in tok:
            inner = tok.split('{', 1)[1].rstrip('}').rstrip('%')
            return '~' + leaf_text('math_var', inner)
        if '\\hat{' in tok or '\\bar{' in tok:
            inner = tok.split('{', 1)[1].rstrip('}').rstrip('%')
            return leaf_text('math_var', inner)
        return '·'
    return tok


def expr_from_tree(node, children_map, annotations):
    """Recursively build Lean4 expression from tree node."""
    typ = node['type']
    tok = node['text']
    kids = children_map.get(node['token_id'], [])
    kids.sort(key=lambda k: k['offset'])

    if typ in {'math_var', 'math_greek'}:
        base = leaf_text(typ, tok)
        ann = annotations.get(node['token_id'])
        if ann and ann in {'ℝ', 'ℤ', 'ℕ', 'ℂ'}:
            base = f"({base} : {ann})"
        if kids:
            for k in kids:
                if k['type'] == 'math_sup':
                    sup = expr_from_tree(k, children_map, annotations)
                    base = f"({base} ^ {sup})"
                elif k['type'] == 'math_sub':
                    sub = expr_from_tree(k, children_map, annotations)
                    base = f"({base}_{sub})"
        return base

    if typ in {'math_num', 'scientific'}:
        return leaf_text(typ, tok)

    if typ in {'math_op', 'math_rel'}:
        return leaf_text(typ, tok)

    if typ in {'math_fn', 'math_sym'}:
        return leaf_text(typ, tok)

    if typ == 'accent':
        return leaf_text(typ, tok)

    if typ == 'frac':
        parts = []
        for k in kids:
            if k['type'] not in ('parens', 'braces'):
                parts.append(expr_from_tree(k, children_map, annotations))
        if len(parts) >= 2:
            return f"({parts[0]} / {parts[1]})"
        elif len(parts) == 1:
            return parts[0]
        return "1"

    if typ == 'sqrt':
        inner = []
        for k in kids:
            inner.append(expr_from_tree(k, children_map, annotations))
        body = ' '.join(inner) if inner else '1'
        return f"Real.sqrt ({body})"

    if typ == 'parens':
        inner = []
        for k in kids:
            inner.append(expr_from_tree(k, children_map, annotations))
        body = ' '.join(inner) if inner else ''
        return f"({body})"

    if typ == 'braces':
        inner = []
        for k in kids:
            inner.append(expr_from_tree(k, children_map, annotations))
        body = ' '.join(inner) if inner else ''
        return body

    if typ in {'sum', 'prod'}:
        inner = []
        for k in kids:
            inner.append(expr_from_tree(k, children_map, annotations))
        body = ' '.join(inner) if inner else '·'
        op = '∑' if typ == 'sum' else '∏'
        return f"({op} {body})"

    if typ in {'int'}:
        inner = []
        for k in kids:
            inner.append(expr_from_tree(k, children_map, annotations))
        body = ' '.join(inner) if inner else '·'
        return f"(∫ {body})"

    if typ in {'lim'}:
        inner = []
        for k in kids:
            inner.append(expr_from_tree(k, children_map, annotations))
        body = ' '.join(inner) if inner else '·'
        return f"(lim {body})"

    if typ == 'math_sub':
        inner = []
        for k in kids:
            inner.append(expr_from_tree(k, children_map, annotations))
        return ' '.join(inner) if inner else '·'

    if typ == 'math_sup':
        inner = []
        for k in kids:
            inner.append(expr_from_tree(k, children_map, annotations))
        return ' '.join(inner) if inner else '·'

    if typ == 'binom':
        parts = []
        for k in kids:
            if k['type'] not in ('parens', 'braces'):
                parts.append(expr_from_tree(k, children_map, annotations))
        if len(parts) >= 2:
            return f"(Nat.choose {parts[0]} {parts[1]})"
        return '0'

    if kids:
        parts = []
        for k in kids:
            parts.append(expr_from_tree(k, children_map, annotations))
        return ' '.join(parts)

    return leaf_text(typ, tok)


def build_theorem(block_node, children_map, annotations):
    """Generate Lean4 theorem from a math block and its children tree."""
    typ = block_node['type']
    kids = children_map.get(block_node['token_id'], [])
    kids.sort(key=lambda k: k['offset'])

    direct_kids = [
        k for k in kids
        if k['type'] not in ('type_annotation', 'constraint', 'label',
                             'begin_doc', 'end_doc', 'comment')
    ]

    n = len(direct_kids)
    if n < 3:
        return None

    rel_idx = None
    for i, k in enumerate(direct_kids):
        if k['type'] == 'math_rel' and k['text'] in {
                '=', '\\equiv', '\\leq', '\\geq', '\\le', '\\ge', '<', '>',
                '\\neq', '\\ne', '\\approx',
        }:
            rel_idx = i
            break

    if rel_idx is None or rel_idx == 0 or rel_idx == n - 1:
        return None

    lhs_parts = []
    for k in direct_kids[:rel_idx]:
        lhs_parts.append(expr_from_tree(k, children_map, annotations))
    lhs = ' '.join(lhs_parts).strip()
    if not lhs:
        return None

    rhs_parts = []
    for k in direct_kids[rel_idx + 1:]:
        rhs_parts.append(expr_from_tree(k, children_map, annotations))
    rhs = ' '.join(rhs_parts).strip()
    if not rhs:
        return None

    rel = OP_MAP.get(direct_kids[rel_idx]['text'],
                     direct_kids[rel_idx]['text'].replace('\\', ''))

    lhs = re.sub(r'\s+', ' ', lhs)
    rhs = re.sub(r'\s+', ' ', rhs)

    safe_name = re.sub(r'[^a-zA-Z0-9]', '_', lhs)[:30].strip('_')
    if not safe_name:
        safe_name = 'eq'

    ann_vars = []
    for k in direct_kids:
        if k['type'] in ('math_var', 'math_greek'):
            ann = annotations.get(k['token_id'])
            if ann and ann in {'ℝ', 'ℤ', 'ℕ', 'ℂ'}:
                vname = leaf_text(k['type'], k['text'])
                ann_vars.append(f"({vname} : {ann})")

    type_sig = ' '.join(ann_vars) + ' : ' if ann_vars else ''

    fp_name = os.path.basename(block_node['filepath'])
    fp_clean = fp_name.replace('.tex', '').replace('-', '_').replace('.', '_')

    return f"""import Mathlib

/-- Equation from {fp_name} -/
theorem {fp_clean}_{safe_name} : {type_sig}{lhs} {rel} {rhs} := by
  sorry
"""


def main(dry_run=True, limit=0, output_dir=None):
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()

    math_types = (
        'math', 'display_math', 'display_2_math', 'equation',
        'align', 'gather', 'multline', 'split', 'eqnarray',
    )

    cursor.execute(f"""
        SELECT DISTINCT token_id, type, token, filepath, filepath_id, CAST(offset AS INTEGER)
        FROM authors
        WHERE type IN ({','.join(f"'{t}'" for t in math_types)})
        ORDER BY filepath, CAST(offset AS INTEGER)
    """)
    all_blocks = cursor.fetchall()

    if limit:
        all_blocks = all_blocks[:limit]

    print(f"Found {len(all_blocks)} math blocks")

    block_ids = set(r[0] for r in all_blocks)
    blocks = {r[0]: {
        'token_id': r[0], 'type': r[1], 'text': r[2],
        'filepath': r[3], 'filepath_id': r[4], 'offset': r[5],
    } for r in all_blocks}

    all_tokens = {}
    children_map = {}

    # Fetch direct children of all math blocks in batches
    block_ids_list = list(block_ids)
    for bi in range(0, len(block_ids_list), 2000):
        batch = block_ids_list[bi:bi + 2000]
        bp = ','.join(f"'{b}'" for b in batch)
        cursor.execute(f"""
            SELECT token_id, type, token, parent_id, CAST(offset AS INTEGER)
            FROM authors WHERE parent_id IN ({bp})
        """)
        for tid, typ, tok, pid, off in cursor.fetchall():
            if tid not in all_tokens:
                node = {
                    'token_id': tid, 'type': typ, 'text': tok,
                    'parent_id': pid, 'offset': off,
                }
                all_tokens[tid] = node
                children_map.setdefault(pid, []).append(node)

    # Recursively fetch descendants of block tokens (frac, sqrt, etc.)
    to_scan = [
        tid for tid, node in all_tokens.items()
        if node['type'] in {
            'frac', 'sqrt', 'parens', 'braces', 'sum', 'prod',
            'lim', 'int', 'abs', 'norm', 'floor', 'ceil', 'binom',
            'pmatrix', 'bmatrix', 'matrix', 'vmatrix', 'Bmatrix',
            'Vmatrix', 'smallmatrix', 'cases', 'dcases', 'array',
        }
    ]
    scanned = set(block_ids) | set(all_tokens.keys())
    depth = 0

    while to_scan and depth < 3:
        batch = to_scan[:2000]
        to_scan = to_scan[2000:]
        depth += 1

        bp = ','.join(f"'{b}'" for b in batch)
        cursor.execute(f"""
            SELECT token_id, type, token, parent_id, CAST(offset AS INTEGER)
            FROM authors WHERE parent_id IN ({bp})
        """)
        for tid, typ, tok, pid, off in cursor.fetchall():
            if tid not in all_tokens:
                node = {
                    'token_id': tid, 'type': typ, 'text': tok,
                    'parent_id': pid, 'offset': off,
                }
                all_tokens[tid] = node
                children_map.setdefault(pid, []).append(node)
                if typ in {
                    'frac', 'sqrt', 'parens', 'braces', 'sum', 'prod',
                    'lim', 'int', 'abs', 'norm', 'floor', 'ceil', 'binom',
                    'pmatrix', 'bmatrix', 'matrix', 'vmatrix',
                    'smallmatrix', 'cases', 'dcases', 'array',
                }:
                    if tid not in scanned:
                        to_scan.append(tid)

    print(f"Loaded {len(all_tokens)} tokens, depth {depth}")

    # Load annotations
    cursor.execute("""
        SELECT token, parent_id FROM authors WHERE type = 'type_annotation'
    """)
    annotations = {}
    for tok, pid in cursor.fetchall():
        if 'type=' in tok:
            type_val = tok.split('type=')[1].split(' ')[0]
            if pid not in annotations:
                annotations[pid] = type_val
    print(f"Loaded {len(annotations)} type annotations")

    generated = 0
    sample_count = 0
    lean_files = []

    for bid, block_node in blocks.items():
        lean = build_theorem(block_node, children_map, annotations)
        if lean is None:
            continue

        generated += 1
        lean_files.append((block_node, lean))

        if sample_count < 3 and not output_dir:
            print(f"\n--- Example {sample_count + 1} ---")
            print(lean)
            sample_count += 1

    if output_dir:
        os.makedirs(output_dir, exist_ok=True)
        all_lean = []
        for block_node, lean in lean_files:
            fp_name = os.path.basename(block_node['filepath'])
            short_dir = os.path.join(output_dir, fp_name.replace('.tex', ''))
            os.makedirs(short_dir, exist_ok=True)
            out_file = os.path.join(short_dir, f"{block_node['token_id']}.lean")
            with open(out_file, 'w') as f:
                f.write(lean)
            all_lean.append(out_file)
        print(f"\nWrote {len(all_lean)} .lean files to {output_dir}")

    conn.close()
    print(f"\nDone. {generated} Lean4 theorems generated from "
          f"{len(all_blocks)} math blocks.")
    return generated


if __name__ == "__main__":
    dry_run = '--dry-run' in sys.argv
    output_dir = None
    limit = 0
    for arg in sys.argv[1:]:
        if arg.startswith('--output='):
            output_dir = arg.split('=', 1)[1]
        elif arg.startswith('--limit='):
            limit = int(arg.split('=', 1)[1])
    main(dry_run=dry_run, limit=limit, output_dir=output_dir)
