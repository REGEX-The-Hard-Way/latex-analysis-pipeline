import os, re, json
from collections import Counter

DIR = "/home/user/latex-analysis-pipeline/scanner/2003"

missing_packages = {
    "a4wide", "alpheqn", "axodraw", "bbm", "bbold", "bozhlogo", "bozhomac",
    "citesort", "curves", "doublespace", "dsfont", "eepic", "emlines", "emlines2",
    "epic", "epsf", "eqsection", "equations", "espcrc2", "feynmf", "feynmp",
    "footmisc", "fortschritte", "fullpage", "german", "iopams", "isolatin1",
    "multibox", "multirow", "mydefs2e", "myref", "mysymb", "paper2e", "physprbb",
    "psfig", "pstcol", "pstricks", "rsfs", "sc3conf", "showlabels", "simplemargins",
    "subeqn", "subeqnar", "subeqnarray", "subequation", "subfigure", "supercite",
    "type1cm", "ujp", "ulem", "wrapfig", "xy", "yfonts"
}

missing_classes = {
    "JHEP", "JHEP3", "amsppt", "cargese", "conm-p-l", "elsart", "iopart",
    "jnmp", "jpsj2", "pacmem", "ptptex", "revtex", "revtex4", "siggraph",
    "smfart", "svmult", "ws", "ws-procs9x6"
}

tex_files = sorted([f for f in os.listdir(DIR) if f.endswith(".tex")])

pkg_counter = Counter()
cls_counter = Counter()
file_info = {}  # file -> {packages: set, classes: set, has_article: bool}

for fname in tex_files:
    fpath = os.path.join(DIR, fname)
    with open(fpath, "r", errors="ignore") as f:
        content = f.read()

    used_pkgs = set()
    used_cls = set()
    has_article = False

    # find \documentclass[...]{...} or \documentclass{...}
    dc_matches = re.findall(r'\\documentclass(?:\[.*?\])?\{(.*?)\}', content)
    for cls in dc_matches:
        cls_name = cls.strip()
        if cls_name in missing_classes:
            used_cls.add(cls_name)
        if cls_name == "article":
            has_article = True

    # find \usepackage[...]{...} or \usepackage{...}
    up_matches = re.findall(r'\\usepackage(?:\[.*?\])?\{(.*?)\}', content)
    for pkg in up_matches:
        # handle multiple packages in one \usepackage{foo,bar}
        for p in pkg.split(","):
            pn = p.strip()
            if pn in missing_packages:
                used_pkgs.add(pn)

    if used_pkgs or used_cls:
        file_info[fname] = {
            "packages": used_pkgs,
            "classes": used_cls,
            "has_article": has_article
        }
        for p in used_pkgs:
            pkg_counter[p] += 1
        for c in used_cls:
            cls_counter[c] += 1

# 1. Frequency table of missing packages (descending)
print("=" * 60)
print("MISSING PACKAGES FREQUENCY (descending)")
print("=" * 60)
for pkg, count in pkg_counter.most_common():
    print(f"  {pkg:25s} {count:4d} files")

# 2. Frequency table of missing classes (descending)
print()
print("=" * 60)
print("MISSING CLASSES FREQUENCY (descending)")
print("=" * 60)
for cls, count in cls_counter.most_common():
    print(f"  {cls:25s} {count:4d} files")

# 3. Files that use 'article' class and only have missing packages (no missing classes)
print()
print("=" * 60)
print("FILES WITH 'article' CLASS AND ONLY MISSING PACKAGES")
print("=" * 60)
article_only_pkg = []
for fname, info in sorted(file_info.items()):
    if info["has_article"] and not info["classes"]:
        article_only_pkg.append(fname)
        pkgs_str = ", ".join(sorted(info["packages"]))
        print(f"  {fname}  [{pkgs_str}]")
print(f"\n  Total: {len(article_only_pkg)} files")

# 4. Files sorted by how many missing items they have (fewest first)
print()
print("=" * 60)
print("FILES SORTED BY MISSING ITEM COUNT (fewest first)")
print("=" * 60)
sorted_files = sorted(file_info.items(), key=lambda x: len(x[1]["packages"]) + len(x[1]["classes"]))
for fname, info in sorted_files:
    total = len(info["packages"]) + len(info["classes"])
    pkgs_str = ", ".join(sorted(info["packages"]))
    cls_str = ", ".join(sorted(info["classes"]))
    detail = []
    if pkgs_str:
        detail.append(f"pkg:[{pkgs_str}]")
    if cls_str:
        detail.append(f"cls:[{cls_str}]")
    print(f"  {fname:15s} ({total:2d} missing)  {'; '.join(detail)}")
