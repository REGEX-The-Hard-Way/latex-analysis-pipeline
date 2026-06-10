#ifndef REGEX_UTIL_H
#define REGEX_UTIL_H

#include <stddef.h>

/*
 * regex_sub: Replace matches of `pattern` in `input` with `replacement`.
 *
 *   - If `global` is non-zero, every non-overlapping match is replaced;
 *     otherwise only the first match is replaced.
 *   - `replacement` is a literal string. Backreferences (e.g. "\\1") are
 *     NOT interpreted; they are inserted verbatim.
 *   - Returns a newly malloc'd, NUL-terminated string the caller must free().
 *   - Returns NULL on allocation failure or if `pattern` fails to compile.
 *
 * All arguments must be non-NULL, NUL-terminated C strings.
 */
char *regex_sub(const char *input,
                const char *pattern,
                const char *replacement,
                int global);

/*
 * regex_split: Split `input` on matches of `pattern`.
 *
 *   - Returns a newly malloc'd, NULL-terminated array of newly malloc'd
 *     NUL-terminated strings. The caller must free each string and the
 *     array itself (a helper, regex_free_split, is provided).
 *   - If `count_out` is non-NULL, the number of returned tokens (excluding
 *     the trailing NULL sentinel) is stored there.
 *   - An empty `input` produces a single empty-string token.
 *   - Zero-width matches are skipped to avoid infinite loops.
 *   - Returns NULL on allocation failure or if `pattern` fails to compile.
 */
char **regex_split(const char *input,
                   const char *pattern,
                   size_t *count_out);

/* Frees the array returned by regex_split. Safe to call with NULL. */
void regex_free_split(char **tokens);

/*
 * regex_find: Locate the first match of `pattern` in `input`.
 *
 *   - On a successful match, writes the byte offset of the match start to
 *     *start_out and the offset one past the match end to *end_out, then
 *     returns 1.
 *   - Returns 0 if there is no match.
 *   - Returns -1 on compilation error or invalid arguments.
 *
 * Either `start_out` or `end_out` may be NULL if the caller does not need
 * that value.
 */
int regex_find(const char *input,
               const char *pattern,
               size_t *start_out,
               size_t *end_out);

#endif /* REGEX_UTIL_H */
