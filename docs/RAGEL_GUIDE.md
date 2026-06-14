# Ragel for C Programmers: A Practical Guide

*Derived from production scanner work — macros, context tracking, include machines,
recursive block scanning, and when to bail out.*

---

## 1. The `do{}while(0)` Macro Rule (Critical)

**Every action macro that expands to multiple C statements MUST be wrapped in
`do { ... } while(0)`.** Otherwise `if (cond) MACRO(...);` only controls the
first statement, and the rest execute unconditionally — silently wrong.

```c
/* WRONG — only first statement is guarded by if */
#define EMIT(t) \
  uint32_t id = murmur3(...); \
  fwrite(buf, 1, len, f); \
  printf("  %u  ", id);

/* RIGHT — entire block is a single compound statement */
#define EMIT(t) do { \
  uint32_t id = murmur3(...); \
  fwrite(buf, 1, len, f); \
  printf("  %u  ", id); \
} while(0)

/* Now this works correctly */
if (g_in_context) EMIT("type");
```

The trailing `while(0)` absorbs the semicolon from `EMIT("type");` so the caller
doesn't need to change.

---

## 2. Context Counters (global `static int`)

For multi-level scanners that need to know "am I inside a math block?", use a
global `static int` counter that `EMIT_BLOCK` increments before recursing and
decrements after:

```c
static int g_in_math = 0;

#define EMIT_BLOCK(t, prefix_len, suffix_len) do { \
  /* ... emit block token ... */ \
  g_in_math++; \
  if (inner_len > 0) scanner(inner_ptr, inner_len, ...); \
  g_in_math--; \
} while(0)
```

Then conditionally emit child tokens:

```c
main :=|*
  math_var  => { if (g_in_math) EMIT("math_var"); };
  math_op   => { if (g_in_math) EMIT("math_op"); };
  # ... etc
*|;
```

This is simpler than a hash table of parent IDs and gives correct nesting:
`EMIT_BLOCK` for `equation` sets `g_in_math=1`, and `EMIT_BLOCK` for `frac`
inside it sets `g_in_math=2` — both are truthy so both work.

---

## 3. Include Machines (modular pattern definitions)

Split pattern definitions into separate `.rl` files, then include them:

```c
%%{
  machine strings;
  include latex "latex.rl";
  
  # local extensions
  citep = '\\citep' (braces | brackets braces);
  
  main :=|*
    begin_doc => { EMIT("begin_doc"); };
    # ...patterns from latex.rl are available here...
  *|;
}%%
```

- The `include` directive pastes the file inline at Ragel compile time.
- The included file does NOT need a `machine` statement matching the importer's
  machine name — Ragel `include` works by file content only.
- Only put pattern *definitions* in included files (named patterns like
  `braces = '{' any* '}';`). Put actions in the main file.

---

## 4. Scanner Mode (`|*`) — Priority-Based Tokenization

`|*` is Ragel's scanner mode. Each pattern is tried in *definition order* —
first match wins. This is NOT longest-match; it's *priority-by-position*.

```c
main :=|*
  # These take priority over generic patterns
  begin_doc => { EMIT("begin_doc"); };
  
  # Math patterns — fire inside math context only
  math_fn   => { if (g_in_math) EMIT("math_fn"); };
  
  # Generic prose — lowest priority, catches everything else
  sc_prose  => { printf("%.*s", (int)(te-ts), ts); };
  
  [ \t]+    => { printf(" "); };
  any       => { printf("%c", *ts); };
*|;
```

Key rules:
- `ts` = start of match, `te` = end of match (set by Ragel before action)
- `{te = p+1;{ ... }}` — Ragel sets `te` for patterns that end with a single
  sentinel character; the double-brace is Ragel's action wrapping
- The `any` catchall MUST come last

---

## 5. EMIT_BLOCK — Recursive Content Scanning

When a LaTeX environment wraps inner content (e.g., `\frac{a}{b}`, `\[...\]`,
`\begin{equation}...\end{equation}`), use `EMIT_BLOCK` which:

1. Emits the parent token (the block itself)
2. Strips the prefix/suffix delimiters
3. Recursively calls `scanner()` on the inner content
4. Passes its own `token_id` as `parent_id` to the recursive call — so child
   tokens link to the block

```c
#define EMIT_BLOCK(t, prefix_len, suffix_len) do { \
  uint32_t token_id = murmur3_seeded_v2(seed0, &in[ts-in], te-ts); \
  /* ... emit parent token with this token_id ... */ \
  g_in_math++; \
  if ((int)(te - (prefix_len + suffix_len) - ts) > 0) { \
    scanner(&in[ts + prefix_len - in], \
            te - (prefix_len + suffix_len) - ts, \
            filename, filepath_id, token_id, prefix_len, suffix_len); \
  } \
  g_in_math--; \
} while(0)
```

The recursive call's `parent_id` parameter is `token_id`, so child tokens
inherit the parent. The `prefix_len` and `suffix_len` are passed to the child
so it knows its offset within the original file (used for `offset` calculation).

Examples:
```c
frac              => { EMIT_BLOCK("frac", 5, 0); };     // \frac{...}{...}
display_math      => { EMIT_BLOCK("display_math", 2, 2); }; // \[ ... \]
inline_math       => { EMIT_BLOCK("math", 1, 1); };     // $ ... $
equation          => { EMIT_BLOCK("equation", 16, 14); }; // \begin{equation}...\end{equation}
```

---

## 6. Balanced/Nested Patterns with `when{!n}`

Ragel patterns can match nested structures (braces, begin/end pairs) using a
counter and the `when` guard:

```ragel
begin_doc = '\\begin{document}' @{n++;};
end_doc   = '\\end{document}'   @{n--;};
doc_body  = any+ - (begin_doc | end_doc);
doc_env   = begin_doc (begin_doc | end_doc | doc_body)* end_doc :> any when{!n};
```

- `@{n++;}` / `@{n--;}` — Ragel actions that update the counter during matching
- `(alt1 | alt2 | alt3)*` — zero or more of any alternative
- `:> any when{!n}` — **entering guard**: the next character must be `any` AND
  the counter must be zero. This prevents matching an unbalanced block.
- `any+ - (pattern)` — "any sequence that does NOT match pattern" (complement)

**Important:** `:> any when{!n}` requires a character AFTER the closing
delimiter. At EOF, `any` fails and the pattern doesn't finalize. Always ensure
a trailing character (typically `\n`) after balanced blocks in test input.

---

## 7. Compile-Time Configuration

### Ragel flags
```bash
ragel -m -G2 scanner.rl    # -m: minimal FSM, -G2: table-driven (fast, big)
ragel -m -T0 scanner.rl    # -T0: flat table (smaller, sometimes faster)
```

### GCC flags
```bash
gcc -O0 scanner.c ...    # ~30s compile (during development)
gcc -O2 scanner.c ...    # minutes with large FSMs (production)
```

- `-G2` produces larger C but compiles to faster code
- `-O2` on a 150K-line generated C file can take 60-120s — use `-O0` during
  iteration
- The scanner function (after `%% write exec;`) is a giant generated switch/goto
  block — this is normal

---

## 8. When NOT to Use Ragel (Dictionary → Hash Table)

Ragel FSMs compile to millions of lines of C for large unordered alternations
(like vocabulary lists). If you have >1000 string alternatives, use a
**sorted-array binary search** or **hash table** instead:

```c
// 13,439 words from vocab.rl → sorted array + bsearch()
// Compiles in <1 second vs. 60s for a 320K-line Ragel FSM
static const char *vocab_words[] = { "ab-initio", "abelian", ... };
int vocab_classify(const char *word, int len, const char **cat) {
    const char *key = word_buf;
    const char **found = bsearch(&key, vocab_words, VOCAB_COUNT,
                                  sizeof(char *), vocab_cmp);
    if (!found) { *cat = "UNKNOWN"; return 0; }
    /* heuristic classification from word shape */
    if (tech_chars) *cat = "TECHNICAL";
    else if (has_upper) *cat = "NAME";
    else if (has_space) *cat = "MULTI_WORD";
    else *cat = "WORD";
    return 1;
}
```

Ragel is optimal for:
- Character-class patterns (digits, whitespace, balanced brackets)
- Keyword alternation with <50 terms (LaTeX command names, math operators)
- Nested/balanced structures that need counter tracking
- Sequential tokenization where order matters (priority-based scanner)

Ragel is wrong for:
- Unordered vocabulary lookup (100+ words)
- Hash-based key matching
- Large flat alternations without character-class structure

---

## 9. Ragel Variables and Action Context

Inside scanner actions, these are always available:

| Variable | Meaning |
|----------|---------|
| `ts` | Start of matched substring (const char*) |
| `te` | End of matched substring (const char*) |
| `p` | Current position in input |
| `pe` | End of input buffer |
| `eof` | End-of-file sentinel (= `pe` for non-streaming) |
| `cs` | Current state (int) |
| `act` | Saved state for backtracking |

Use `(int)(te - ts)` for match length, `ts` for content pointer.
The `in` pointer (start of the current scanner input) is passed to `scanner()`,
and `&in[ts - in]` is the absolute pointer to the match within the input buffer.

---

## 10. Generated Code Structure

After `%% write data;` and `%% write exec;`, Ragel generates:

1. Static transition tables (arrays of ints)
2. A `_resume:` label with `st` (state) variable
3. A series of `_stNNNN:` goto labels for each state
4. Within states: character-range checks → `goto _stXXXX` transitions
5. At match boundaries: `trNNNN:` transition labels that trigger actions

The generated code is verbose but structurally simple — it's a DFA traversing
character by character with `goto` state transitions.

### Debugging generated code:
```bash
# Find where a specific action fires
grep -n 'EMIT_BLOCK.*frac' scanner.c

# Check that do-while wrapping is correct
grep -A2 'define EMIT(' scanner.c

# Verify guards are present
grep 'if (g_in_math)' scanner.c
```

---

## Cheat Sheet

```c
// Pattern definition
braces = '{' (any - '}')* '}';

// Machine declaration with include
%%{ machine strings; include latex "latex.rl"; }%%

// Scanner with priority actions  
main :=|*
  keyword  => { EMIT("keyword"); };
  pattern  => { EMIT_BLOCK("block", prefix, suffix); };
  any      => { printf("%c", *ts); };
*|;

// EMIT macro (always do-while)
#define EMIT(t) do { \
  uint32_t id = murmur3(...); \
  fwrite(buf, 1, len, out); \
} while(0)

// EMIT_BLOCK macro (always do-while, always g_in_math++)
#define EMIT_BLOCK(t, pl, sl) do { \
  uint32_t id = murmur3(...); \
  fwrite(buf, 1, len, out); \
  g_in_math++; \
  if (inner_len > 0) scanner(inner, inner_len, ..., id, ...); \
  g_in_math--; \
} while(0)

// Context guard (using do-while safe EMIT)
math_var => { if (g_in_math) EMIT("math_var"); };

// Balanced nesting
env_begin = '\\begin{env}' @{n++;};
env_end   = '\\end{env}'   @{n--;};
env_body  = any+ - (env_begin | env_end);
env       = env_begin (env_begin | env_end | env_body)*
            env_end :> any when{!n};
```
