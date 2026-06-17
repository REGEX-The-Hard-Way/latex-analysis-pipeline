"""
Cross-document graph builder: resolves cite→bibitem, ref→label, and
builds author→children chains into an explicit graph_edges table.
"""
import sqlite3
import re
import os
import hashlib

DB_PATH = "/mnt/x/home/user/my_data.db"
SCAN_DIR = "/home/user/latex-analysis-pipeline/scanner/2003"


def fp_hash(filepath):
    return str(abs(hash(filepath)) % 10 ** 10)


def tok_hash(base, counter):
    return str(abs(hash(f"{base}_{counter}")) % 10 ** 10)


def parse_tex_file(filepath):
    """Extract cite, ref, label keys from a .tex file."""
    encodings = ["ISO-8859-1", "utf-8", "latin-1"]
    content = None
    for enc in encodings:
        try:
            with open(filepath, "r", encoding=enc) as f:
                content = f.read()
            break
        except (UnicodeDecodeError, FileNotFoundError):
            continue
    if content is None:
        return [], [], []

    cites = []
    refs = []
    labels = []

    # Match \cite{...}, \citep{...}, \citet{...}, etc. (comma-separated keys)
    for m in re.finditer(
        r"\\(?:cite|citep|citet|citeauthor|citeyear|citealp|autocite|textcite|parencite|footcite|nocite)\s*\{([^}]+)\}",
        content,
    ):
        cmd = m.group(0).split("{")[0].strip()
        keys_raw = m.group(1)
        offset = m.start()
        for key in re.split(r"\s*,\s*", keys_raw):
            key = key.strip()
            if key:
                cites.append((cmd, key, offset))

    # Match \ref{...}, \eqref{...}
    for m in re.finditer(r"\\(?:ref|eqref|pageref)\s*\{([^}]+)\}", content):
        cmd = m.group(0).split("{")[0].strip()
        key = m.group(1).strip()
        offset = m.start()
        if key:
            refs.append((cmd, key, offset))

    # Match \label{...}
    for m in re.finditer(r"\\label\s*\{([^}]+)\}", content):
        key = m.group(1).strip()
        offset = m.start()
        if key:
            labels.append(("label", key, offset))

    return cites, refs, labels


def main():
    os.system(f"cp {DB_PATH} {DB_PATH}.bak4")
    print(f"Backup: {DB_PATH}.bak4")

    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()

    # Create graph_edges table
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS graph_edges (
            edge_id TEXT PRIMARY KEY,
            source_type TEXT,
            source_key TEXT,
            source_filepath TEXT,
            source_filepath_id TEXT,
            target_type TEXT,
            target_key TEXT,
            target_filepath TEXT,
            target_filepath_id TEXT,
            target_token_id TEXT,
            relationship TEXT,
            status TEXT
        )
    """)

    # Build index: bibitem key → (filepath, token_id)
    cursor.execute("SELECT token, filepath, token_id FROM authors WHERE type='bibitem'")
    bibitem_map = {}
    for token, fp, tid in cursor.fetchall():
        m = re.search(r"\\bibitem\{([^}]+)\}", token)
        if m:
            key = m.group(1).strip()
            if key not in bibitem_map:
                bibitem_map[key] = []
            bibitem_map[key].append((fp, tid))
    print(f"Bibitem index: {len(bibitem_map)} unique keys")

    # Build index: label → (filepath, token_id) - from tex files
    label_map = {}  # Will be populated during scan

    tex_files = sorted(
        [os.path.join(SCAN_DIR, f) for f in os.listdir(SCAN_DIR) if f.endswith(".tex")]
    )
    print(f"Scanning {len(tex_files)} .tex files...")

    total_cites = 0
    total_refs = 0
    total_labels = 0
    resolved_cites = 0
    resolved_refs = 0
    cited_bibitems = set()
    cross_doc_cites = 0

    for filepath in tex_files:
        cites, refs, labels = parse_tex_file(filepath)
        fp_id = fp_hash(filepath)
        total_cites += len(cites)
        total_refs += len(refs)
        total_labels += len(labels)

        # Register labels from this file
        for cmd, key, offset in labels:
            if key not in label_map:
                label_map[key] = []
            label_map[key].append(filepath)

        # Resolve citations
        for cmd, key, offset in cites:
            if key in bibitem_map:
                resolved_cites += 1
                for target_fp, target_tid in bibitem_map[key]:
                    edge_id = tok_hash(f"cite:{filepath}:{key}:{target_tid}", len(cited_bibitems))
                    cited_bibitems.add(edge_id)

                    is_cross_doc = 0
                    if os.path.basename(target_fp) != os.path.basename(filepath):
                        cross_doc_cites += 1
                        is_cross_doc = 1

                    cursor.execute(
                        """INSERT OR IGNORE INTO graph_edges
                        (edge_id, source_type, source_key, source_filepath, source_filepath_id,
                         target_type, target_key, target_filepath, target_filepath_id,
                         target_token_id, relationship, status)
                        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)""",
                        (
                            edge_id,
                            cmd,
                            key,
                            filepath,
                            fp_id,
                            "bibitem",
                            key,
                            target_fp,
                            fp_hash(target_fp),
                            target_tid,
                            "cites",
                            "resolved",
                        ),
                    )
            else:
                edge_id = tok_hash(f"cite:{filepath}:{key}:unresolved", len(cited_bibitems))
                cited_bibitems.add(edge_id)
                cursor.execute(
                    """INSERT OR IGNORE INTO graph_edges
                    (edge_id, source_type, source_key, source_filepath, source_filepath_id,
                     target_type, target_key, target_filepath, target_filepath_id,
                     target_token_id, relationship, status)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)""",
                    (
                        edge_id,
                        cmd,
                        key,
                        filepath,
                        fp_id,
                        "bibitem",
                        key,
                        "",
                        "",
                        "",
                        "cites",
                        "unresolved",
                    ),
                )

    # Resolve refs → labels (labels accumulated across all files)
    for filepath in tex_files:
        cites, refs, labels = parse_tex_file(filepath)
        fp_id = fp_hash(filepath)

        for cmd, key, offset in refs:
            if key in label_map:
                resolved_refs += 1
                for target_fp in label_map[key]:
                    edge_id = tok_hash(f"ref:{filepath}:{key}:{target_fp}", len(cited_bibitems))
                    cursor.execute(
                        """INSERT OR IGNORE INTO graph_edges
                        (edge_id, source_type, source_key, source_filepath, source_filepath_id,
                         target_type, target_key, target_filepath, target_filepath_id,
                         target_token_id, relationship, status)
                        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)""",
                        (
                            edge_id,
                            cmd,
                            key,
                            filepath,
                            fp_id,
                            "label",
                            key,
                            target_fp,
                            fp_hash(target_fp),
                            "",
                            "refers_to",
                            "resolved",
                        ),
                    )
            else:
                edge_id = tok_hash(f"ref:{filepath}:{key}:unresolved", len(cited_bibitems))
                cursor.execute(
                    """INSERT OR IGNORE INTO graph_edges
                    (edge_id, source_type, source_key, source_filepath, source_filepath_id,
                     target_type, target_key, target_filepath, target_filepath_id,
                     target_token_id, relationship, status)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)""",
                    (
                        edge_id,
                        cmd,
                        key,
                        filepath,
                        fp_id,
                        "label",
                        key,
                        "",
                        "",
                        "",
                        "refers_to",
                        "unresolved",
                    ),
                )

    # Build author → children explicit edges
    cursor.execute(
        """SELECT a.token_id, a.filepath, a.token
           FROM authors a WHERE a.type='author'"""
    )
    author_edges = 0
    for author_id, author_fp, author_token in cursor.fetchall():
        fp_id = fp_hash(author_fp)

        # Find all children of this author
        cursor.execute(
            """SELECT type, token_id, token FROM authors
               WHERE parent_id=? AND type IN ('footnote','email','address','thanks')""",
            (author_id,),
        )
        children = cursor.fetchall()
        for child_type, child_tid, child_token in children:
            edge_id = tok_hash(f"author_child:{author_id}:{child_tid}", author_edges)
            author_edges += 1
            cursor.execute(
                """INSERT OR IGNORE INTO graph_edges
                (edge_id, source_type, source_key, source_filepath, source_filepath_id,
                 target_type, target_key, target_filepath, target_filepath_id,
                 target_token_id, relationship, status)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)""",
                (
                    edge_id,
                    "author",
                    author_id,
                    author_fp,
                    fp_id,
                    child_type,
                    child_tid,
                    author_fp,
                    fp_id,
                    child_tid,
                    "has_child",
                    "resolved",
                ),
            )

    conn.commit()

    # Statistics
    cursor.execute("SELECT COUNT(*) FROM graph_edges")
    total_edges = cursor.fetchone()[0]
    cursor.execute("SELECT COUNT(*) FROM graph_edges WHERE relationship='cites'")
    cite_edges = cursor.fetchone()[0]
    cursor.execute("SELECT COUNT(*) FROM graph_edges WHERE relationship='refers_to'")
    ref_edges = cursor.fetchone()[0]
    cursor.execute("SELECT COUNT(*) FROM graph_edges WHERE relationship='has_child'")
    child_edges = cursor.fetchone()[0]
    cursor.execute("SELECT COUNT(*) FROM graph_edges WHERE status='unresolved'")
    unresolved = cursor.fetchone()[0]

    conn.close()

    print(f"\n=== Cross-Document Graph Summary ===")
    print(f"Total .tex files scanned: {len(tex_files)}")
    print(f"Citations found:  {total_cites}")
    print(f"References found: {total_refs}")
    print(f"Labels found:     {total_labels}")
    print(f"")
    print(f"Graph edges created: {total_edges}")
    print(f"  cite→bibitem:   {cite_edges}  (resolved: {resolved_cites})")
    print(f"  ref→label:      {ref_edges}   (resolved: {resolved_refs})")
    print(f"  author→child:   {child_edges}")
    print(f"  cross-document cites: {cross_doc_cites}")
    print(f"  unresolved:     {unresolved}")
    print(f"")
    print(f"Bibitem index:   {len(bibitem_map)} unique keys")
    print(f"Label index:     {len(label_map)} unique labels")


if __name__ == "__main__":
    main()
