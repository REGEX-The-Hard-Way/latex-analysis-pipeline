#!/usr/bin/env python3
"""
Load sentences (from sent_split.py output) into SQLite for NLP processing.

Each sentence is stored with its source file, sentence index, and the full
text containing hash IDs. A sentence_tokens table links sentences to their
constituent token IDs for fast lookup.

Usage:
    python3 tools/sent_split.py scanner/sound1.tex sidecar.tok \
        | python3 tools/load_sentences.py --db my_data.db --source sound1.tex
"""
import re
import sqlite3
import sys
import os


def create_tables(cursor):
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS sentences (
            sentence_id  INTEGER PRIMARY KEY AUTOINCREMENT,
            filepath     TEXT NOT NULL,
            sent_index   INTEGER NOT NULL,
            text         TEXT NOT NULL,
            token_count  INTEGER NOT NULL DEFAULT 0
        )
    """)
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS sentence_tokens (
            sentence_id  INTEGER NOT NULL REFERENCES sentences(sentence_id),
            token_id     TEXT NOT NULL,
            PRIMARY KEY (sentence_id, token_id)
        )
    """)
    cursor.execute("""
        CREATE INDEX IF NOT EXISTS idx_sent_tokens_tid
        ON sentence_tokens(token_id)
    """)
    cursor.execute("""
        CREATE INDEX IF NOT EXISTS idx_sentences_filepath
        ON sentences(filepath)
    """)


def load_sentences(db_path, source_name, lines):
    conn = sqlite3.connect(db_path)
    conn.execute("PRAGMA journal_mode=WAL")
    cursor = conn.cursor()
    create_tables(cursor)

    id_pattern = re.compile(r'\b(\d{6,})\b')

    batch = []
    for idx, line in enumerate(lines):
        line = line.strip()
        if not line or len(line) < 10:
            continue

        token_ids = id_pattern.findall(line)
        sent_text = line
        batch.append((source_name, idx, sent_text, len(token_ids), '|'.join(token_ids)))

        if len(batch) >= 100:
            cursor.executemany(
                "INSERT INTO sentences (filepath, sent_index, text, token_count) "
                "VALUES (?, ?, ?, ?)",
                [(s[0], s[1], s[2], s[3]) for s in batch])
            conn.commit()
            # Re-select the IDs we just inserted to get correct sentence_ids
            # Use sent_index to find them
            for s in batch:
                cursor.execute(
                    "SELECT sentence_id FROM sentences "
                    "WHERE filepath=? AND sent_index=?",
                    (s[0], s[1]))
                row = cursor.fetchone()
                if row:
                    sent_id = row[0]
                    tids = s[4].split('|') if s[4] else []
                    for tid in tids:
                        cursor.execute(
                            "INSERT OR IGNORE INTO sentence_tokens "
                            "(sentence_id, token_id) VALUES (?, ?)",
                            (sent_id, tid))
            conn.commit()
            batch = []

    if batch:
        cursor.executemany(
            "INSERT INTO sentences (filepath, sent_index, text, token_count) "
            "VALUES (?, ?, ?, ?)",
            [(s[0], s[1], s[2], s[3]) for s in batch])
        conn.commit()
        for s in batch:
            cursor.execute(
                "SELECT sentence_id FROM sentences "
                "WHERE filepath=? AND sent_index=?",
                (s[0], s[1]))
            row = cursor.fetchone()
            if row:
                sent_id = row[0]
                tids = s[4].split('|') if s[4] else []
                for tid in tids:
                    cursor.execute(
                        "INSERT OR IGNORE INTO sentence_tokens "
                        "(sentence_id, token_id) VALUES (?, ?)",
                        (sent_id, tid))
        conn.commit()

    conn.close()
    return len(lines)


def main():
    db_path = "/mnt/x/home/user/my_data.db"
    source = "unknown.tex"
    args = sys.argv[1:]
    i = 0
    while i < len(args):
        if args[i] == '--db' and i + 1 < len(args):
            db_path = args[i + 1]; i += 2
        elif args[i] == '--source' and i + 1 < len(args):
            source = args[i + 1]; i += 2
        else:
            i += 1

    lines = sys.stdin.read().strip().split('\n')
    n = load_sentences(db_path, source, lines)
    print(f"Loaded {n} sentences from {source} into {db_path}", file=sys.stderr)


if __name__ == '__main__':
    main()
