#!/usr/bin/env python3
"""batch_1019.py — Macro-expand + pdflatex all 1019 files.  Categorise every failure."""

import os, sys, re, glob, subprocess, json, time
from collections import Counter, defaultdict

DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "2003")
OUT = "/tmp/pdfs"
os.makedirs(OUT, exist_ok=True)

MACRO_BIN = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "macro_expander.out"
)
COMMENT_SH = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "comment_missing.sh"
)

files = sorted(glob.glob(f"{DIR}/03*.tex"))
N = len(files)
print(f"Processing {N} files...")

results = []
t0 = time.time()

for i, f in enumerate(files):
    base = os.path.splitext(os.path.basename(f))[0]
    r = {
        "file": base,
        "phase": "start",
        "pdflatex_ok": False,
        "pdf_pages": 0,
        "errors": [],
        "warnings": [],
    }

    # ---- Step 1: comment missing deps ----
    cleaned = os.path.join(OUT, f"{base}.tex")
    cp = subprocess.run(
        ["bash", COMMENT_SH, f, cleaned], capture_output=True, text=True, timeout=10
    )
    if cp.returncode != 0 or not os.path.exists(cleaned):
        r["phase"] = "comment_failed"
        r["errors"].append(f"comment_missing: rc={cp.returncode}")
        results.append(r)
        if (i + 1) % 100 == 0:
            print(f"  [{i+1}/{N}] {base}: comment failed")
        continue

    # ---- Step 2: macro expand ----
    expanded = os.path.join(OUT, f"{base}_exp.tex")
    with open(cleaned, "rb") as fh:
        raw = fh.read()
    try:
        mp = subprocess.run([MACRO_BIN], input=raw, capture_output=True, timeout=30)
    except subprocess.TimeoutExpired:
        r["phase"] = "expand_timeout"
        r["errors"].append("macro_expander: TIMEOUT")
        results.append(r)
        if (i + 1) % 50 == 0:
            print(f"  [{i+1}/{N}] {base}: expand timeout")
        continue
    if mp.returncode != 0 or len(mp.stdout) == 0:
        r["phase"] = "expand_failed"
        r["errors"].append(f"macro_expander: rc={mp.returncode} len={len(mp.stdout)}")
        results.append(r)
        if (i + 1) % 100 == 0:
            print(f"  [{i+1}/{N}] {base}: expand failed")
        continue
    with open(expanded, "wb") as fh:
        fh.write(mp.stdout)

    # ---- Step 3: count macros before/after ----
    try:
        orig_text = raw.decode("utf-8", errors="replace")
        exp_text = mp.stdout.decode("utf-8", errors="replace")
        macros_before = len(
            re.findall(r"\\(?:newcommand|renewcommand|def|newenvironment)\b", orig_text)
        )
        macros_after = len(
            re.findall(r"\\(?:newcommand|renewcommand|def|newenvironment)\b", exp_text)
        )
        r["macros_before"] = macros_before
        r["macros_after"] = macros_after
    except:
        r["macros_before"] = 0
        r["macros_after"] = 0

    # ---- Step 4: pdflatex ----
    logfile = os.path.join(OUT, f"{base}.log")
    try:
        pp = subprocess.run(
            [
                "pdflatex",
                "-interaction=nonstopmode",
                "-output-directory",
                OUT,
                expanded,
            ],
            capture_output=True,
            timeout=45,
        )
    except subprocess.TimeoutExpired:
        r["phase"] = "pdflatex_timeout"
        r["category"] = "pdflatex_timeout"
        r["errors"].append("pdflatex: TIMEOUT")
        results.append(r)
        if (i + 1) % 50 == 0:
            print(f"  [{i+1}/{N}] {base}: pdflatex timeout")
        continue
    log_text = pp.stdout.decode("utf-8", errors="replace") + pp.stderr.decode(
        "utf-8", errors="replace"
    )
    with open(logfile, "w", encoding="utf-8") as fh:
        fh.write(log_text)

    # ---- Step 5: analyse log ----
    pdf = os.path.join(OUT, f"{base}_exp.pdf")
    if os.path.exists(pdf):
        r["pdflatex_ok"] = True
        try:
            pi = subprocess.run(
                ["pdfinfo", pdf], capture_output=True, text=True, timeout=5
            )
            for line in pi.stdout.split("\n"):
                if "Pages:" in line:
                    r["pdf_pages"] = int(line.split(":")[1].strip())
        except:
            pass

    # Parse LaTeX errors
    latex_errors = re.findall(r"^!\s+(.*)$", log_text, re.MULTILINE)
    r["errors"] = latex_errors[:5]  # first 5 unique

    # Parse warnings
    latex_warnings = re.findall(r"LaTeX Warning:\s+(.*)$", log_text, re.MULTILINE)
    r["warnings"] = latex_warnings[:5]

    # Categorise the primary error
    if latex_errors:
        first = latex_errors[0]
        if "Missing \\begin{document}" in first:
            r["category"] = "missing_begin_document"
        elif "Two \\documentclass" in first or "\\documentstyle" in first:
            r["category"] = "duplicate_documentclass"
        elif "File `" in first and ".sty' not found" in first:
            r["category"] = "missing_sty"
        elif "File `" in first and "not found" in first:
            r["category"] = "missing_file"
        elif "Undefined control sequence" in first:
            r["category"] = "undefined_command"
        elif "Missing number" in first or "Illegal unit" in first:
            r["category"] = "bad_argument"
        elif "Missing } inserted" in first or "Missing { inserted" in first:
            r["category"] = "unmatched_braces"
        elif "\\begin{document} ended by" in first:
            r["category"] = "bad_structure"
        else:
            r["category"] = "other_latex_error"
    elif r["pdflatex_ok"] and latex_warnings:
        r["category"] = "warnings_only"
    elif r["pdflatex_ok"]:
        r["category"] = "clean"
    else:
        r["category"] = "no_pdf_no_error"

    r["phase"] = "done"
    results.append(r)
    if r["category"] == "clean":
        print(
            f"  [{i+1}/{N}] {base}: ✓ PDF {r['pdf_pages']}p  macros:{r['macros_before']}→{r['macros_after']}"
        )
    else:
        print(
            f"  [{i+1}/{N}] {base}: {r['category']}  (pdf={r['pdf_pages']}p errs={len(latex_errors)})"
        )

td = time.time() - t0
print(f"\nDone in {td:.0f}s ({td/N:.1f}s/file)")

# ---- Aggregate ----
cats = Counter(r.get("category", "unknown") for r in results)
phases = Counter(r.get("phase", "unknown") for r in results)
pdf_ok = sum(1 for r in results if r["pdflatex_ok"])
total_macros_before = sum(r.get("macros_before", 0) for r in results)
total_macros_after = sum(r.get("macros_after", 0) for r in results)

print("\n" + "=" * 60)
print("  RESULTS — ALL 1019 FILES")
print("=" * 60)
print(f"\n  {'Category':35s} {'Count':>6s}")
print(f"  {'-'*35} {'-'*6}")
for cat, cnt in cats.most_common():
    print(f"  {cat:35s} {cnt:6d}")
print(f"\n  {'Phase outcomes':35s} {'Count':>6s}")
print(f"  {'-'*35} {'-'*6}")
for ph, cnt in phases.most_common():
    print(f"  {ph:35s} {cnt:6d}")
print(f"\n  PDFs produced:     {pdf_ok:6d} / {N}")
print(f"  Total macros before: {total_macros_before}")
print(f"  Total macros after:  {total_macros_after}")
print(f"  Expansion rate:      {total_macros_before - total_macros_after} removed")

# Save full JSON
report = {
    "total_files": N,
    "categories": dict(cats),
    "phases": dict(phases),
    "pdfs_produced": pdf_ok,
    "macros_before": total_macros_before,
    "macros_after": total_macros_after,
    "results": results,
}
json_path = os.path.join(OUT, "batch_1019_report.json")
with open(json_path, "w") as fh:
    json.dump(report, fh, indent=2)
print(f"\nFull report: {json_path}")

# ---- Top error details ----
print(f"\n{'='*60}")
print("  TOP LATEX ERRORS (first 5 unique)")
print("=" * 60)
error_counter = Counter()
for r in results:
    for e in r.get("errors", []):
        error_counter[e[:120]] += 1
for err, cnt in error_counter.most_common(20):
    print(f"  [{cnt:4d}] {err[:110]}")
