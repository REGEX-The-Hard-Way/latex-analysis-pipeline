#!/usr/bin/env python3
"""
Phase G: Cross-Paper Expression Equivalence

Finds structurally equivalent math expressions across papers by
computing child-type fingerprints and matching across filepaths.
Stores equivalent_to edges in graph_edges.

Algorithm: For each math block, count child token types, sort
alphabetically, and produce a signature like 'math_greek.2|math_op.1'.
Two blocks with the same signature from different papers are candidates
for equivalence.
"""
import sqlite3
import hashlib
import sys
from collections import defaultdict

DB_PATH = "/mnt/x/home/user/my_data.db"

MATH_BLOCK_TYPES = {
    'math', 'display_math', 'display_2_math', 'equation',
    'align', 'gather', 'multline', 'split', 'eqnarray',
}

CHILD_TOKEN_TYPES = {
    'math_var', 'math_greek', 'math_sym', 'math_num', 'math_fn',
    'math_op', 'math_rel', 'accent', 'parens', 'braces', 'frac',
    'sqrt', 'math_sub', 'math_sup', 'sum', 'prod', 'lim', 'int',
    'binom', 'abs', 'norm', 'floor', 'ceil', 'scientific',
    'matrix_col_delim', 'matrix_row_delim',
}


def compute_fingerprint(children):
    """children: list of token types. Returns 'type1.N|type2.N|...' """
    counts = defaultdict(int)
    for ctype in children:
        counts[ctype] += 1
    parts = [f"{t}.{counts[t]}" for t in sorted(counts)]
    return '|'.join(parts)


def main(dry_run=False, min_matches=2):
    conn = sqlite3.connect(DB_PATH)
    conn.execute("PRAGMA journal_mode=WAL")
    cursor = conn.cursor()

    # 1. Get all math blocks with their filepaths (keep all rows, don't dedup by token_id)
    type_list = ','.join(f"'{t}'" for t in MATH_BLOCK_TYPES)
    cursor.execute(f"""
        SELECT token_id, type, filepath, filepath_id, CAST(offset AS INTEGER)
        FROM authors
        WHERE type IN ({type_list})
        ORDER BY filepath, CAST(offset AS INTEGER)
    """)
    block_rows = cursor.fetchall()
    # Use (token_id, filepath) as key since token_id is not unique
    blocks = {}
    for r in block_rows:
        key = (r[0], r[2])  # (token_id, filepath)
        blocks[key] = r
    print(f"Math blocks: {len(block_rows)} rows, {len(blocks)} unique (token_id, filepath) pairs")

    # 2. Get children for all math blocks
    child_list = ','.join(f"'{t}'" for t in CHILD_TOKEN_TYPES)
    cursor.execute(f"""
        SELECT parent_id, type
        FROM authors
        WHERE type IN ({child_list})
        ORDER BY parent_id, CAST(offset AS INTEGER)
    """)
    children_by_parent = defaultdict(list)
    for parent_id, ctype in cursor.fetchall():
        children_by_parent[parent_id].append(ctype)

    # 3. Compute fingerprints and group
    fp_to_blocks = defaultdict(list)
    total_with_children = 0
    for (token_id, fp), (_, btype, fp_path, fpid, _) in blocks.items():
        children = children_by_parent.get(token_id, [])
        if len(children) < 2:
            continue
        total_with_children += 1
        sig = compute_fingerprint(children)
        fp_to_blocks[sig].append((token_id, btype, fp_path, fpid))

    print(f"Blocks with children: {total_with_children}")
    print(f"Unique fingerprints: {len(fp_to_blocks)}")

    # 4. Find cross-paper matches
    cross_paper = []
    single_paper = 0
    top_fps = []

    for fp, block_list in fp_to_blocks.items():
        # Count unique filepaths
        filepaths = {b[2] for b in block_list}
        if len(filepaths) >= min_matches:
            cross_paper.append((fp, block_list, len(filepaths)))
        elif len(block_list) > 1:
            # Same paper, multiple occurrences
            pass
        else:
            single_paper += 1

        if len(block_list) >= 3:
            top_fps.append((fp, len(block_list)))

    cross_paper.sort(key=lambda x: -x[2])

    print(f"\n=== Cross-paper fingerprint matches ===")
    print(f"Fingerprints appearing in >= {min_matches} papers: {len(cross_paper)}")
    print(f"Unique fingerprints (single paper only): {single_paper}")
    print()

    # Show top fingerprints
    print("Top fingerprints by occurrence count:")
    for fp, count in sorted(top_fps, key=lambda x: -x[1])[:15]:
        blocks_with_fp = fp_to_blocks[fp]
        filepaths = {b[2] for b in blocks_with_fp}
        # Show sample math types
        types = defaultdict(int)
        for _, btype, _, _ in blocks_with_fp:
            types[btype] += 1
        type_str = ', '.join(f'{t}:{c}' for t, c in sorted(types.items()))
        print(f"  [{count:4d}x in {len(filepaths):3d} papers] {fp}")
        print(f"         types: {type_str[:80]}")

    # 5. Show cross-paper matches
    print(f"\n=== Cross-paper matches (>= {min_matches} papers) ===")
    for fp, block_list, n_papers in cross_paper[:10]:
        print(f"\n  Fingerprint: {fp}")
        print(f"  Papers: {n_papers}, Occurrences: {len(block_list)}")
        for token_id, btype, fp_path, fpid in block_list[:5]:
            short = fp_path.split('/')[-1][:50]
            print(f"    {short:50s} [{btype}]")

    # 6. Insert equivalent_to edges
    if not dry_run:
        print(f"\n=== Inserting equivalent_to edges ===")
        edge_count = 0
        batch = []
        counter = 0

        for fp, block_list, n_papers in cross_paper:
            # Link every pair of blocks from different papers
            for i in range(len(block_list)):
                for j in range(i + 1, len(block_list)):
                    a = block_list[i]
                    b = block_list[j]
                    if a[2] == b[2]:  # same filepath, skip
                        continue

                    counter += 1
                    edge_id = f"equiv_{counter:08d}"
                    batch.append((
                        edge_id,
                        'expression', str(a[0]),
                        a[2], str(a[3]),
                        'expression', str(b[0]),
                        b[2], str(b[3]),
                        str(b[0]),
                        'equivalent_to',
                        f'structural_match:{fp}',
                    ))

            if len(batch) >= 5000:
                cursor.executemany("""
                    INSERT OR IGNORE INTO graph_edges
                    (edge_id, source_type, source_key, source_filepath,
                     source_filepath_id, target_type, target_key,
                     target_filepath, target_filepath_id, target_token_id,
                     relationship, status)
                    VALUES (?,?,?,?,?,?,?,?,?,?,?,?)
                """, batch)
                conn.commit()
                edge_count += len(batch)
                print(f"  Inserted {edge_count} edges...")
                batch = []

        if batch:
            cursor.executemany("""
                INSERT OR IGNORE INTO graph_edges
                (edge_id, source_type, source_key, source_filepath,
                 source_filepath_id, target_type, target_key,
                 target_filepath, target_filepath_id, target_token_id,
                 relationship, status)
                VALUES (?,?,?,?,?,?,?,?,?,?,?,?)
            """, batch)
            conn.commit()
            edge_count += len(batch)

        print(f"  Total: {edge_count} equivalent_to edges inserted")

    conn.close()

    # Summary
    print(f"\n=== Phase G Summary ===")
    print(f"Math blocks:           {len(blocks)}")
    print(f"Blocks with children:  {total_with_children}")
    print(f"Unique fingerprints:   {len(fp_to_blocks)}")
    print(f"Cross-paper matches:   {len(cross_paper)}")
    print(f"Fingerprints covering >1 paper: {len(cross_paper)}")
    return len(cross_paper)


if __name__ == '__main__':
    dry = '--dry-run' in sys.argv
    min_m = 2
    for a in sys.argv[1:]:
        if a.startswith('--min-papers='):
            min_m = int(a.split('=')[1])
    main(dry_run=dry, min_matches=min_m)
