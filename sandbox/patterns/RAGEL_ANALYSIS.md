# Ragel State Machine Compiler: Complete Analysis

## Project Summary

Ragel (version 7.0.4, 9,568 lines of C++ across 22 `.cc` files and 8 `.h` files, plus 38 Colm parser definitions) is a state machine compiler that generates executable finite state machines from regular expressions and state machine specifications embedded in host language source code. It supports **12 target languages** (C, C++, D, Java, C#, Go, Ruby, OCaml, Rust, JavaScript, Julia, GNU ASM x86-64), **8 minimization levels**, **7 code generation styles**, **3 control flow backends**, and **17 builtin character class machines**.

---

## Complete Functionality Inventory

### 1. Target Languages (12 Backends)

| Language | Backend Source | Alphabet Types | Control Flow | Key Types |
|----------|---------------|----------------|-------------|-----------|
| **C** | `host-c/main.cc` | 8 types (char, uchar, short, ushort, int, uint, long, ulong) | GotoFeature | Primary reference implementation |
| **D** | `host-d/main.cc` | 9 types (byte, ubyte, char, short, ushort, int, uint, wchar, dchar) | GotoFeature | 2-byte wchar/dchar support |
| **Java** | `host-java/main.cc` | 4 types (byte, short, char, int) | BreakFeature + loop labels | No unsigned primitives |
| **C#** | `host-csharp/main.cc` | 9 types (sbyte, byte, short, ushort, char, int, uint, long, ulong) | GotoFeature | Default alphtype=char |
| **Go** | `host-go/main.cc` | 10 types (byte..int64, rune) | GotoFeature | No `unsigned` prefix needed; custom `out-go.lm` template |
| **Ruby** | `host-ruby/main.cc` | 2 types (char, int) | BreakFeature | Minimal type set |
| **OCaml** | `host-ocaml/main.cc` | 1 type (int, 31-bit) | VarFeature (pattern matching) | Only var-feature backend |
| **Rust** | `host-rust/main.cc` | 1 type (u8) | BreakFeature + loop labels | Minimal; `'label: loop {}` syntax |
| **JavaScript** | `host-js/main.cc` | 7 types (s8, u8, s16, u16, i32, u32, number) | BreakFeature + loop labels | Explicit bit-width type names |
| **Julia** | `host-julia/main.cc` | 1 type (u8/byte) | GotoFeature | Uses `@label`/`@goto` macros |
| **ASM x86-64** | `host-asm/main.cc` | 8 types (C-like) | GotoFeature, **Direct** backend | Only Direct (non-translated) backend |
| **Crack** | `host-crack/main.cc` | 5 types (byte, int32, uint32, int, uint) | BreakFeature | Custom host scanner in `ragel-crack.lm` |

### 2. Code Generation Styles

| Style | CLI Flag | Description |
|-------|----------|-------------|
| Binary Loop | `-T0` | Binary search table, loop-driven (default) |
| Binary Expanded | `-T1` | Binary search table, inlined |
| Flat Table Loop | `-F0` | Single flat transition table, loop-driven |
| Flat Table Expanded | `-F1` | Flat table, inlined |
| Switch Loop | `-G0` | Switch/case dispatch, loop-driven |
| Switch Expanded | `-G1` | Switch/case, inlined |
| IP-Goto | `-G2` | Computed goto (GCC extension), inlined |

### 3. Control Flow Backends

| Feature | Used By | Mechanism |
|---------|---------|-----------|
| `GotoFeature` | C, C++, D, C#, Go, Julia, ASM | Native `goto` for state transitions |
| `BreakFeature` | Java, Ruby, JS, Rust, Crack | Labeled `break` from loops |
| `VarFeature` | OCaml | Pattern-matching variable assignment |

### 4. Minimization Levels (8 total)

| Flag | Level | Description |
|------|-------|-------------|
| `-n` | MinimizeNone | No minimization |
| `-m` | MinimizeEnd | Minimize after building each machine |
| `-l` | MinimizeMostOps | Minimize after most operations |
| `-e` | MinimizeEveryOp | Minimize after every operation |
| `-a` | MinimizeApprox | Approximate (faster, potentially suboptimal) |
| `-b` | MinimizeStable | Stable partition-based |
| `-j` | MinimizePartition1 | Exact partition variant 1 |
| `-k` | MinimizePartition2 | Exact partition variant 2 |

### 5. Parser Frontends

| Frontend | Flag | Description |
|----------|------|-------------|
| Reduce-based | `--reduce-frontend` (default) | Colm-based recursive descent |
| Kelbt-based | `--kelbt-frontend` | Legacy kelbt parser |
| Colm-based | `--colm-frontend` | Direct Colm parsing |
| rlhc | `--rlhc` | Ragel Host Compiler sub-tool |

### 6. Ragel DSL Feature Catalog

#### Pattern Atoms
- Literal strings: `'abc'`, `"abc"`, `0xHH`
- Character ranges: `[a-z]`, `[^a-z]`, `'a' .. 'z'`, `'a' ../i 'z'` (case-insensitive)
- Regular expressions: `/pattern/`, `/pattern/i`
- 17 builtin machines: `any`, `ascii`, `extend`, `alpha`, `digit`, `alnum`, `lower`, `upper`, `cntrl`, `graph`, `print`, `punct`, `space`, `xdigit`, `null`/`zlen`, `empty`
- Dot/any: `.` (regex any), `any` (builtin any)
- Negation: `!char` or `^char`, character class negation `[^...]`

#### Composition Operators
- Union: `|`
- Concatenation: `.` or juxtaposition
- Intersection: `&`
- Subtraction: `-`
- Strong subtraction: `--` (subtract anywhere, with `any*` padding)
- NFA-style union: `<nfa ( term1, term2, ... )`
- Priority union: `|= (priority, priority)`

#### Repetition Operators
- Kleene star: `*` (zero or more), `**` (greedy star)
- Kleene plus: `+` (one or more)
- Optional: `?` (zero or one)
- Exact count: `{n}`
- Max count: `{,m}`
- Min count: `{n,}`
- Range count: `{n,m}`
- Conditional star: `:condstar(pat, init, inc, min, max):`
- Conditional plus: `:condplus(pat, init, inc, min, max):`
- NFA generalized: `:nfa(pat, push, pop, init, stay, repeat, exit):`

#### Action Placement Operators (30+ total)
- Transition: `>` (entering), `@` (finishing), `$` (all), `%` (leaving)
- Global error: `>err`, `$err`, `%err`, `<>err`, `>!`, `$!`, `%!`
- Local error: `>lerr(n)`, `$lerr(n)`, `%lerr(n)`, `<>lerr(n)`
- EOF: `>eof`, `$eof`, `%eof`, `<>eof`, `@eof`, `$eof` variants
- To-state: `>to(n)`, `$to(n)`, `%to(n)`, `<>to(n)`
- From-state: `>from(n)`, `$from(n)`, `%from(n)`, `<>from(n)`
- Scanner-specific: `=>` (token action), `$~` (from-start), `$*` (to-start)

#### Runtime Control Variables
- `fpc` – current frame pointer position
- `fc` – current character (`*fpc`)
- `fcurs` – current state at point of call
- `fentry(name)` – entry state ID for named machine
- `ftargs` – target arguments of current call context

#### Runtime Control Commands
- `fgoto name` – immediate state transfer
- `fnext name` – schedule next state
- `fcall name` – push state + jump to entry
- `fret` – pop state from stack
- `fncall` / `fnret` – named variants
- `fbreak` – break out of current machine
- `fnbreak` – named break variant
- `fhold` – hold current character
- `fexec expr` – set input pointer and re-execute

#### Conditions & Guards
- `when condition` – guard transition on boolean condition
- `outwhen condition` – exit-gating condition
- `%when condition` – condition action on epsilon transition
- Condition actions: parameterized via `action c(v) { $v }`
- Inline conditions: `when { runtime_expr }`

#### Declarations & Configuration
- `machine name;` – define machine
- `alphtype type;` – set alphabet type
- `getkey expr;` – custom key access
- `access prefix;` – struct member prefix
- `variable p expr; variable pe expr; variable eof expr;` – rename data pointers
- `prepush { ... }; postpop { ... };` – stack management hooks
- `export name = expr;` – export character constant
- `import "file";` – import machine definitions
- `include "file";` / `include mach "file";` – include machines

#### Write Statements
- `write data;` / `write data noerror;` / `write data nofinal;`
- `write init;` / `write init nocs;`
- `write exec;` / `write exec noend;`
- `write start;` / `write first_final;` / `write error;`
- `write exports;`

### 7. Command-Line Options

**Output:** `-o <file>`, `-V` (DOT visualization), `-p` (printable chars), `-S <spec>`, `-M <machine>`
**Code Style:** `-T0/-T1/-F0/-F1/-G0/-G1/-G2`
**Minimization:** `-n`, `-m`, `-l`, `-e`, `-a`, `-b`, `-j`, `-k`
**Analysis:** `--breadth-check`, `--conds-depth`, `--trans-span`, `--state-limit`, `--input-histogram`
**Frontend:** `--reduce-frontend`, `--kelbt-frontend`, `--colm-frontend`
**Other:** `--var-backend`, `--no-fork`, `--save-temps`, `--no-intermediate`, `-L` (no line directives), `-I <dir>`, `-s` (stats), `-d` (keep dup actions), `-v` (version), `-h` (help)

---

## Top 100 Usage Patterns

### 1. Basic Literal Pattern Matching

```ragel
main := 'hello' '\n';
```

Matches the literal string "hello" followed by a newline. This is the simplest possible Ragel machine. The compiler builds a DFA with exactly 6 states (one per character) and a final accepting state after the newline.

**When to use:** The foundation of all Ragel machines. Use whenever you need to recognize a specific sequence of characters in a byte stream. Unlike `strncmp`, this compiles into an O(n) DFA with no function call overhead per character.

**Why it's useful:** Provides the simplest entry point to Ragel. All other patterns build on this concept. The generated code is a tight loop that consumes characters and transitions states, producing machine code that's typically faster than hand-written equivalent loops.

---

### 2. Alternation (Union of Patterns)

```ragel
main := ( 'hello' | 'world' | 'foo' | 'bar' ) '\n';
```

Accepts any of the four words followed by newline. Ragel compiles this into a prefix-sharing DFA — common prefixes between alternatives (e.g., common characters in "hello"/"help") share states transparently.

**When to use:** Keyword recognition, command dispatch, protocol verb parsing — any time multiple literal strings must be recognized. The machine automatically resolves ambiguous prefixes.

**Why it's useful:** Unlike chained `strcmp` calls (O(k×n) where k = keywords, n = length), the DFA approach is O(n). The compiler builds a trie-like structure with shared prefixes. Scales efficiently to thousands of keywords with no per-keyword overhead at runtime.

---

### 3. Character Class Ranges

```ragel
main := [a-zA-Z_] [a-zA-Z0-9_]* '\n';
```

Matches a C-style identifier (letter or underscore followed by zero or more alphanumerics or underscores) followed by newline. The `[... ]` syntax defines character sets as unions of ranges.

**When to use:** Defining token classes: identifiers, numbers, whitespace, symbols. Character classes are the building blocks of lexers.

**Why it's useful:** Compiles to efficient bitmask or range checks. The range `[a-zA-Z_]` internally becomes a compact representation that's checked with minimal instructions. Overlapping ranges are automatically merged by the compiler.

---

### 4. Negated Character Classes

```ragel
comment := '/*' [^*]* '*/';
# Or equivalently:
comment := '/*' ( any - '*' )* '*/';
```

Matches a C block comment using `[^*]*` to consume everything except the `*` character (avoiding premature terminator matching). The alternation shows both bracket-style negation and the `-` subtraction operator.

**When to use:** Matching "everything except X" patterns. Common in comment scanners, string literal matching (everything except the quote character), and data extraction.

**Why it's useful:** Negation eliminates the need for lookahead assertions. Without it, matching "any character except `*`" would require enumerating all 255 other characters. The subtraction operator compiles to efficient complementary set operations on the DFA's transition table.

---

### 5. The Dot/Any Builtins

```ragel
main := any;         # accepts exactly one character
main := any*;        # accepts any sequence
main := [^\n];       # any character except newline (bracket syntax)
```

The `any` builtin matches exactly one byte (0-255). The `any*` pattern matches zero or more characters. `[^\n]` is the bracket equivalent of character negation.

**When to use:** `any` is used as a catch-all in scanner patterns. `any*` appears in comments, string content, and skip-ahead patterns. `any` is also the foundation for character class subtraction.

**Why it's useful:** `any` is the universal character set — all other character classes are subsets. It's essential for building patterns that consume everything until a specific terminator, like `any* :>> '*/'` for block comments.

---

### 6. Builtin Character Classes (17 predefined)

```ragel
word     := alpha+;                    # [a-zA-Z]
number   := digit+;                    # [0-9]
alnum_u  := alnum | '_';              # [a-zA-Z0-9_]
spaces   := space+;                    # [ \t\n\r\f\v]
hexnum   := '0x' xdigit+;             # 0-9 a-f A-F
printable := print+;                   # printable ASCII
punct_tok := punct;                    # punctuation characters
cntrl_ch  := cntrl;                    # control characters
graphic   := graph;                    # visible characters
lowercase := lower;                    # [a-z]
uppercase := upper;                    # [A-Z]
ascii_ch  := ascii;                    # 0-127
any_byte  := any;                      # 0-255
ext_ch    := extend;                   # 128-255 (extended ASCII)
nothing   := empty;                    # never matches
epsilon   := zlen;                     # zero-length match
null_ch   := null;                     # the NUL character (0)
```

Lists all 17 builtin character class machines available in Ragel. Each is a pre-defined machine with a fixed character set mapping.

**When to use:** Any time you need standard character classification. `alpha`/`digit`/`alnum` for identifiers, `space` for whitespace, `xdigit` for hex numbers, `punct` for operators, `print`/`graph` for displayable characters.

**Why it's useful:** These classes match standard C/POSIX locale character classifications. They're implemented as FSMs built from range unions at compile time, so there's zero runtime overhead compared to hand-written character test functions.

---

### 7. The Empty and Zlen Machines

```ragel
main := empty '\n';    # Always fails — never matches
main := zlen '\n';     # Matches only newline (zlen matches empty string)
main := 'a' | zlen;    # Matches 'a' OR empty string (equivalent to 'a'?)
```

`empty` is the degenerate machine that rejects everything (equivalent to the empty set in automata theory). `zlen` matches the zero-length string (the epsilon transition — no input consumed).

**When to use:** `zlen` as a building block for `?`-like optional patterns, as an epsilon transition in statecharts, or as a null alternative in unions. `empty` is rarely used directly but appears in intermediate stages when operations produce the empty set.

**Why it's useful:** These are the identity elements of Ragel's FSM algebra. `zlen` is the identity for concatenation (X . zlen = X). `empty` is the identity for union (X | empty = X). Understanding them clarifies how Ragel's mathematical operators compose.

---

### 8. Kleene Star and Plus

```ragel
digits  := digit*;      # zero or more digits
digits1 := digit+;      # one or more digits
word    := alpha+;      # one or more letters
spaces  := ' '*;        # zero or more literal spaces
```

The basic repetition operators. `*` matches zero or more (Kleene star). `+` matches one or more (Kleene plus).

**When to use:** `*` for optional repetition (whitespace between tokens, optional trailing characters). `+` for required repetition (identifiers must have at least one character, numbers must have at least one digit).

**Why it's useful:** These are the fundamental iteration constructs. Ragel's implementation generates tight loops with conditional branches — no backtracking, no stack usage. The star/plus operations are closed under DFA minimization, so the generated machine is always optimal.

---

### 9. Exact Count Repetition `{n}` and Range Repetition `{n,m}`

```ragel
exact4  := 'a'{4};       # exactly four 'a's
atmost5 := 'a'{,5};      # zero to five 'a's
atleast3 := 'a'{3,};     # three or more 'a's
range    := 'a'{2,5};    # two to five 'a's
```

Braced repetition operators for precise count control. `{n}` for exact count, `{,m}` for at-most, `{n,}` for at-least, `{n,m}` for range.

**When to use:** Fixed-width fields (4-digit year, 2-digit month), bounded-length identifiers (username between 3-20 chars), repeat counts in protocols (exactly N bytes of payload).

**Why it's useful:** More precise than `*` or `+` when exact constraints matter. The compiler optimizes `{n}` by unrolling into n states where possible. Range constraints are enforced by the DFA structure — no runtime counter overhead for simple cases.

---

### 10. Greedy Kleene Star `**` (Priority Repetition)

```ragel
line    := ( blineElements** '\n' );
comment := '/*' any* :>> '*/';
```

The `**` operator is the greedy Kleene star: it uses Ragel's priority system to prefer matching MORE repetitions. Combined with the `:>>` (right-guarded concatenation), it correctly handles terminator recognition.

**When to use:** Any pattern where "longest match" semantics are needed within a non-scanner machine. Essential for comment matching, string content extraction, and any construct where a terminator must not be consumed by a preceding repetition.

**Why it's useful:** The default `*` operator may match minimally in some ambiguous contexts. `**` uses higher enter-priority (1) and lower leave-priority (0) to bias toward longer matches. This eliminates the need for manual priority annotations in common greedy-matching scenarios.

---

### 11. Case-Insensitive Matching with `../i`

```ragel
letters := 'a' ../i 'z';     # matches [a-zA-Z]
alpha_custom := 0x41 ../i 0x5a;  # same thing via hex codes
```

The `../i` operator creates a case-insensitive character range. It matches both the lowercase and uppercase versions of every character in the range.

**When to use:** Case-insensitive keyword matching, case-insensitive identifiers, HTML tag matching (`<html../i`), protocol parsing where case doesn't matter.

**Why it's useful:** Avoids manually writing `[a-zA-Z]` for every letter range. Works with numeric ASCII codes too — `0x61 ../i 0x7a` is equivalent to `[a-zA-Z]`. The compiler internally generates the dual-range machine.

---

### 12. String Literals (Single/Double/Hex)

```ragel
single := 'hello';              # single-quoted literal
double := "world";              # double-quoted literal
hex    := 0x48 0x65 0x6C 0x6C; # hex-encoded: "Hell"
```

Three ways to write literal character sequences. Single-quoted is standard for most patterns. Double-quoted is available but conventionally single is preferred. Hex literals allow specifying non-printable or binary characters.

**When to use:** Single-quoted for readable text. Double-quoted when the string contains single quotes. Hex-encoded for binary protocols, non-ASCII characters, or when exact byte values are critical.

**Why it's useful:** Flexibility in expressing literals. Hex encoding is essential for binary protocol parsers where control characters or high bytes must be matched precisely.

---

### 13. Regular Expression Literals (`/pattern/`)

```ragel
day  := /[A-Z][a-z][a-z]/;                     # three-letter day
time := /[0-9][0-9]:[0-9][0-9]/;               # HH:MM
url  := /[a-zA-Z][a-zA-Z0-9+.-]*/;             # URL scheme
date := day . ' ' . month . ' ' . time;         # concatenated components
```

POSIX-style regular expression syntax embedded in Ragel. Supports character classes, ranges, quantifiers (`*`, `+`, `?`), and alternation. Regex machines can be combined with Ragel operators and other regex machines.

**When to use:** When patterns are naturally expressed as regex (many common patterns are). The `/pattern/` syntax is often more compact and readable than equivalent Ragel character class notation.

**Why it's useful:** Combined expressive power of regex syntax with compiled DFA performance. No backtracking, no capture groups, just pure DFA state machine compilation. The regex is compiled at Ragel-compile-time (not runtime), so there's zero regex engine overhead.

---

### 14. Concatenation (Sequencing)

```ragel
# Implicit concatenation (juxtaposition):
main := 'hello' ' ' 'world' '\n';

# Explicit concatenation with '.':
main := 'hello' . ' ' . 'world' . '\n';

# Mixed:
date := day . ' ' . month . ' ' . year;
```

Joins machines in sequence. The first machine must match, followed immediately by the second. Both juxtaposition and the `.` operator are equivalent — `.` is syntactically clearer in some contexts.

**When to use:** Building structured patterns from components. Datetime formats (`YYYY-MM-DD`), protocol message layouts, structured text parsing. Every multi-element pattern uses concatenation.

**Why it's useful:** Enables compositional pattern building — define simple patterns and combine them. The compiler's concatenation operation is closed under DFA: concatenation of two DFAs produces a DFA (with epsilon-elimination applied).

---

### 15. Intersection `&` of Machines

```ragel
identifier := ( alpha | '_' ) ( alnum | '_' )*;
not_keyword := identifier & !keyword;
# Equivalent: match identifier but not keyword
```

The `&` operator matches input accepted by BOTH machines. Combined with negation (`!`), it creates powerful constraints.

**When to use:** Filtering one machine's language by another. "Match any identifier except reserved words," "match any character in range X that's also in range Y," or "intersect two different validation criteria."

**Why it's useful:** Enables compositional constraint definition. Define broad patterns and narrow them with constraint machines, rather than encoding all constraints in a single complex pattern. The intersection compiles to a product-DFA construction.

---

### 16. Subtraction `-` and Strong Subtraction `--`

```ragel
# Ordinary subtraction: exclude from start
no_digits := any* - digit+;

# Strong subtraction: exclude from anywhere
messageLine := ( [^\n]* '\n' - fromLine );
not_in_comment := any* -- comment;
```

Ordinary subtraction (`-`) requires the right pattern to match from the start of the left. Strong subtraction (`--`) pads the right pattern with `any*` on both sides, so it's excluded wherever it appears.

**When to use:** `-` for "match X, but not if it starts with Y" — e.g., identifiers that aren't keywords. `--` for "match X, but exclude Y wherever it appears" — e.g., code outside of comments, text outside of markup tags.

**Why it's useful:** Subtraction is a set operation unavailable in standard regex engines. It compiles to efficient DFA complement/intersection. Strong subtraction saves manually writing `any*` wrappers around excluded patterns.

---

### 17. The Commit Operator `:` and Strong Commit `:>>`

```ragel
# Commit: prefer the left side when ambiguous
url_main := 'http' 's'? '://' : hostname path?;

# Strong commit: force termination (comment scanner)
block_comment := '/*' any* :>> '*/';
```

`:` gives the left operand higher priority when patterns overlap. `:>>` is the strong (right-guarded) commit that forces the right pattern to be recognized as the terminator.

**When to use:** `:` for disambiguating optional components in URLs, file paths, or argument lists. `:>>` for multi-character terminators like `*/` in block comments or `-->` in XML comments, where the terminator characters could otherwise be consumed by `any*`.

**Why it's useful:** Eliminates non-determinism without backtracking. The commit operator creates a priority barrier in the DFA that resolves ambiguity at compile time. Essential for correctness in patterns where greedy consumption would eat the terminator.

---

### 18. Transition Actions: `>` (Entering)

```ragel
action begin {
    neg = false;
    val = 0;
}
atoi := ( '-' @see_neg | '+' )? ( digit @add_digit )+ >begin;
```

The `>` operator fires an action when the machine is first entered — on the transition from outside the machine into its start state. It fires exactly once per machine activation.

**When to use:** Initialization before pattern matching begins. Reset accumulators, clear flags, mark start positions. Essential for any stateful matching where variables must be initialized.

**Why it's useful:** Separates initialization from matching logic. The `>` action fires before any characters are consumed, ensuring state is clean. Without it, initialization would need to happen in host code before calling the machine, mixing concerns.

---

### 19. Transition Actions: `%` (Leaving/Finish)

```ragel
action finish {
    if ( neg ) val = -1 * val;
}
atoi := ( '-' @see_neg | '+' )? ( digit @add_digit )+ >begin %finish;
```

The `%` operator fires an action when leaving the machine from a final (accepting) state. It fires after all characters are matched, as the machine transitions out.

**When to use:** Finalization after successful pattern match: compute results, emit output, close data structures. The counterpart to `>` — one initializes, the other finalizes.

**Why it's useful:** The `>...%` pattern is Ragel's "begin/end" idiom — actions bracketing a machine. The `%` fires only when reaching a final state (not on error/abort), so it naturally handles completion vs error paths differently.

---

### 20. Transition Actions: `@` (Finishing) and `$` (All)

```ragel
action add_digit { val = val * 10 + (fc - '0'); }
action log_char  { printf("processing: %c\n", fc); }

# @ fires on the finishing transition of each element
digits := digit+ @add_digit;

# $ fires on every single transition within the machine
trace  := 'hello' $log_char;
```

`@` fires on the finishing transition into a state (useful for "on each matched element" semantics). `$` fires on ALL transitions within the machine (every character processed).

**When to use:** `@` for per-element accumulation (each digit, each word, each field). `$` for tracing, logging, or counters that must update on every single character consumed within the machine.

**Why it's useful:** `@` is the most commonly used action for accumulation — it fires exactly once per matched element at the right time. `$` is powerful for debugging and instrumentation since it reveals every state transition.

---

### 21. Action Chaining: `>begin @accum %finish`

```ragel
word := ( any-space )+ >start_word @accum_char %end_word;
```

Combining `>`, `@`, and `%` on the same machine creates a complete lifecycle: initialize on entry, accumulate on each element, finalize on completion. This is the standard Ragel action pattern for most machines.

**When to use:** Any machine that produces a value (number, string, token) from matched input. The three action phases handle initialization, accumulation, and finalization cleanly.

**Why it's useful:** This triad is Ragel's answer to parser generator "semantic actions." Each phase has clearly defined semantics: `>` fires once, `@` fires per-element, `%` fires once. The compiler ensures correct execution order even with nested machines.

---

### 22. Statechart / Named State Paradigm

```ragel
atoi := (
    start: (
        '-' @see_neg ->om_num |
        '+' ->om_num |
        [0-9] @add_digit ->more_nums
    ),
    om_num: (
        [0-9] @add_digit ->more_nums
    ),
    more_nums: (
        [0-9] @add_digit ->more_nums |
        '' -> final
    )
) >begin %finish;
```

Defines machines using named states (`start:`, `om_num:`, `more_nums:`) with explicit transitions (`->target`). The `''` (empty string) creates an epsilon transition. `final` is the built-in final state marker.

**When to use:** Complex state machines where explicit state naming improves readability. Protocol implementations, UI state machines, multi-phase parsers. The named-state approach maps directly to state diagrams.

**Why it's useful:** Self-documenting state machines. Each state's behavior and transitions are explicit. The epsilon transition (`'' -> final`) enables "fallthrough" logic without consuming input. This syntax is more verbose than regex-style but far clearer for complex logic.

---

### 23. Named Patterns (Variable Definitions)

```ragel
digit    := [0-9];
hexdigit := [0-9a-fA-F];
alpha    := [a-zA-Z];
alnum    := [a-zA-Z0-9];
word     := alpha+;
number   := digit+;
hexnum   := '0x' hexdigit+;

identifier := alpha ( alnum | '_' )*;
```

Defines named sub-patterns using `:=` assignment. These can be reused in other patterns like variables.

**When to use:** Building a library of reusable pattern components. Named patterns make complex grammars modular and maintainable. Especially useful for shared definitions across multiple machines in a single file.

**Why it's useful:** Promotes DRY (Don't Repeat Yourself) in pattern definitions. Change a character class in one place and all dependent patterns update. Named patterns are expanded at compile time — no runtime indirection.

---

### 24. Epsilon Transitions in Statecharts (`'' -> state`)

```ragel
optional_sign := (
    have_sign: (
        '' -> no_sign    # epsilon: skip to no_sign
    ),
    no_sign: (
        '+' -> have_sign |
        '-' -> have_sign |
        digit -> digits
    ),
    ...
);
```

Epsilon transitions allow moving between states without consuming input. In the example, from `have_sign` you can epsilon-jump directly to `no_sign`, providing an "optional" path through the statechart.

**When to use:** Optional components in statecharts, fallthrough logic, "skip this state" paths. The epsilon is essential for making named states work like regex `?` or alternation.

**Why it's useful:** Maps directly to NFA epsilon transitions. The Ragel compiler eliminates epsilons during DFA construction via epsilon-closure computation. The programmer writes epsilon transitions declaratively; the compiler makes them efficient.

---

### 25. Action-Only Patterns (Zero-Length with Semantic Effect)

```ragel
action commit { committed = true; }
main := 'data' @commit_pattern | '' @commit;
```

Empty-string patterns (`''`) with actions attached serve as pure semantic triggers — they fire the action at a specific point in the machine without consuming input.

**When to use:** Checkpoint markers, conditional state transitions, invariant assertions. Any time you need to execute code at a specific point in the machine's lifecycle without advancing the input pointer.

**Why it's useful:** Separates semantic concerns from syntactic matching. You can insert actions at precise points in the control flow without affecting what the machine matches. Combined with labels, this enables event-driven state machines.

---

### 26. Global Error Actions: `$err`, `>err`, `%err`

```ragel
action err_start  { printf("err at start\n"); }
action err_all    { printf("err on all chars\n"); }
action err_out    { printf("err on exit\n"); }

main := 'hello' >err err_start $err err_all %err err_out;
```

Global error actions fire when the machine enters the error state (unexpected character). Six scoping levels exist:
- `>err` — error from start state only
- `$err` — error from all states
- `%err` — error from non-start states
- `<>err` — error from non-start, non-final states ("middle")
- `<!err` — error from non-start states variant
- `<.err` — error from non-final states variant

**When to use:** Input validation with rich error reporting. Distinguish "expected more data" from "completely invalid input." The different scopes let you customize error messages based on where in the pattern the failure occurred.

**Why it's useful:** No need to manually wrap every alternative in error handlers. Ragel automates error action propagation through all reachable states. Error actions are embedded in the DFA's transitions to the error sink state.

---

### 27. Local Error Actions: `$lerr(name)`

```ragel
action hello_fails { printf("hello didn't match\n"); }
hello := 'hello\n'* $lerr hello_fails;
```

Local error actions scope the error handler to a specific machine only. Unlike global `$err` which catches all errors, `$lerr` fires only when an error occurs within the designated machine's states.

**When to use:** Modular parsers where different sub-machines need different error recovery strategies. A typical pattern: each alternative in a union has its own local error handler for tailored recovery.

**Why it's useful:** Fine-grained error recovery. A number parser and a string parser can have completely different error behaviors even though they're alternatives in the same union. Local errors don't propagate to outer machines.

---

### 28. The `$!` (Error Transition) for Recovery

```ragel
command := ( [a-z0-9] @{comm = fc;} ' ' @comm_arg '\n' );
main := command* $!{ fhold; fnext garble_line; };
garble_line := ( (any-'\n')* '\n' ) >err_msg @goto_main;
```

`$!` fires an action when the machine encounters an error character. Combined with `fhold` (hold the character) and `fgoto`/`fnext` (redirect to recovery machine), it implements structured error recovery — consume the bad line and resume.

**When to use:** Production parsers that must handle malformed input gracefully. Line-based recovery in command interpreters, config file parsers, or network protocols.

**Why it's useful:** Implements "panic mode" error recovery — skip the current line/record and resume. Unlike parser generators that require special error tokens, Ragel's fhold+fgoto pattern uses the same state machine mechanisms for recovery as for normal matching.

---

### 29. The `>!` and `%!` Error Operators

```ragel
float :=
    [\-+] >!expect_digit_plus_minus %!expect_digit
    [0-9][0-9]* $!expect_digit_decimal
    ( '.' [0-9]+ $!expect_digit )?;
```

`>!` fires an error action when transitioning from the start state on an invalid character. `%!` fires an error action on leaving from a non-final state. These provide even more granular error placement than `$err` variants.

**When to use:** Fine-grained parse error reporting. Different error messages for "invalid sign character" vs "expected digit" vs "expected digit after decimal point." Each position in the pattern can have its own error message.

**Why it's useful:** User-facing parsers (compilers, config validators) benefit from precise error messages. `>!` catches problems right at the entry point. `$!` catches problems mid-pattern. `%!` catches premature termination. Together they cover every error position.

---

### 30. Tokenization Scanner (`|* ... *|`)

```ragel
main := |*
    alpha alnum* => { token(IDENT); };
    digit+       => { token(INT); };
    '+'          => { token(PLUS); };
    [ \t\n]+;                  # whitespace, silently consumed
*|;
```

The scanner construct `|* ... *|` defines multiple competing patterns. The scanner uses longest-match semantics: all patterns run in parallel on each character, and at the point where no pattern can continue, the longest successful match wins. Ties are broken by definition order. The `=>` action fires only when a token is committed.

**When to use:** Building lexers/tokenizers for any language. This is Ragel's primary lexer construction mechanism. The scanner automatically resolves ambiguous tokens (e.g., `for` vs `format` — whichever is longer wins).

**Why it's useful:** Replaces hand-written lexers entirely. The scanner handles lookahead (one character), backtracking for longest-match, and buffer management. Performance is O(n) regardless of pattern count. This is how the C++ standard library lexer examples in the test suite work.

---

### 31. Scanner with Silent (Actionless) Patterns

```ragel
main := |*
    alpha alnum* => { token(IDENT); };
    digit+       => { token(INT); };
    [ \t\n]+;                    # whitespace: no action, just consumed
    '//' [^\n]* '\n';            # comment: no action, just consumed
    any;                         # fallback: silently consume unknown chars
*|;
```

Not every scanner pattern needs a `=>` action. Patterns without actions are silently consumed — they match and discard input without triggering any token emission.

**When to use:** Whitespace, comments, noise characters. Any input you want to recognize and skip without producing a token.

**Why it's useful:** Separates token production from input consumption. The scanner still uses longest-match on all patterns; actionless patterns participate in the competition but produce no output. This makes lexers cleaner — token patterns focus on tokens, skip patterns focus on skipping.

---

### 32. Scanner with Priority Disambiguation

```ragel
main := |*
    'foil'  => { token(FOIL_WORD); };
    [a-z]+  => { token(WORD); };
    '0x' xdigit+ => { token(HEX); };
    digit+       => { token(INT); };
    '0'          => { token(ZERO); };
*|;
```

When patterns overlap (e.g., `'foil'` is also matched by `[a-z]+`), the scanner uses longest-match priority. Since `'foil'` (4 chars) is longer than any prefix that `[a-z]+` would match in isolation, the foil token wins for input `"foil"`.

**When to use:** Keyword recognition combined with general identifier matching. The keyword is a subset of the identifier pattern, but the scanner naturally prefers the longer (keyword) match.

**Why it's useful:** No need for manual "try keyword first" logic. The scanner's DFA tracks all active patterns simultaneously. At the commit point, it selects the best match based on length, then declaration order for ties.

---

### 33. Scanner with Lag/Next/Last Actions

```ragel
main := |*
    'a'           => { printf("on last\n"); };
    'b'+          => { printf("on next\n"); };
    'c1' 'dxxx'?  => { printf("on lag\n"); };
    'd1'          => { printf("switch1\n"); };
    'd2'          => { printf("switch2\n"); };
*|;
```

Scanner patterns demonstrate three matching modes:
- **On last**: pattern matched at the last possible character (no more data or conflicting patterns)
- **On next**: pattern needed one lookahead character to commit
- **On lag**: pattern with an optional suffix — the scanner lagged behind to try the optional part

**When to use:** Understanding scanner internals for debugging. These action types reveal how the scanner resolves ambiguity at each character position. Useful when a lexer isn't producing expected tokens.

**Why it's useful:** The scanner's lookahead mechanism is invisible in normal usage. These action types make it observable. They confirm that the scanner correctly implements "longest match with one character lookahead" semantics.

---

### 34. Scanner with Leaving Actions (`%` before `=>`)

```ragel
main := |*
    'a'   %{ printf("leave a\n"); }   => { printf("match a\n"); };
    'b'+  %{ printf("leave b\n"); }   => { printf("match b\n"); };
    ( 'e1' '...'? ) %{ printf("leave e1\n"); } => { printf("match e1\n"); };
*|;
```

Leaving actions (`%`) on scanner patterns fire when the scanner is ABANDONING a pattern to try longer matches. They fire BEFORE the `=>` action. This reveals the scanner's trial-and-error process at the action level.

**When to use:** Tracking scanner behavior for debugging or for implementing incremental processing within a scanner. The `%` fires on every attempt to leave a pattern, even if it's later resumed.

**Why it's useful:** The leaving action is the scanner equivalent of backtracking annotation. It tells you which patterns were considered and rejected at each input position. This is essential for understanding scanner behavior with complex overlapping patterns.

---

### 35. The `$~` (From-Start) and `$*` (To-Start) Scanner Actions

```ragel
main := |*
    ( 'L'? "'" ( [^'\\\n] | /\\./ )* "'" ) $~ from_act $* to_act;
    [a-zA-Z_][a-zA-Z0-9_]* $~ from_act $* to_act;
*|;
```

`$~` fires when leaving a state to start a new token (the "from" transition of token start). `$*` fires when entering a state after completing a token (the "to" transition of token start). Together they precisely track `ts` (token start pointer) state changes.

**When to use:** Explicit token-boundary tracking in scanners. When you need fine-grained control over `ts` management (rather than relying on the scanner's automatic tracking).

**Why it's useful:** Provides observability into scanner token boundary management. The `$~` and `$*` actions fire at transition points that would be invisible otherwise. Essential for custom buffer management strategies.

---

### 36. Call/Return: `fcall` and `fret`

```ragel
even := 'even' any @{ fhold; fret; };
odd  := 'odd' any @{ fhold; fret; };

action check_num {
    if ( num & 1 )
        fcall *fentry(odd);
    else
        fcall even;
}

main := [0-9]+ ${ num = num * 10 + (fc - '0'); } ' ' @check_num '\n';
```

`fcall` pushes the current state onto the call stack and jumps to the target machine. `fret` pops the stack and resumes in the calling machine. This implements subroutine-like behavior within the state machine.

**When to use:** Context-dependent sub-patterns where the outer machine determines which inner machine to call. Recursive parsing (nested structures). Sub-pattern factorization (call a shared machine from multiple call sites).

**Why it's useful:** Extends Ragel beyond regular languages. The call stack enables context-free parsing. Critical for languages with nested structure (parentheses, braces, XML tags). The stack depth is limited only by available memory when using `prepush` dynamic growth.

---

### 37. Dynamic Dispatch with `fentry()` and `fcall *target`

```ragel
target := 0;

one := 'one' @{ printf("one\n"); fret; };
two := 'two' @{ printf("two\n"); fret; };

main :=
    '1' @{ target = fentry(one); fcall *target; } |
    '2' @{ target = fentry(two); fcall *target; } |
    '\n';
```

`fentry(machine)` returns the numeric state ID of the named machine's entry point. `fcall *target` performs an indirect call to whatever state `target` points to. This enables runtime dispatch to different sub-machines.

**When to use:** Table-driven dispatch, command pattern implementation, state-machine interpreters. Any scenario where the next machine to call depends on runtime data (a lookup table, a function pointer array, a user command).

**Why it's useful:** Decouples the call target from the static machine definition. You can swap sub-machine behavior at runtime by changing `target`. Combined with `fcall`, this is Ragel's equivalent of function pointers in the state machine world.

---

### 38. Named Call/Return: `fncall` and `fnret`

```ragel
command := [a-z0-9] @{comm = fc;} ' ' @comm_arg '\n';

action comm_arg {
    if ( comm >= 'a' ) fncall alp_comm;
    else fncall dig_comm;
}

alp_comm := alpha+ $!{ fhold; fnret; };
dig_comm := digit+ $!{ fhold; fnret; };
```

`fncall` is the named variant of `fcall`. `fnret` is the named variant of `fret`. They are used in code generators that require named call/return constructs (e.g., the `--var-backend` for OCaml pattern-matching style).

**When to use:** In code generators that distinguish between goto-based and name-based call/return semantics. Most users should use `fcall`/`fret`; `fncall`/`fnret` exist for compatibility with all backends.

**Why it's useful:** Provides the same call/return semantics across all code generation backends, including the variable-based backend where direct goto-style calls aren't possible.

---

### 39. Goto: `fgoto` and `fnext`

```ragel
error_handler := ( any - '\n' )* '\n' @{ fgoto main; };

main := 'hello' '\n' @{ printf("ok\n"); }
    $!{ fhold; fgoto error_handler; };
```

`fgoto` jumps immediately to a named machine's entry point. Unlike `fcall`, it does NOT save the current state — it's a one-way jump. `fnext` schedules the target state for entry after the current action returns.

**When to use:** `fgoto` for error recovery (jump to error handler, skip bad input, resume). `fnext` for controlled transitions where you want to complete current processing first.

**Why it's useful:** `fgoto` is the simplest state transition — it's a direct jump, no stack manipulation. `fnext` is useful when combined with `fbreak` — break out of the current machine, then on the next iteration, enter the target state.

---

### 40. Call with Return Value via `fnext *return_to`

```ragel
return_to := 0;

one := 'one' @{
    printf("one\n");
    fnext *return_to;
};

main := (
    '1' @{ return_to = ftargs; fnext one; } |
    '2' @{ return_to = ftargs; fnext two; } |
    '\n'
)*;
```

`ftargs` captures the target arguments of the current call context. Combined with `fnext *return_to`, sub-machines can "return" to the right continuation point without using a call stack. The `return_to` variable holds the state to re-enter.

**When to use:** Implementing coroutine-like behavior where sub-machines need to return control to specific points. Alternative to the call stack pattern when stack depth is a concern.

**Why it's useful:** Provides a lightweight call/return mechanism without consuming stack space. Each sub-machine explicitly specifies its return point. This pattern is especially useful in memory-constrained environments.

---

### 41. The `fcurs` Variable

```ragel
return_to := 0;

main :=
    '1' @{ return_to = fcurs; fnext one; } |
    '2' @{ return_to = fcurs; fnext two; } |
    '\n';
```

`fcurs` captures the CURRENT STATE at the point of a call-like transition. Unlike `ftargs` (which captures target arguments), `fcurs` captures the state index.

**When to use:** Similar to `ftargs` but for backends that use integer state IDs. `fcurs` is the state-based equivalent of argument-based return tracking.

**Why it's useful:** In the `indep` (language-independent) target, state IDs are the primary mechanism for machine references. `fcurs` provides a consistent way to capture the "where to return" information.

---

### 42. Break: `fbreak` and `fnbreak`

```ragel
main := 'h' @{ fnext e; fbreak; };
e    := 'e' @{ fnext l; } @{ fbreak; };
l    := 'll' @{ fnext o; } ${ fbreak; };
o    := |* 'o' { fnext nl; fbreak; }; *|;
nl   := '\n' @{ fbreak; printf("ACCEPT\n"); };
```

`fbreak` exits the current machine's execution loop. When combined with `fnext`, it schedules the next state and returns control to the caller. This creates a step-by-step, character-at-a-time execution model.

**When to use:** Pull-model scanners (caller invokes the machine, machine returns after processing one element). Incremental processing. Coroutine-style cooperative multitasking between machine and host code.

**Why it's useful:** Inverts control — instead of the machine driving the loop, the host code drives it. Each call to `write exec` processes one state transition, then `fbreak` returns. This enables integration with event loops, UI frameworks, or any callback-driven architecture.

---

### 43. The `fhold` Instruction

```ragel
action check_header {
    if ( condition ) {
        fhold;  // don't consume this character
        fret;
    }
}
header := hchar+ ':' @check_header;
```

`fhold` tells the machine to NOT advance the input pointer after the current transition. The same character will be re-examined on the next state transition.

**When to use:** When a character that terminates one pattern should also be the first character of the next pattern. Common in scanner patterns: the delimiter (`:`, `=`, `{`) ends one token and starts the next.

**Why it's useful:** Implements character peeking without manual pointer manipulation. The machine naturally handles boundary cases where delimiters serve dual roles. Without `fhold`, you'd need to manage the input pointer manually in host code.

---

### 44. The `fexec` Instruction

```ragel
action onError {
    if ( tok != 0 ) {
        // emit the partial token, then restart from error point
        fexec rst_data;
        fgoto main;
    }
}
```

`fexec` sets the input data pointer to a new position, effectively rewinding or forwarding the input. Combined with `fgoto`, it redirects the machine to restart parsing at the new position.

**When to use:** Error recovery where you need to re-process from an earlier point. Restarting a scanner after emitting a partial match. Complex backtracking scenarios.

**Why it's useful:** Provides explicit control over the input position when the automatic pointer management isn't sufficient. The `fexec` + `fgoto` combination resets both the data pointer and the state, enabling clean restart semantics.

---

### 45. The `=>` (Scanner Token Action) vs Explicit `ts`/`te`

```ragel
# Method 1: scanner => operator (automatic ts/te)
main := |*
    alpha alnum* => { token(ts, te-ts); };
*|;

# Method 2: explicit ts/te management
main := alpha alnum* >{ ts = p; } %{ te = p; token(ts, te-ts); };
```

Two approaches to token boundary management. The scanner `=>` operator automatically captures `ts` at token start and `te` at token end. The explicit approach uses `>` to capture `ts` and `%` to capture `te`.

**When to use:** `=>` is preferred for scanner patterns — it's concise and clearly expresses "emit this as a token." The explicit `>` / `%` approach is used in non-scanner machines where you need manual control.

**Why it's useful:** The `=>` operator is syntactic sugar that makes scanner patterns cleaner. It handles edge cases (zero-length matches, buffer boundaries, partial tokens) that the explicit approach would need to handle manually.

---

### 46. String Accumulation with `$` and Buffer Management

```ragel
action buf { buffer.append(fc); }
action emit { buffer.terminate(); process(buffer.data); buffer.clear(); }

word := alpha+ $buf %emit;
```

Uses `$` (all-transition action) to accumulate characters into a growable buffer, and `%` (leaving action) to finalize and emit the accumulated string. This pattern captures arbitrary-length tokens without fixed-size buffers.

**When to use:** Extracting matched text for identifiers, string literals, numbers — any token where you need the actual matched text for further processing.

**Why it's useful:** Gives you full control over token buffering. The growable buffer handles tokens of any length. The clear-after-emit pattern prevents unbounded memory growth. This is more flexible than relying on `ts`/`te` pointers alone, especially when tokens need post-processing (escaping, case conversion).

---

### 47. Condition Guards (`when`)

```ragel
action c1 { i != 0 }
action one { printf("one\n"); }

main := ( [a-z] | '\n' when c1 @one )* '\n';
```

The `when condition` clause guards a transition: the character must match the pattern AND the boolean condition must evaluate to true. Conditions are action references that return boolean values.

**When to use:** Context-sensitive parsing where the same input character should be treated differently based on runtime state. Parsing non-regular languages where transitions depend on computed variables, not just input history.

**Why it's useful:** Extends Ragel beyond regular languages. Without conditions, a DFA's state transitions are fixed at compile time. Conditions inject runtime boolean tests into transitions, enabling dynamic behavior like "accept this only if counter < N" or "only if flag X is set."

---

### 48. `outwhen` — Exit-Gating Condition

```ragel
action testi { i > 0 }
action inc { i--; printf("item: %c\n", fc); }

count := [0-9] @{ i = fc - '0'; };
sub := count ( digit when testi @inc )* outwhen !testi;
```

`outwhen` fires when leaving a machine — it's true only at the exit point. Here, `sub` reads a count digit, then consumes that many more digits. `outwhen !testi` ensures the machine exits only when `i` has reached 0.

**When to use:** Bounded repetition enforced by runtime conditions. Counted fields: "read N bytes," "match exactly `count` items." The `outwhen` ensures you don't exit prematurely.

**Why it's useful:** Implements counted loops within the state machine. Combined with `when` for entry-gating and `outwhen` for exit-gating, you get full runtime control over how many iterations occur. This handles patterns like netstrings (`length:data,`) or chunked encodings.

---

### 49. Condition-Tracked Balancer (Parentheses/Brackets)

```ragel
comment := '(' @{ n=0; }
    ( '(' @{ n++; } | ')' @{ n--; } | [^()] )*
:> ')' when { !n };
```

Uses a counter variable `n` to track nesting depth. Each `(` increments, each `)` decrements. The `:>` guarded concatenation prevents the inner star from consuming the final `)`. The condition `when { !n }` passes only when nesting returns to zero.

**When to use:** Balanced delimiter parsing: parentheses in expressions, braces in code blocks, XML/HTML tags, JSON brackets. Any construct where openers and closers must balance.

**Why it's useful:** This is the classic non-regular language that standard regex can't handle. Ragel's conditions make it possible by injecting runtime state into the DFA. The counter tracks depth; the condition enforces balance. This enables full context-free parsing within the Ragel framework.

---

### 50. Inline Conditions (`when { expr }`)

```ragel
see_five := ( [0-9] when { c++ < 5 } @d1 )* '\n';
in_sequence := ( [0-9] when { *p == last+1 } @d2 )* '\n';
```

Conditions can be inline code blocks (`{ expr }`) rather than named action references. The expression's last value (or explicit return) determines the boolean result.

**When to use:** Simple one-off conditions that don't warrant a named action. Prototyping and experimentation. Conditions that are naturally expressed as a single expression.

**Why it's useful:** More concise for simple conditions. Reduces the number of action declarations. The inline syntax is particularly clean for counter checks, flag tests, and pointer comparisons.

---

### 51. Parameterized Action Conditions

```ragel
action c(v) { $v }
action c1 { i }
action c2 { j }

main := '\n' when c(c1) @action;
```

Actions can be parameterized with `$v` substitution. The `c(v)` pattern accepts an argument that substitutes into the action body. This enables generic condition wrappers.

**When to use:** Library-style action definitions. A single parameterized condition can wrap multiple concrete conditions. Reduces duplication when many conditions share structure.

**Why it's useful:** Promotes code reuse among conditions. Instead of `action c1 {i}`, `action c2 {j}`, etc., you define one `action c(v){$v}` and pass arguments. The parameterization is compile-time substitution, so there's no runtime overhead.

---

### 52. `:condstar` — Conditional Kleene Star

```ragel
action init { q = 0; }
action inc  { q++; }
action min  { q >= 1 }
action max  { q < 25 }

block := :condstar( ( ' ' ), init, inc, min, max ):;
```

`condstar` is a Kleene star operator with user-defined conditions for min/max repetition bounds. Arguments:
- `init` — reset counter to zero
- `inc` — increment counter on each repetition
- `min` — minimum condition (must be true to stop; false => continue at least until true)
- `max` — maximum condition (must be true to continue; false => stop)

**When to use:** Repetition with complex runtime-controlled bounds. When the simple `{n}`, `{,m}`, `{n,m}` operators aren't sufficient because bounds depend on runtime state.

**Why it's useful:** The most general repetition operator. `{n,m}` is syntactic sugar that `condstar` generalizes. With custom min/max conditions, you can implement any repetition policy, including state-dependent bounds, probabilistic stopping, or adaptive limits.

---

### 53. `:condplus` — Conditional Kleene Plus

```ragel
block := :condplus( ( ' ' ), init, inc, min, max ):;
```

Same as `condstar` but requires at least one repetition (like `+` vs `*`). Empty input always fails regardless of min/max.

**When to use:** When "at least one" is a hard requirement, combined with runtime-controlled upper bounds.

**Why it's useful:** Combines the semantic clarity of `+` (must match at least once) with the flexibility of runtime-controlled bounds. The compiler validates empty-input rejection at compile time.

---

### 54. `:nfa(...):` — NFA Generalized Repetition

```ragel
action psh    { nfa_bp[nfa_len].q = q; }
action pop    { q = nfa_bp[nfa_len].q; }
action init   { q = 0; }
action stay   { true; }
action repeat { ++q < max; }
action exit   { ++q >= min; }

spaces := :nfa( ( ' ' ), psh, pop, init, stay, repeat, exit ): {2};
```

The NFA generalized repetition operator provides push/pop-based backtracking with custom entry/stay/repeat/exit guards. Six user-defined actions control the entire repetition lifecycle:
- `psh`/`pop` — save/restore state on the NFA backtrack stack
- `init` — initialize the repetition counter
- `stay` — condition to remain in the current iteration
- `repeat` — condition to perform another iteration
- `exit` — condition to allow exiting the repetition

**When to use:** When no other repetition operator is sufficient. Bounded repetition with backtracking, pattern-match with save/restore semantics, complex multi-way alternations where several possible repetition counts exist.

**Why it's useful:** The escape hatch for repetition. The NFA mechanism explores all possible repetition counts via backtracking within the DFA framework. The `psh`/`pop` actions preserve counter state across backtrack points so each branch has its own counter value.

---

### 55. NFA Repetition with Multi-Match Pop

```ragel
main := ( :nfa( ( 'a' ), psh, pop, init, stay, repeat, exit ): ) {2}
    '|x' @{ printf("MATCH\n"); };
```

When the NFA repetition fails in a non-final state, the NFA stack pops and backtracks, potentially producing MULTIPLE matches for the same input region. Each successful backtrack path that reaches the trailing pattern produces a match.

**When to use:** Multi-match extraction: find all valid sub-sequences meeting a constraint. Parsing ambiguous grammars where multiple interpretations exist.

**Why it's useful:** Unlike standard DFA matching (one path through the machine), NFA mode explores all paths. When multiple paths satisfy the criteria, each produces an output. This is useful for discovering all possible parses of ambiguous input.

---

### 56. `%when` — Condition Action on Epsilon Transitions

```ragel
action marker { printf("marker\n"); }
action t { true }

main := ( :condstar( 'a', init, inc, min, max ): )
    ( '' %when t | '' %when !t )
    %when { 1==1 } %match;
```

`%when` fires on epsilon (zero-length) transitions when the condition is true. Combined with `%match`, it controls acceptance on epsilon paths. This enables fine-grained control over which epsilon transitions are taken.

**When to use:** Disambiguating multiple epsilon paths through a machine. Conditional acceptance: only accept if a specific runtime condition holds at the acceptance point.

**Why it's useful:** Epsilon transitions are invisible to character consumption but crucial for machine structure. `%when` makes them conditionally active: take this epsilon path only if condition X is true. This adds runtime control to the machine's structural decisions.

---

### 57. Priority-based Disambiguation with `$` and `%`

```ragel
length := (
    'h'  %length_h   |    # leaving action: fires on exit
    'l'  %length_l   |    # try longer match first
    'hh' @length_hh  |    # finishing action: fires on entry
    'll' @length_ll       # commits immediately
)?;
```

The distinction between `@` (finishing action — fires on entering the state) and `%` (leaving action — fires on exiting) controls priority. `@` gives HIGHER priority (commits early); `%` gives LOWER priority (allows longer matches).

**When to use:** Disambiguating shared-prefix patterns: `++` vs `+`, `==` vs `=`, `>>` vs `>`. The `@` variant commits to the longer match; the `%` variant is willing to match shorter.

**Why it's useful:** The `@` vs `%` distinction is Ragel's main priority mechanism for character-level patterns. Without it, ambiguous prefixes would be resolved non-deterministically. The compiler uses action type to determine priority order in the generated DFA.

---

### 58. Priority Union with `$1 %0` / `$0 %1`

```ragel
words     := ( ( word | space ) $1 %0 )*;
comments  := ( ( comment | any ) $1 %0 )**;
```

Priority numbers on `$` (enter priority) and `%` (leave priority) control which machine wins in overlapping matches. Higher enter-priority + lower leave-priority = greedier. Lower enter-priority + higher leave-priority = less greedy.

**When to use:** Concurrent machines running in parallel where one should "dominate" certain characters. In the example, `word` should dominate `space` when both can match.

**Why it's useful:** Implements lexical scoping of priorities — each sub-machine gets a priority weight. The compiler resolves all priority conflicts at compile time into deterministic DFA transitions. No runtime overhead for priority resolution.

---

### 59. `Prepush` and `Postpop` for Dynamic Stack Growth

```ragel
prepush {
    if ( top == stack_size ) {
        stack_size = top * 2;
        stack = (int*)realloc( stack, sizeof(int) * stack_size );
    }
}
postpop {
    if ( stack_size > top * 4 ) {
        stack_size = top * 2;
        stack = (int*)realloc( stack, sizeof(int) * stack_size );
    }
}
```

Hooks that execute automatically before every `fcall` (prepush) and after every `fret` (postpop). The prepush block checks if the stack is full and doubles its size. The postpop block shrinks the stack when it's severely underutilized.

**When to use:** Recursive parsing where the maximum nesting depth is unknown. Without prepush, you must allocate a fixed-size stack that may overflow. With prepush, the stack grows on demand.

**Why it's useful:** Enables safe recursive descent with unbounded depth. The amortized growth strategy (double on push, halve when 4x over-allocated) keeps memory proportional to the current nesting depth. Essential for production parsers handling arbitrary input.

---

### 60. Struct-based State Access (`access fsm->`)

```ragel
%%{
    machine scanner;
    access fsm->;
    variable p fsm->p;
    variable pe fsm->pe;
    variable eof fsm->eof;
}%%
```

The `access` directive prefixes all Ragel-generated state variables (`cs`, `stack`, `top`, `act`) with `fsm->`. The `variable` directives rename the input pointers. This wraps all state into a struct for multi-instance usage.

**When to use:** Object-oriented scanners where multiple instances must coexist. Server applications with per-connection state, reentrant libraries, container classes wrapping Ragel machines.

**Why it's useful:** Thread safety, reusability, clean separation. Without `access`, Ragel generates global variables. With `access`, each instance has its own state in a struct. The `variable` directives let you use custom field names.

---

### 61. Custom Alphabet Type (`alphtype`)

```ragel
alphtype unsigned char;
main := 0xe8 @gothigh '\n';
```

Sets the alphabet type to control how character values are interpreted. `unsigned char` (0–255) vs `char` (-128–127). This matters for characters >= 128 (extended ASCII, binary data).

**When to use:** When parsing binary data or extended character sets. Characters over 127 need `unsigned` type to avoid being interpreted as negative values in signed comparison.

**Why it's useful:** Correctness for binary protocols. Without `unsigned char`, `0xe8` would be `-24` in signed comparison, potentially causing incorrect state transitions. The `alphtype` directive ensures the generated code uses the correct signedness.

---

### 62. Custom Key Access (`getkey`)

```ragel
alphtype int;
getkey fpc->type;

main := ( 23 @shift ->s1 | 25 @shift ->s3 | ... );
```

`getkey` extracts the "character" from input using a custom expression. Instead of `*p` (the byte at the data pointer), it uses `fpc->type` (a struct field). This allows parsing sequences of structured elements rather than byte streams.

**When to use:** Token-stream parsing (the output of a lexer feeding a parser). Processing arrays of structs. Any scenario where the "alphabet" is not raw bytes but typed values.

**Why it's useful:** Enables Keller-style parser generation where Ragel machines parse token streams produced by other Ragel machines. Each element in the stream has a type code; `getkey` extracts it. This is how Ragel itself is bootstrapped — Ragel parsers parse Ragel token streams.

---

### 63. Custom Variable Naming (`variable p`, `variable pe`, `variable eof`)

```ragel
variable p data_ptr;
variable pe end_ptr;
variable eof eof_ptr;
```

Renames the Ragel-generated input pointer variables. By default they're `p`, `pe`, `eof`. With `variable`, they become `data_ptr`, `end_ptr`, `eof_ptr`.

**When to use:** When the default names conflict with existing code. When integrating with libraries that already use `p` and `pe`. When the meaning is clearer with descriptive names.

**Why it's useful:** Avoids naming conflicts in large codebases. The default names are short and convenient but can clash. Custom names resolve conflicts without requiring code changes to the existing codebase.

---

### 64. The `write` Statement Family

```c
%% write data;       // int cs; int stack[]; int top; int act;
%% write init;       // cs = machine_start;
%% write exec;       // the main while-loop or gotos
%% write start;      // just the start state constant
%% write first_final;// just the first_final constant
%% write error;      // just the error state constant
%% write exports;    // #define constants for exports
```

Each `write` statement emits a specific piece of generated code at its insertion point. This gives fine-grained control over code placement.

**When to use:** `write data` + `write init` in initialization code. `write exec` in the processing loop. `write exports` after machine definitions. Use `nofinal` when you don't need final state checking. Use `noerror` when you handle errors yourself. Use `nocs` in `write init` when setting cs manually.

**Why it's useful:** Unlike code generators that produce entire functions, Ragel's `write` statements let you interleave generated code with hand-written code exactly where needed. This is essential for embedding state machines within existing code structures.

---

### 65. Exporting Character Constants

```ragel
export c1 = 'c';
export c2 = 'z';
export c3 = 't';

commands := (
    c1 . digit* '\n' @{ printf("c1\n"); } |
    c2 . alpha* '\n' @{ printf("c2\n"); } |
    c3 . '.'* '\n'  @{ printf("c3\n"); }
)*;

%% write exports;
```

`export name = expression;` defines a single-character constant. `write exports;` generates the corresponding host-language constants (e.g., C `#define test_ex_c1 'c'`). These constants are used to construct input for token-stream-based parsers.

**When to use:** Multi-stage parsing pipelines. A lexer/scanner produces tokens encoded as single characters; a parser consumes them. The export mechanism generates the token constants shared between lexer output and parser input.

**Why it's useful:** Type-safe token communication. Instead of using raw character codes, both sides reference named constants. If the token set changes, only the Ragel file needs updating — the generated constants stay in sync.

---

### 66. Importing Machines from External Files

```ragel
import "atoms.rl";

foobar := foo @{ printf("foo\n"); } | bar @{ printf("bar\n"); };
main := ( foobar SP foobar NL )*;
```

`import "file.rl"` inlines a Ragel file's machine definitions at the import point, making them available as local variables. The imported file can define patterns, actions, and machines.

**When to use:** Sharing pattern definitions across multiple `.rl` files. Building libraries of reusable Ragel components (character classes, common patterns, utility actions).

**Why it's useful:** Modularity and code reuse at the Ragel level. Define atoms, character classes, and helper machines once in a library file, import them where needed. Changes propagate automatically.

---

### 67. Including Named Machines

```ragel
machine my_parser;
include "library.rl";                    # include same-named machine from file
include other_machine "library.rl";      # include different-named machine
include third_machine;                   # include machine from current scope
```

Three include forms:
1. `include "file"` — includes the same-named machine from the file
2. `include name "file"` — includes a specific machine from the file
3. `include name` — includes a machine from the current compilation scope

**When to use:** Building parsers from separately-compiled sub-machines. Multi-file projects where each file defines one or more machines, and the main file includes them.

**Why it's useful:** True modular compilation. Each sub-machine can be in its own file with its own actions and patterns. The main file imports them by name. This scales to large projects with dozens of machines across many files.

---

### 68. Concurrent Machines (Union Running in Parallel)

```ragel
chars    := ( any @next_char )*;
word     := ( any-space )+ >start_word %end_word;
comment  := ( '/*' any* :>> '*/' ) >start_comment %end_comment;
literal  := ( '\'' literalChar* '\'' ) >start_literal %end_literal;

main := chars | words | comments | literals;
```

Multiple independent machines united with `|` read the SAME input stream simultaneously. Each tracks its own progress. This extracts four different features (characters, words, comments, string literals) from the same input in ONE pass.

**When to use:** Multi-feature extraction. Line counting + token extraction + comment tracking, all in one pass. Parallel validation (check multiple constraints simultaneously). Multi-pattern search.

**Why it's useful:** Eliminates multiple input passes. Four independent scanners would require four reads of the data. One concurrent machine does it in one pass. The state space grows (product of individual machines) but the compiler handles this transparently.

---

### 69. Concurrent Line/Column Tracking (`& position`)

```ragel
position := (
    '\n' @{ line++; col = 1; } |
    [^\n] @{ col++; }
)*;

main := ( scanner_tokens ) & position;
```

A `position` machine tracks line and column numbers. It's intersected (`&`) with the primary scanner, so both run simultaneously on every character. The scanner matches tokens; the position tracker updates line/col.

**When to use:** Any parser that needs source location tracking for error messages. Compiler front-ends, linters, syntax highlighters.

**Why it's useful:** Separation of concerns. The position tracking is a completely independent machine that just happens to run concurrently. Changes to the scanner don't affect position tracking, and vice versa. The `&` operator composes them without interference.

---

### 70. Buffer Management (Streaming Input Loop)

```c
while ( !done ) {
    int space = BUFSIZE - have;
    char *p = buf + have;
    int len = fread( p, 1, space, stdin );
    char *pe = p + len;
    char *eof = is_last ? pe : 0;

    %% write exec;

    if ( ts == 0 )
        have = 0;
    else {
        have = pe - ts;
        memmove( buf, ts, have );
        te = buf + (te - ts);
        ts = buf;
    }
}
```

The canonical streaming input loop: read a chunk into a fixed buffer, run the machine, preserve any token-in-progress prefix, shift it to the buffer start, refill the rest.

**When to use:** Processing data from streams (files, sockets, pipes). Any input source where data arrives in chunks rather than all at once.

**Why it's useful:** Memory-efficient processing of arbitrarily large inputs with a fixed buffer. The shift-and-refill pattern handles tokens that span buffer boundaries. The `eof` flag tells Ragel this is the last chunk, triggering EOF handling.

---

### 71. Pull-Style Scanner (Inversion of Control)

```c
int scan( Scanner *s ) {
    while (1) {
        if ( s->p == s->pe ) {
            // refill buffer...
        }
        %%{
            main := |*
                alpha alnum* => { ret_tok(TK_Id); fbreak; };
                digit+       => { ret_tok(TK_Num); fbreak; };
            *|;
            write exec;
        }%%
        if ( token != TK_NO_TOKEN )
            return token;
    }
}
```

The scanner function is called repeatedly; each call returns one token. State persists in the struct between calls. `fbreak` exits the machine's execution loop while preserving state.

**When to use:** Token-by-token interfaces for parser generators. Iterator-style access to a token stream. Any scenario where the consumer pulls tokens rather than having the scanner push them via callbacks.

**Why it's useful:** Inverts control flow. The consumer drives the scanner, requesting tokens on demand. This integrates naturally with recursive-descent parsers and other pull-based consumers. The `fbreak` + state persistence pattern acts as a coroutine: suspend, yield a value, resume.

---

### 72. EOF Actions: `@eof`, `%eof`, `>eof`, `$eof`

```ragel
action eof_start { printf("eof at start\n"); }
action eof_middle { printf("eof mid\n"); }
action eof_out { printf("eof out\n"); }

main := ( 'hello'
    >eof eof_start $eof eof_all <>eof eof_middle %eof eof_out
) '\n';
```

EOF actions fire when end-of-input is reached in specific states. Six scoping levels mirror error action scopes.

**When to use:** Detecting incomplete input ("expected more but got EOF"). Graceful handling of truncated data. Protocols where EOF signals message boundaries.

**Why it's useful:** Distinct from error handling — EOF is not a character but a condition. Ragel generates code to check `p == eof` and fire appropriate EOF actions for the current state. This handles the difference between "bad character" and "no more data" naturally.

---

### 73. EOF with `fgoto` Dispatch

```ragel
action extra {
    fhold;
    fgoto *fentry(extra);
}
extra := "" %done;

main := atoi '\n' @print %extra;
```

After `atoi` succeeds and reaches `%extra`, the action uses `fhold` (retain the newline) and `fgoto *fentry(extra)` to jump to a separate machine. This implements post-processing: after main parsing completes, do something extra.

**When to use:** Post-parse actions that should happen after the main machine accepts. Cleanup, final validation, emitting epilogue data.

**Why it's useful:** Separates main parsing from post-processing. The main machine focuses on pattern matching; the post-processing machine handles "after acceptance" logic. The `fhold` + `fgoto` pattern cleanly transitions between phases.

---

### 74. Complete Atoi (ASCII to Integer) — The Canonical Example

```ragel
action begin     { neg = false; val = 0; }
action see_neg   { neg = true; }
action add_digit { val = val * 10 + (fc - '0'); }
action finish {
    if ( neg ) val = -1 * val;
}

atoi := ( '-' @see_neg | '+' )? ( digit @add_digit )+ >begin %finish;
main := atoi '\n' @{ printf("%d\n", val); };
```

The classic Ragel introductory example: parse an optional sign followed by digits into an integer. Demonstrates `>`, `@`, `%`, `|`, `?`, `+`, builtin `digit`, and multi-action lifecycle.

**When to use:** The "Hello World" of Ragel. Teaches the basic action model. Also a real, useful pattern for extracting integers.

**Why it's useful:** Compact yet demonstrates Ragel's core concepts. The `>begin`/`%finish` pattern, `@` for per-element accumulation, and the overall structure apply to most Ragel machines. Understanding this example is understanding Ragel.

---

### 75. Printf Format String Parser

```ragel
action flags_hash  { flags |= FL_HASH; }
action width_num   { width = 10 * width + (fc-'0'); }
action prec_num    { prec = 10 * prec + (fc-'0'); }
action conversion  { do_conv( fc ); }

flags      := ( '#' @flags_hash | '0' @flags_zero | '-' @flags_dash | ' ' @flags_space | '+' @flags_plus )*;
width      := ( ( nznum $width_num | '*' @width_arg ) %width )?;
precision  := ( '.' ( digit* $prec_num %prec | '*' @prec_arg ) )?;
length     := ( 'h' %length_h | 'l' %length_l | 'hh' @length_hh | 'll' @length_ll )?;
conversion := [diouxXcsp] @conversion;

fmt_spec := '%' @clear flags width precision length conversion;
main := ( [^%] @emit | '%%' @emit | fmt_spec )* ...;
```

A complete printf format specifier parser extracting flags, width, precision, length, and conversion character.

**When to use:** Parsing structured mini-languages: format strings, date/time patterns, SQL placeholders, URL templates. Demonstrates modular pattern composition with independent semantic actions per component.

**Why it's useful:** Each format component is defined independently then composed. Adding a new format flag is a one-line change. The priority-based length matching (`%length_h` vs `@length_hh`) correctly handles `hh` as a unit rather than two `h` matches. Production-quality pattern.

---

### 76. Unix Mbox Parser

```ragel
fromLine := 'From ' . (any-'\n')* . ' ' . date . '\n' @(new_msg,1) @msgstart;
header   := hchar+ $bufHeadName . ':' . ( (extend-'\n') $bufHeadContent | '\n' [ \t] @bufHeadContent )* . '\n' %finBufHeadContent;
messageLine := ( any - '\n' )* . '\n' - fromLine;
message  := ( fromLine . header* . '\n' @blankLine . messageLine* );

main := message*;
```

A complete UNIX mbox parser handling message boundaries, header field extraction, line continuations, and date parsing. Uses priority annotations (`@(new_msg,1)`) to correctly distinguish `From` lines in bodies from actual message separators.

**When to use:** Real-world email processing, structured text archives, multi-record file formats. Demonstrates how priority annotations solve the "delimiter appears in content" problem.

**Why it's useful:** A production-quality parser demonstrating advanced Ragel features: priority-based disambiguation, character class subtraction, header continuation handling, buffer accumulation. The `IncludeHistoryItem` mechanism tracks include files for error reporting.

---

### 77. URL Parser (RFC 3986)

```ragel
escape     := '%' xdigit xdigit;
userchars  := any -- ( authdelims | scary );
authority  := ( userpass ( '@' @atsymbol ) )? hostport params?;
path       := ( '/' segment )*;
query      := '?' [^#]*;
fragment   := '#' any*;

url := scheme ':' '//'? authority path? query? fragment?;
```

A production RFC 3986 URL parser. Uses strong subtraction (`--`) for safe character class definition, handles percent-encoding, parses scheme/authority/path/query/fragment.

**When to use:** URL parsing, URI validation, web crawlers, HTTP libraries. This is a reference implementation of the standard URL decomposition.

**Why it's useful:** Demonstrates safe character class definition with `--` (safe exclusion from `any` ensures future-proof character handling). The percent-encoding parser (`%xdigit xdigit`) handles escaped characters incrementally with two-digit hex accumulation.

---

### 78. XML / Unicode Parser

```ragel
alphtype unsigned short;

BaseChar     := 0x0041..0x005A | 0x0061..0x007A | 0x00C0..0x00D6 | ... ;
Ideographic  := 0x4E00..0x9FA5 | 0x3007 | 0x3021..0x3029;
Letter       := BaseChar | Ideographic;
NameChar     := Letter | Digit | '.' | '-' | '_' | ':' | CombiningChar | Extender;
```

A Unicode XML parser skeleton using `unsigned short` alphabet (16-bit). Defines massive Unicode character classes from the XML specification. Demonstrates that Ragel handles large alphabets (up to 65536 code points).

**When to use:** Unicode-aware parsers, XML/HTML processing, international text handling. Any parser that must operate on code points rather than bytes.

**Why it's useful:** Ragel's DFA approach scales to large alphabets. The compiler handles 16-bit key types, building transition tables for the full Unicode BMP. The named character classes (`BaseChar`, `Ideographic`, etc.) map directly to the XML specification for verifiable correctness.

---

### 79. Reverse Polish Notation Calculator

```ragel
action mark { mark = p; }
action push { stack.push(atoi(mark, p-mark)); }
action add  { b = stack.pop(); a = stack.pop(); stack.push(a + b); }

stuff := digit+ >mark %push
    | '+' @add | '-' @sub | '*' @mul | '/' @div
    | 'abs' %abs;

main := ( space | stuff space )*;
```

An RPN calculator: tokens are numbers or operators, separated by spaces. Numbers are accumulated and pushed; operators pop operands and push results. All in one pass.

**When to use:** Expression evaluators, calculator implementations, stack-machine interpreters. Demonstrates action-based computation during parsing.

**Why it's useful:** Integrates parsing and evaluation in a single pass. No intermediate AST — the computation happens as tokens are recognized. The `>mark`/`%push` pattern captures the number string span for conversion.

---

### 80. C++ Lexer (Production Quality)

```ragel
main := |*
    ( 'L'? "'" ( [^'\\\n] | /\\./ )* "'" ) => { token(TK_Slit); };
    ( 'L'? '"' ( [^"\\\n] | /\\./ )* '"' ) => { token(TK_Dlit); };
    [a-zA-Z_][a-zA-Z0-9_]* => { token(TK_Id); };
    ( digit+ '.' digit+ | digit+ '.' ) exponent? => { token(TK_Float); };
    ( '0' | [1-9][0-9]* ) [ulUL]{0,3} => { token(TK_IntDec); };
    '0' [0-7]+ [ulUL]{0,2} => { token(TK_IntOct); };
    '0x' [0-9a-fA-F]+ [ulUL]{0,2} => { token(TK_IntHex); };
    '::'         => { token(TK_NameSep); };
    '=='         => { token(TK_EqualsEquals); };
    '++'         => { token(TK_PlusPlus); };
    '/*' { fgoto c_comment; };
    '//' [^\n]* '\n';
    ( any - 33..126 )+;
*|;
```

A complete C++ tokenizer handling: string/char literals, identifiers, floats (with optional exponent/suffix), decimals, octals, hex integers, compound operators, block comments, line comments, and whitespace.

**When to use:** Production lexer for C-like languages. The pattern demonstrates all essential lexer features: overlapping token recognition, longest-match automation, comment handling via `fgoto`, silent pattern consumption.

**Why it's useful:** Serves as a template for any programming language lexer. The pattern division (literals, identifiers, numbers, operators, comments, whitespace) applies to virtually every language. The `fgoto` state switching handles comment nesting cleanly.

---

### 81. Command-line Argument Parser

```ragel
string  := [^\0]+ >clear $append %term;
help    := ('-h' | '-H' | '-?' | '--help') 0 @help;
version := ('-v' | '--version') 0 @version;
output  := '-o' 0? string 0 @output;
spec    := '-S' 0? string 0 @spec;

main := ( help | version | output | spec | mach )*;
```

Parses argv strings using a state machine. Each argument is processed as a null-terminated string. The `0` (NUL) character acts as an argument boundary. `0?` handles optional separating space between flag and value.

**When to use:** Command-line argument parsing, configuration directive parsing, protocol command interpretation. The pattern scales to complex argument grammars with subcommands and required/optional values.

**Why it's useful:** Replaces getopt-style hand-written arg parsing with declarative patterns. Error handling is implicit — unrecognized arguments cause error state. Clean separation between pattern definition and action logic.

---

### 82. Awk Emulation (Line-based Processing)

```ragel
action start_word { ws[nwords] = fpc; }
action end_word   { we[nwords++] = fpc; }
action start_line { nwords = 0; ls = fpc; }
action end_line   { printf("line(%d): ...", nwords); }

word       := ^[ \t\n]+;
whitespace := [ \t];
line       := ( word >start_word %end_word | whitespace )** '\n' >start_line @end_line;

main := line*;
```

Emulates awk-like line splitting: breaks each line into whitespace-separated words, tracking each word's start/end positions. Uses `**` for greedy star with priority-shifted word/whitespace handling.

**When to use:** Log file analysis, CSV parsing, tabular data processing. Any line-oriented format where fields need extraction with position tracking.

**Why it's useful:** Demonstrates the `**` operator for greedy field extraction. The start/end tracking gives byte-precise positions of each field. Processes lines one at a time for memory efficiency.

---

### 83. Incremental/Line-based Processing (Newline Anchoring)

```c
// Find the last newline by searching backwards
p = buf;
pe = buf + have + len - 1;
while ( *pe != '\n' && pe >= buf )
    pe--;
pe += 1;

%% write exec;

// Preserve incomplete line
have = data + len - pe;
if ( have > 0 )
    memmove( buf, pe, have );
```

Processes input line-by-line by scanning backward from the chunk end to find the last complete newline. Only processes complete lines; preserves the incomplete line prefix for the next iteration.

**When to use:** Line-oriented protocols, log processing, any format where processing must happen on complete lines. More memory-efficient than reading entire file into memory.

**Why it's useful:** Enables streaming line processing without splitting the buffer at arbitrary positions. The backward newline search guarantees processing only complete lines. The partial-line preservation prevents mid-line buffer splits from corrupting tokens.

---

### 84. Statechart-based Protocol Handler

```ragel
protocol := (
    idle: (
        'S' @start_cmd ->cmd_char |
        any ->idle
    ),
    cmd_char: (
        [A-Z] ->cmd_end |
        any @error ->idle
    ),
    cmd_end: (
        '\n' @exec_cmd ->idle |
        ' ' ->args
    ),
    args: ( ... )
);
```

A protocol handler expressed as explicit named states. Each state handles specific input characters with well-defined transitions. Error handling returns to the `idle` state.

**When to use:** Network protocol implementations, serial device handlers, UI state machines. Any scenario with a well-defined state diagram that benefits from explicit naming.

**Why it's useful:** Directly maps to protocol state diagrams. Each state corresponds to a distinct phase of processing. The named states are self-documenting. Easy to add new states and transitions without breaking existing logic.

---

### 85. Email Address Parser (RFC 2822)

```ragel
addrWord    := ( alnum | punct )+;
addrAddr1   := '<' [^>]* '>';
addrAddr2   := addrWord '@' addrWord;
addrComment := '(' ( [^()] | addrComment )* ')';
address     := ( addrWS | addrComment | addrItem )**;

# Subject with line continuations
subjectHeader := 'Subject:' @prn_subj ' '* 
    <: ( [^\n] @prn_char | ( '\n' [ \t]+ ) %prn_space )**
    '\n' %prn_nl;
```

Parses email addresses in multiple formats: simple `user@host`, angle-bracket `<user@host>`, with optional `(comments)`. Subject parsing handles RFC 2822 line continuations (newline followed by whitespace means the header continues).

**When to use:** Email processing, contact extraction, header parsing in MIME/HTTP-like protocols.

**Why it's useful:** Demonstrates recursive comment parsing and line continuation handling. The `<:` (entry-guarded repetition) on subject ensures the star starts after whitespace. The `**` priority ensures correct component ordering.

---

### 86. Nested Brace Parser (Recursive Descent)

```ragel
prepush {
    if ( top == stack_size ) {
        stack_size = top * 2;
        stack = (int*)realloc( stack, sizeof(int)*stack_size );
    }
}

action call_main { fncall main; }
action ret_main {
    if ( top == 0 ) { fbreak; }
    fhold; fnret;
}

main := (
    ws |
    ( number | id ) >item_start %item_finish |
    '{' @call_main '}' |
    '}' @ret_main
)**;
```

A recursive descent parser for nested braces with token extraction. `{` triggers a recursive call to `main`. `}` returns from the call. Stray `}` when stack is empty triggers `fbreak`.

**When to use:** JSON/XML/S-expression parsing. Any language with recursive nested structure. The pattern handles arbitrary nesting depth.

**Why it's useful:** Demonstrates Ragel's context-free parsing capability. The push/pop state stack (+ dynamic growth) enables truly recursive state machines. This handles languages that pushdown automata theory says finite automata alone can't parse.

---

### 87. Mail Header Field Extractor

```ragel
action onHeader {
    headName.append(0);
    if ( strcmp(headName.data, "From") == 0 ||
         strcmp(headName.data, "To") == 0 ||
         strcmp(headName.data, "Subject") == 0 )
    {
        fcall printHeader;
    }
    headName.clear();
    fcall consumeHeader;
}

header := hchar+ $bufHeadName ':' @onHeader;
```

Parses mail headers, extracts specific fields (From, To, Subject), and prints their content. Headers not in the target set are consumed silently. Header continuation lines are properly handled.

**When to use:** Extracting specific fields from structured headers. Email processing, HTTP header processing, MIME parsing.

**Why it's useful:** Selective field extraction — only processes headers of interest, silently consumes the rest. The `fcall` to `printHeader` or `consumeHeader` dynamically chooses the right sub-machine based on field name. This is more efficient than parsing all headers then filtering.

---

### 88. Multi-Language Target Example (Same DSL, Different Hosts)

```ragel
# C: 
machine atoi;
action add_digit { val = val * 10 + (fc - '0'); }

# Ruby:
machine atoi;
action add_digit { val = val * 10 + (fc - "0"[0].ord); }

# OCaml:
machine atoi;
action add_digit { res := !res * 10 + (fc - Char.code '0'); }

# ASM x86-64:
machine atoi;
action add_digit {
    movl val(%rip), %eax
    imul $10, %eax
    movsbl (%r12), %ecx
    subl $48, %ecx
    add %ecx, %eax
    movl %eax, val(%rip)
}
```

The same atoi machine specification with actions in four different host languages. The Ragel DSL is identical; only the action code changes to match the host language syntax.

**When to use:** Cross-language projects, embedded systems with C core and scripting language interfaces, code generation targeting multiple platforms.

**Why it's useful:** Write once, generate for many. The state machine logic is language-independent; only the actions are language-specific. This enables consistent parsers across a project's C core, Ruby tooling, and OCaml verification.

---

### 89. Language-Independent (indep) Testing

```ragel
# @LANG: indep
ptr ts;
ptr te;
int act;

%%{
    machine scanner;
    main := |*
        'a' => { print_str "pat1\n"; };
        'b' => { print_str "pat2\n"; };
    *|;
}%%
```

The `indep` (independent) target is Ragel's built-in test harness. It provides `print_str`, `print_int`, `print_token` intrinsics for output verification. The `ptr` type is a language-independent pointer.

**When to use:** Writing test cases for Ragel patterns. The indep target verifies that the state machine logic is correct, independent of any host language. Test output is compared against `##### OUTPUT #####` blocks.

**Why it's useful:** Enables TDD for Ragel machines. Write a test, run `./runtests.sh`, verify output. The indep target removes host-language variables from testing, focusing purely on FSM behavior. All 150+ tests in the test suite use this approach.

---

### 90. Kellerville Parser (Ragel Parsing Ragel)

```ragel
alphtype int;
getkey fpc->type;

main :=
s0: start: ( 23 @shift ->s1 | 25 @shift ->s3 | ... ),
s1: ( 20 @shift ->s54 ),
s2: ( (0|20) @pop2 @newM @r_M_0 @rem2 ->s54 ),
...
```

An auto-generated LALR(1) parser table compiled into a Ragel state machine. Each state (`s0`, `s1`, ...) corresponds to an LR item set. `shift` pushes tokens, `pop` manipulates the parse stack, `newX` creates AST nodes.

**When to use:** This is how Ragel parses itself (bootstrapping). The Keller tool generates Ragel code that implements a parser for Ragel's own DSL.

**Why it's useful:** Demonstrates the ultimate meta-circular use of Ragel: a state machine compiler whose parser is itself a compiled state machine. The integer alphabet + custom getkey pattern enables processing structured token streams, not just byte streams.

---

### 91. GCC Computed Goto Backend (`-G2`)

```bash
ragel -G2 input.rl -o output.c
```

The `-G2` flag generates code using GCC's computed goto extension (labels-as-values). This produces the fastest possible state machine on GCC-compatible compilers, at the cost of portability.

**When to use:** Performance-critical applications on GCC/Clang toolchains. Embedded systems where every cycle matters. The computed goto eliminates the switch/case dispatch overhead.

**Why it's useful:** Direct jump to the next state via an array of label pointers. Eliminates the branch predictor misprediction overhead of switch statements. Can be 2-5x faster than switch-based dispatch for dense state machines. Only available with `GotoFeature`.

---

### 92. Flat Table Backend (`-F0`, `-F1`)

```bash
ragel -F0 input.rl -o scanner.c
```

The flat table backend generates a single contiguous transition table. Each state is a row; each character value is a column. The transition is a simple table lookup: `cs = table[cs * 256 + *p]`.

**When to use:** Small machines where table lookup outperforms binary search. Embedded systems with fast RAM access. Machines with dense transition tables (most states have transitions on most characters).

**Why it's useful:** Constant-time per-character dispatch. No branches, no binary search, just a table index. Best for: small alphabets, dense transitions. Worst for: large alphabets (65K+), sparse transitions.

---

### 93. Binary Search Backend (`-T0`, `-T1`)

```bash
ragel -T0 input.rl -o scanner.c
```

The binary search backend generates transition tables with binary search dispatch. Each state's transitions are sorted; the code binary-searches to find the matching character range.

**When to use:** Sparse transition tables where flat tables would waste memory. Large alphabets (Unicode). Most general-purpose Ragel compilation.

**Why it's useful:** Memory-efficient for sparse machines. The binary search uses O(log k) comparisons where k is the number of distinct transitions from that state. This is the default (`-T0`) because it balances speed and size well.

---

### 94. Visualization with Graphviz DOT (`-V`)

```bash
ragel -V -p input.rl -o machine.dot
dot -Tpng machine.dot -o machine.png
```

The `-V` flag generates a Graphviz DOT file instead of code. `-p` renders characters as printable labels. `-S spec` and `-M machine` select specific machines.

**When to use:** Debugging machine behavior. Understanding what a complex pattern compiles to. Documenting protocol state machines. Teaching FSM concepts.

**Why it's useful:** Makes the state machine's structure visible. You can see which states exist, what transitions connect them, where actions attach, and how minimization affects the graph. Invaluable for understanding why a pattern matches (or doesn't).

---

### 95. FSM Stats with `-s`

```bash
ragel -s input.rl
```

The `-s` flag prints statistics to stderr: state count, transition count, alphabet size, code size estimates.

**When to use:** Performance tuning. Comparing minimization levels. Understanding the cost of different pattern structures. Capacity planning for memory-constrained systems.

**Why it's useful:** Quantitative feedback on machine size. Helps choose between `-T0`/`-F0`/`-G0` backends based on actual state counts. Combined with `-n` through `-k` minimization flags, you can find the best compile-time/machine-size tradeoff.

---

### 96. Minimization Level Selection

```bash
ragel -m input.rl     # minimize after building (good default)
ragel -e input.rl     # minimize after every op (smallest, slowest compile)
ragel -n input.rl     # no minimization (fastest compile)
ragel -l input.rl     # minimize after most ops
```

Each minimization flag controls when Hopcroft's DFA minimization algorithm runs. More minimization = smaller machine, slower compilation.

**When to use:** `-m` for development (good balance). `-e` for production builds where machine size matters. `-n` for iteration speed during development.

**Why it's useful:** The compile-time vs machine-size tradeoff is significant for large grammars. `-n` can compile in seconds where `-e` takes minutes. The minimized machine is identical in behavior regardless of level — only size differs.

---

### 97. Breadth/Depth Analysis (`--breadth-check`)

```bash
ragel --breadth-check --input-histogram chars.txt input.rl
```

Performs cost analysis from entry points. The histogram provides character frequency data; Ragel computes the expected number of state transitions per character for each entry point, weighted by frequency.

**When to use:** Optimizing state machine entry points. Understanding which entry path is most expensive. Input-sensitive performance tuning.

**Why it's useful:** Identifies hot paths through the state machine. The compiler can optimize code generation based on this analysis (e.g., inline the most common paths). The histogram can be generated from representative input data for production workload modeling.

---

### 98. The Direct Backend (ASM-only)

```bash
ragel -G2 input.rl -o output.s    # generates GNU ASM x86-64
```

The ASM host is the ONLY backend using the `Direct` (non-translated) mode. Instead of generating a virtual machine that's executed by generated C code, it generates native x86-64 instructions that directly implement the state machine.

**When to use:** Extremely performance-sensitive parsing on x86-64. Embedded systems with assembly-level control requirements. Demonstrating maximum theoretical Ragel performance.

**Why it's useful:** Eliminates the VM dispatch overhead entirely. State transitions are direct `jmp`/`jne` instructions. Character consumption and state updates are register operations. This is the fastest possible Ragel output for x86-64.

---

### 99. OCaml VarFeature Backend

```ragel
# OCaml generates pattern-match based state dispatch:
match cs with
| 0 -> (* state 0 *)
    (match Char.code data.[!p] with
    | 104 -> p := !p + 1; cs := 1; (* 'h' -> state 1 *)
    | _ -> cs := -1) (* error *)
| 1 -> (* state 1 *)
    ...
```

The OCaml backend uses `VarFeature` because OCaml has neither `goto` nor labeled `break`. Instead, it generates a tail-recursive function with pattern matching on the current state and input character.

**When to use:** OCaml projects needing fast lexers. The pattern-match approach is idiomatic OCaml that the OCaml compiler can optimize well.

**Why it's useful:** Demonstrates Ragel's backend flexibility. The same Ragel DSL compiles to fundamentally different control flow constructs depending on the target language's capabilities. OCaml developers get a state machine compiler that produces native-feeling code.

---

### 100. Integration Build Pattern (autogen + configure + make)

```bash
./autogen.sh
./configure --prefix=/usr/local --with-colm=/path/to/colm
make -j$(nproc)
make install

# Test
cd test && bash runtests.sh
```

The standard Ragel build workflow using Autotools. Requires Colm 0.14.7 installed separately. Generates all 12 language-specific binaries (`ragel-c`, `ragel-java`, `ragel-ruby`, etc.).

**When to use:** Building Ragel from source. Integrating into a larger build system. CI/CD pipelines for projects that use Ragel as a code generation step.

**Why it's useful:** The multi-host architecture means you only need to build the backends for languages you use. The `configure` script probes for host compilers and skips unavailable ones. CMakeLists.txt files provide an alternative build path.

---

## Cross-Reference: Feature to Pattern Map

| Feature Category | Patterns |
|-----------------|----------|
| Basic Matching | 1, 2, 6, 7 |
| Character Sets | 3, 4, 5, 11, 12, 13 |
| Repetition | 8, 9, 10, 52, 53, 54, 55 |
| Actions | 18, 19, 20, 21, 25, 29 |
| Error Handling | 26, 27, 28, 29, 42, 43, 44 |
| EOF Handling | 72, 73 |
| Call/Return/Goto | 36, 37, 38, 39, 40, 41, 42 |
| Scanners | 30, 31, 32, 33, 34, 35, 45 |
| Conditions | 47, 48, 49, 50, 51 |
| Priorities | 57, 58 |
| Advanced Operators | 14, 15, 16, 17 |
| Statechart | 22, 23, 24 |
| Struct/Multi-Instance | 60, 61, 62, 63 |
| Export/Import/Include | 65, 66, 67 |
| Concurrent Machines | 68, 69 |
| Buffer/Streaming | 70, 71, 83 |
| Write Statements | 64 |
| Real-World Parsers | 75, 76, 77, 78, 79, 80, 81, 82, 85, 86, 87 |
| Code Generation | 91, 92, 93, 96, 98, 99 |
| Tooling/Testing | 89, 90, 94, 95, 97, 100 |
| Multi-Language | 88 |
