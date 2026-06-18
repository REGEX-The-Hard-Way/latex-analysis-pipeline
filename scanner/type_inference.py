"""
Phase E: Semantic Extraction - Type Inference (optimized)

Infers mathematical types for variables from context within math blocks.
Inserts type_annotation and constraint tokens into the authors table.
"""
import sqlite3
import sys

DB_PATH = "/mnt/x/home/user/my_data.db"

BLACKBOARD_PREFIXES = {
    '\\mathbb{R}': '\u211d', '\\mathbb{Z}': '\u2124', '\\mathbb{N}': '\u2115',
    '\\mathbb{C}': '\u2102', '\\mathbb{Q}': '\u211a', '\\mathbbm{R}': '\u211d',
    '\\mathbbm{C}': '\u2102', '\\mathbbm{Z}': '\u2124',
}

REAL_FUNCTIONS = {
    '\\sin', '\\cos', '\\tan', '\\csc', '\\sec', '\\cot',
    '\\arcsin', '\\arccos', '\\arctan',
    '\\sinh', '\\cosh', '\\tanh',
    '\\log', '\\ln', '\\exp', '\\sqrt', '\\floor', '\\ceil',
}

INTEGER_VARS = {'n', 'm', 'i', 'j', 'k', 'l', 'p', 'q', 'r', 's'}
REAL_VARS = {'x', 'y', 'z', 't', 'u', 'v', 'w', '\u03b5', '\u03b4', 'c'}

VARIABLE_TYPES = {'math_var', 'math_greek'}
REL_TYPES = {'math_rel'}
FN_TYPES = {'math_fn'}
SYM_TYPES = {'math_sym'}
NUM_TYPES = {'math_num', 'scientific'}
ACCENT_TYPES = {'accent'}

GREEK_MAP = {
    '\\epsilon': '\u03b5', '\\varepsilon': '\u03b5', '\\delta': '\u03b4',
    '\\rho': '\u03c1', '\\sigma': '\u03c3', '\\phi': '\u03c6',
    '\\varphi': '\u03c6', '\\alpha': '\u03b1', '\\beta': '\u03b2',
    '\\gamma': '\u03b3', '\\lambda': '\u03bb', '\\mu': '\u03bc',
    '\\nu': '\u03bd', '\\omega': '\u03c9', '\\tau': '\u03c4',
    '\\theta': '\u03b8', '\\eta': '\u03b7', '\\kappa': '\u03ba',
    '\\pi': '\u03c0', '\\psi': '\u03c8', '\\xi': '\u03be',
    '\\zeta': '\u03b6', '\\chi': '\u03c7', '\\iota': '\u03b9',
    '\\upsilon': '\u03c5', '\\partial': '\u2202', '\\infty': '\u221e',
    '\\nabla': '\u2207',
}


def new_token_id(prefix, parent_id, counter):
    raw = f"{prefix}_{parent_id}_{counter[0]}"
    counter[0] += 1
    return str(abs(hash(raw)) % 10 ** 10)


def get_bb_type(accent_text):
    for key in sorted(BLACKBOARD_PREFIXES, key=len, reverse=True):
        if accent_text.startswith(key):
            suffix = accent_text[len(key):]
            if suffix.startswith('^'):
                exp = suffix[1:].strip('{}')
                return f"{BLACKBOARD_PREFIXES[key]}^{exp}"
            return BLACKBOARD_PREFIXES[key]
    return None


def extract_varname(token_text):
    v = token_text.strip()
    return GREEK_MAP.get(v, v)


def infer_types_for_block(tokens):
    """tokens: list of (token_id, token_text, token_type)"""
    n = len(tokens)
    type_annotations = {}
    constraints = []

    for i in range(n):
        tid, text, ttype = tokens[i]

        if ttype not in VARIABLE_TYPES:
            continue

        vname = extract_varname(text)

        if i + 2 < n and tokens[i + 1][2] in SYM_TYPES and tokens[i + 1][1] == '\\in':
            bb_type = scan_for_bb_type(tokens, i + 2, n)
            if bb_type:
                type_annotations[tid] = (bb_type, 'high', 'in_set')
                continue

        for j in range(max(0, i - 5), i):
            _, ft, ftype = tokens[j]
            if ftype in FN_TYPES and ft in REAL_FUNCTIONS:
                between = {t[2] for t in tokens[j + 1:i]}
                if between & {'parens', 'braces'}:
                    type_annotations[tid] = ('\u211d', 'high', 'fn_arg')
                    break
        else:
            if i + 1 < n and tokens[i + 1][2] == 'math_sup':
                type_annotations[tid] = ('\u211d', 'medium', 'exp_base')

            elif vname in REAL_VARS:
                type_annotations[tid] = ('\u211d', 'low', 'name')
            elif vname in INTEGER_VARS:
                type_annotations[tid] = ('\u2124', 'low', 'name')

    for i in range(n):
        _, text, ttype = tokens[i]
        if ttype not in REL_TYPES:
            continue
        if text not in {'=', '\\leq', '\\geq', '<', '>', '\\le', '\\ge'}:
            continue

        left_var = None
        left_name = ''
        for j in range(i - 1, max(0, i - 4), -1):
            if tokens[j][2] in VARIABLE_TYPES:
                left_var = tokens[j][0]
                left_name = extract_varname(tokens[j][1])
                break
        if not left_var:
            continue

        right_val = ''
        for j in range(i + 1, min(i + 6, n)):
            t = tokens[j]
            if t[2] in VARIABLE_TYPES:
                right_val = extract_varname(t[1])
                break
            elif t[2] in NUM_TYPES:
                right_val = t[1].strip()
                break
            elif t[2] in REL_TYPES:
                break
        if not right_val:
            continue

        rel_map = {
            '=': '=', '\\leq': '\u2264', '\\le': '\u2264',
            '\\geq': '\u2265', '\\ge': '\u2265',
            '<': '<', '>': '>',
        }
        op = rel_map[text]
        constraints.append((left_var, f'{left_name} {op} {right_val}'))

    return type_annotations, constraints


def scan_for_bb_type(tokens, start, n):
    for j in range(start, min(start + 8, n)):
        _, text, ttype = tokens[j]
        if ttype in ACCENT_TYPES:
            bt = get_bb_type(text)
            if bt:
                return bt
        if ttype in VARIABLE_TYPES or ttype in NUM_TYPES:
            break
    return None


def main(dry_run=False, limit=0):
    conn = sqlite3.connect(DB_PATH)
    conn.execute("PRAGMA journal_mode=WAL")
    cursor = conn.cursor()

    cursor.execute("""
        SELECT DISTINCT token_id FROM authors
        WHERE type IN ('math','display_math','display_2_math','equation',
                       'align','gather','multline','split',
                       'matrix','bmatrix','pmatrix','vmatrix','Bmatrix','Vmatrix',
                       'smallmatrix','cases','dcases','array','eqnarray')
    """)
    all_ids = [r[0] for r in cursor.fetchall()]
    print(f"Found {len(all_ids)} math blocks")

    if limit:
        all_ids = all_ids[:limit]

    cursor.execute("""
        SELECT token_id, filepath, filepath_id FROM authors
        WHERE type IN ('math','display_math','display_2_math','equation',
                       'align','gather','multline','split',
                       'matrix','bmatrix','pmatrix','vmatrix','Bmatrix','Vmatrix',
                       'smallmatrix','cases','dcases','array','eqnarray')
    """)
    fp_map = {r[0]: (r[1], r[2]) for r in cursor.fetchall()}
    print(f"Loaded {len(fp_map)} filepath mappings")

    total_annotations = 0
    total_constraints = 0
    counter = [0]
    BATCH_SIZE = 200
    insert_batch = []

    for start in range(0, len(all_ids), BATCH_SIZE):
        batch_ids = all_ids[start:start + BATCH_SIZE]
        placeholders = ",".join(f"'{m}'" for m in batch_ids)

        cursor.execute(f"""
            SELECT t.token_id, t.token, t.type, t.parent_id
            FROM authors t
            WHERE t.parent_id IN ({placeholders})
            AND t.type IN ('math_var','math_greek','math_sym','math_fn','math_op',
                            'math_rel','math_num','accent','parens','braces','frac',
                            'sqrt','math_sub','math_sup','sum','prod',
                            'scientific','binom')
            ORDER BY t.parent_id, CAST(t.offset AS INTEGER)
        """)
        rows = cursor.fetchall()

        from collections import defaultdict
        groups = defaultdict(list)
        for tid, text, typ, parent_id in rows:
            groups[parent_id].append((tid, text, typ))

        for parent_id, tokens in groups.items():
            if len(tokens) < 2:
                continue

            annotations, constraints = infer_types_for_block(tokens)

            fp_info = fp_map.get(parent_id)
            if not fp_info:
                continue
            filepath, fp_id = fp_info

            for var_tid, (inferred_type, confidence, evidence) in annotations.items():
                annot_tid = new_token_id("ta", parent_id, counter)
                annotation_text = (
                    f"type={inferred_type} conf={confidence} ev={evidence}"
                )
                insert_batch.append((filepath, fp_id, annot_tid, var_tid, "0",
                              str(len(annotation_text)), "type_annotation",
                              annotation_text))
                total_annotations += 1

            for var_tid, constraint_text in constraints:
                constr_tid = new_token_id("ct", parent_id, counter)
                insert_batch.append((filepath, fp_id, constr_tid, var_tid, "0",
                              str(len(constraint_text)), "constraint",
                              constraint_text))
                total_constraints += 1

        if insert_batch and not dry_run:
            cursor.executemany(
                "INSERT OR IGNORE INTO authors (filepath, filepath_id, token_id, "
                "parent_id, offset, length, type, token) VALUES (?,?,?,?,?,?,?,?)",
                insert_batch,
            )
            conn.commit()
            insert_batch = []

        if (start // BATCH_SIZE) % 25 == 0:
            pct = min(100, 100 * (start + len(batch_ids)) // len(all_ids))
            print(f"  {pct}%: {total_annotations} annotations, "
                  f"{total_constraints} constraints")

    if insert_batch and not dry_run:
        cursor.executemany(
            "INSERT OR IGNORE INTO authors (filepath, filepath_id, token_id, "
            "parent_id, offset, length, type, token) VALUES (?,?,?,?,?,?,?,?)",
            insert_batch,
        )
        conn.commit()

    conn.close()
    print(f"\nDone. {total_annotations} type_annotation tokens, "
          f"{total_constraints} constraint tokens.")
    return total_annotations, total_constraints


if __name__ == "__main__":
    dry_run = '--dry-run' in sys.argv
    limit = 0
    for arg in sys.argv[1:]:
        if arg.startswith('--limit='):
            limit = int(arg.split('=')[1])
    main(dry_run=dry_run, limit=limit)
