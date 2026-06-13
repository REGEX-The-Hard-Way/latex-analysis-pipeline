#ifndef MACRO_EXPANDER_H
#define MACRO_EXPANDER_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct macro_expander macro_expander_t;

macro_expander_t *macro_expander_create(void);

void macro_expander_destroy(macro_expander_t *me);

void macro_expander_feed(macro_expander_t *me, const char *input, size_t len);

const char *macro_expander_get_output(macro_expander_t *me, size_t *out_len);

typedef void (*macro_expander_callback)(const char *text, size_t len, void *user);

void macro_expander_feed_cb(macro_expander_t *me, const char *input, size_t len,
                            macro_expander_callback cb, void *user);

void macro_expander_reset(macro_expander_t *me);

int macro_expander_macro_count(macro_expander_t *me);

#ifdef __cplusplus
}
#endif

#endif /* MACRO_EXPANDER_H */
