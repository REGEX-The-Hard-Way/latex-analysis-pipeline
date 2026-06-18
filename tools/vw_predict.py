#!/usr/bin/env python3
"""
VW-Based Definition & Type Predictor

Reads sentences (from sent_split.py output), generates n-gram candidates
for each inline math variable, scores them with a trained VW model, and
outputs the best definition per variable with type annotations.

Usage:
    # Train
    python3 tools/vw_train.py > training.vw
    vw -d training.vw -f model.vw --loss_function logistic -b 24

    # Predict
    python3 tools/sent_split.py paper.tex sidecar.tok \
        | python3 tools/vw_predict.py --model model.vw

Output (TSV):
    token_id \t math_text \t definition \t type \t confidence
"""
import re, sys, os, subprocess, json
from collections import defaultdict

# ── Same feature definitions as vw_train.py ───────────────────
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
    'bulk', 'shear', 'hardness', 'coupling', 'fine',
}

TYPE_HINTS = {
    'constant': 'constant', 'Constant': 'constant',
    'integer': 'integer', 'Integer': 'integer',
    'real': 'real', 'Real': 'real',
    'complex': 'complex', 'Complex': 'complex',
    'dimensionless': 'constant',
    'dimension': 'integer',
    'scalar': 'real', 'vector': 'vector', 'tensor': 'tensor',
    'matrix': 'matrix',
    'function': 'function', 'operator': 'function',
}

# ── Candidate generation ──────────────────────────────────────

def ngrams(words, min_n=1, max_n=8):
    for n in range(min_n, min(max_n + 1, len(words) + 1)):
        for i in range(len(words) - n + 1):
            yield words[i:i + n], i, i + n


def compute_features(ngram_words, ngram_start, ngram_end, var_pos, all_words):
    f = {}
    ngram_text = ' '.join(ngram_words).lower()
    ngram_clean = ngram_text.strip('.,;:()[]{}"\'')

    f['wc'] = len(ngram_words)
    f['dist'] = (ngram_end - var_pos - 1) if ngram_end <= var_pos else (ngram_start - var_pos)
    f['art_the'] = 1 if ngram_clean.startswith('the ') else 0
    f['art_a'] = 1 if ngram_clean.startswith('a ') else 0
    f['art_an'] = 1 if ngram_clean.startswith('an ') else 0

    key_hits = [w for w in ngram_words if w.lower().strip('.,;:()[]{}') in KEY_TERMS]
    f['key_count'] = len(key_hits)
    if key_hits:
        f['key_terms'] = '|'.join(key_hits[:5]).lower()

    type_words = [TYPE_HINTS.get(w, '') for w in ngram_words if w in TYPE_HINTS]
    if type_words:
        f['type_hint'] = '|'.join(type_words)
        f['hint_count'] = len(type_words)

    f['caps'] = sum(1 for w in ngram_words if w and w[0].isupper())
    f['ends_comma'] = 1 if ngram_text.rstrip().endswith(',') else 0

    bigrams = []
    for i in range(len(ngram_words) - 1):
        bg = f"{ngram_words[i].lower()}-{ngram_words[i+1].lower()}"
        bg = re.sub(r'[^a-z-]', '', bg)
        if bg: bigrams.append(bg)
    if bigrams:
        f['bigrams'] = '|'.join(bigrams[:5])

    f['rel_pos'] = int((ngram_start / max(1, len(all_words))) * 10)
    f['char_len'] = len(ngram_text)
    return f


def features_to_vw(features):
    parts = []
    for k in ['wc', 'dist', 'art_the', 'art_a', 'art_an', 'key_count',
              'hint_count', 'caps', 'ends_comma', 'rel_pos', 'char_len']:
        if k in features:
            parts.append(f'{k}:{features[k]}')
    for k in ['key_terms', 'type_hint', 'bigrams']:
        if k in features:
            for val in features[k].split('|'):
                if val: parts.append(f'{k}_{val}')
    return ' '.join(parts)


def candidate_ngrams(context, var_symbol):
    words = context.split()
    if not words: return []

    var_lower = var_symbol.lower().strip()
    var_positions = [i for i, w in enumerate(words)
                     if w.strip('.,;:()[]{}"\'').lower() == var_lower
                     or var_lower in w.strip('.,;:()[]{}"\'').lower()]
    if not var_positions:
        var_positions = [len(words) // 2]
    var_pos = var_positions[0]

    candidates = []
    for ngram_words, start, end in ngrams(words):
        if start > var_pos + 12 or end < var_pos - 3:
            continue
        ngram_text = ' '.join(ngram_words)
        if ngram_text.strip().lower() == var_lower:
            continue
        features = compute_features(ngram_words, start, end, var_pos, words)
        candidates.append((ngram_text, features))
    return candidates


# ── Heuristic scoring (no model) ──────────────────────────────

def heuristic_score(features):
    """
    Score an n-gram without a trained model.
    Returns float 0-1 where higher = more likely definition.
    Used as fallback when no VW model available.
    """
    score = 0.0

    # Article start + physics term = strong signal
    if features.get('art_the') or features.get('art_a') or features.get('art_an'):
        score += 0.3
    if features.get('key_count', 0) >= 1:
        score += 0.2
    if features.get('key_count', 0) >= 2:
        score += 0.1
    if features.get('hint_count', 0) >= 1:
        score += 0.1

    # Ideal word count: 3-5 words
    wc = features.get('wc', 0)
    if 3 <= wc <= 5:
        score += 0.1
    elif 2 <= wc <= 6:
        score += 0.05

    # Before variable, close distance
    dist = features.get('dist', -100)
    if -6 <= dist <= -1:
        score += 0.1

    # Capitalized = likely proper noun, not definition
    if features.get('caps', 0) > features.get('wc', 0) // 2:
        score -= 0.1

    # Comma before = appositive, strong signal
    if features.get('ends_comma'):
        score += 0.1

    return min(max(score, 0.0), 1.0)


# ── Prediction ────────────────────────────────────────────────

def predict_definitions(sentences, model_path=None):
    """
    For each sentence with token IDs, find the best definition for each ID.
    Returns dict: token_id -> (definition, confidence, type)
    """
    results = {}
    id_pattern = re.compile(r'\b(\d{6,})\b')

    for sent in sentences:
        token_ids = id_pattern.findall(sent)
        if not token_ids:
            continue

        words = sent.split()
        # Find position of each token ID
        id_positions = {}
        for i, w in enumerate(words):
            if w.isdigit() and len(w) >= 6:
                id_positions[w] = i

        for tid, pos in id_positions.items():
            # Use the sentence as context (the ID itself is the "variable")
            candidates = []
            for ngram_words, start, end in ngrams(words):
                # Skip n-grams that contain the token ID
                if any(pos <= i < pos + 1 for i in range(start, end)
                       if i < len(words) and words[i] == tid):
                    continue
                if start > pos + 12 or end < pos - 3:
                    continue
                ngram_text = ' '.join(ngram_words)
                features = compute_features(ngram_words, start, end, pos, words)
                score = heuristic_score(features)
                candidates.append((ngram_text, score, features))

            if not candidates:
                continue

            # Sort by score, pick best
            candidates.sort(key=lambda x: -x[1])
            best_text, best_score, best_features = candidates[0]

            if best_score >= 0.3:  # minimum confidence threshold
                # Infer type from features
                inferred_type = 'unknown'
                if best_features.get('type_hint'):
                    types = best_features['type_hint'].split('|')
                    from collections import Counter
                    inferred_type = Counter(types).most_common(1)[0][0]

                results[tid] = (best_text.strip(), best_score, inferred_type)

    return results


def main():
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument('--model', default=None, help='VW model file')
    ap.add_argument('--threshold', type=float, default=0.3,
                    help='Minimum confidence threshold')
    args = ap.parse_args()

    lines = [l.strip() for l in sys.stdin if l.strip() and len(l.strip()) > 10]
    definitions = predict_definitions(lines, args.model)

    for tid, (defn, score, itype) in sorted(definitions.items()):
        print(f'{tid}\t{tid}\t{defn}\t{itype}\t{score:.3f}')


if __name__ == '__main__':
    main()
