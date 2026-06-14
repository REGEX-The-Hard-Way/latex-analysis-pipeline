#!/usr/bin/env python3
"""
spectral_cluster.py — Spectral Clustering on sidecar token data.

Builds similarity graph from token co-occurrence (same filepath_id)
and parent-child relationships, then applies spectral clustering
via normalized Laplacian + eigendecomposition + k-means.

Usage: python3 spectral_cluster.py sidecar.json [--type label] [--k 5]
"""
import json, sys, argparse
from collections import defaultdict, Counter
import numpy as np

def load_tokens(filename, type_filter=None):
    """Load sidecar tokens, filter by type."""
    tokens = []
    for line in open(filename):
        if not line.strip(): continue
        rec = json.loads(line)
        if type_filter and rec.get('type') != type_filter: continue
        tokens.append(rec)
    return tokens

def build_graph(tokens):
    """Build sparse adjacency from co-occurrence (same filepath_id)."""
    node_ids = {t['token_id']: i for i, t in enumerate(tokens)}
    n = len(tokens)
    
    # group by filepath_id
    fp_groups = defaultdict(list)
    for i, t in enumerate(tokens):
        fp_groups[t.get('filepath_id', 0)].append(i)
    
    from scipy.sparse import lil_matrix
    W = lil_matrix((n, n))
    
    # same-file edges: connect all tokens in same paper
    for fid, indices in fp_groups.items():
        m = len(indices)
        if m > 100:  # too large, subsample
            indices = indices[:100]
            m = 100
        for ii in range(m):
            for jj in range(ii + 1, m):
                i, j = indices[ii], indices[jj]
                W[i, j] = 1.0
                W[j, i] = 1.0
    
    # parent-child edges
    for i, t in enumerate(tokens):
        pid = t.get('parent_id', 0)
        fid = t.get('filepath_id', 0)
        if pid != fid:
            for j, t2 in enumerate(tokens):
                if t2['token_id'] == pid:
                    W[i, j] = 0.5
                    W[j, i] = 0.5
                    break
    
    return W.tocsr(), node_ids

def spectral_cluster(W, k=5):
    """Spectral clustering via normalized Laplacian."""
    from scipy.sparse import diags, eye
    from scipy.sparse.linalg import eigsh
    
    n = W.shape[0]
    if n < k:
        print(f"Warning: {n} nodes < {k} clusters", file=sys.stderr)
        k = max(2, n)
    
    # normalized Laplacian
    d = np.array(W.sum(axis=1)).flatten()
    d_inv_sqrt = np.where(d > 0, 1.0 / np.sqrt(d), 0.0)
    L = eye(n) - diags(d_inv_sqrt) @ W @ diags(d_inv_sqrt)
    
    # smallest k+1 eigenvectors (skip the trivial zero eigenvector)
    vals, vecs = eigsh(L, k=k+1, which='SM')
    
    # skip first eigenvector (constant, eigenvalue ~0)
    embedding = vecs[:, 1:k+1]
    
    # normalize rows to unit length
    norms = np.linalg.norm(embedding, axis=1, keepdims=True)
    norms[norms == 0] = 1
    embedding = embedding / norms
    
    # k-means
    from sklearn.cluster import KMeans
    km = KMeans(n_clusters=k, random_state=42, n_init=10)
    labels = km.fit_predict(embedding)
    return labels

def main():
    ap = argparse.ArgumentParser(description='Spectral clustering on sidecar tokens')
    ap.add_argument('sidecar', help='sidecar.json file')
    ap.add_argument('--type', '-t', help='Filter by token type')
    ap.add_argument('--k', type=int, default=5, help='Number of clusters')
    ap.add_argument('--max-nodes', type=int, default=5000, help='Max nodes (subsample if needed)')
    args = ap.parse_args()

    print(f"Loading tokens (type={args.type or 'all'})...", file=sys.stderr)
    tokens = load_tokens(args.sidecar, args.type)
    
    if len(tokens) > args.max_nodes:
        print(f"  Subsampling {len(tokens)} → {args.max_nodes}", file=sys.stderr)
        tokens = tokens[:args.max_nodes]
    
    print(f"  {len(tokens)} tokens", file=sys.stderr)
    
    print("Building similarity graph...", file=sys.stderr)
    W, node_ids = build_graph(tokens)
    print(f"  {W.shape[0]} nodes, {W.nnz} edges", file=sys.stderr)
    
    print(f"Spectral clustering (k={args.k})...", file=sys.stderr)
    labels = spectral_cluster(W, k=args.k)
    
    # group by cluster
    clusters = defaultdict(list)
    for i, label in enumerate(labels):
        clusters[int(label)].append(tokens[i])
    
    # output
    for cid, members in sorted(clusters.items(), key=lambda x: -len(x[1])):
        texts = [t.get('text', '')[:80] for t in members]
        # canonical = most frequent text
        canonical = Counter(texts).most_common(1)[0][0]
        print(json.dumps({
            'type': args.type or '*',
            'canonical': canonical,
            'members': texts[:15],
            'count': len(members),
            'algorithm': 'spectral'
        }))
    
    print(f"\nDone: {len(clusters)} clusters", file=sys.stderr)

if __name__ == '__main__':
    main()
