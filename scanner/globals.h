#include <ftw.h>
#include <stdio.h>  /* printf */
#include <stdlib.h> /* atoi, malloc */
#include <string.h> /* strcpy */

#define _XOPEN_SOURCE 500 // Required for nftw in some systems
#include <errno.h>
#include <ftw.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define MAX_TOKEN_LENGTH 2000

int scanner(const char *in, int length,char filename[],uint32_t filenpath_id,uint32_t parent_id,int prefix_len,int suffix_len);
uint32_t murmur3_seeded_v2(uint32_t seed, const char *data, size_t len);
