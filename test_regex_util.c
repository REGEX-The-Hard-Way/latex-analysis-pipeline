#include "regex_util.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures = 0;

#define CHECK(cond)                                                       \
    do {                                                                  \
        if (!(cond)) {                                                    \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);\
            failures++;                                                   \
        }                                                                 \
    } while (0)

static void test_sub(void)
{
    char *r;

    r = regex_sub("hello world", "world", "there", 0);
    CHECK(r && strcmp(r, "hello there") == 0);
    free(r);

    r = regex_sub("aaa", "a", "b", 0);
    CHECK(r && strcmp(r, "baa") == 0);
    free(r);

    r = regex_sub("aaa", "a", "b", 1);
    CHECK(r && strcmp(r, "bbb") == 0);
    free(r);

    r = regex_sub("foo123bar456", "[0-9]+", "#", 1);
    CHECK(r && strcmp(r, "foo#bar#") == 0);
    free(r);

    r = regex_sub("", "a", "b", 1);
    CHECK(r && strcmp(r, "") == 0);
    free(r);

    r = regex_sub("abc", "x", "y", 1);
    CHECK(r && strcmp(r, "abc") == 0);
    free(r);

    /* Zero-width match (empty pattern alternative). */
    r = regex_sub("abc", "x*", "-", 1);
    CHECK(r != NULL);
    free(r);

    /* Bad pattern. */
    r = regex_sub("abc", "[", "x", 1);
    CHECK(r == NULL);
}

static void test_split(void)
{
    size_t n = 0;
    char **t;

    t = regex_split("a,b,c", ",", &n);
    CHECK(t && n == 3);
    CHECK(strcmp(t[0], "a") == 0);
    CHECK(strcmp(t[1], "b") == 0);
    CHECK(strcmp(t[2], "c") == 0);
    CHECK(t[3] == NULL);
    regex_free_split(t);

    t = regex_split("", ",", &n);
    CHECK(t && n == 1 && strcmp(t[0], "") == 0);
    regex_free_split(t);

    t = regex_split(",a,,b,", ",", &n);
    CHECK(t && n == 5);
    CHECK(strcmp(t[0], "") == 0);
    CHECK(strcmp(t[1], "a") == 0);
    CHECK(strcmp(t[2], "") == 0);
    CHECK(strcmp(t[3], "b") == 0);
    CHECK(strcmp(t[4], "") == 0);
    regex_free_split(t);

    t = regex_split("one  two   three", "[ ]+", &n);
    CHECK(t && n == 3);
    CHECK(strcmp(t[0], "one") == 0);
    CHECK(strcmp(t[1], "two") == 0);
    CHECK(strcmp(t[2], "three") == 0);
    regex_free_split(t);

    t = regex_split("nodelim", ",", &n);
    CHECK(t && n == 1 && strcmp(t[0], "nodelim") == 0);
    regex_free_split(t);

    t = regex_split("abc", "[", &n);
    CHECK(t == NULL && n == 0);

    /* Zero-width pattern shouldn't infinite loop. */
    t = regex_split("abc", "x*", &n);
    CHECK(t != NULL);
    regex_free_split(t);

    /* Free of NULL is safe. */
    regex_free_split(NULL);
}

static void test_find(void)
{
    size_t s = 0, e = 0;
    int rc;

    rc = regex_find("hello world", "world", &s, &e);
    CHECK(rc == 1 && s == 6 && e == 11);

    rc = regex_find("hello world", "xyz", &s, &e);
    CHECK(rc == 0);

    rc = regex_find("abc123def", "[0-9]+", &s, &e);
    CHECK(rc == 1 && s == 3 && e == 6);

    rc = regex_find("abc", "[", &s, &e);
    CHECK(rc == -1);

    /* NULL output pointers allowed. */
    rc = regex_find("hello", "ell", NULL, NULL);
    CHECK(rc == 1);

    rc = regex_find(NULL, "x", &s, &e);
    CHECK(rc == -1);
}

int main(void)
{
    test_sub();
    test_split();
    test_find();

    if (failures == 0) {
        printf("All tests passed.\n");
        return 0;
    }
    printf("%d test(s) failed.\n", failures);
    return 1;
}
