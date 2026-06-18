#!/usr/bin/env python3
"""
Batch definition extraction pipeline.
Runs the full pipeline (scanner → sent_split → parse → extract_defs) 
on multiple papers and collects yield metrics.

Usage: python3 tools/batch_extract.py --papers N [--sample] [--out results.json]
"""
import subprocess, re, json, os, sys, time, glob, random

SCANNER = '/home/user/latex-analysis-pipeline/scanner/scanner.out'
SCANNER_DIR = '/home/user/latex-analysis-pipeline/scanner'
SENT_SPLIT = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'sent_split.py')
EXTRACT_DEFS = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'extract_defs')
PARSER = '/mnt/x/home/user/meta_dev/meta/build/profile'
PARSER_CONFIG = '/mnt/x/home/user/meta_dev/meta/build/config.toml'
PARSER_CWD = '/mnt/x/home/user/meta_dev/meta/build'
PAPERS_DIR = os.path.join(os.path.dirname(__file__), '../scanner/2003')


def process_paper(paper_path):
    """Run full pipeline on one paper. Returns dict of metrics."""
    paper_name = os.path.basename(paper_path)
    result = {'paper': paper_name, 'status': 'ok'}
    t0 = time.time()

    workdir = '/tmp/batch_extract'
    os.makedirs(workdir, exist_ok=True)

    # 1. Scanner → sidecar.tok
    sidecar = os.path.join(workdir, f'{paper_name}.tok')
    r = subprocess.run([SCANNER, os.path.abspath(paper_path), 'tex'],
                       capture_output=True, text=True, timeout=60,
                       cwd=SCANNER_DIR)
    # Scanner writes sidecar.tok to its CWD
    src_tok = os.path.join(SCANNER_DIR, 'sidecar.tok')
    if os.path.exists(src_tok):
        import shutil
        shutil.copy(src_tok, sidecar)
        os.remove(src_tok)
    else:
        result['status'] = 'scanner_failed'
        result['time'] = time.time() - t0
        return result

    # 2. sent_split → sentences
    r = subprocess.run(
        ['python3', SENT_SPLIT, paper_path, sidecar],
        capture_output=True, text=True, timeout=30)
    sentences = [s.strip() for s in r.stdout.strip().split('\n') if s.strip() and len(s.strip()) > 10]
    sents_file = os.path.join(workdir, f'{paper_name}.sents')
    with open(sents_file, 'w') as f:
        for s in sentences: f.write(s + '\n')

    sent_with_ids = sum(1 for s in sentences if re.search(r'\b\d{6,}\b', s))
    result['sentences'] = len(sentences)
    result['sentences_with_ids'] = sent_with_ids

    if sent_with_ids == 0:
        result['status'] = 'no_math'
        result['time'] = time.time() - t0
        return result

    # 3. Parse with meta/profile
    r = subprocess.run(
        [PARSER, PARSER_CONFIG, sents_file, '--parse'],
        capture_output=True, text=True, timeout=120, cwd=PARSER_CWD)

    # 4. extract_defs — run directly on the parsed output
    # Parser writes to <input>.parsed.txt (replacing .sents extension)
    parsed_src = sents_file.rsplit('.', 1)[0] + '.parsed.txt'
    if not os.path.exists(parsed_src):
        # Also check CWD and sents_file + '.parsed.txt'
        for alt in [sents_file + '.parsed.txt',
                     os.path.join(PARSER_CWD, os.path.basename(sents_file).rsplit('.',1)[0] + '.parsed.txt')]:
            if os.path.exists(alt):
                parsed_src = alt
                break

    if not os.path.exists(parsed_src):
        result['status'] = 'parser_failed'
        result['time'] = time.time() - t0
        return result

    # Copy to our workdir for consistency
    import shutil
    parsed_file = os.path.join(workdir, f'{paper_name}.parsed')
    shutil.copy(parsed_src, parsed_file)
    parsed_file = parsed_src  # use the source directly

    # Count parse trees
    with open(parsed_file) as f:
        ptree_text = f.read()
    trees = []
    depth = 0; start = -1
    for i, c in enumerate(ptree_text):
        if c == '(':
            if depth == 0: start = i
            depth += 1
        elif c == ')':
            depth -= 1
            if depth == 0 and start >= 0: trees.append(ptree_text[start:i+1]); start = -1
    result['parse_trees'] = len(trees)
    if len(trees) == 0:
        result['status'] = 'no_trees'
        result['time'] = time.time() - t0
        return result

    # 5. extract_defs
    r = subprocess.run(
        [EXTRACT_DEFS, parsed_file, sidecar],
        capture_output=True, text=True, timeout=30)
    defs = [l for l in r.stdout.strip().split('\n') if l.strip()]
    result['definitions'] = len(defs)

    # Count unique token IDs with definitions
    def_ids = set()
    for line in defs:
        parts = line.split('\t')
        if len(parts) >= 3 and parts[2].strip():
            def_ids.add(parts[0])
    result['unique_defined_ids'] = len(def_ids)

    result['time'] = time.time() - t0
    return result


def main():
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument('--papers', type=int, default=10)
    ap.add_argument('--sample', action='store_true')
    ap.add_argument('--out', default=None)
    args = ap.parse_args()

    papers = sorted(glob.glob(os.path.join(PAPERS_DIR, '*.tex')))
    if args.sample:
        random.seed(42)
        papers = random.sample(papers, min(args.papers, len(papers)))
    else:
        papers = papers[:args.papers]

    print(f'Processing {len(papers)} papers...')
    results = []
    ok, failed, no_math = 0, 0, 0
    total_defs = 0

    for i, p in enumerate(papers):
        name = os.path.basename(p)[:50]
        r = process_paper(p)
        results.append(r)

        if r['status'] == 'ok':
            ok += 1
            total_defs += r.get('definitions', 0)
        elif r['status'] == 'no_math':
            no_math += 1
        else:
            failed += 1

        defs = r.get('definitions', 0)
        sents = r.get('sentences', 0)
        trees = r.get('parse_trees', 0)
        t = r.get('time', 0)
        print(f'  [{i+1:3d}/{len(papers)}] {name:50s} '
              f'sents={sents:3d} trees={trees:3d} defs={defs:3d} '
              f'{t:.1f}s {r["status"]}')

    print(f'\n=== Summary ===')
    print(f'Papers processed: {len(papers)}')
    print(f'  OK:          {ok}')
    print(f'  No math:     {no_math}')
    print(f'  Failed:      {failed}')
    print(f'  Total defs:  {total_defs}')
    print(f'  Avg defs/OK: {total_defs/ok:.0f}' if ok > 0 else '  N/A')
    total_time = sum(r.get('time', 0) for r in results)
    print(f'  Total time:  {total_time:.0f}s ({total_time/len(papers):.1f}s/paper)')

    if args.out:
        with open(args.out, 'w') as f:
            json.dump(results, f, indent=2)
        print(f'\nResults written to {args.out}')


if __name__ == '__main__':
    main()
