"""
Phase C: Operator precedence chain detection.
Post-processes flat math token streams to insert additive/multiplicative
bracketing tokens, enabling AST reconstruction.
"""
import sqlite3
import re

DB_PATH = "/mnt/x/home/user/my_data.db"

# PS.g4 precedence: expr → additive → mp → unary → postfix → exp → comp → atom
# We detect chains at two levels: additive (+/-) and multiplicative (*/×/·)
ADDITIVE_OPS = {"+", "-", "\\pm", "\\mp"}
MULTIPLICATIVE_OPS = {
    "*", "/", "\\times", "\\cdot", "\\div", "\\ast", "\\star",
    "\\circ", "\\bullet", "\\oplus", "\\otimes", "\\odot",
    "\\cap", "\\cup", "\\setminus", "\\wedge", "\\vee",
}

ATOM_TYPES = {
    "math_var", "math_greek", "math_sym", "math_num", "math_fn",
    "scientific", "percent",
}
GROUP_START = {
    "parens", "braces", "abs", "norm", "floor", "ceil",
    "group", "brackets",
}
OP_TYPES = {"math_op", "math_rel"}


def token_text_is_op(token_text, op_set):
    """Check if a math_op token matches an operator in the set."""
    return token_text.strip() in op_set


def emit_bracketed(tokens, start_id_counter):
    """
    Given a flat list of (token_id, token_text, token_type) tuples from inside
    a math block, insert additive/multiplicative bracketing tokens.
    """
    result = []
    counter = [start_id_counter]

    def new_tid():
        counter[0] += 1
        return str(abs(hash(f"chain_{counter[0]}")) % 10 ** 10)

    i = 0
    n = len(tokens)

    while i < n:
        tid, ttext, ttype = tokens[i]

        if ttype in ATOM_TYPES or ttype in GROUP_START:
            # Look ahead: is this atom followed by an operator chain?
            chain_ops = []
            chain_segments = [tokens[i]]
            j = i + 1

            while j < n:
                next_tid, next_text, next_type = tokens[j]
                if next_type == "math_op" and j + 1 < n:
                    after_op = tokens[j + 1]
                    if after_op[2] in ATOM_TYPES or after_op[2] in GROUP_START:
                        chain_ops.append(next_text)
                        chain_segments.append(tokens[j])       # op
                        chain_segments.append(tokens[j + 1])   # atom
                        j += 2
                        continue
                break

            if len(chain_ops) > 0:
                # Determine dominant chain type
                is_mult = all(
                    token_text_is_op(op, MULTIPLICATIVE_OPS) for op in chain_ops
                )
                is_add = all(
                    token_text_is_op(op, ADDITIVE_OPS) for op in chain_ops
                )

                if is_mult or is_add:
                    chain_type = "mp" if is_mult else "additive"
                    chain_tid = new_tid()

                    result.append((chain_tid, f"{chain_type}_begin", f"{chain_type}_begin"))

                    for seg_tid, seg_text, seg_type in chain_segments:
                        result.append((seg_tid, seg_text, seg_type))

                    result.append((new_tid(), f"{chain_type}_end", f"{chain_type}_end"))
                    i = j
                    continue

        result.append((tid, ttext, ttype))
        i += 1

    return result


def main():
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()

    # Find all math blocks (inline math and display math)
    cursor.execute(
        """SELECT token_id, filepath, filepath_id, parent_id, token
           FROM authors WHERE type IN ('math','display_math','display_2_math',
           'equation','align','gather','multline','split','aligned','gathered')"""
    )
    math_blocks = cursor.fetchall()
    print(f"Found {len(math_blocks)} math blocks")

    # Count existing chain tokens
    cursor.execute(
        "SELECT COUNT(*) FROM authors WHERE type IN ('additive_begin','additive_end','mp_begin','mp_end')"
    )
    existing = cursor.fetchone()[0]
    if existing > 0:
        cursor.execute(
            "DELETE FROM authors WHERE type IN ('additive_begin','additive_end','mp_begin','mp_end')"
        )
        print(f"Cleared {existing} existing chain tokens")

    total_chains = 0
    blocks_processed = 0

    for block_id, filepath, fp_id, parent_id, token_text in math_blocks:
        # Get all child tokens of this math block, ordered by offset
        cursor.execute(
            """SELECT token_id, CAST(offset AS INTEGER) as off, type, token
               FROM authors WHERE parent_id=? 
               AND type NOT IN ('math_sub','math_sup','comment')
               ORDER BY off""",
            (block_id,),
        )
        children = cursor.fetchall()
        if len(children) < 3:
            continue

        # Build flat token list
        flat = [(tid, tok, typ) for tid, off, typ, tok in children]

        # Run chain detection
        bracketed = emit_bracketed(flat, total_chains)

        # If chains were found, insert begin/end tokens
        new_tokens = [b for b in bracketed if b[2] in ("additive_begin", "additive_end", "mp_begin", "mp_end")]
        if not new_tokens:
            continue

        blocks_processed += 1
        total_chains += len(new_tokens) // 2

        for tid, ttext, ttype in new_tokens:
            cursor.execute(
                """INSERT OR IGNORE INTO authors
                (filepath, filepath_id, token_id, parent_id, offset, length, type, token)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?)""",
                (filepath, fp_id, tid, block_id, "0", str(len(ttext)), ttype, ttext),
            )

        if blocks_processed % 500 == 0:
            conn.commit()
            print(f"  {blocks_processed} blocks processed, {total_chains} chains...")

    conn.commit()
    conn.close()

    print(f"\nDone. {blocks_processed} math blocks modified, {total_chains} chains added.")
    print(f"New token types: additive_begin/end, mp_begin/end")


if __name__ == "__main__":
    main()
