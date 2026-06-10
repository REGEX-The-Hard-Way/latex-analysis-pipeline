/**
 * porter2.h - Porter2 (English) stemmer
 *
 * Public API for the Porter2 stemming algorithm.
 */

#ifndef PORTER2_H
#define PORTER2_H

#include <stddef.h>

/**
 * Stem a word using the Porter2 algorithm.
 * @param input The word to stem (modified in-place)
 * @return The length of the stemmed word, or 0 on error
 */
int porter2_stem(char *input);

#endif /* PORTER2_H */
