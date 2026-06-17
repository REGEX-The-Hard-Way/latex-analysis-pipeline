"""
Phase C: Operator precedence chain detection (optimized).
Finds additive/multiplicative chains in math blocks and inserts
additive_begin/additive_end and mp_begin/mp_end tokens.
"""
import sqlite3
import os

DB_PATH = "/mnt/x/home/user/my_data.db"

ADDITIVE_OPS = {"+", "-"}
MULTIPLICATIVE_OPS = {
    "*", "/",
}

ATOM_TYPES = {
    "math_var", "math_greek", "math_sym", "math_num", "math_fn",
    "scientific", "percent",
}
GROUP_TYPES = {
    "parens", "braces", "abs", "norm", "floor", "ceil", "frac",
}


def detect_chains(tokens):
    """
    tokens: list of (token_id, token_text, token_type) sorted by offset.
    Returns list of new (token_id, token_text, token_type) to insert as chain markers.
    """
    result = []
    counter = [0]

    def new_tid():
        counter[0] += 1
        return str(abs(hash(f"chain_{counter[0]}")) % 10 ** 10)

    i = 0
    n = len(tokens)

    while i < n:
        tid, ttext, ttype = tokens[i]

        if ttype in ATOM_TYPES or ttype in GROUP_TYPES:
            # Look ahead for op-atom chains
            chain_ops = []
            j = i + 1

            while j < n:
                next_type = tokens[j][2]
                next_text = tokens[j][1]

                if next_type == "math_op" and j + 1 < n:
                    after = tokens[j + 1]
                    if after[2] in ATOM_TYPES or after[2] in GROUP_TYPES:
                        chain_ops.append(next_text)
                        j += 2
                        continue
                break

            if len(chain_ops) >= 1:
                is_mult = all(
                    op in MULTIPLICATIVE_OPS for op in chain_ops
                )
                is_add = all(
                    op in ADDITIVE_OPS for op in chain_ops
                )

                if is_mult or is_add:
                    ctype = "mp" if is_mult else "additive"
                    chain_tid = new_tid()
                    result.append((chain_tid, f"{ctype}_begin", f"{ctype}_begin"))

                    # Mark all tokens in the chain segment
                    k = i
                    while k < j:
                        k += 1

                    result.append((new_tid(), f"{ctype}_end", f"{ctype}_end"))
                    i = j
                    continue

        i += 1

    return result


def main():
    os.system(f"cp {DB_PATH} {DB_PATH}.bak6")

    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()

    # Clear old chain tokens
    cursor.execute(
        "DELETE FROM authors WHERE type IN ('additive_begin','additive_end','mp_begin','mp_end')"
    )
    print(f"Cleared {cursor.rowcount} old chain tokens")

    # Get all math block IDs
    cursor.execute(
        """SELECT DISTINCT token_id FROM authors 
           WHERE type IN ('math','display_math','display_2_math','equation',
           'align','gather','multline','split')"""
    )
    math_ids = {r[0] for r in cursor.fetchall()}
    print(f"Found {len(math_ids)} math blocks")

    # Get ALL children of math blocks in one query
    cursor.execute(
        """SELECT parent_id, CAST(offset AS INTEGER), type, token, token_id
           FROM authors 
           WHERE parent_id IN ({})
           AND type IN ('math_var','math_greek','math_sym','math_num','math_fn',
                        'math_op','math_rel','parens','braces','abs','norm',
                        'floor','ceil','frac','accent','binom','sqrt',
                        'scientific','percent')
           ORDER BY parent_id, CAST(offset AS INTEGER)""".format(
            ",".join(f"'{m}'" for m in math_ids)
        )
    )
    all_children = cursor.fetchall()
    print(f"Found {len(all_children)} math block children")

    # Group by parent_id
    from collections import defaultdict
    groups = defaultdict(list)
    for parent_id, offset, typ, tok_text, tok_id in all_children:
        groups[parent_id].append((tok_id, tok_text, typ))

    # Process each group
    total_chains = 0
    batch = []
    batch_limit = 5000

    for parent_id, tokens in groups.items():
        if len(tokens) < 3:
            continue

        chains = detect_chains(tokens)
        if not chains:
            continue

        # Get filepath for this block
        cursor.execute(
            "SELECT filepath, filepath_id FROM authors WHERE token_id=? LIMIT 1",
            (parent_id,),
        )
        fp_row = cursor.fetchone()
        if not fp_row:
            continue
        filepath, fp_id = fp_row

        total_chains += len(chains) // 2

        for tid, ttext, ttype in chains:
            batch.append((filepath, fp_id, tid, parent_id, "0", str(len(ttext)), ttype, ttext))

        if len(batch) >= batch_limit:
            cursor.executemany(
                "INSERT OR IGNORE INTO authors (filepath, filepath_id, token_id, parent_id, offset, length, type, token) VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
                batch,
            )
            conn.commit()
            print(f"  Inserted {len(batch)} chain tokens...")
            batch = []

    if batch:
        cursor.executemany(
            "INSERT OR IGNORE INTO authors (filepath, filepath_id, token_id, parent_id, offset, length, type, token) VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
            batch,
        )
        conn.commit()

    conn.close()
    print(f"\nDone. {total_chains} chains added ({total_chains * 2} begin/end tokens).")


if __name__ == "__main__":
    main()
