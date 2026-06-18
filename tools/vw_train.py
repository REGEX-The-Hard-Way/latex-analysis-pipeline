#!/usr/bin/env python3
"""
VW Training Data Generator for Definition Extraction

Reads benepar ground truth (sandbox/qa/output.json) and sound1.tex to
generate Vowpal Wabbit training data. Each example is an n-gram candidate
labeled +1 (definition) or -1 (not definition).

Output: VW-format training data to stdout.

Usage:
    python3 tools/vw_train.py > training.vw
    vw -d training.vw -f model.vw --loss_function logistic
"""
import re, json, sys, os, random
from collections import defaultdict

# ── Ground truth ──────────────────────────────────────────────
GROUND_TRUTH_FILE = os.path.join(os.path.dirname(__file__),
                                  '../sandbox/qa/output.json')
SOUND1_TEX = os.path.join(os.path.dirname(__file__),
                          '../scanner/sound1.tex')

# ── Physics/math key terms (strong signal for definitions) ────
KEY_TERMS = {
    'constant', 'mass', 'ratio', 'speed', 'energy', 'radius',
    'charge', 'limit', 'bound', 'function', 'density', 'frequency',
    'temperature', 'pressure', 'distance', 'velocity', 'coefficient',
    'modulus', 'length', 'number', 'time', 'rate', 'separation',
    'scale', 'parameter', 'factor', 'property', 'equation',
    'electron', 'proton', 'atom', 'molecule', 'particle',
    'sound', 'light', 'heat', 'entropy', 'viscosity', 'diffusion',
    'momentum', 'field', 'force', 'current', 'potential',
    'elastic', 'magnetic', 'electric', 'thermal', 'mechanical',
    'Planck', 'Bohr', 'Rydberg', 'Debye', 'Fermi',
    'interatomic', 'bonding', 'phase', 'wave', 'lattice',
    'vibrational', 'elasticity', 'liquid', 'solid', 'metal',
    'bulk', 'shear', 'hardness', 'coupling', 'Fermi', 'fine',
}

# ── Type hint words ───────────────────────────────────────────
TYPE_HINTS = {
    'constant': 'constant',
    'Constant': 'constant',
    'integer': 'integer',
    'Integer': 'integer',
    'real': 'real',
    'Real': 'real',
    'complex': 'complex',
    'Complex': 'complex',
    'dimensionless': 'constant',
    'dimension': 'integer',
    'scalar': 'real',
    'vector': 'vector',
    'tensor': 'tensor',
    'matrix': 'matrix',
    'function': 'function',
    'operator': 'function',
}


def load_ground_truth(path=GROUND_TRUTH_FILE):
    """Load benepar ground truth. Returns list of (symbol, definition, char_span, sentence)."""
    if not os.path.exists(path):
        return []
    with open(path) as f:
        text = f.read()
    # Extract individual JSON objects from list
    objects = []
    depth = 0; start = -1
    for i, c in enumerate(text):
        if c == '{':
            if depth == 0: start = i
            depth += 1
        elif c == '}':
            depth -= 1
            if depth == 0 and start >= 0:
                objects.append(text[start:i+1])
                start = -1
    data = []
    for obj in objects:
        try:
            data.append(json.loads(obj))
        except json.JSONDecodeError:
            pass
    return [(e['symbol'], e['definition'], e.get('char_span', []), e.get('sentence', ''))
            for e in data]


def extract_inline_math(tex_path=SOUND1_TEX):
    """Extract all inline math spans with surrounding text."""
    with open(tex_path) as f:
        tex = f.read()

    body_start = tex.find(r'\begin{document}')
    body_end = tex.find(r'\begin{thebibliography}')
    if body_end < 0: body_end = len(tex)
    body = tex[body_start:body_end]

    spans = []
    for m in re.finditer(r'(?<!\$)\$(?!\$)([^$]+?)\$(?!\$)', body):
        body_clean = m.group(1).strip()
        # Clean LaTeX
        body_clean = re.sub(
            r'\\(?:mathrm|mathbf|mathcal|mathbb|mathit|textrm|text|it|bf|rm|operatorname)\{([^}]*)\}',
            r'\1', body_clean)
        body_clean = body_clean.replace('\\', '').replace('{', '').replace('}', '')
        body_clean = body_clean.strip()

        # Get surrounding text (±80 chars)
        start = max(0, m.start() - 80)
        end = min(len(body), m.end() + 80)
        context = body[start:end]
        # Clean context of LaTeX
        context = re.sub(r'\$', ' ', context)
        context = re.sub(r'\\[a-zA-Z]+(\{[^}]*\})?', ' ', context)
        context = re.sub(r'[{}]', ' ', context)
        context = re.sub(r'\s+', ' ', context).strip()

        spans.append({
            'symbol': body_clean,
            'math_text': m.group(0),
            'offset': m.start(),
            'context': context,
        })

    return spans


def ngrams(words, min_n=1, max_n=8):
    """Generate all n-grams from a word list."""
    for n in range(min_n, min(max_n + 1, len(words) + 1)):
        for i in range(len(words) - n + 1):
            yield words[i:i + n], i, i + n


def candidate_ngrams(context_text, variable_symbol):
    """
    Generate candidate n-grams from context around a variable.
    Returns list of (ngram_text, features_dict).
    """
    # Tokenize: split on whitespace, keep punctuation attached
    words = context_text.split()
    if not words:
        return []

    # Find the variable in the context
    var_positions = []
    var_lower = variable_symbol.lower().strip()
    for i, w in enumerate(words):
        w_clean = w.strip('.,;:()[]{}"\'').lower()
        if w_clean == var_lower or var_lower in w_clean:
            var_positions.append(i)

    if not var_positions:
        # Fallback: use middle of context
        var_positions = [len(words) // 2]
    var_pos = var_positions[0]

    candidates = []
    for ngram_words, start, end in ngrams(words):
        # Only consider n-grams within ±N words of the variable
        if start > var_pos + 12 or end < var_pos - 3:
            continue

        ngram_text = ' '.join(ngram_words)
        # Skip if the ngram IS the variable itself
        if ngram_text.strip().lower() == var_lower:
            continue

        features = compute_features(ngram_words, start, end, var_pos, words)
        candidates.append((ngram_text, features))

    return candidates


def compute_features(ngram_words, ngram_start, ngram_end, var_pos, all_words):
    """Compute VW features for an n-gram candidate."""
    f = {}
    ngram_text = ' '.join(ngram_words).lower()
    ngram_clean = ngram_text.strip('.,;:()[]{}"\'')

    # 1. Word count
    f['wc'] = len(ngram_words)

    # 2. Distance from variable (negative = before, positive = after)
    if ngram_end <= var_pos:
        f['dist'] = ngram_end - var_pos - 1  # ends before var → negative
    else:
        f['dist'] = ngram_start - var_pos   # starts after var → positive

    # 3. Starts with article
    f['art_the'] = 1 if ngram_clean.startswith('the ') else 0
    f['art_a'] = 1 if ngram_clean.startswith('a ') else 0
    f['art_an'] = 1 if ngram_clean.startswith('an ') else 0

    # 4. Contains physics/math term
    key_hits = [w for w in ngram_words if w.lower().strip('.,;:()[]{}') in KEY_TERMS]
    f['key_count'] = len(key_hits)
    if key_hits:
        f['key_terms'] = '|'.join(key_hits[:5]).lower()

    # 5. Contains type hint
    type_words = [TYPE_HINTS.get(w, '') for w in ngram_words
                  if w in TYPE_HINTS]
    if type_words:
        f['type_hint'] = '|'.join(type_words)
        f['hint_count'] = len(type_words)

    # 6. Capitalized (proper noun indicator)
    caps = sum(1 for w in ngram_words if w and w[0].isupper())
    f['caps'] = caps

    # 7. Ends with comma (appositive indicator)
    f['ends_comma'] = 1 if ngram_text.rstrip().endswith(',') else 0

    # 8. Bigrams (top 5 by frequency)
    bigrams = []
    for i in range(len(ngram_words) - 1):
        bg = f"{ngram_words[i].lower()}-{ngram_words[i+1].lower()}"
        bg = re.sub(r'[^a-z-]', '', bg)
        if bg:
            bigrams.append(bg)
    if bigrams:
        f['bigrams'] = '|'.join(bigrams[:5])

    # 9. Position relative to variable end of context
    total_words = len(all_words)
    f['rel_pos'] = int((ngram_start / max(1, total_words)) * 10)  # decile 0-10

    # 10. Length in characters
    f['char_len'] = len(ngram_text)

    return f


def features_to_vw(features, namespace='d'):
    """Convert feature dict to VW string."""
    parts = []
    order = ['wc', 'dist', 'art_the', 'art_a', 'art_an', 'key_count',
             'hint_count', 'caps', 'ends_comma', 'rel_pos', 'char_len']
    for k in order:
        if k in features:
            parts.append(f'{k}:{features[k]}')

    # String features (hashed by VW)
    for k in ['key_terms', 'type_hint', 'bigrams']:
        if k in features:
            for val in features[k].split('|'):
                if val:
                    parts.append(f'{k}_{val}')

    return ' '.join(parts)


def generate_training_data():
    """Generate VW-format training data."""
    gt = load_ground_truth()
    math_spans = extract_inline_math()

    # Build ground truth lookup with fuzzy matching
    gt_entries = []  # (symbol_lower, definition_lower, definition_words)
    for sym, defn, span, sent in gt:
        gt_entries.append((sym.strip().lower(), defn.strip().lower(), 
                          set(defn.strip().lower().split())))

    positive_count = 0
    negative_count = 0

    for span in math_spans:
        sym = span['symbol']
        ctx = span['context']
        candidates = candidate_ngrams(ctx, sym)

        sym_lower = sym.strip().lower()

        # Find which candidates match ground truth
        has_positive = False
        for ngram_text, features in candidates:
            ngram_lower = ngram_text.strip().lower()
            is_def = False
            for gt_sym, gt_def, gt_words in gt_entries:
                if gt_sym == sym_lower:
                    # Exact match
                    if ngram_lower == gt_def:
                        is_def = True; break
                    # Fuzzy match: all GT words appear in ngram
                    if gt_words.issubset(set(ngram_lower.split())):
                        is_def = True; break
                    # Contains: GT definition is substring of ngram
                    if gt_def in ngram_lower:
                        is_def = True; break

            if is_def:
                print(f'1 |defn {features_to_vw(features)}')
                positive_count += 1
                has_positive = True

        # Add negative examples: random n-grams from same context
        if candidates:
            negatives = []
            for c in candidates:
                ngram_lower = c[0].strip().lower()
                is_pos = False
                for gt_sym, gt_def, gt_words in gt_entries:
                    if gt_sym == sym_lower:
                        if ngram_lower == gt_def or gt_words.issubset(set(ngram_lower.split())) or gt_def in ngram_lower:
                            is_pos = True; break
                if not is_pos:
                    negatives.append(c)
            random.shuffle(negatives)
            for ngram_text, features in negatives[:5]:  # max 5 negatives per variable
                print(f'-1 |defn {features_to_vw(features)}')
                negative_count += 1

    print(f'# Generated {positive_count} positive, {negative_count} negative examples',
          file=sys.stderr)


if __name__ == '__main__':
    generate_training_data()
