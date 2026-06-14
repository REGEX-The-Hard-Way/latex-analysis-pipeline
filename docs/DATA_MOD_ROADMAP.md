# Data Manipulation Roadmap

OpenRefine-inspired improvements for sidecar data transformation, clustering,
and interactive editing.

## Current State

| Capability | Status | Tool |
|-----------|--------|------|
| Rule engine (regex, strip, trim) | ✅ | `sidecar_mod` |
| Undo log generation | ✅ | `sidecar_mod` |
| Fingerprint clustering | ✅ | `cluster_find` |
| Levenshtein clustering | ✅ | `cluster_find` |
| Interactive fzf cluster merge | ✅ | `cluster_apply.sh` |
| Interactive fzf rule application | ✅ | `sidecar_apply.sh` |
| LaTeX delimiter stripping rules | ✅ | `strip_latex.rules` |

## Phase 1: Robustness (near-term)

| Task | Why | Effort |
|------|-----|--------|
| **Proper JSON parser** | `json_get_str` breaks on nested escapes, embedded `"`, and escaped quotes. Use Ragel `fcall`/`fret` recursive scanner (from deep dive) or link `jansson` | Medium |
| **Length + offset recomputation** | After text transforms, `length` and `offset` fields need updating for downstream tools | Small |
| **token_id, parent_id recomputation** | Hash changes cascade through parent-child relationships; parent_ids must be updated when child token_ids change | Medium |
| **Batch apply all clusters** | Currently merges one cluster at a time via fzf. Add `--auto` flag to apply all clusters with confirmation | Small |
| **Dry-run mode** | Preview changes without writing output. Shows before/after diff per record | Small |

## Phase 2: Clustering Improvements

| Task | Why | Effort |
|------|-----|--------|
| **N-gram fingerprint** | Better than simple normalize for LaTeX — captures structural similarity even with different parameter values. e.g., `\frac{a}{b}` and `\frac{x}{y}` cluster together | Medium |
| **Phonetic clustering** | Soundex/Metaphone for author name deduplication (e.g., `Trachenko` vs `Trachenco`) | Small |
| **PP M (positional) distance** | Better than Levenshtein for LaTeX — weights leading characters higher (commands matter more than arguments) | Medium |
| **Cluster size preview** | Heatmap showing which types have the most clusters. `cluster_find --stats` mode | Small |
| **Manual merge annotation** | Flag records as "do not merge" during fzf review, persist decisions across runs | Medium |

## Phase 3: Rule Chain Architecture

| Task | Why | Effort |
|------|-----|--------|
| **Named rule sets** | Save/load rule chains: `./sidecar_apply.sh --ruleset "strip-latex"` | Small |
| **Rule conditions** | Add `if_not_empty`, `if_matches`, `if_length_gt` guards on rules | Medium |
| **Multi-property transforms** | Apply rules to multiple properties per record (e.g., strip `\author{` from text AND update type) | Medium |
| **Chain undo (full reversal)** | Replay undo log in reverse order to restore original sidecar. `./sidecar_mod --undo` | Small |
| **Rule chaining with preview** | fzf preview shows cumulative effect of all rules in sequence on a sample record | Small |

## Phase 4: Interactive Editing

| Task | Why | Effort |
|------|-----|--------|
| **In-place record editing** | fzf → select record → edit text value in $EDITOR → recompute hashes → write back | Medium |
| **Bulk type renaming** | Rename all records of type X to type Y (e.g., `display_2_math` → `display_math`) | Small |
| **Record splitting** | Split long text into multiple tokens (e.g., concatenated citations into individual `\cite{...}` entries) | Large |
| **Record merging** | Merge adjacent tokens of same type (e.g., merge split math expressions) | Large |
| **Cross-reference repair** | After token_id changes, update all parent_id references. Validate no orphans | Large |

## Phase 5: OpenRefine-Style UI Concepts

| Task | Why | Effort |
|------|-----|--------|
| **Column view in REPL** | `.browse type` shows all values of a given type/property in a scrollable table | Medium |
| **Faceted browsing** | `.facet type` shows type distribution with counts, click to filter | Medium |
| **Expression language** | GREL-like mini-language for transforms: `value.replace(/\\author{(.*)}/, "$1")` | Large |
| **Operation history in REPL** | `.history` shows applied transforms with undo capability | Medium |
| **Export as .cypher** | Convert sidecar records to CREATE statements for the graph store | Small |

## Integration with fv.sh Workflow

The core workflow remains:
```
search → fzf preview → select → apply → verify
```

Applied to:
- **Rules**: `sidecar_apply.sh` — preview transform on first match, confirm, apply all
- **Clusters**: `cluster_apply.sh` — browse clusters, select canonical, merge
- **Records**: future — browse/filter records with fzf, open in $EDITOR, save
