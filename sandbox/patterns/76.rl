/* Pattern 76: Mbox Parser — From_ line + header + body */
#include <stdio.h>
#include <string.h>
#include <assert.h>

int msg_count = 0;

%%{ machine p76;

action msg_start  { msg_count++; printf("MSG_%d\n", msg_count); }

day    = /[A-Z][a-z][a-z]/;
month  = /[A-Z][a-z][a-z]/;
dayNum = /[0-9 ][0-9]/;
time   = /[0-9][0-9]:[0-9][0-9]/ . ( /:[0-9][0-9]/ | '' );
zone   = /[A-Z][A-Z][A-Z]/ | /[+\-][0-9][0-9][0-9][0-9]/;
year   = /[0-9][0-9][0-9][0-9]/;

date   = day ' ' month ' ' dayNum ' ' time ' ' ( year | year ' ' zone | zone ' ' year );

fromLine = 'From ' [^\n]* ' ' date '\n' @msg_start;

action hdr     { printf("  HDR: %c", fc); }
action hdrEnd  { printf("\n"); }

hchar       = print - [\n:];
headerLine  = hchar+ $hdr ':' (any-'\n')* $hdr '\n' %hdrEnd;
bodyLine    = (any-'\n')* '\n';

message = fromLine headerLine* '\n' bodyLine* '\n';
main := message*;

write data;
}%%
int main(void){int cs;const char *p,*pe,*eof;
printf("Pattern 76: Mbox Parser\n\n");
const char *mbox =
"From sender@host.dom Wed Nov 28 13:30:05 2001\n"
"Subject: Hello\n"
"From: alice@wonderland.com\n"
"\n"
"Mail body here.\n"
"\n"
"From other@host.dom Thu Dec 02 09:15:00 EST 2001\n"
"\n"
"Second message.\n"
"\n";

p=mbox;pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
printf("-> %s ok\n",cs>=p76_first_final?"ACCEPT":"FAIL");
assert(cs>=p76_first_final);
printf("\nAll tests passed.\n");return 0;}
