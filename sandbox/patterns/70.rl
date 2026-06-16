/* Pattern 70: Buffer Streaming — shift+refill buffer size 16 */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define BUFSIZE 16
char buffer[BUFSIZE];
int have = 0;

%%{ machine p70;

action show { printf("TOKEN(%c) ", fc); }

main := ( [a-zA-Z]+ @show )*;

write data;
}%%

int main(void){int cs;const char *p,*pe,*eof;
const char *input = "abcdefghijklmnopqrstuvwxyz";
int input_len = strlen(input);
int offset = 0;

printf("Pattern 70: Buffer Streaming (BS=%d)\n\n", BUFSIZE);

%% write init;

while (offset < input_len) {
    if (have > 0) {
        memmove(buffer, p, have);
    }
    int space = BUFSIZE - have;
    int chunk = input_len - offset;
    if (chunk > space) chunk = space;
    memcpy(buffer + have, input + offset, chunk);
    offset += chunk;
    int len = have + chunk;

    p = buffer;
    pe = buffer + len;
    eof = (offset >= input_len) ? pe : NULL;

    %% write exec;

    have = pe - p;
    printf("chunk=%d have=%d\n", chunk, have);
}

if (cs >= p70_first_final)
    printf("\nACCEPT\n");
else
    printf("\nFAIL\n");

assert(cs >= p70_first_final);
printf("\nAll tests passed.\n");return 0;}
