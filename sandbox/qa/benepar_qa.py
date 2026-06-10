"""
Constituency-parser based extractor for inline-variable definitions in TeX,
using spaCy + benepar (Berkeley Neural Parser).

Approach:
    1. Preprocess TeX: replace every $...$ inline math token with a unique
       placeholder noun ("VARSYMnnn") and remember the mapping
       placeholder -> (symbol_key, original_char_span).
    2. Sentence-split + parse with spaCy + benepar.
    3. For each placeholder token, extract its definition NP using:
         (a) Apposition pattern   -- NP placeholder, NP placeholder,
                                     [det N ...] $X$
         (b) Copular pattern      -- placeholder is/are/denotes/represents NP
                                     where $X$ is NP
         (c) List apposition      -- "NP1 X1, NP2 X2, and NP3 X3"
                                     (siblings in the same coordinated NP)
    4. Map the chosen NP token span back to character offsets in the
       ORIGINAL TeX file via the placeholder mapping.

Install:
    pip install -U spacy benepar
    python -m spacy download en_core_web_md
    python -c "import benepar; benepar.download('benepar_en3')"

Usage:
    python benepar_qa.py sound1.tex --vars alpha m_e m_p v_u
"""

import argparse
import re
from dataclasses import dataclass
from pathlib import Path

import benepar
import spacy

SPACY_MODEL = "en_core_web_md"
BENEPAR_MODEL = "benepar_en3_large"

PLACEHOLDER_FMT = "VARSYM{:04d}"
PLACEHOLDER_RE = re.compile(r"VARSYM\d{4}")
EQN_FMT = "EQN{:04d}"
EQN_RE = re.compile(r"EQN\d{4}")
RELATION_RE = re.compile(r"=|\\approx|\\sim|\\equiv|\\propto|\\simeq|\\cong")
# Python 3.13+ re: unescaped $ at end of pattern is treated as anchor, not
# literal $.  Use a character class [$] everywhere to avoid this ambiguity.
DOLLAR_RE = re.compile(r"[$]([^$]{1,40})[$]")


# ---------------------------------------------------------------------------
# 1. Symbol key extraction from raw $...$ body
# ---------------------------------------------------------------------------
def symbol_key(math_body: str) -> str | None:
    s = math_body.strip()
    # Reject fractions / ratios outright — these are compound expressions and
    # any nearby definition usually applies to the whole ratio, not the
    # numerator alone (e.g. "the ratio $\frac{v_u}{c}$" defines the ratio).
    if re.search(r"\\frac\{", s) or re.search(r"\\d?frac\{", s):
        return None
    s = re.sub(r"\\rm\s*", "", s)
    s = re.sub(r"\\mathrm\{([^}]*)\}", r"\1", s)
    s = re.sub(r"\\text\{([^}]*)\}", r"\1", s)
    s = s.replace("\\", "").replace("{", "").replace("}", "").strip()
    if not s or len(s) > 12:
        return None
    if not re.match(r"^[A-Za-z][A-Za-z0-9_^]*$", s):
        return None
    return s


# ---------------------------------------------------------------------------
# 2. TeX preprocessing: $...$ -> VARSYMnnnn   (mapping kept for offset recovery)
# ---------------------------------------------------------------------------
@dataclass
class MathSpan:
    placeholder: str
    symbol: str  # canonical key, or "" for an equation
    orig_start: int
    orig_end: int
    raw: str = ""  # the original "$...$" text (for equations)
    is_equation: bool = False
    lhs_symbol: str = ""  # for equations: leftmost variable on the LHS


def _lhs_symbol_of_equation(body: str) -> str:
    """Return the canonical key of the leftmost variable on the LHS, or ''."""
    rel = RELATION_RE.search(body)
    lhs = body[: rel.start()] if rel else body
    # Try the whole LHS first, then leading token before any operator.
    for cand in (lhs, re.split(r"[\s+\-*/^_(]", lhs.strip(), maxsplit=1)[0]):
        sym = symbol_key(cand.strip())
        if sym:
            return sym
    return ""


# Regexes that match TeX command definitions whose bodies may contain
# spurious $...$ pairs (e.g. \\newcommand{\\foo}{$\\alpha$}).  We strip
# these *before* scanning for inline math so they don't pollute the
# symbol list.
#
# Because \\newcommand bodies can have deeply nested braces, we use a
# two-step approach: (1) regex to locate the start of a definition,
# (2) a simple brace-counter to find the matching closing brace.
_CMD_DEF_START_RE = re.compile(
    r"\\(?:newcommand|renewcommand|providecommand)"
    r"(?:\{[^}]*})?"
    r"(?:\[[^\]]*\])?"
    r"\{"
)


def _strip_command_defs(text: str) -> str:
    """Remove \\newcommand, \\renewcommand, \\def, \\newenvironment bodies
    from the TeX source so $...$ inside them doesn't produce spurious
    symbol keys."""
    out = []
    i = 0
    while i < len(text):
        m = _CMD_DEF_START_RE.search(text, i)
        if not m:
            out.append(text[i:])
            break
        # Copy everything before the opening brace.
        out.append(text[i : m.end() - 1])  # keep the preamble, drop the {
        # Now find the matching closing brace with a simple counter.
        depth = 1
        j = m.end()
        while j < len(text) and depth > 0:
            if text[j] == "{":
                depth += 1
            elif text[j] == "}":
                depth -= 1
            j += 1
        # Replace the body (including braces) with a single space.
        out.append(" ")
        i = j
    return "".join(out)


def preprocess_tex(text: str):
    """Return (processed_text, list_of_MathSpan).

    Inline math falls into three buckets:
      - bare variable     -> VARSYMnnnn placeholder
      - equation / value  -> EQNnnnn placeholder (kept so we can recover it
                             as a definition for the LHS variable)
      - other math        -> stripped to the literal token \"MATHEXPR\"
    """
    out, mapping = [], []
    i, idx = 0, 0
    while i < len(text):
        if text[i] == "$":
            j = text.find("$", i + 1)
            if j == -1:
                out.append(text[i:])
                break
            body = text[i + 1 : j]
            raw = text[i : j + 1]
            sym = symbol_key(body)
            if sym is not None:
                ph = PLACEHOLDER_FMT.format(idx)
                mapping.append(MathSpan(ph, sym, i, j + 1, raw=raw))
                out.append(ph)
                idx += 1
            elif RELATION_RE.search(body):
                lhs = _lhs_symbol_of_equation(body)
                ph = EQN_FMT.format(idx)
                mapping.append(
                    MathSpan(
                        ph, "", i, j + 1, raw=raw, is_equation=True, lhs_symbol=lhs
                    )
                )
                out.append(ph)
                idx += 1
            else:
                out.append("MATHEXPR")
            i = j + 1
        else:
            out.append(text[i])
            i += 1
    # Light TeX cleanup so spaCy doesn't choke
    processed = "".join(out)
    processed = re.sub(r"\\(cite|ref|label|eqref|footnote)\{[^}]*\}", "", processed)
    processed = re.sub(r"\\[a-zA-Z]+\*?", " ", processed)  # other commands
    processed = re.sub(r"[{}]", " ", processed)
    processed = re.sub(r"\s+", " ", processed)
    return processed, mapping


# ---------------------------------------------------------------------------
# 3. spaCy + benepar pipeline
# ---------------------------------------------------------------------------
def load_pipeline():
    nlp = spacy.load(SPACY_MODEL)
    if "benepar" not in nlp.pipe_names:
        nlp.add_pipe("benepar", config={"model": BENEPAR_MODEL})
    return nlp


# ---------------------------------------------------------------------------
# 4. Definition extraction from a parsed sentence
# ---------------------------------------------------------------------------
COPULAR_LEMMAS = {"be", "denote", "represent", "stand", "mean", "give"}


def find_def_for_token(tok) -> "spacy.tokens.Span | None":
    """Given a placeholder token, return the spaCy Span that defines it,
    or None if no clear definition is present."""
    sent = tok.sent

    # ---- Constituency-based: smallest NP containing the placeholder ----
    # benepar exposes parse via tok._.parse_string and span._.children
    np_span = _smallest_enclosing_np(tok)

    # (a) Apposition: NP -> [defining-NP] [placeholder-NP]
    #     e.g. "the fine structure constant VARSYM0"
    if np_span is not None:
        # If the NP contains the placeholder *and* extra material to the
        # left, that left material is the definition.
        left = sent[np_span.start : tok.i]
        if _looks_like_np(left):
            return _trim(left)

        # Or NP comma NP (apposition):  "the speed of sound, VARSYM0,"
        parent = _parent_constituent(np_span)
        if parent is not None:
            sib = _previous_np_sibling(parent, np_span)
            if sib is not None and _looks_like_np(sib):
                return _trim(sib)

    # ---- Dependency-based copular: "VARSYM is the NP" / "where VARSYM is NP"
    head = tok.head
    # When a token is a conjunct, its dependency head is the previous
    # conjunct, not the copula verb.  Walk up to find the real copula
    # and try _conjoined_copular_match; fall back to constituency parse.
    if tok.dep_ == "conj" and head.lemma_ not in COPULAR_LEMMAS:
        t = tok
        while t.dep_ == "conj" and t.head.dep_ != "ROOT":
            t = t.head
        if t.head is not None and t.head.lemma_ in COPULAR_LEMMAS:
            conj = _conjoined_copular_match(tok, t.head)
            if conj is not None:
                return _trim(conj)
        # If dependency-based conj-match fails, try constituency fallback.
        const_def = _constituency_copular_definition(tok)
        if const_def is not None:
            return _trim(const_def)
        # Do NOT fall through to regular copular attr lookup — it would
        # find the attr for the first conjunct, not this one.
    elif head.lemma_ in COPULAR_LEMMAS:
        # First: handle conjoined subjects with conjoined predicates,
        # e.g. "where $e$ and $m_e$ are electron charge and mass"
        conj = _conjoined_copular_match(tok, head)
        if conj is not None:
            return _trim(conj)
        # Find an attr/acomp/dobj NP child that isn't the placeholder
        for child in head.children:
            if child is tok:
                continue
            if child.dep_ in {"attr", "acomp", "dobj", "oprd"}:
                span = _expand_np(child)
                if span is not None and PLACEHOLDER_RE.search(span.text) is None:
                    return _trim(span)
    # Reverse copular: "the NP is VARSYM"
    if tok.dep_ in {"attr", "acomp"} and tok.head.lemma_ in COPULAR_LEMMAS:
        for child in tok.head.children:
            if child.dep_ == "nsubj":
                span = _expand_np(child)
                if span is not None and PLACEHOLDER_RE.search(span.text) is None:
                    return _trim(span)

    # ---- List-apposition fallback: "NP1 X1, NP2 X2, and NP3 X3" ----
    # Look immediately to the left for an NP-shaped run.
    left = _np_run_to_left(tok)
    if left is not None:
        return _trim(left)

    # ---- Comma-appositive fallback (parser-independent) --------------
    # Pattern:  "..., <NP>, VARSYMnnnn[, .]"
    # Walk left from the token: skip a comma, then collect the longest
    # determiner-led NP-shaped run.
    span = _comma_apposition_left(tok)
    if span is not None:
        return _trim(span)

    return None


def _conjoined_copular_match(tok, head):
    """Handle 'X and Y are A and B'  -> X→A, Y→B (positional alignment).

    Works whether `tok` is the head of the subject conjunction or one of the
    later conjuncts. Returns a Span over the matching predicate, or None.
    """
    # 1. Collect conjoined subjects in left-to-right order.
    subj_root = None
    for c in head.children:
        if c.dep_ == "nsubj":
            subj_root = c
            break
    if subj_root is None:
        # Maybe `tok` itself is a conj of nsubj attached elsewhere
        if tok.dep_ == "conj":
            t = tok
            while t.dep_ == "conj" and t.head.dep_ != "ROOT":
                t = t.head
            if t.dep_ == "nsubj":
                subj_root = t
        if subj_root is None:
            return None
    subjects = [subj_root] + [c for c in subj_root.conjuncts]
    subjects = sorted({s.i: s for s in subjects}.values(), key=lambda x: x.i)

    # 2. Collect conjoined predicates of the same head in document order.
    pred_root = None
    for c in head.children:
        if c is tok:
            continue
        if c.dep_ in {"attr", "acomp", "dobj", "oprd"}:
            pred_root = c
            break
    if pred_root is None:
        return None
    predicates = [pred_root] + list(pred_root.conjuncts)
    predicates = sorted({p.i: p for p in predicates}.values(), key=lambda x: x.i)

    if len(subjects) < 2 or len(predicates) < 2:
        return None
    # 3. Position-align tok to its predicate.
    try:
        idx = next(i for i, s in enumerate(subjects) if s.i == tok.i)
    except StopIteration:
        return None
    if idx >= len(predicates):
        idx = len(predicates) - 1  # fall back to last predicate
    pred = predicates[idx]
    span = _expand_np(pred)
    if span is None or PLACEHOLDER_RE.search(span.text):
        return None
    # Trim away any trailing "and <next predicate>" left in the subtree.
    next_starts = [p.i for p in predicates if p.i > pred.i]
    if next_starts:
        cutoff = min(next_starts)
        span = span.doc[span.start : min(span.end, cutoff)]
        # Drop a trailing "and"/"or"/"," cc.
        while len(span) > 1 and span[-1].lower_ in {"and", "or", ","}:
            span = span.doc[span.start : span.end - 1]
    return span


def _constituency_copular_definition(tok):
    """When a token is a conjunct whose dependency head is NOT a copula
    (e.g.  "...where a is X, and VARSYM is Y"), use the constituency
    parse to locate the S that contains the token and extract the NP
    complement of its copular VP.  Returns a Span or None.

    Only returns a result when the token sits inside a subject NP child
    of the S — otherwise we might pick up the wrong clause (e.g. a PP
    containing the token inside a larger S with an unrelated copula)."""
    sent = tok.sent
    # Find the smallest S constituent that contains the token, AND has
    # the token inside a subject NP (child of the S).
    best_s = None
    for span in _all_constituents(sent):
        if span.start <= tok.i < span.end and "S" in span._.labels:
            # Must have the token inside an NP child of this S.
            for child in span._.children:
                if "NP" in child._.labels and child.start <= tok.i < child.end:
                    if best_s is None or (span.end - span.start) < (
                        best_s.end - best_s.start
                    ):
                        best_s = span
                    break
    if best_s is None:
        return None
    # Check for a copular VP inside this S:
    #   (VP (VBZ/VBP/VBD is/are) (NP ...))
    for child in best_s._.children:
        if "VP" not in child._.labels:
            continue
        # Look for a copula token and an NP complement inside the VP.
        has_copula = False
        np_span = None
        for t in tok.doc[child.start : child.end]:
            # Only "be" is unambiguous; "give", "mean" etc. can be
            # non-copular transitives too easily in this heuristic.
            if t.lemma_ == "be":
                has_copula = True
                break
        if not has_copula:
            continue
        for vp_child in child._.children:
            if "NP" in vp_child._.labels:
                np_span = tok.doc[vp_child.start : vp_child.end]
                break
        if np_span is not None:
            if PLACEHOLDER_RE.search(np_span.text) is None and _looks_like_np(np_span):
                return np_span
    return None


def _comma_apposition_left(tok):
    """Handle '..., the long noun phrase, VARSYMnnnn[, .]'."""
    sent = tok.sent
    i = tok.i - 1
    # Require an immediately-preceding comma.
    while i >= sent.start and tok.doc[i].is_space:
        i -= 1
    if i < sent.start or tok.doc[i].text != ",":
        return None
    end = i  # exclusive end of NP run
    i -= 1
    # Walk left collecting NP-ish tokens (det/adj/noun/prep/det chain).
    allowed = {"DET", "ADJ", "NOUN", "PROPN", "ADP", "NUM"}
    while i >= sent.start:
        t = tok.doc[i]
        if PLACEHOLDER_RE.match(t.text):
            break
        if t.pos_ in allowed:
            i -= 1
            continue
        break
    start = i + 1
    if end - start < 2:
        return None
    span = tok.doc[start:end]
    # Must start with a determiner to look like a real NP definition.
    if span[0].lower_ not in {"the", "a", "an", "its", "their"}:
        return None
    return span


# ---------------- helpers over benepar constituency ----------------
def _smallest_enclosing_np(tok):
    sent = tok.sent
    best = None
    for span in _all_constituents(sent):
        if span.start <= tok.i < span.end and "NP" in span._.labels:
            if best is None or (span.end - span.start) < (best.end - best.start):
                best = span
    return best


def _all_constituents(sent):
    out = [sent]
    stack = [sent]
    while stack:
        s = stack.pop()
        for child in s._.children:
            out.append(child)
            stack.append(child)
    return out


def _parent_constituent(span):
    sent = span.sent
    for cand in _all_constituents(sent):
        if cand.start <= span.start and cand.end >= span.end and cand != span:
            children = list(cand._.children)
            if span in children:
                return cand
    return None


def _previous_np_sibling(parent, span):
    prev = None
    for child in parent._.children:
        if child == span:
            return prev
        if "NP" in child._.labels:
            prev = child
    return prev


def _looks_like_np(span) -> bool:
    if len(span) < 2 or len(span) > 12:
        return False
    if PLACEHOLDER_RE.search(span.text):
        return False
    first = span[0].lower_
    return first in {"the", "a", "an", "its", "their"} or span[0].pos_ in {
        "DET",
        "ADJ",
        "NOUN",
        "PROPN",
    }


def _expand_np(tok):
    """Expand a token to its full NP subtree as a Span."""
    sub = list(tok.subtree)
    if not sub:
        return None
    start = min(t.i for t in sub)
    end = max(t.i for t in sub) + 1
    return tok.doc[start:end]


def _np_run_to_left(tok):
    sent = tok.sent
    end = tok.i
    start = end
    # Walk left while we're inside a determiner-led NP-ish run.
    while start - 1 >= sent.start:
        t = tok.doc[start - 1]
        if t.pos_ in {
            "DET",
            "ADJ",
            "NOUN",
            "PROPN",
            "NUM",
        } and not PLACEHOLDER_RE.match(t.text):
            start -= 1
        else:
            break
    if end - start < 2:
        return None
    span = tok.doc[start:end]
    return span if _looks_like_np(span) else None


VAGUE_DEFS = {
    "the same",
    "the other",
    "the corresponding",
    "the calculated",
    "the experimental",
    "the form",
    "the order",
    "the value",
    "the case",
    "the result",
    "the number",
    "the set",
    "the factor",
    "the ratio",
    "the range",
    "the limit",
    "the sum",
    "the product",
    "a similar",
    "a different",
    "an other",
}
PRONOUNS = {"it", "this", "that", "these", "those", "such", "one", "they"}


def _trim(span):
    """Strip leading/trailing punctuation and dangling prepositions."""
    toks = [t for t in span if not t.is_punct]
    while toks and toks[-1].lower_ in {
        "of",
        "in",
        "with",
        "for",
        "by",
        "from",
        "to",
        "on",
        "at",
        "and",
        "or",
        "as",
    }:
        toks.pop()
    while toks and toks[0].is_punct:
        toks.pop(0)
    if not toks:
        return None
    out = span.doc[toks[0].i : toks[-1].i + 1]
    if not _is_definition_worthy(out):
        return None
    return out


def _is_definition_worthy(span) -> bool:
    """Reject vague/pronominal/too-short definitions."""
    text_lower = " ".join(t.lower_ for t in span if not t.is_punct)
    if text_lower in VAGUE_DEFS or text_lower in PRONOUNS:
        return False
    # Reject pronoun-only.
    if all(t.pos_ == "PRON" or t.is_punct for t in span):
        return False
    # Must contain at least one real NOUN or PROPN (not just det+adj).
    if not any(t.pos_ in {"NOUN", "PROPN"} for t in span):
        return False
    # Reject single-word definitions that are too short to be meaningful
    # (e.g. bare "the value" with short noun), but accept "the density",
    # "the mass", etc. when anchored by a determiner.
    content = [t for t in span if t.pos_ in {"NOUN", "PROPN", "ADJ"}]
    if len(content) < 2:
        if not content:
            return False
        # Single content word: require ≥5 chars OR a determiner is present
        # in the span (signals a real NP like "the density", not bare noun).
        has_det = any(t.pos_ == "DET" for t in span)
        if not has_det and len(content[0].text) < 5:
            return False
    return True


# ---------------------------------------------------------------------------
# 5. End-to-end extraction
# ---------------------------------------------------------------------------
@dataclass
class Result:
    symbol: str
    definition: str
    char_span: tuple[int, int]  # offsets in the ORIGINAL TeX
    sentence: str  # processed sentence (with placeholders)
    orig_sentence: str = ""  # original sentence text (with $...$ math)
    orig_sentence_offset: int = 0  # absolute start of orig_sentence in raw TeX


BENEPAR_MAX_SUBWORDS = 480  # leave headroom under the 512 hard limit


def _split_long(text: str, nlp_basic) -> list[str]:
    """Split a too-long sentence on ;, :, or — and finally hard-chunk."""
    parts = re.split(r"\s*(?:;|:| — |--)\s*", text)
    out = []
    for p in parts:
        if not p.strip():
            continue
        # Hard chunk by word count if still very long.
        words = p.split()
        if len(words) > 80:
            for i in range(0, len(words), 70):
                out.append(" ".join(words[i : i + 70]))
        else:
            out.append(p)
    return out


def _dump_parse(sent, console=None):
    """Pretty-print a benepar parse tree + dependency edges for one sentence."""
    if console is None:
        try:
            from rich.console import Console

            console = Console()
        except ImportError:
            console = None
    parse_str = sent._.parse_string if hasattr(sent._, "parse_string") else "(no parse)"
    pretty = _pretty_tree(parse_str)
    deps = "\n".join(
        f"  {t.i:>3}  {t.text:<18} {t.pos_:<6} {t.dep_:<10} ← {t.head.text}"
        for t in sent
    )
    block = (
        f"--- SENTENCE ---\n{sent.text}\n\n"
        f"--- CONSTITUENCY ---\n{pretty}\n\n"
        f"--- DEPENDENCIES ---\n{deps}\n"
    )
    if console is not None:
        from rich.panel import Panel

        console.print(Panel(block, border_style="magenta", expand=False))
    else:
        print(block)


def _pretty_tree(parse_str: str) -> str:
    """Indent a Penn-style bracketed parse string for readability."""
    out, depth = [], 0
    for ch in parse_str:
        if ch == "(":
            out.append("\n" + "  " * depth + "(")
            depth += 1
        elif ch == ")":
            depth -= 1
            out.append(")")
        else:
            out.append(ch)
    return "".join(out).lstrip("\n")


def extract(
    tex_path: Path,
    want_symbols: list[str],
    nlp,
    debug: bool = False,
    nlp_basic=None,
) -> list[Result]:
    raw = tex_path.read_text(encoding="utf-8", errors="replace")
    processed, mapping = preprocess_tex(raw)
    by_ph = {m.placeholder: m for m in mapping}
    want = set(want_symbols)

    # First pass: spaCy sentence-split (no benepar) to find sentence boundaries
    # that we can pre-filter / pre-split before passing to the heavy parser.
    if nlp_basic is None:
        nlp_basic = spacy.load(SPACY_MODEL, disable=["ner", "lemmatizer"])
    raw_sents = [s.text for s in nlp_basic(processed).sents]

    # Keep only sentences that contain a placeholder for a wanted symbol.
    keep = []
    for s in raw_sents:
        phs = PLACEHOLDER_RE.findall(s)
        if not phs:
            continue
        if not any(by_ph.get(ph) and by_ph[ph].symbol in want for ph in phs):
            continue
        keep.append(s)

    results: list[Result] = []
    for sent_text in keep:
        # Length-guard: benepar caps inputs at 512 sub-word tokens.
        chunks = [sent_text]
        # Cheap upper bound: ~1.3 sub-words per word for benepar's tokenizer.
        if len(sent_text.split()) > BENEPAR_MAX_SUBWORDS / 1.4:
            chunks = _split_long(sent_text, nlp_basic)

        for chunk in chunks:
            try:
                doc = nlp(chunk)
            except ValueError as e:
                # benepar's own length error -> last-ditch hard truncation
                if "exceeds the maximum supported length" in str(e):
                    words = chunk.split()
                    chunk = " ".join(words[:300])
                    try:
                        doc = nlp(chunk)
                    except Exception:
                        print(f"[skip] could not parse: {chunk[:80]}…")
                        continue
                else:
                    raise

            for sent in doc.sents:
                if debug and any(
                    PLACEHOLDER_RE.fullmatch(t.text)
                    and by_ph.get(t.text)
                    and by_ph[t.text].symbol in want
                    for t in sent
                ):
                    _dump_parse(sent)
                for tok in sent:
                    if not PLACEHOLDER_RE.fullmatch(tok.text):
                        continue
                    ms = by_ph.get(tok.text)
                    if ms is None or ms.symbol not in want:
                        continue
                    # Equation-as-definition shortcut: "<NP> X is/=  <eqn>"
                    # where <eqn> begins with X.
                    eqn_def = _equation_definition(tok, by_ph)
                    if eqn_def is not None:
                        defn_text = eqn_def.raw
                        char_span = (eqn_def.orig_start, eqn_def.orig_end)
                    else:
                        span = find_def_for_token(tok)
                        if span is None:
                            continue
                        defn_text = _clean_def_text(span.text)
                        if not defn_text:
                            continue
                        char_span = _locate_in_raw(defn_text, raw, ms.orig_start)
                    orig_sent, orig_off = _orig_sentence_for(sent.text, by_ph, raw)
                    results.append(
                        Result(
                            symbol=ms.symbol,
                            definition=defn_text,
                            char_span=char_span,
                            sentence=sent.text.strip(),
                            orig_sentence=orig_sent,
                            orig_sentence_offset=orig_off,
                        )
                    )
    return _postfilter(results)


def _equation_definition(tok, by_ph: dict) -> MathSpan | None:
    """Detect '<NP> X is/equals/approx <EQN>' where <EQN>'s LHS is X.

    Returns the EQN MathSpan to use as the definition, or None.
    """
    var = by_ph.get(tok.text)
    if var is None:
        return None

    head = tok.head
    sent = tok.sent

    # Case A: head is a copular/equality verb.
    if head.lemma_ in COPULAR_LEMMAS or head.lemma_ in {"equal", "give"}:
        for child in head.children:
            if child is tok:
                continue
            ms = by_ph.get(child.text)
            if ms is not None and ms.is_equation and ms.lhs_symbol == var.symbol:
                return ms

    # Case B: same sentence, EQN placeholder appearing immediately to the
    # right of `tok` (skipping a few words like "is", "approx", "=", "≈").
    for j in range(tok.i + 1, min(tok.i + 6, sent.end)):
        nxt = tok.doc[j]
        ms = by_ph.get(nxt.text)
        if ms is not None and ms.is_equation and ms.lhs_symbol == var.symbol:
            return ms
    return None


def _postfilter(results: list[Result]) -> list[Result]:
    """Per-symbol cleanup:
    (a) drop exact / substring duplicates of an earlier match;
    (b) once a 'strong' definition (>=3 words AND >=2 content tokens) exists
        for a symbol, drop any later definition that is shorter or weaker."""
    by_sym: dict[str, list[Result]] = {}
    for r in sorted(results, key=lambda r: r.char_span[0]):
        prev = by_sym.setdefault(r.symbol, [])
        rd = r.definition.lower().strip()
        # (a) duplicate / substring of an earlier definition
        if any(rd == p.definition.lower() or rd in p.definition.lower() for p in prev):
            continue
        # (b) once any prior definition is "strong", new ones must be at
        #     least as long (in words).
        strong_prev = [p for p in prev if len(p.definition.split()) >= 3]
        if strong_prev and len(r.definition.split()) < min(
            len(p.definition.split()) for p in strong_prev
        ):
            continue
        prev.append(r)
    out = [r for rs in by_sym.values() for r in rs]
    out.sort(key=lambda r: r.char_span[0])
    return out


def _orig_sentence_for(processed_sent: str, by_ph: dict, raw: str) -> tuple[str, int]:
    """Recover the original-TeX text for a sentence by using placeholder anchors."""
    phs = PLACEHOLDER_RE.findall(processed_sent) + EQN_RE.findall(processed_sent)
    spans = [by_ph[p] for p in phs if p in by_ph]
    if not spans:
        return processed_sent.strip(), 0
    lo = min(s.orig_start for s in spans)
    hi = max(s.orig_end for s in spans)
    # Expand to nearest sentence boundary on each side.
    start = max(0, raw.rfind(". ", 0, lo) + 1) or 0
    if start and raw[start] == " ":
        start += 1
    end_dot = raw.find(". ", hi)
    end = end_dot + 1 if end_dot != -1 else len(raw)
    return raw[start:end].strip(), start


# Placeholder residue that may leak into definition text and must be cleaned.
_DEF_CLEAN_RE = re.compile(r"\b(?:MATHEXPR|VARSYM\d{4}|EQN\d{4})\b")


def _clean_def_text(text: str) -> str:
    """Remove placeholder tokens (MATHEXPR, VARSYM..., EQN...) from a
    definition string, strip TeX comments, and collapse whitespace."""
    # 1. Remove TeX comments: % ... up to end of line or end of string.
    cleaned = re.sub(r"%.*$", "", text, flags=re.MULTILINE)
    # 2. Remove placeholder tokens.
    cleaned = _DEF_CLEAN_RE.sub(" ", cleaned)
    # 3. Collapse whitespace.
    cleaned = re.sub(r"\s+", " ", cleaned).strip()
    # 4. Drop leading/trailing punctuation and conjunctions left orphaned.
    cleaned = re.sub(r"^[,\s]+", "", cleaned)
    cleaned = re.sub(r"[,\s]+$", "", cleaned)
    # 5. Strip trailing "and"/"or" and dangling punctuation fragments.
    cleaned = re.sub(r"\s+(?:and|or)$", "", cleaned)
    cleaned = re.sub(r"^\s*(?:and|or)\s+", "", cleaned)
    # 6. Collapse repeated ", ," patterns from MATHEXPR removal.
    cleaned = re.sub(r",\s*,", ",", cleaned)
    cleaned = re.sub(r"\s+", " ", cleaned).strip()
    return cleaned


def _locate_in_raw(definition: str, raw: str, hint: int) -> tuple[int, int]:
    """Best-effort: find `definition` near `hint` in the raw TeX."""
    # Clean the definition text first, then try to locate it.
    clean = _clean_def_text(definition)
    if not clean:
        return (-1, -1)
    # Try exact match first (on cleaned text).
    idx = raw.rfind(clean, 0, hint)
    if idx == -1:
        idx = raw.find(clean, hint)
    if idx != -1:
        return (idx, idx + len(clean))
    # Whitespace-tolerant fallback on the cleaned definition.
    words = [re.escape(w) for w in clean.split() if w]
    if not words:
        return (-1, -1)
    pat = re.compile(r"\s+".join(words))
    m = (list(pat.finditer(raw, 0, hint))[-1:] or [None])[0] or pat.search(raw, hint)
    return (m.start(), m.end()) if m else (-1, -1)


# ---------------------------------------------------------------------------
def discover_symbols(tex_path: Path) -> list[str]:
    """Return all unique inline-math symbol keys appearing in the file."""
    raw = tex_path.read_text(encoding="utf-8", errors="replace")
    # Strip command definitions so $...$ inside \\newcommand etc. aren't
    # picked up as inline math.
    raw = _strip_command_defs(raw)
    seen: dict[str, None] = {}  # ordered set
    i = 0
    while i < len(raw):
        if raw[i] == "$":
            j = raw.find("$", i + 1)
            if j == -1:
                break
            body = raw[i + 1 : j]
            if len(body) <= 40:
                sym = symbol_key(body)
                if sym is not None and sym not in seen:
                    seen[sym] = None
            i = j + 1
        else:
            i += 1
    return list(seen)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("tex", type=Path)
    ap.add_argument(
        "--vars",
        nargs="+",
        default=None,
        help="Variable keys to look up, e.g. alpha m_e v_u. "
        "If omitted, all inline-math symbols in the file "
        "are auto-discovered.",
    )
    ap.add_argument(
        "--debug",
        action="store_true",
        help="Print constituency parse trees and dependency edges "
        "for every sentence containing a wanted symbol.",
    )
    ap.add_argument(
        "--out",
        type=Path,
        default=None,
        help="Write all results to this JSON file "
        "(easier to copy/paste than terminal output).",
    )
    ap.add_argument(
        "--review",
        action="store_true",
        help="Interactively step through every match and label "
        "it as + (correct), - (incorrect), s (skip), q (quit). "
        "Labels are saved alongside --out (or to "
        "review_labels.json by default).",
    )
    args = ap.parse_args()

    if args.vars is None:
        args.vars = discover_symbols(args.tex)
        print(
            f"[auto] discovered {len(args.vars)} inline-math symbols: "
            f"{', '.join(args.vars[:20])}"
            f"{' …' if len(args.vars) > 20 else ''}"
        )

    nlp = load_pipeline()
    results = extract(args.tex, args.vars, nlp, debug=args.debug)

    by_sym: dict[str, list[Result]] = {}
    for r in results:
        by_sym.setdefault(r.symbol, []).append(r)

    _render(args.vars, by_sym)

    # JSON dump (copy/paste friendly).
    if args.out is not None:
        import json

        payload = [
            {
                "symbol": r.symbol,
                "definition": r.definition,
                "char_span": list(r.char_span),
                "sentence": r.orig_sentence,
                "sentence_offset": r.orig_sentence_offset,
            }
            for r in results
        ]
        args.out.write_text(json.dumps(payload, indent=2, ensure_ascii=False))
        print(f"\nWrote {len(payload)} results to {args.out}")

    if args.review:
        _review(results, args.out or Path("review_labels.json"))


def _review(results: list["Result"], labels_path: Path):
    """Step through results and prompt the user for +/-/s/q."""
    import json

    try:
        from rich.console import Console
        from rich.panel import Panel
        from rich.text import Text

        console = Console()
    except ImportError:
        console = None

    # Resume support: load any prior labels.
    prior: dict[str, str] = {}
    label_file = labels_path.with_name(labels_path.stem + "_labels.json")
    if label_file.exists():
        prior = {row["key"]: row["label"] for row in json.loads(label_file.read_text())}
        print(f"[review] resuming – loaded {len(prior)} prior labels from {label_file}")

    def key(r):
        return f"{r.symbol}|{r.char_span[0]}-{r.char_span[1]}"

    labeled = []
    plus = minus = skipped = 0
    total = len(results)
    for i, r in enumerate(results, 1):
        k = key(r)
        if k in prior:
            labeled.append(
                {
                    "key": k,
                    "symbol": r.symbol,
                    "definition": r.definition,
                    "char_span": list(r.char_span),
                    "sentence": r.orig_sentence,
                    "label": prior[k],
                }
            )
            if prior[k] == "+":
                plus += 1
            elif prior[k] == "-":
                minus += 1
            else:
                skipped += 1
            continue

        # Render this match.
        header = f"[{i}/{total}]  ${r.symbol}$  →  “{r.definition}”"
        if console is not None:
            text = Text(r.orig_sentence)
            rel_s = r.char_span[0] - r.orig_sentence_offset
            rel_e = r.char_span[1] - r.orig_sentence_offset
            if 0 <= rel_s < rel_e <= len(r.orig_sentence):
                text.stylize("bold yellow on grey23", rel_s, rel_e)
            else:
                idx = r.orig_sentence.find(r.definition)
                if idx != -1:
                    text.stylize("bold yellow on grey23", idx, idx + len(r.definition))
            console.print(Panel(text, title=header, border_style="cyan"))
        else:
            print(f"\n{header}\n  {r.orig_sentence}")

        while True:
            try:
                ans = (
                    input("  [+] correct  [-] incorrect  [s] skip  [q] quit  > ")
                    .strip()
                    .lower()
                )
            except EOFError:
                ans = "q"
            if ans in {"+", "-", "s", "q"}:
                break
            print("  please enter +, -, s, or q")
        if ans == "q":
            break
        if ans == "+":
            plus += 1
        elif ans == "-":
            minus += 1
        else:
            skipped += 1
        labeled.append(
            {
                "key": k,
                "symbol": r.symbol,
                "definition": r.definition,
                "char_span": list(r.char_span),
                "sentence": r.orig_sentence,
                "label": ans,
            }
        )
        # Persist after every label so progress isn't lost.
        label_file.write_text(json.dumps(labeled, indent=2, ensure_ascii=False))

    print(
        f"\nReview done.  +{plus}   -{minus}   skipped {skipped}   "
        f"(saved {len(labeled)} rows to {label_file})"
    )


def _render(want_vars: list[str], by_sym: dict[str, list["Result"]]):
    """Pretty-print results with rich, restoring $...$ math and highlighting
    the matched span inside the original sentence."""
    try:
        from rich.console import Console
        from rich.panel import Panel
        from rich.text import Text
    except ImportError:
        print("(install `rich` for highlighted output: pip install rich)")
        for sym in want_vars:
            print(f"\n=== {sym} ===")
            for r in by_sym.get(sym, [])[:5]:
                print(f"  def={r.definition!r}  span={r.char_span}")
                print(f"     ⤷ {r.orig_sentence}")
        return

    console = Console()
    for sym in want_vars:
        title = f"[bold cyan]${sym}$[/]"
        hits = by_sym.get(sym, [])
        if not hits:
            console.print(
                Panel(
                    "[dim]no definition found[/]",
                    title=title,
                    border_style="red",
                    expand=False,
                )
            )
            continue
        for r in hits[:5]:
            text = Text(r.orig_sentence)
            # Highlight the match relative to the original sentence.
            rel_start = r.char_span[0] - r.orig_sentence_offset
            rel_end = r.char_span[1] - r.orig_sentence_offset
            if 0 <= rel_start < rel_end <= len(r.orig_sentence):
                text.stylize("bold yellow on grey23", rel_start, rel_end)
            else:
                # Fallback: highlight by literal substring search.
                idx = r.orig_sentence.find(r.definition)
                if idx != -1:
                    text.stylize("bold yellow on grey23", idx, idx + len(r.definition))
            subtitle = (
                f"[green]{r.definition}[/]   "
                f"[dim]chars {r.char_span[0]}–{r.char_span[1]}[/]"
            )
            console.print(
                Panel(
                    text,
                    title=title,
                    subtitle=subtitle,
                    border_style="cyan",
                    expand=True,
                )
            )


if __name__ == "__main__":
    main()
