import sqlite3
import re
import os
from collections import OrderedDict

DB_PATH = "/mnt/x/home/user/my_data.db"
SCAN_DIR = "/home/user/latex-analysis-pipeline/scanner/2003"


def filepath_hash(filepath):
    return str(abs(hash(filepath)) % 10 ** 10)


def token_hash(base, counter=0):
    return str(abs(hash(f"{base}_{counter}")) % 10 ** 10)


def _clean_latex(s):
    """Strip LaTeX formatting commands from text while preserving content."""
    s = re.sub(r"\\sc\s*", "", s)
    s = re.sub(r"\\scshape\s*", "", s)
    s = re.sub(r"\\it\s*", "", s)
    s = re.sub(r"\\em\s*", "", s)
    s = re.sub(r"\\sl\s*", "", s)
    s = re.sub(r"\\bf\s*", "", s)
    s = re.sub(r"\\tt\s*", "", s)
    s = re.sub(r"\\rm\s*", "", s)
    s = re.sub(r"\\newblock", "", s)
    s = re.sub(r"\\newline", "", s)
    s = re.sub(r"\\/", "", s)
    s = re.sub(r"\{\\/?\}", "", s)
    s = re.sub(r"~", " ", s)
    s = re.sub(r"\s+", " ", s)
    return s.strip()


def parse_bibitem(text):
    """Parse a single bibitem entry, extracting structured components."""
    result = OrderedDict()

    m = re.match(r"\\bibitem\s*\{([^}]+)\}", text)
    if not m:
        return None
    result["key"] = m.group(1).strip()
    body = text[m.end() :].strip()

    result["authors"] = ""
    result["title"] = ""
    result["journal"] = ""
    result["volume"] = ""
    result["year"] = ""
    result["pages"] = ""
    result["arxiv"] = ""
    result["raw"] = text.strip()

    # Extract authors from {\sc ...} or {\scshape ...} blocks
    sc_matches = re.findall(
        r"\{\s*\\(?:sc|scshape)\s+((?:[^{}]|\{[^{}]*\})*)\}",
        body,
        re.DOTALL,
    )
    if sc_matches:
        result["authors"] = ", ".join(
            _clean_latex(m).rstrip(":,. ").strip() for m in sc_matches
        )

    # If no {\sc ...} found, try to get authors from the start of body
    # (first text block before any {\em...}, {\it...}, \newblock, \newline, \\, or numbered marker)
    if not result["authors"]:
        before_em = re.split(
            r"\{\s*\\(?:em|it|sl)\s+\{",
            body,
            maxsplit=1,
        )[0]
        before_em = re.split(r"\\newblock", before_em, maxsplit=1)[0]
        before_em = re.split(r"\\newline", before_em, maxsplit=1)[0]
        before_em = re.split(r"\\\\", before_em, maxsplit=1)[0]
        # Strip off leading labels like "See, for instance," or "For a review, see"
        before_em = re.sub(
            r"^(?:See,?\s*(?:for\s*(?:instance|a\s*review|details)|e\.g\.),?\s*)",
            "",
            before_em,
            flags=re.IGNORECASE,
        )
        # Handle custom macros: \np{...}{...}{...}, \pl{...}{...}{...}, etc.
        before_em = re.sub(r"\\(?:np|pl|jhep|prl|prd|jhep)\{[^}]*\}\{[^}]*\}\{[^}]*\}", "", before_em)
        before_em = before_em.strip().rstrip(",. ")
        if before_em and len(before_em) > 3 and not re.match(r"^[\d\s,.\[\]]+$", before_em):
            result["authors"] = _clean_latex(before_em)

    # Extract title: first {\em ...}, {\it ...} or {\sl ...} block
    title_m = re.search(
        r"\{\s*\\(?:em|it|sl)\s+((?:[^{}]|\{[^{}]*\})*)\}",
        body,
        re.DOTALL,
    )
    if title_m:
        result["title"] = _clean_latex(title_m.group(1))

    # Extract arXiv ID: {\tt ...} containing arXiv-style identifiers
    arxiv_patterns = [
        r"\{\s*\\tt\s+([^}]*\b(?:hep-(?:th|ph|l|ex)|math(?:-ph)?|gr-qc|nucl-(?:th|ex)|astro-ph|cond-mat|quant-ph|physics)/[0-9]+[^}]*)\}",
        r"\[(hep-(?:th|ph|l|ex)|math(?:-ph)?|gr-qc|nucl-(?:th|ex)|astro-ph|cond-mat|quant-ph|physics)/[0-9]+\]",
        r"(?:hep-(?:th|ph|l|ex)|math(?:-ph)?|gr-qc|nucl-(?:th|ex)|astro-ph|cond-mat|quant-ph|physics)/[0-9]+",
    ]
    for pat in arxiv_patterns:
        arxiv_m = re.search(pat, body, re.IGNORECASE)
        if arxiv_m:
            result["arxiv"] = arxiv_m.group(0).strip("[]{} ").replace("\\tt ", "")
            break

    # Extract volume: {\bf NUMBER}
    vol_m = re.search(r"\{\s*\\bf\s+(\d+)\s*\}", body)
    if vol_m:
        result["volume"] = vol_m.group(1)

    # Extract year: (YEAR) pattern
    year_m = re.search(r"\((\d{4})\)", body)
    if not year_m:
        year_m = re.search(r"(?:^|\s|[,. ])(\d{4})(?=[,.\s)\]$])", body)
    if year_m:
        yr = int(year_m.group(1))
        if 1900 <= yr <= 2025:
            result["year"] = str(yr)

    # Extract pages: NUMBER--NUMBER or NUMBER-NUMBER (not arXiv)
    pages_m = re.search(r"(\d+)\s*[-]{1,2}\s*(\d+)", body)
    if pages_m:
        pg = f"{pages_m.group(1)}-{pages_m.group(2)}"
        if not pg.startswith("19") and not pg.startswith("20"):
            result["pages"] = pg

    # Extract journal: {\em ...} or {\it ...} blocks containing journal keywords
    # but NOT the same as the extracted title
    journals = re.findall(
        r"\{\s*\\(?:em|it|sl)\s+((?:[^{}]|\{[^{}]*\})*)\}",
        body,
        re.DOTALL,
    )
    for j in journals:
        j_clean = _clean_latex(j)
        if j_clean and j_clean != result["title"] and re.search(
            r"(?:Phys|Math|Lett|Rev|Nucl|Journ|Ann|Comm|Class|Quant|Grav|JHEP|Nucl\.?\s*Phys|Mod\.?\s*Phys|Europhys)",
            j_clean,
            re.IGNORECASE,
        ):
            result["journal"] = j_clean
            break

    return result


def extract_bibitems_from_tex(filepath):
    """Extract all bibitem entries from a .tex file."""
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
        return []

    # Find thebibliography blocks
    bib_blocks = re.findall(
        r"\\begin\{thebibliography\}.*?\\end\{thebibliography\}",
        content,
        re.DOTALL,
    )
    if not bib_blocks:
        return []

    all_items = []
    for block in bib_blocks:
        # Split into individual bibitems
        bibitems = re.split(r"(?=\\bibitem\s*\{)", block)
        for item in bibitems:
            item = item.strip()
            if not item.startswith("\\bibitem"):
                continue
            # Remove \end{thebibliography} if present
            item = re.sub(r"\\end\{thebibliography\}\s*$", "", item).strip()
            parsed = parse_bibitem(item)
            if parsed:
                all_items.append(parsed)

    return all_items


def main():
    db_backup = DB_PATH + ".bak3"
    os.system(f"cp {DB_PATH} {db_backup}")
    print(f"Backup: {db_backup}")

    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()

    tex_files = []
    for f in sorted(os.listdir(SCAN_DIR)):
        if f.endswith(".tex"):
            tex_files.append(os.path.join(SCAN_DIR, f))

    print(f"Found {len(tex_files)} .tex files")

    total_bibitems = 0
    total_files_with_bib = 0

    for filepath in tex_files:
        items = extract_bibitems_from_tex(filepath)
        if not items:
            continue
        total_files_with_bib += 1
        fp_id = filepath_hash(filepath)

        for i, item in enumerate(items):
            total_bibitems += 1
            bibitem_id = token_hash(f"bibitem:{item['key']}", total_bibitems)

            # Insert bibitem main record
            bibitem_token = f"\\bibitem{{{item['key']}}}"
            cursor.execute(
                "INSERT INTO authors (filepath, filepath_id, token_id, parent_id, offset, length, type, token) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
                (
                    filepath,
                    fp_id,
                    bibitem_id,
                    fp_id,
                    "0",
                    str(len(item["raw"])),
                    "bibitem",
                    bibitem_token,
                ),
            )

            # Insert sub-components as child records
            components = [
                ("bibitem_author", item["authors"]),
                ("bibitem_title", item["title"]),
                ("bibitem_journal", item["journal"]),
                ("bibitem_year", item["year"]),
                ("bibitem_volume", item["volume"]),
                ("bibitem_pages", item["pages"]),
                ("bibitem_arxiv", item["arxiv"]),
            ]

            for comp_type, comp_value in components:
                if comp_value:
                    comp_id = token_hash(f"{item['key']}_{comp_type}", total_bibitems)
                    cursor.execute(
                        "INSERT INTO authors (filepath, filepath_id, token_id, parent_id, offset, length, type, token) "
                        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
                        (
                            filepath,
                            fp_id,
                            comp_id,
                            bibitem_id,
                            "0",
                            str(len(comp_value)),
                            comp_type,
                            comp_value,
                        ),
                    )

        if total_files_with_bib % 100 == 0:
            conn.commit()
            print(f"  Processed {total_files_with_bib} files, {total_bibitems} bibitems...")

    conn.commit()
    conn.close()

    print(f"\nDone. {total_files_with_bib} files with bibliography, {total_bibitems} bibitems extracted.")


if __name__ == "__main__":
    main()
