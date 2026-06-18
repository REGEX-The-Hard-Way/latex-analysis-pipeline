"""
Extract mathematical variable definitions from parse trees.
Uses constituency parse trees from meta/profile parser.
Input: .parsed.txt file with S-expression trees
Output: (token_id, math_text, definition) triples

Algorithm: for each (CD <id>) leaf, find the smallest article-starting
NP containing it, and collect all words from that NP minus the ID.
"""
import re, sys

def parse_parsed_file(path):
    """Read .parsed.txt, return list of S-expression tree strings."""
    with open(path) as f:
        text = f.read()
    trees, depth, start = [], 0, -1
    for i, c in enumerate(text):
        if c == '(':
            if depth == 0: start = i
            depth += 1
        elif c == ')':
            depth -= 1
            if depth == 0 and start >= 0:
                trees.append(text[start:i+1])
                start = -1
    return trees


def parse_tree(s, i=0):
    """Parse one S-expression tree into ('N', label, [children]) or ('T', tag, word)."""
    while i < len(s) and s[i] in ' \n\t\r': i += 1
    if i >= len(s) or s[i] != '(': return None, i
    i += 1
    while i < len(s) and s[i] in ' \n\t\r': i += 1
    j = i
    while j < len(s) and s[j] not in ' \n\t\r()': j += 1
    label = s[i:j]; i = j
    children = []
    while i < len(s):
        while i < len(s) and s[i] in ' \n\t\r': i += 1
        if i >= len(s): break
        if s[i] == ')': i += 1; break
        if s[i] == '(':
            child, i = parse_tree(s, i)
            if child: children.append(child)
        else:
            j = i
            while j < len(s) and s[j] not in ' \n\t\r()': j += 1
            children.append(('T', label, s[i:j])); i = j
    return ('N', label, children), i


def contains_id(node, tid):
    if node[0] == 'T': return node[2] == tid
    return any(contains_id(c, tid) for c in node[2])


PUNCT = {'.',',',':',';','--','-','(',')',"''",'``','$','#',
         "n't","'s","'ll","'ve","'re",'...','%','&','='}


def get_words(node, exclude=None):
    """Get all English words from a subtree, skipping IDs and punctuation."""
    if node[0] == 'T':
        w = node[2]
        if w == exclude: return []
        if w.isdigit(): return []
        if w in PUNCT: return []
        return [w]
    words = []
    for c in node[2]:
        words.extend(get_words(c, exclude))
    return words


def find_ids(node):
    """Find all numeric token IDs in a tree."""
    ids = []
    def walk(n):
        if n[0] == 'T' and n[2].isdigit(): ids.append(n[2])
        elif n[0] == 'N':
            for c in n[2]: walk(c)
    walk(node)
    return ids


def np_phrase(np_node, tid):
    """Extract the defining phrase from an NP node containing tid.
    Collects all words from the NP, skipping the ID itself."""
    words = []
    for c in np_node[2]:
        if contains_id(c, tid):
            if c[0] == 'N':
                words.extend(get_words(c, tid))
        else:
            words.extend(get_words(c, tid))
    return ' '.join(words).strip()


def extract_definition(root, tid):
    """Find the best definition NP for a token ID.
    
    Returns the smallest article-starting NP phrase containing the ID.
    If no article-starting NP exists, returns the smallest NP with 3+ words.
    """
    candidates = []
    
    def search(n, depth=0):
        if n[0] != 'N': return
        if n[1] == 'NP' and any(contains_id(c, tid) for c in n[2]):
            phrase = np_phrase(n, tid)
            if len(phrase) >= 3:
                starts_art = phrase.lower().startswith(('the ', 'a ', 'an '))
                wc = len(phrase.split())
                candidates.append((not starts_art, wc, -depth, phrase))
        for c in n[2]:
            search(c, depth + 1)
    
    search(root)
    if not candidates: return None
    
    # Prefer: starts with article, then smallest word count, then deepest
    candidates.sort()
    return candidates[0][3]


def extract_all(trees, id_info=None):
    """Extract definitions from all parse trees.
    id_info: optional dict of token_id -> (type, text) for display.
    Returns list of (token_id, definition) tuples.
    """
    results = []
    for tree_text in trees:
        parsed, _ = parse_tree(tree_text)
        if not parsed: continue
        for tid in find_ids(parsed):
            defn = extract_definition(parsed, tid)
            if defn:
                results.append((tid, defn))
    return results


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python extract_defs.py <file.parsed.txt> [sidecar.tok]")
        sys.exit(1)
    
    trees = parse_parsed_file(sys.argv[1])
    
    # Optional: load token info lookup
    id_info = {}
    if len(sys.argv) > 2:
        with open(sys.argv[2]) as f:
            for line in f:
                m = re.search(r'token_id:(\d+).*?type:([^,]+).*?<tok:(.*?)>', line)
                if m: id_info[m.group(1)] = (m.group(2), m.group(3).strip())
    
    for tid, defn in extract_all(trees, id_info):
        ttype, text = id_info.get(tid, ('?', '?'))
        print(f'{tid} {text[:50]:50s} {defn}')

