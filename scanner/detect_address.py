#!/usr/bin/env python3
"""
detect_address.py

Reads /tmp/authors (one author block per line, each being the content
inside \\author{...} in LaTeX), matches each block to a row in
my_data.db (table authors, type='author'), detects embedded address
text, inserts address rows linked to the parent author, and strips the
address from the author token.

Usage:
    python3 detect_address.py
"""

import re
import sqlite3
import shutil
import sys

DB_PATH = "/mnt/x/home/user/my_data.db"
DB_BAK = "/mnt/x/home/user/my_data.db.bak2"
AUTHORS_FILE = "/tmp/authors"

# ── Address-indicating keywords ──────────────────────────────────────
_ADDR_KW = [
    "Department", "University", "Universidade", "Universidad",
    "Universitat", "Universit", "Institute", "Laboratory",
    "Laboratoire", "Laboratori", "College", "School", "Center",
    "Centre", "Faculty", "Facult", "Faculdade", "INFN", "CERN",
    "P.O. Box", "P.O.Box", "P. O. Box", "Street", "Road", "Avenue",
    "Observatoire", "Academy", "DESY", "JINR", "SISSA",
    "Perimeter Institute", "Jefferson Laboratory", "Racah Institute",
    "Max-Planck", "Steklov", "Saha Institute", "Harish-chandra",
    "Tata Institute", "Bogoliubov", "CBPF", "ICTP", "DAMTP",
    "KEK", "LPTHE", "LPTMC", "IHES", "NORDITA", "SUBATECH",
    "ITEP", "SLAC", "MPI/AEI",
]

_COUNTRY_RX = re.compile(
    r"\b(?:Canada|France|Italy|Germany|USA|U\.?S\.?A\.?|Russia|Spain|"
    r"Brazil|Japan|China|India|UK|U\.?K\.?|Sweden|Switzerland|Israel|"
    r"Iran|Korea|Taiwan|Poland|Portugal|Ukraine|Belgium|Netherlands|"
    r"Mexico|Hungary|Romania|Turkey|Finland|Greece|Norway|Denmark|"
    r"Ireland|Chile|Argentina|Yugoslavia|Serbia|Croatia|Bulgaria|"
    r"Czech|Slovak|Austria|Australia|New Zealand|Armenia|Georgia|"
    r"Morocco|Jordan|South Africa|Indonesia|Thailand|Vietnam|Egypt|"
    r"Belarus|Estonia|Latvia|Lithuania|Peru|Venezuela|Pakistan|"
    r"Bangladesh|Nepal|Sri Lanka|Malaysia|Singapore|Philippines)\b",
    re.IGNORECASE,
)


# ── Helper: balanced-brace extraction ────────────────────────────────
def _balanced_extract(text: str, start: int, lbrace: str = "{", rbrace: str = "}") -> tuple:
    """Return (content_inside, position_after_closing_brace)."""
    depth = 0
    for i in range(start, len(text)):
        ch = text[i]
        if ch == lbrace:
            depth += 1
        elif ch == rbrace:
            depth -= 1
            if depth == 0:
                return text[start + 1 : i], i + 1
    return None, start


def strip_balanced_outer(s: str) -> str:
    """Strip one layer of { } if the whole string is balanced-wrapped."""
    s = s.strip()
    if not (s.startswith("{") and s.endswith("}")):
        return s
    content, end = _balanced_extract(s, 0)
    if content is not None and end == len(s):
        return content.strip()
    return s


# ── Normalisation helpers ────────────────────────────────────────────
def _normalise(s: str) -> str:
    """Normalise a string for fuzzy matching."""
    # Remove \\author{…} wrapper if present
    t = s.strip()
    if t.startswith("\\author{") and t.endswith("}"):
        content, end = _balanced_extract(t, 7, "{", "}")
        if content is not None and end == len(t):
            t = content
    # Collapse all line-break / whitespace variations
    t = t.replace("\\\\", " ")   # LaTeX \\   → space
    t = t.replace("\n", " ")
    t = t.replace("\r", " ")
    t = t.replace("\t", " ")
    t = re.sub(r"\s+", " ", t).strip()
    return t


def _normalise_full(s: str) -> str:
    """Extra-aggressive normalisation: also strip outer braces."""
    return _normalise(strip_balanced_outer(s))


# ── Address keyword check ───────────────────────────────────────────
def has_address_content(text: str) -> bool:
    """True when *text* contains enough address-like signals."""
    if not text or len(text) < 10:
        return False
    t = text.lower()
    for kw in _ADDR_KW:
        if kw.lower() in t:
            return True
    if _COUNTRY_RX.search(text):
        return True
    # Postal-code heuristic only when also street-ish
    if re.search(r"\b\d{5}(?:[-\s]\d{4})?\b", text) and re.search(
        r"\b(?:Street|Road|Avenue|St\.|Rd\.|Ave\.|Blvd|Drive|Lane|Way|Place|"
        r"Square|Park|Row|Court|Circle|Highway|Route|Boulevard)\b", text, re.IGNORECASE
    ):
        return True
    return False


# ── Stripping non-address constructs ─────────────────────────────────
def _remove_thanks(text: str) -> str:
    out, i = [], 0
    while i < len(text):
        if text[i : i + 8] == "\\thanks{":
            _, i = _balanced_extract(text, i + 7)
            continue
        out.append(text[i])
        i += 1
    return "".join(out)


def _remove_footnote(text: str) -> str:
    out, i = [], 0
    while i < len(text):
        if text[i : i + 10] == "\\footnote{":
            _, i = _balanced_extract(text, i + 9)
            continue
        if text[i : i + 13] == "\\footnotemark":
            j = i + 13
            if j < len(text) and text[j] == "[":
                k = text.find("]", j)
                if k != -1:
                    i = k + 1
                    continue
            i = j
            continue
        out.append(text[i])
        i += 1
    return "".join(out)


def _remove_emails(text: str) -> str:
    c = text
    c = re.sub(r"\\email\{[^}]*\}", "", c)
    c = re.sub(r"[Ee]-?mail\s*:\s*[^\n\\]*(?:\\[^\n\\]*)?", "", c)
    c = re.sub(r"\\texttt\{[^@]*@[^}]*\}", "", c)
    c = re.sub(r"\{\\tt\s[^@]*@[^}]*\}", "", c)
    c = re.sub(r"\\pdflink\{mailto:[^}]*\}", "", c)
    # Remove {\small e-mail:...} style
    c = re.sub(r"\{\s*\\small\s+(?:e-?mail|E?-?mail)\s*:\s*[^}]*\}", "", c)
    c = re.sub(r"[Ee]-?mail\s*:?\s*", "", c)
    return c


def _remove_urls(text: str) -> str:
    c = text
    c = re.sub(r"\\url\{[^}]*\}", "", c)
    c = re.sub(r"\bhttps?://\S+", "", c)
    # E-print identifiers
    c = re.sub(r"\b(?:hep-th|hep-ph|gr-qc|math|astro-ph|nucl-th|cond-mat)/\d+", "", c)
    return c


def clean_for_detection(content: str) -> str:
    c = content
    c = _remove_thanks(c)
    c = _remove_footnote(c)
    c = _remove_emails(c)
    c = _remove_urls(c)
    # Remove \date{...}
    c = re.sub(r"\\date\{[^}]*\}", "", c)
    # Remove \keywords{...}
    c = re.sub(r"\\keywords\{.*?(?<!\\)\}", "", c, flags=re.DOTALL)
    # Remove commented lines  %...
    c = re.sub(r"%.*", "", c)
    return c


# ── Separator detection ─────────────────────────────────────────────
# Words that should NOT be treated as author names
_NON_NAME_WORDS = {
    "department", "university", "universidade", "universidad",
    "universitat", "universit", "institute", "laboratory",
    "laboratoire", "laboratori", "college", "school", "center",
    "centre", "faculty", "facult", "faculdade", "infn", "cern",
    "observatoire", "academy", "desy", "jinr", "sissa", "ictp",
    "damtp", "kek", "lpthe", "lptmc", "ihes", "nordita",
    "subatech", "itep", "slac", "cbpf", "steklov", "max", "planck",
    "bogoliubov", "tata", "harish", "chandra", "saha", "racah",
    "perimeter", "jefferson", "mit", "eth", "theory", "theoretical",
    "division", "mathematical", "physics", "chemistry", "science",
    "sciences", "particle", "nuclear", "statistics", "mathematics",
    "applied", "advanced", "studies", "research", "quantum",
    "national", "international", "european", "american", "russian",
    "chinese", "japanese", "french", "german", "italian", "spanish",
}


def _author_name_likely(text: str) -> bool:
    """Heuristic: does *text* very likely start with an author name
    (as opposed to continuing the address)?"""
    t = text.strip()
    if not t:
        return False
    # Strip leading {…} group
    if t.startswith("{"):
        inner, _ = _balanced_extract(t, 0)
        if inner is not None:
            t = inner.strip()
    # Strip optional LaTeX formatting tokens
    t = re.sub(
        r"^(?:\\sc\b|\\bf\b|\\sf\b|\\it\b|\\textbf\b|\\textsf\b|"
        r"\\textit\b|\\textsc\b|\\large\b|\\Large\b|\\small\b|"
        r"\\footnotesize\b|\\normalsize\b|\\rm\b|\\sl\b|\\em\b|"
        r"\\itshape\b|\\scshape\b|\$[\^_]\{[^}]*\}\$|\$\^[a-zA-Z\d]\$)\s*",
        "", t,
        count=1,
    ).strip()
    # Try to extract a person name
    m = re.match(r"([A-Z][a-z]*(?:\.[\s~]?[A-Z]|~[A-Z])?)", t)
    if not m:
        return False
    name_candidate = m.group(1).lower()
    # Reject if it looks like an institution word
    if name_candidate in _NON_NAME_WORDS:
        return False
    # Also reject single letters followed by more content without a dot
    if len(name_candidate) <= 2 and not m.group(1).endswith("."):
        # Short name like "H." is fine, "H " alone is suspicious
        pass  # still accept very short names with dots
    return True


def _contains_author_like(text: str) -> bool:
    """Does *text* contain another \\and or author-name pattern?"""
    if re.search(r"\\and\b", text):
        after = text.split("\\and")[-1].strip()
        if _author_name_likely(after):
            return True
    for m in re.finditer(r"\\\\", text):
        after = text[m.end():].strip()
        if _author_name_likely(after):
            return True
    if _author_name_likely(text):
        return True
    return False


def _find_authors_end(cleaned: str) -> int:
    """Return index where author portion ends and address portion begins.
    -1 when no separation is found."""
    candidates = []

    # ── \vspace{…}, \bigskip, \smallskip, \medskip, \hspace{…} ──
    # These are strong separators – trust them without extra checks.
    for m in re.finditer(r"\\vspace\*?\{[^}]*\}|\\bigskip|\\smallskip|\\medskip|\\hspace\*?\{[^}]*\}", cleaned):
        after = cleaned[m.end():].strip()
        if has_address_content(after) and len(after) > 15:
            candidates.append((m.end(), 10))

    # ── 3+ LaTeX \\  commands ────────────────────────────────────
    # Strong separator – co-authors rarely need 3+ line breaks.
    for m in re.finditer(r"(?:\\\\\s*){3,}", cleaned):
        after = cleaned[m.end():].strip()
        if has_address_content(after) and len(after) > 15:
            candidates.append((m.end(), 8))

    # ── 2  LaTeX \\  (ambiguous – may separate co-authors) ───────
    for m in re.finditer(r"(?:\\\\\s*){2}", cleaned):
        after = cleaned[m.end():].strip()
        if has_address_content(after) and len(after) > 15:
            if not _contains_author_like(after):
                candidates.append((m.end(), 4))

    # ── single \\ (last resort, very ambiguous) ──────────────────
    if not candidates:
        for m in re.finditer(r"\\\\", cleaned):
            after = cleaned[m.end():].strip()
            if has_address_content(after) and len(after) > 20:
                if not _contains_author_like(after):
                    candidates.append((m.end(), 2))

    if candidates:
        # Prefer higher priority, then earliest position
        candidates.sort(key=lambda x: (-x[1], x[0]))
        return candidates[0][0]
    return -1


_FMT_CMDS = (
    r"\\small\b|\\normalsize\b|\\it\b|\\bf\b|\\sf\b|\\sl\b|"
    r"\\sc\b|\\em\b|\\rm\b|\\itshape\b|\\bfseries\b|\\scshape\b|"
    r"\\large\b|\\Large\b|\\footnotesize\b|\\tiny\b|\\scriptsize\b|"
    r"\\textbf\b|\\textsf\b|\\textit\b|\\textsc\b|\\emph\b|"
    r"\\textcolor\b"
)


def _strip_formatting(text: str) -> str:
    """Remove LaTeX formatting commands and outer formatting braces
    from address text, preserving the meaningful content."""
    t = text.strip()

    # Repeatedly strip outermost {…} when they contain formatting commands
    while t.startswith("{"):
        inner, _ = _balanced_extract(t, 0)
        if inner is None:
            break
        if re.match(r"^(?:" + _FMT_CMDS + r")", inner):
            t = inner.strip()
        else:
            break

    # Strip leading formatting commands not wrapped in braces
    t = re.sub(r"^(?:" + _FMT_CMDS + r")\s*", "", t)

    # Remove remaining formatting commands globally (just the command, not args)
    t = re.sub(r"\\(?:small|normalsize|it|bf|sf|sl|sc|em|rm|itshape|bfseries|scshape|"
               r"large|Large|LARGE|footnotesize|tiny|scriptsize|"
               r"textbf|textsf|textit|textsc|emph|textcolor"
               r")\s*", " ", t)
    t = re.sub(r"\s+", " ", t).strip()

    # Strip stray trailing } not balanced by {
    while t.endswith("}") and t.count("{") < t.count("}"):
        t = t[: t.rfind("}")].strip()

    # Strip leading separators
    t = re.sub(
        r"^\s*(?:\\\\\s*|\\vspace\*?\{[^}]*\}\s*|\\\[[\d.]+[a-z]*\\\]\s*|"
        r"\\bigskip\s*|\\smallskip\s*|\\medskip\s*|\\hspace\*?\{[^}]*\}\s*)+",
        "", t,
    )
    # Strip trailing separators
    t = re.sub(
        r"\s*(?:\\\\\s*|\\vspace\*?\{[^}]*\}\s*|\\\[[\d.]+[a-z]*\\\]\s*|"
        r"\\bigskip\s*|\\smallskip\s*|\\medskip\s*|\\hspace\*?\{[^}]*\}\s*)+$",
        "", t,
    ).strip()

    return t


def _extract_trailing_address(content: str, sep: int) -> str | None:
    """Extract a single address block trailing after *sep*."""
    raw = content[sep:].strip()
    raw = re.sub(
        r"^(?:\\\\\s*|\\vspace\*?\{[^}]*\}\s*|\\[\[].*?\\[\]\]\s*|"
        r"\\bigskip\s*|\\smallskip\s*|\\medskip\s*|\\hspace\*?\{[^}]*\}\s*|"
        r"%[^\n]*\s*)+",
        "", raw,
    )
    raw = raw.strip()
    if has_address_content(raw) and len(raw) > 10:
        raw = _strip_formatting(raw)
        return raw
    return None


def extract_addresses(file_line: str):
    """Return (addresses: list[str] | None, author_cleaned: str | None)."""
    if not file_line or not file_line.strip():
        return None, None

    inner = strip_balanced_outer(file_line)
    if not inner or len(inner.strip()) < 3:
        return None, None

    cleaned = clean_for_detection(inner)
    if not cleaned.strip():
        return None, None

    # Strategy-1: explicit \address{…} blocks
    addrs_via_macro: list[str] = []
    remaining = inner
    while True:
        m = re.search(r"\\address\{", remaining)
        if not m:
            break
        addr_body, endpos = _balanced_extract(remaining, m.start() + 8)
        if addr_body is not None:
            addrs_via_macro.append(addr_body.strip())
            remaining = remaining[: m.start()] + remaining[endpos:]
        else:
            break

    # Strategy-2: find separator → trailing address
    sep = _find_authors_end(cleaned)
    trailing_raw = _extract_trailing_address(cleaned, sep) if sep >= 0 else None

    # Strategy-3: fallback split at last \\ if still no match
    if not trailing_raw and sep < 0:
        for m in reversed(list(re.finditer(r"\\\\", cleaned))):
            after = cleaned[m.end():].strip()
            if has_address_content(after) and len(after) > 15:
                if not _contains_author_like(after):
                    trailing_raw = after
                    sep = m.start()
                    break

    # ── Assemble final address list ──────────────────────────────
    addresses: list[str] = []

    # Add explicit \address{…} blocks
    if addrs_via_macro:
        for a in addrs_via_macro:
            a = _strip_formatting(a)
            if a and a not in addresses:
                addresses.append(a)

    # Add trailing address
    if trailing_raw:
        trailing_raw = _strip_formatting(trailing_raw)
        if trailing_raw and trailing_raw not in addresses:
            addresses.append(trailing_raw)

    if not addresses:
        return None, None

    # ── Build cleaned author content ─────────────────────────────
    if addrs_via_macro:
        author_text = inner
        for m in re.finditer(r"\\address\{", author_text):
            _, endpos = _balanced_extract(author_text, m.start() + 8)
            if endpos:
                author_text = author_text[: m.start()] + author_text[endpos:]
    else:
        author_text = inner

    if sep >= 0 and not addrs_via_macro:
        author_text = inner[:sep]

    author_text = re.sub(
        r"(?:\\\\\s*|\\vspace\*?\{[^}]*\}\s*|\\\[[\d.]+[a-z]*\\\]\s*|"
        r"\\bigskip\s*|\\smallskip\s*|\\medskip\s*|\\hspace\*?\{[^}]*\}\s*)+$",
        "", author_text,
    )
    author_text = author_text.strip()

    if not author_text or len(author_text) < 3:
        author_text = inner

    return addresses, author_text


# ── Database matching ────────────────────────────────────────────────
def build_db_lookup(conn) -> dict[str, list[tuple]]:
    """Return {normalised_content: [db_rows]} for all type='author' rows."""
    cur = conn.cursor()
    cur.execute(
        "SELECT filepath, filepath_id, token_id, parent_id, "
        "offset, length, type, token FROM authors WHERE type='author'"
    )
    lookup: dict[str, list[tuple]] = {}
    for row in cur.fetchall():
        norm = _normalise_full(row[7])
        if not norm:
            continue
        lookup.setdefault(norm, []).append(row)
    return lookup


def match_author(file_line: str, db_lookup: dict) -> tuple | None:
    """Find the DB author row matching *file_line* (raw line from
    /tmp/authors).  Returns the row tuple or None."""
    norm_f = _normalise_full(file_line)
    if not norm_f:
        return None
    if norm_f in db_lookup:
        return db_lookup[norm_f][0]
    # Fuzzy: substring containment
    for db_norm, rows in db_lookup.items():
        if len(norm_f) > 25 and len(db_norm) > 25:
            if norm_f in db_norm or db_norm in norm_f:
                return rows[0]
    return None


# ── Main routine ─────────────────────────────────────────────────────
def main() -> None:
    print("=" * 64)
    print("  Address Detection & Database Update")
    print("=" * 64)

    # 1. Backup DB
    print(f"\nBacking up {DB_PATH}  →  {DB_BAK}")
    shutil.copy2(DB_PATH, DB_BAK)
    print("Backup done.")

    # 2. Read /tmp/authors
    print(f"\nReading {AUTHORS_FILE} …")
    with open(AUTHORS_FILE, encoding="utf-8") as fh:
        file_lines = [ln.rstrip("\n") for ln in fh]
    print(f"  {len(file_lines)} lines (including blanks).")

    # 3. Open DB
    conn = sqlite3.connect(DB_PATH)
    db_lookup = build_db_lookup(conn)
    print(f"  {len(db_lookup)} unique normalised author tokens in DB.")

    stats = {
        "file_lines_total": 0,
        "address_detected_lines": 0,
        "address_blocks": 0,
        "matched_authors": 0,
        "unmatched": 0,
        "db_author_updated": 0,
        "address_rows_inserted": 0,
    }

    # 4. Process each file line
    for idx, line in enumerate(file_lines):
        if not line.strip():
            continue
        stats["file_lines_total"] += 1

        addresses, author_cleaned = extract_addresses(line)
        if not addresses:
            continue

        stats["address_detected_lines"] += 1
        stats["address_blocks"] += len(addresses)

        # Match to DB
        db_row = match_author(line, db_lookup)
        if db_row is None:
            stats["unmatched"] += 1
            continue
        stats["matched_authors"] += 1

        filepath, filepath_id, author_token_id, parent_id, offset, length, _, db_token = db_row

        # ── Update author token ─────────────────────────────────
        clean_content = author_cleaned if author_cleaned else strip_balanced_outer(line)
        # Re-wrap in \\author{…}
        new_token = "\\author{" + clean_content + "}"
        if new_token != db_token:
            conn.execute(
                "UPDATE authors SET token = ?, length = ? WHERE token_id = ? AND type = 'author'",
                (new_token, str(len(clean_content)), author_token_id),
            )
            stats["db_author_updated"] += 1

        # ── Insert address rows ────────────────────────────────
        for counter, addr_text in enumerate(addresses):
            new_id = str(abs(hash(f"{filepath_id}_{addr_text}_{counter}")) % 10**10)
            # Ensure uniqueness
            exists = conn.execute(
                "SELECT 1 FROM authors WHERE token_id = ?", (new_id,)
            ).fetchone()
            if exists:
                new_id = str(abs(hash(f"{filepath_id}_{addr_text}_{counter}_v2")) % 10**10)

            conn.execute(
                "INSERT INTO authors "
                "(filepath, filepath_id, token_id, parent_id, offset, length, type, token) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
                (
                    filepath,
                    filepath_id,
                    new_id,
                    author_token_id,
                    "0",
                    str(len(addr_text)),
                    "address",
                    "\\address{" + addr_text + "}",
                ),
            )
            stats["address_rows_inserted"] += 1

        # Progress indicator
        if stats["matched_authors"] % 50 == 0:
            print(f"  … processed {stats['matched_authors']} matched authors …")

    conn.commit()

    # 5. Verify
    final_count = conn.execute(
        "SELECT COUNT(*) FROM authors WHERE type='address'"
    ).fetchone()[0]
    conn.close()

    # 6. Summary
    print("\n" + "=" * 64)
    print("  SUMMARY")
    print("=" * 64)
    print(f"  Non-blank lines in /tmp/authors:         {stats['file_lines_total']:>5d}")
    print(f"  Lines with detected address(es):         {stats['address_detected_lines']:>5d}")
    print(f"  Total address blocks found:              {stats['address_blocks']:>5d}")
    print(f"  Authors matched to DB:                   {stats['matched_authors']:>5d}")
    print(f"  Unmatched (no DB counterpart):           {stats['unmatched']:>5d}")
    print(f"  DB author rows updated (token stripped): {stats['db_author_updated']:>5d}")
    print(f"  Address records inserted into DB:        {stats['address_rows_inserted']:>5d}")
    print(f"  Address rows in DB after run:            {final_count:>5d}")
    print("\nDone.")


if __name__ == "__main__":
    main()
