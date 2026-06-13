#include <ftw.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_TOKEN_LENGTH 2000

/* Global flag: when non-zero, emit JSON Lines instead of custom format */
extern int g_json_mode;

/* Output file handles — opened once in main() for performance */
extern FILE *g_out_tok;
extern FILE *g_out_json;

int scanner(const char *in, int length,char filename[],uint32_t filenpath_id,uint32_t parent_id,int prefix_len,int suffix_len);
uint32_t murmur3_seeded_v2(uint32_t seed, const char *data, size_t len);
