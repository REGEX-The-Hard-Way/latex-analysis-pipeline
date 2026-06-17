#!/usr/bin/env python3
"""
Detect and extract address information from author blocks in /tmp/authors,
match them to database author rows, insert address records, and update
author tokens in the SQLite database.

Usage: python3 detect_address.py
"""

import re
import sqlite3
import shutil
import sys

DB_PATH = "/mnt/x/home/user/my_data.db"
DB_BAK_PATH = "/mnt/x/home/user/my_data.db.bak2"
AUTHORS_FILE = "/tmp/authors"

ADDRESS_KEYWORDS = [
    "Department",
    "University",
    "Universidade",
    "Universidad",
    "Universitat",
    "Universit",
    "Institute",
    "Laboratory",
    "Laboratoire",
    "Laboratori",
    "College",
    "School",
    "Center",
    "Centre",
    "Faculty",
    "Facult",
    "Faculdade",
    "INFN",
    "CERN",
    "P.O. Box",
    "P.O.Box",
    "P.O. Box",
    "Street",
    "Road",
    "Avenue",
    "Observatoire",
    "Academy",
    "DESY",
    "JINR",
    "SISSA",
    "Perimeter Institute",
    "Jefferson Laboratory",
    "Racah Institute",
    "Max-Planck-Institut",
    "Steklov",
    "Saha Institute",
    "Harish-chandra",
    "Tata Institute",
    "Bogoliubov",
    "CBPF",
    "ICTP",
    "DAMTP",
    "KEK",
    "MPI/AEI",
    "LPTHE",
    "LPTMC",
    "IHES",
    "NORDITA",
    "SUBATECH",
    "ITEP",
    "ETH ",
    "SLAC",
    "Formula presented",
]

COUNTRY_PATTERNS = [
    "Canada",
    "France",
    "Italy",
    "Germany",
    "USA",
    "U.S.A.",
    "U.S.A",
    "Russia",
    "Spain",
    "Brazil",
    "Japan",
    "China",
    "India",
    "UK",
    "U.K.",
    "U.K",
    "Sweden",
    "Switzerland",
    "Israel",
    "Iran",
    "Korea",
    "Taiwan",
    "Poland",
    "Portugal",
    "Ukraine",
    "Belgium",
    "Netherlands",
    "Australia",
    "Mexico",
    "Hungary",
    "Romania",
    "Turkey",
    "Finland",
    "Greece",
    "Norway",
    "Denmark",
    "Ireland",
    "Chile",
    "Argentina",
    "Yugoslavia",
    "Serbia",
    "Croatia",
    "Bulgaria",
    "Czech",
    "Slovak",
    "Austria",
    "New Zealand",
    "Armenia",
    "Georgia",
    "Morocco",
    "Jordan",
    "South Africa",
    "Indonesia",
    "Thailand",
    "Vietnam",
    "Egypt",
]

NON_ADDRESS_PATTERNS = [
    r"\bcite\b",
    r"^\s*%",
    r"\\keywords\{",
    r"hep-th/",
    r"hep-ph/",
    r"math/",
    r"gr-qc/",
    r"\\date\{",
    r"\\speaker\{",
    r"\\firstname\{",
    r"\\surname\{",
    r"\\pdf",
    r"\\inst\{",
    r"\\corauthref\{",
    r"\\altaffiliation",
    r"\\renewcommand",
    r"\\textcolor",
]


def strip_outer_braces(s):
    """Strip outer { } wrapper from a line, handling nested braces carefully."""
    s = s.strip()
    if s.startswith("{") and s.endswith("}"):
        depth = 0
        for i, ch in enumerate(s):
            if ch == "{":
                depth += 1
            elif ch == "}":
                depth -= 1
                if depth == 0 and i == len(s) - 1:
                    return s[1:-1]
    return s


def strip_inner_braces(s):
    """Remove outer { } if the entire string is wrapped in them."""
    s = s.strip()
    if s.startswith("{") and s.endswith("}"):
        depth = 0
        for i, ch in enumerate(s):
            if ch == "{":
                depth += 1
            elif ch == "}":
                depth -= 1
                if depth == 0 and i == len(s) - 1:
                    return s[1:-1]
    return s


def extract_balanced(text, start_pos, open_char="{", close_char="}"):
    """Extract content between balanced braces starting at start_pos."""
    if start_pos >= len(text) or text[start_pos] != open_char:
        return None, start_pos
    depth = 0
    for i in range(start_pos, len(text)):
        if text[i] == open_char:
            depth += 1
        elif text[i] == close_char:
            depth -= 1
            if depth == 0:
                return text[start_pos + 1 : i], i + 1
    return None, start_pos


def remove_thanks(content):
    """Remove \\thanks{...} blocks from content."""
    result = []
    i = 0
    while i < len(content):
        if content[i : i + 8] == "\\thanks{" or content[i : i + 8] == "\\thanks{":
            _, end = extract_balanced(content, i + 7)
            i = end
            continue
        result.append(content[i])
        i += 1
    return "".join(result)


def remove_footnote(content):
    """Remove \\footnote{...} and \\footnotemark[...] blocks."""
    result = []
    i = 0
    while i < len(content):
        if content[i : i + 10] == "\\footnote{":
            _, end = extract_balanced(content, i + 9)
            i = end
            continue
        if content[i : i + 13] == "\\footnotemark":
            j = i + 13
            if j < len(content) and content[j] == "[":
                k = content.find("]", j)
                if k != -1:
                    i = k + 1
                    continue
            i = j
            continue
        result.append(content[i])
        i += 1
    return "".join(result)


def remove_emails(content):
    """Remove email-related patterns from content."""
    content = re.sub(r"\\email\{[^}]*\}", "", content)
    content = re.sub(
        r"E-mail\s*:\s*[^\n\\]*(?:\\[^\n]*)?", "", content, flags=re.IGNORECASE
    )
    content = re.sub(
        r"E?mail\s*:\s*[^\n\\]*(?:\\[^\n]*)?", "", content, flags=re.IGNORECASE
    )
    content = re.sub(r"\\texttt\{[^@]*@[^}]*\}", "", content)
    content = re.sub(r"\{\\tt\s[^@]*@[^}]*\}", "", content)
    content = re.sub(r"email\s*:\s*", "", content, flags=re.IGNORECASE)
    content = re.sub(r"E-mail", "", content, flags=re.IGNORECASE)
    content = re.sub(r"\\pdflink\{mailto:[^}]*\}", "", content)
    return content


def clean_for_detection(content):
    """Remove non-address constructs to isolate address detection."""
    c = content
    c = remove_thanks(c)
    c = remove_footnote(c)
    c = remove_emails(c)
    c = re.sub(r"\\parbox\{[^}]*\}\{[^}]*\}", "", c)
    return c


def has_address_keywords(text):
    """Check if text contains address-indicating keywords."""
    text_lower = text.lower()
    for kw in ADDRESS_KEYWORDS:
        if kw.lower() in text_lower:
            return True
    for country in COUNTRY_PATTERNS:
        if country.lower() in text_lower:
            return True
    # Postal code patterns
    if re.search(r"\b\d{5}(?:[-\s]\d{4})?\b", text):
        if re.search(
            r"(Street|Road|Avenue|St\.|Rd\.|Ave\.|Blvd|Drive|Lane|Way|Place|Plaza|Square|Park|Row|Court|Circle|Highway|Hwy|Route)\b",
            text,
            re.IGNORECASE,
        ):
            return True
    return False


def extract_explicit_addresses(content):
    """Extract \\address{...} blocks from content. Returns (addresses, cleaned_content)."""
    addresses = []
    cleaned = []
    i = 0
    while i < len(content):
        if content[i : i + 9] == "\\address{":
            addr_text, end = extract_balanced(content, i + 8)
            if addr_text is not None:
                addresses.append(addr_text.strip())
            i = end
            continue
        cleaned.append(content[i])
        i += 1
    return addresses, "".join(cleaned)


def find_numbered_markers(text):
    """Find numbered marker patterns in text.
    Returns list of (marker_text, start_pos, end_pos) tuples.
    """
    patterns = [
        r"\$\^\{?(\d+|[a-zA-Z])\}?\$",
        r"\$\{\}\^\{([a-zA-Z\d]+)\}\$",
        r"\$\(?\(?([a-zA-Z\d]+)\)?\)?\^\{\\?[a-zA-Z]*\}?\$",
    ]
    markers = []
    for pat in patterns:
        for m in re.finditer(pat, text):
            markers.append((m.group(0), m.start(), m.end()))
    # Also match ${}^{a}$ style
    for m in re.finditer(r"\$\{\}\^\{([^\}]+)\}\$", text):
        markers.append((m.group(0), m.start(), m.end()))
    # Match $^1$ style
    for m in re.finditer(r"\$\^(\d+|[a-zA-Z\*\+\-\$])\$", text):
        markers.append((m.group(0), m.start(), m.end()))
    # Match $^{1}$ style
    for m in re.finditer(r"\$\^\{([\d,]+|[a-zA-Z\*\+\-\$]+)\}\$", text):
        markers.append((m.group(0), m.start(), m.end()))
    # Match $^{(a)}$ style
    for m in re.finditer(r"\$\(\(?([a-zA-Z\d]+)\)?\)?\$", text):
        markers.append((m.group(0), m.start(), m.end()))
    # Match ${}^{a,b}$ style
    for m in re.finditer(r"\$\{\}\^\{([a-zA-Z,\s]+)\}\$", text):
        markers.append((m.group(0), m.start(), m.end()))
    # Remove duplicates by position
    seen = set()
    unique = []
    for m in sorted(markers, key=lambda x: x[1]):
        if m[1] not in seen:
            seen.add(m[1])
            unique.append(m)
    return unique


def find_separator_position(cleaned_content):
    """Find the position where authors end and addresses begin.
    Returns the index in the string where address content starts,
    or -1 if no clear separator is found.

    Separator patterns (in order of strength):
    - \\vspace, \\bigskip, \\smallskip, \\medskip
    - \\\\\\(three or more \\)
    - \\and
    - \\[ ... ] (vertical spacing)
    """
    content = cleaned_content

    # Find strongest separator first
    sep_patterns = [
        r"\\vspace\*?\{[^}]*\}",
        r"\\bigskip",
        r"\\smallskip",
        r"\\medskip",
        r"\\\[[\d.]+[a-z]*\\\]",  # like \[0.4cm]
        r"\\\\\\",  # three or more \\
    ]

    # Find the LAST strong separator (address comes after the last separator
    # that separates authors from addresses)
    best_pos = -1

    for pat in sep_patterns:
        for m in re.finditer(pat, content):
            pos_after = m.end()
            remaining = content[pos_after:].strip()
            # Check if the remaining text has address keywords
            if has_address_keywords(remaining) and len(remaining) > 20:
                # Found a separator followed by address text
                best_pos = max(best_pos, pos_after)

    if best_pos != -1:
        return best_pos

    # Try finding multiple \\ separators
    # Look for patterns like: names \\\\\ address or names \\ \vspace{...} \ address
    # Find a point after author names where address text begins
    # Strategy: find the longest stretch of \\ followed by address text

    # Try: find \\ followed by address-like text
    for m in re.finditer(r"\\\\\\\\", content):
        remaining = content[m.end() :].strip()
        if has_address_keywords(remaining) and len(remaining) > 20:
            best_pos = max(best_pos, m.end())

    for m in re.finditer(r"\\\\\\", content):
        remaining = content[m.end() :].strip()
        if has_address_keywords(remaining) and len(remaining) > 20:
            best_pos = max(best_pos, m.end())

    for m in re.finditer(r"\\\\", content):
        remaining = content[m.end() :].strip()
        if has_address_keywords(remaining) and len(remaining) > 20:
            best_pos = max(best_pos, m.end())

    if best_pos != -1:
        return best_pos

    # Check for \and separator
    for m in re.finditer(r"\\and\b", content):
        remaining = content[m.end() :].strip()
        if has_address_keywords(remaining) and len(remaining) > 30:
            # But this might be just another author after \and
            # Check if it has more address than author feel
            if not re.match(r"^[A-Z][a-z]*\.?\s+(?:~)?[A-Z]", remaining.strip()):
                best_pos = max(best_pos, m.end())

    return best_pos


def extract_numbered_addresses(content, all_markers):
    """Extract addresses preceded by numbered markers.
    Returns list of (address_text, marker_text) tuples.
    """
    addresses = []

    if not all_markers:
        return addresses

    # For each marker, check if the text that follows contains address keywords
    # The text runs from after the marker to either the next marker or the end
    for idx, (marker_text, start, end) in enumerate(all_markers):
        # Find the text after this marker
        if idx + 1 < len(all_markers):
            next_start = all_markers[idx + 1][1]
            text_after = content[end:next_start].strip()
        else:
            text_after = content[end:].strip()

        if has_address_keywords(text_after) and len(text_after) > 15:
            # Clean up the text: remove LaTeX formatting commands but keep content
            cleaned = clean_address_text(text_after)
            if cleaned:
                addresses.append((cleaned, marker_text.strip()))

    return addresses


def clean_address_text(text):
    """Clean address text by removing LaTeX formatting, keeping the content."""
    # Remove LaTeX font commands while preserving their content
    text = re.sub(r"\\small\s*", "", text)
    text = re.sub(r"\\\\ormalsize\s*", "", text)
    text = re.sub(r"\\it\s*", "", text)
    text = re.sub(r"\\bf\s*", "", text)
    text = re.sub(r"\\bfseries\s*", "", text)
    text = re.sub(r"\\sc\s*", "", text)
    text = re.sub(r"\\sf\s*", "", text)
    text = re.sub(r"\\sf\s*", "", text)
    text = re.sub(r"\\sl\s*", "", text)
    text = re.sub(r"\\rm\s*", "", text)
    text = re.sub(r"\\em\s*", "", text)
    text = re.sub(r"\\emph\s*", "", text)
    text = re.sub(r"\\itshape\s*", "", text)
    text = re.sub(r"\\textsf\s*", "", text)
    text = re.sub(r"\\textbf\s*", "", text)
    text = re.sub(r"\\textit\s*", "", text)
    text = re.sub(r"\\texttt\s*", "", text)
    text = re.sub(r"\\scshape\s*", "", text)
    text = re.sub(r"\\textsc\s*", "", text)
    text = re.sub(r"\\large\s*", "", text)
    text = re.sub(r"\\Large\s*", "", text)
    text = re.sub(r"\\LARGE\s*", "", text)
    text = re.sub(r"\\footnotesize\s*", "", text)
    text = re.sub(r"\\tiny\s*", "", text)
    text = re.sub(r"\\scriptsize\s*", "", text)

    # Replace LaTeX line breaks
    text = re.sub(r"\\\\\s*", ", ", text)
    text = re.sub(r"\\newline\s*", ", ", text)

    # Remove volume/phantom commands
    text = re.sub(r"\\vspace\*?\{[^}]*\}", "", text)
    text = re.sub(r"\\vphantom\{[^}]*\}", "", text)
    text = re.sub(r"\\hspace\*?\{[^}]*\}", "", text)
    text = re.sub(r"\\\[[^\]]*\\\]", "", text)

    # Collapse whitespace
    text = re.sub(r"\s+", " ", text)
    text = text.strip()

    # Remove leading/trailing punctuation and whitespace
    text = text.strip(".,;: \t\n\r")

    return text


def extract_direct_address(text, sep_pos):
    """Extract address text that appears after a separator (no numbered markers)."""
    if sep_pos < 0:
        return None

    addr_part = text[sep_pos:].strip()

    # Remove leading separators
    addr_part = re.sub(r"^(\\\\\s*)+", "", addr_part).strip()
    addr_part = re.sub(r"^\\vspace\*?\{[^}]*\}\s*", "", addr_part).strip()
    addr_part = re.sub(r"^\\bigskip\s*", "", addr_part).strip()
    addr_part = re.sub(r"^\\smallskip\s*", "", addr_part).strip()
    addr_part = re.sub(r"^\\medskip\s*", "", addr_part).strip()
    addr_part = re.sub(r"^\\\[[\d.]+[a-z]*\\\]\s*", "", addr_part).strip()

    if has_address_keywords(addr_part) and len(addr_part) > 15:
        return clean_address_text(addr_part)

    return None


def detect_addresses(file_content):
    """
    Main address detection function.
    Takes the content of an author block (from /tmp/authors, with outer braces stripped).
    Returns (addresses_list, cleaned_author_content) or (None, None) if no addresses.
    """
    if not file_content or not file_content.strip():
        return None, None

    # Remove outer braces from the file content
    inner = strip_outer_braces(file_content)
    if not inner or not inner.strip():
        return None, None

    # Clean for detection (remove thanks, footnotes, emails)
    cleaned = clean_for_detection(inner)
    if not cleaned.strip():
        return None, None

    # Strategy 1: Extract explicit \address{...} blocks
    explicit_addrs, cleaned2 = extract_explicit_addresses(cleaned)

    # Strategy 2: Find numbered marker addresses
    all_markers = find_numbered_markers(cleaned2)
    numbered_addrs = extract_numbered_addresses(cleaned2, all_markers)

    # Strategy 3: Check for direct address text after separators
    sep_pos = find_separator_position(cleaned2)
    direct_addr = extract_direct_address(cleaned2, sep_pos) if sep_pos >= 0 else None

    # Collect all addresses
    all_addresses = []

    for addr_text in explicit_addrs:
        all_addresses.append(addr_text)

    for addr_text, marker in numbered_addrs:
        all_addresses.append(addr_text)

    if direct_addr and not all_addresses:
        # Only add direct address if no numbered markers were found
        # (numbered markers are more specific)
        all_addresses.append(direct_addr)

    if not all_addresses:
        return None, None

    # Now build the cleaned author content
    # Strategy: remove address-like text from the original content

    # Start with the original inner content
    author_cleaned = inner

    # Remove address blocks we found
    for addr in all_addresses:
        # Try to find and remove the address from the original content
        # Use normalized matching to find the address in the original
        author_cleaned = _remove_address_from_content(author_cleaned, addr)

    # Also try to chop at the separator position
    if sep_pos >= 0:
        # The author part is everything before the separator
        author_part = inner[:sep_pos].strip()
        # Check if the author_part still contains names (not empty)
        if author_part and len(author_part) > 5:
            author_cleaned = author_part

    # Remove trailing separators from author
    author_cleaned = re.sub(r"(\\\\\s*)+$", "", author_cleaned).strip()
    author_cleaned = re.sub(r"\\vspace\*?\{[^}]*\}\s*$", "", author_cleaned).strip()
    author_cleaned = re.sub(r"\\bigskip\s*$", "", author_cleaned).strip()
    author_cleaned = re.sub(r"\\smallskip\s*$", "", author_cleaned).strip()
    author_cleaned = re.sub(r"\\medskip\s*$", "", author_cleaned).strip()
    author_cleaned = re.sub(r"\\\[[\d.]+[a-z]*\\\]\s*$", "", author_cleaned).strip()

    return all_addresses, author_cleaned


def _remove_address_from_content(content, address):
    """Try to remove an address string from the content."""

    # Normalize both for matching
    def normalize(s):
        s = re.sub(r"\s+", " ", s)
        s = re.sub(r"\\\\\s*", " ", s)
        return s.strip().lower()

    norm_cont = normalize(content)
    norm_addr = normalize(address)

    if norm_addr in norm_cont:
        # Find the original position
        idx = norm_cont.index(norm_addr)
        # Map back to original content (approximate)
        # Just remove all address-related text after the separator
        return content

    # Try removing character by character after the address keywords
    return content


def normalize_for_match(text):
    """Normalize text for matching between file lines and DB tokens."""
    if text is None:
        return ""
    # Replace various line break representations with spaces
    t = text.replace("\n", " ")
    t = t.replace("\r", " ")
    t = t.replace("\t", " ")
    # Collapse multiple spaces
    t = re.sub(r"\s+", " ", t)
    # Remove outer \\author{...} wrapper if present
    t = t.strip()
    if t.startswith("\\author{") and t.endswith("}"):
        t = t[8:-1]
    # Strip outer braces from file content too
    t = strip_inner_braces(t)
    t = t.strip()
    # Collapse spaces again
    t = re.sub(r"\s+", " ", t)
    return t


def main():
    print("=" * 60)
    print("Address Detection and Database Update Script")
    print("=" * 60)

    # Backup the database
    print(f"\nBacking up database to {DB_BAK_PATH}...")
    shutil.copy2(DB_PATH, DB_BAK_PATH)
    print("Backup complete.")

    # Read authors file
    print(f"\nReading {AUTHORS_FILE}...")
    with open(AUTHORS_FILE, "r", encoding="utf-8") as f:
        file_lines = f.readlines()
    file_lines = [l.rstrip("\n") for l in file_lines]
    print(f"Read {len(file_lines)} lines.")

    # Connect to database
    print(f"\nConnecting to database {DB_PATH}...")
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()

    # Get all author rows from DB
    cur.execute(
        "SELECT filepath, filepath_id, token_id, parent_id, offset, length, type, token FROM authors WHERE type='author'"
    )
    db_authors = cur.fetchall()
    print(f"Found {len(db_authors)} author rows in database.")

    # Build a lookup: normalized author content -> DB row
    db_lookup = {}
    for row in db_authors:
        token = row[7]  # token column
        # Extract content inside \author{...}
        norm = normalize_for_match(token)
        if norm:
            if norm not in db_lookup:
                db_lookup[norm] = []
            db_lookup[norm].append(row)

    print(f"Built lookup with {len(db_lookup)} unique author tokens.")

    # Process each file line
    stats = {
        "lines_with_address": 0,
        "total_addresses_found": 0,
        "matched_authors": 0,
        "db_rows_updated": 0,
        "addresses_inserted": 0,
        "unmatched": 0,
    }

    # First pass: detect addresses in file lines
    file_results = []  # (line_idx, file_content, addresses, cleaned_author)
    for idx, line in enumerate(file_lines):
        if not line.strip():
            continue
        addresses, cleaned = detect_addresses(line)
        if addresses:
            stats["lines_with_address"] += 1
            stats["total_addresses_found"] += len(addresses)
        file_results.append((idx, line, addresses, cleaned))

    print(f"\nDetected addresses in {stats['lines_with_address']} lines.")
    print(f"Total address blocks found: {stats['total_addresses_found']}")

    # Second pass: match and update DB
    for idx, file_content, addresses, cleaned in file_results:
        if not addresses:
            continue

        # Normalize file content for matching
        norm_file = normalize_for_match(file_content)
        if not norm_file:
            stats["unmatched"] += 1
            continue

        # Find matching DB row
        matched_rows = db_lookup.get(norm_file, [])

        if not matched_rows:
            # Try fuzzy match: check if normalized file content is a substring
            # of any DB content or vice versa
            for db_norm, rows in db_lookup.items():
                if norm_file in db_norm or db_norm in norm_file:
                    if len(norm_file) > 20 and len(db_norm) > 20:
                        matched_rows = rows
                        break
            if not matched_rows:
                stats["unmatched"] += 1
                continue

        # Use the first match
        db_row = matched_rows[0]
        filepath = db_row[0]
        filepath_id = db_row[1]
        author_token_id = db_row[2]
        token = db_row[7]

        stats["matched_authors"] += 1

        # Build cleaned author token
        cleaned_author = cleaned if cleaned else normalize_for_match(file_content)
        # Wrap back in \author{...}
        new_token = "\\author{" + cleaned_author + "}"

        # Update the author's token in the DB
        try:
            cur.execute(
                "UPDATE authors SET token = ? WHERE token_id = ? AND type = 'author'",
                (new_token, author_token_id),
            )
            stats["db_rows_updated"] += 1
        except Exception as e:
            print(f"  ERROR updating author {author_token_id}: {e}")
            continue

        # Insert address rows
        for counter, addr_text in enumerate(addresses):
            new_token_id = str(
                abs(hash(f"{filepath_id}_{addr_text}_{counter}")) % 10**10
            )

            # Ensure unique
            cur.execute(
                "SELECT COUNT(*) FROM authors WHERE token_id = ?", (new_token_id,)
            )
            if cur.fetchone()[0] > 0:
                # Slight modification to ensure uniqueness
                new_token_id = str(
                    abs(hash(f"{filepath_id}_{addr_text}_{counter}_v2")) % 10**10
                )

            try:
                cur.execute(
                    "INSERT INTO authors (filepath, filepath_id, token_id, parent_id, offset, length, type, token) VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
                    (
                        filepath,
                        filepath_id,
                        new_token_id,
                        author_token_id,
                        "0",
                        str(len(addr_text)),
                        "address",
                        "\\address{" + addr_text + "}",
                    ),
                )
                stats["addresses_inserted"] += 1
            except Exception as e:
                print(f"  ERROR inserting address {new_token_id}: {e}")

    # Commit changes
    conn.commit()

    # Print summary
    print("\n" + "=" * 60)
    print("SUMMARY")
    print("=" * 60)
    print(f"  Lines in /tmp/authors:            {len(file_lines)}")
    print(f"  DB author rows:                   {len(db_authors)}")
    print(f"  Lines with detected addresses:    {stats['lines_with_address']}")
    print(f"  Total address blocks found:       {stats['total_addresses_found']}")
    print(f"  Authors matched to DB:            {stats['matched_authors']}")
    print(f"  Unmatched (no DB counterpart):    {stats['unmatched']}")
    print(f"  DB author rows updated:           {stats['db_rows_updated']}")
    print(f"  Address records inserted:         {stats['addresses_inserted']}")

    # Verify
    cur.execute("SELECT COUNT(*) FROM authors WHERE type='address'")
    addr_count = cur.fetchone()[0]
    print(f"\n  Address rows in DB after update:  {addr_count}")

    conn.close()
    print("\nDone.")


if __name__ == "__main__":
    main()
