"""
Load all scanner .tok output into the database, including math tokens
that were previously filtered out during original ingestion.
Uses INSERT OR IGNORE to avoid duplicating existing tokens.
"""
import sqlite3
import re
import pandas as pd
import os

DB_PATH = "/mnt/x/home/user/my_data.db"
TOK_FILE = "/tmp/sidecar.tok"


def main():
    os.system(f"cp {DB_PATH} {DB_PATH}.bak5")
    print(f"Backup: {DB_PATH}.bak5")

    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()

    # Count existing tokens
    cursor.execute("SELECT COUNT(*) FROM authors")
    before_count = cursor.fetchone()[0]
    print(f"Existing tokens before load: {before_count}")

    # Parse .tok file
    pattern = re.compile(
        r"\{<filepath:(.*?)>,filepath_id:(\d+),token_id:(\d+),parent_id:(\d+),"
        r"offset:(-?\d+),length:(\d+),type:([a-z_0-9]+),<tok:(.*?)>\}",
        re.DOTALL,
    )

    count = 0
    batch = []
    batch_size = 5000

    with open(TOK_FILE, "r", encoding="ISO-8859-1") as f:
        content = f.read()

    matches = pattern.findall(content)
    print(f"Parsed {len(matches)} tokens from {TOK_FILE}")

    # Filter out prose tokens (words, spaces, newlines) — keep only typed tokens
    skip_types = {"sc_prose"}  # prose text matches, not useful as tokens

    for m in matches:
        filepath, fp_id, tok_id, parent_id, offset, length, typ, tok_text = m
        if typ in skip_types:
            continue
        batch.append(
            (
                filepath,
                fp_id,
                tok_id,
                parent_id,
                offset,
                length,
                typ,
                tok_text,
            )
        )
        count += 1

        if len(batch) >= batch_size:
            df = pd.DataFrame(
                batch,
                columns=[
                    "filepath", "filepath_id", "token_id", "parent_id",
                    "offset", "length", "type", "token",
                ],
            )
            # Use INSERT OR IGNORE to skip duplicates
            for _, row in df.iterrows():
                try:
                    cursor.execute(
                        "INSERT OR IGNORE INTO authors (filepath, filepath_id, token_id, parent_id, offset, length, type, token) VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
                        (row["filepath"], row["filepath_id"], row["token_id"],
                         row["parent_id"], row["offset"], row["length"],
                         row["type"], row["token"]),
                    )
                except:
                    pass
            batch = []
            conn.commit()
            print(f"  Loaded {count} tokens...")

    if batch:
        for _, row in pd.DataFrame(batch, columns=[
            "filepath", "filepath_id", "token_id", "parent_id",
            "offset", "length", "type", "token",
        ]).iterrows():
            try:
                cursor.execute(
                    "INSERT OR IGNORE INTO authors (filepath, filepath_id, token_id, parent_id, offset, length, type, token) VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
                    (row["filepath"], row["filepath_id"], row["token_id"],
                     row["parent_id"], row["offset"], row["length"],
                     row["type"], row["token"]),
                )
            except:
                pass
        conn.commit()

    cursor.execute("SELECT COUNT(*) FROM authors")
    after_count = cursor.fetchone()[0]
    print(f"Tokens after load: {after_count} (added {after_count - before_count})")

    # Show type breakdown
    cursor.execute(
        "SELECT type, COUNT(*) FROM authors GROUP BY type ORDER BY COUNT(*) DESC LIMIT 30"
    )
    print("\nTop 30 token types:")
    for r in cursor.fetchall():
        print(f"  {r[0]}: {r[1]}")

    conn.close()
    print("\nDone.")


if __name__ == "__main__":
    main()
