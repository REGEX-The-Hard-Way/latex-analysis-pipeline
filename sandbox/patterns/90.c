/* Pattern 90: Keller Parser Concept — C program demonstrating getkey on struct tokens */
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* The Keller parser pattern in Ragel uses fhold/fgoto/fbreak within
 * actions. This simulates a token-stream parser where 'getkey'
 * retrieves the next token type for the FSM to transition on.
 */

typedef enum { TK_WORD, TK_NUM, TK_PUNCT, TK_NL, TK_EOF } TokenType;

typedef struct {
  TokenType type;
  const char *text;
  int len;
} Token;

static Token tokens[] = {
  {TK_WORD, "hello", 5},
  {TK_NUM,  "42",    2},
  {TK_PUNCT,"!",     1},
  {TK_NL,   "\n",    1},
  {TK_EOF,  "",      0}
};

static int getkey(const Token *t) {
  if (!t) return -1;
  switch (t->type) {
    case TK_WORD:  return 'w';
    case TK_NUM:   return 'd';
    case TK_PUNCT: return 'p';
    case TK_NL:    return '\n';
    default:       return 0;
  }
}

int main(void) {
  printf("Pattern 90: Keller Parser Concept\n\n");
  printf("In Ragel, getkey would be called in an fcall/fret loop.\n");
  printf("This simulates tokenization and classification:\n\n");

  for (int i = 0; i < 5; i++) {
    int k = getkey(&tokens[i]);
    printf("  Token[%d]: type=%d text=\"%.*s\" key=%c\n",
           i, tokens[i].type, tokens[i].len, tokens[i].text, k);
  }

  int r = (getkey(&tokens[3]) == '\n') ? 1 : 0;
  printf("\n  -> %s\n", r ? "ACCEPT" : "FAIL");
  assert(r == 1);
  printf("All tests passed.\n");
  return 0;
}
