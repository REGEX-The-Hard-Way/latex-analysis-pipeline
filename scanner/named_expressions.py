"""
Phase E: Named Expression Recognition

Computes structural fingerprints for math blocks, matches against
curated dictionary of known expressions, and creates named_expression
edges in graph_edges table.
"""
import sqlite3
import hashlib
import json
import sys
import os

DB_PATH = "/mnt/x/home/user/my_data.db"
NAMED_EXPRESSIONS_PATH = os.path.join(
    os.path.dirname(__file__), "..", "docs", "named_expressions.json"
)

CHILD_TYPES = {
    'math_var', 'math_greek', 'math_sym', 'math_num', 'math_fn',
    'math_op', 'math_rel', 'accent', 'parens', 'braces', 'frac',
    'sqrt', 'math_sub', 'math_sup', 'sum', 'prod', 'lim', 'int',
    'binom', 'abs', 'norm', 'floor', 'ceil', 'scientific',
    'matrix_col_delim', 'matrix_row_delim',
}


def compute_signature(children):
    """Compute structural fingerprint from child token types.
    children: list of (type, text) tuples.
    Returns signature string like 'equation:math_var.2|math_op.1|math_rel.1'
    """
    if not children:
        return None

    type_counts = {}
    for typ, _ in children:
        type_counts[typ] = type_counts.get(typ, 0) + 1

    sorted_types = sorted(type_counts.keys())
    parts = [f"{t}.{type_counts[t]}" for t in sorted_types]
    return "|".join(parts)


def load_named_expressions():
    """Load curated named expression dictionary."""
    if os.path.exists(NAMED_EXPRESSIONS_PATH):
        with open(NAMED_EXPRESSIONS_PATH) as f:
            return json.load(f)

    return {
        "pythagorean": {
            "canonical_name": "Pythagorean theorem",
            "lean4_name": "pythagorean_theorem",
            "description": "a^2 + b^2 = c^2",
            "signatures": [
                "accent.1|math_rel.1|math_sup.3|math_var.3",
            ],
        },
        "euler_identity": {
            "canonical_name": "Euler's identity",
            "lean4_name": "euler_identity",
            "description": "e^{i*pi} + 1 = 0",
            "signatures": [
                "math_fn.1|math_num.2|math_op.1|math_rel.1|math_sup.1",
            ],
        },
        "quadratic_formula": {
            "canonical_name": "Quadratic formula",
            "lean4_name": "quadratic_formula",
            "description": "x = (-b +/- sqrt(b^2 - 4ac)) / (2a)",
            "signatures": [
                "frac.1|math_op.2|math_rel.1|math_sup.1|parens.1|sqrt.1|math_var.3|math_num.2",
            ],
        },
        "derivative_def": {
            "canonical_name": "Derivative definition",
            "lean4_name": "derivative_definition",
            "description": "f'(x) = lim_{h->0} (f(x+h) - f(x)) / h",
            "signatures": [
                "frac.1|lim.1|math_fn.2|math_op.2|math_rel.1|parens.2|math_var.4",
            ],
        },
        "schrodinger": {
            "canonical_name": "Schroedinger equation",
            "lean4_name": "schrodinger_equation",
            "description": "i*hbar*partial/partial_t Psi = H Psi",
            "signatures": [
                "math_fn.1|frac.1|math_sub.1|math_op.1|math_rel.1|math_greek.1|accent.1",
            ],
        },
    }


def main(dry_run=False, limit=0):
    named_exprs = load_named_expressions()
    print(f"Loaded {len(named_exprs)} named expression patterns")

    conn = sqlite3.connect(DB_PATH)
    conn.execute("PRAGMA journal_mode=WAL")

    cursor = conn.cursor()

    cursor.execute("""
        SELECT DISTINCT token_id FROM authors
        WHERE type IN ('math','display_math','display_2_math','equation',
                       'align','gather','multline','split','eqnarray')
    """)
    all_ids = [r[0] for r in cursor.fetchall()]
    print(f"Found {len(all_ids)} math blocks")

    if limit:
        all_ids = all_ids[:limit]

    matches = []
    counter = [0]
    BATCH_SIZE = 500

    for start in range(0, len(all_ids), BATCH_SIZE):
        batch_ids = all_ids[start:start + BATCH_SIZE]
        placeholders = ",".join(f"'{m}'" for m in batch_ids)

        cursor.execute(f"""
            SELECT t.parent_id, t.type, t.token
            FROM authors t
            WHERE t.parent_id IN ({placeholders})
            AND t.type IN ({','.join(f"'{ct}'" for ct in CHILD_TYPES)})
            ORDER BY t.parent_id, CAST(t.offset AS INTEGER)
        """)
        rows = cursor.fetchall()

        from collections import defaultdict
        groups = defaultdict(list)
        for parent_id, typ, tok in rows:
            groups[parent_id].append((typ, tok))

        for parent_id, children in groups.items():
            sig = compute_signature(children)
            if not sig:
                continue

            for expr_key, expr_info in named_exprs.items():
                if sig in expr_info.get("signatures", []):
                    cursor.execute(
                        "SELECT filepath, filepath_id FROM authors "
                        "WHERE token_id=? LIMIT 1",
                        (parent_id,),
                    )
                    fp = cursor.fetchone()
                    if fp:
                        matches.append({
                            "expr_key": expr_key,
                            "canonical_name": expr_info["canonical_name"],
                            "filepath": fp[0],
                            "filepath_id": fp[1],
                            "token_id": parent_id,
                            "signature": sig,
                        })
                    break

        pct = 100 * (start + len(batch_ids)) // len(all_ids)
        if start % (BATCH_SIZE * 10) == 0:
            print(f"  {pct}%: {len(matches)} matches so far")

    cursor.execute(
        "SELECT COUNT(*) FROM graph_edges WHERE relationship='named_expression'"
    )
    existing = cursor.fetchone()[0]

    if not dry_run and matches:
        for i, match in enumerate(matches):
            edge_id = f"ne_{match['token_id']}_{i}"
            cursor.execute(
                """INSERT OR IGNORE INTO graph_edges
                (edge_id, source_type, source_key, source_filepath,
                 source_filepath_id, target_type, target_key,
                 target_filepath, target_filepath_id, target_token_id,
                 relationship, status)
                VALUES (?,?,?,?,?,?,?,?,?,?,?,?)""",
                (
                    edge_id,
                    "expression",
                    match["canonical_name"],
                    match["filepath"],
                    match["filepath_id"],
                    "named_entity",
                    match["expr_key"],
                    "",
                    "",
                    match["token_id"],
                    "named_expression",
                    "structural_match",
                ),
            )
        conn.commit()
        print(f"  Inserted {len(matches)} named_expression edges")

    conn.close()

    print(f"\nDone. {len(matches)} named expression matches found.")
    if dry_run:
        for m in matches[:10]:
            print(f"  {m['canonical_name']}: {m['signature']} "
                  f"({os.path.basename(m['filepath'])})")
    return len(matches)


if __name__ == "__main__":
    dry_run = '--dry-run' in sys.argv
    limit = 0
    for arg in sys.argv[1:]:
        if arg.startswith('--limit='):
            limit = int(arg.split('=')[1])
    main(dry_run=dry_run, limit=limit)
