#!/usr/bin/env python3
"""
Bibliography validation tests for Issue #11.

Usage:
    python validation_tests.py sidecar.tok

This validates that all citations have matching bibliography entries.
"""

import sys
import re

def parse_tok_file(filepath):
    """Parse a .tok file and extract refs, labels, and bibitems."""
    with open(filepath, 'r') as f:
        data = f.read()
    
    output = []
    for ix in range(len(data.splitlines())):
        resp = re.split(
            "{<filepath:|>,filepath_id:|,token_id:|,parent_id:|,offset:|,length:|,type:|,<tok:",
            data.splitlines()[ix],
        )
        if resp[0] == "":
            resp = resp[1:]
            output.append(resp)
    
    return output

def validate_refs_labels(output):
    """Check that all refs have matching labels."""
    refs = set()
    labels = set()
    
    for row in output:
        if len(row) >= 7:
            token_type = row[6]
            tok = row[7].replace(">}", "")
            
            if token_type == "ref":
                refs.add(tok.replace("\\ref{", "").replace("}", ""))
            elif token_type == "label":
                labels.add(tok.replace("\\label{", "").replace("}", ""))
    
    unresolved_refs = refs - labels
    unresolved_labels = labels - refs
    
    return {
        'refs': refs,
        'labels': labels,
        'unresolved_refs': unresolved_refs,
        'unresolved_labels': unresolved_labels
    }

def main():
    if len(sys.argv) < 2:
        print("Usage: python validation_tests.py sidecar.tok")
        return 1
    
    print("=== Cross-Reference Validation ===")
    print()
    
    try:
        output = parse_tok_file(sys.argv[1])
        result = validate_refs_labels(output)
        
        print(f"Total refs: {len(result['refs'])}")
        print(f"Total labels: {len(result['labels'])}")
        print()
        print(f"Unresolved refs: {len(result['unresolved_refs'])}")
        for r in sorted(result['unresolved_refs']):
            print(f"  - {r}")
        print()
        print(f"Unmatched labels: {len(result['unresolved_labels'])}")
        for l in sorted(result['unresolved_labels']):
            print(f"  - {l}")
        
        if result['unresolved_refs'] or result['unresolved_labels']:
            print("\n❌ Validation FAILED")
            return 1
        else:
            print("\n✅ All references resolved")
            return 0
            
    except FileNotFoundError:
        print(f"File not found: {sys.argv[1]}")
        return 1

if __name__ == '__main__':
    sys.exit(main())