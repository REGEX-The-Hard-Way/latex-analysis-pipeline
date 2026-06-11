#include <ftw.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_TOKEN_LENGTH 2000

int scanner(const char *in, int length,char filename[],uint32_t filenpath_id,uint32_t parent_id,int prefix_len,int suffix_len);
uint32_t murmur3_seeded_v2(uint32_t seed, const char *data, size_t len);
