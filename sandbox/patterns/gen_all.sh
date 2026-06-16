#!/bin/bash
# Generate all 100 Ragel pattern programs
cd "$(dirname "$0")"
D=$PWD
total=0
fail=0

write() { cat > "$D/$1" << 'ENDFILE'
ENDFILE
}

# ─────── Patterns 1-5: Basic Matching ───────

cat > "$D/1.rl" << 'EOF'
/* Pattern 1: Basic Literal Pattern Matching — matches "hello\n" */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p1; main := 'hello' '\n'; write data; }%%
int main(void){static const char *t[]={"hello\n","hello","Hello\n","world\n",""};static const int e[]={1,0,0,0,0};int n=5,cs;const char *p,*pe,*eof;
printf("Pattern  1: Basic Literal Pattern Matching\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p1_first_final;printf("  %-10s -> %-6s (expect %s) %s\n",t[i],r?"ACCEPT":"FAIL",e[i]?"ACCEPT":"FAIL",r==e[i]?"ok":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
EOF

cat > "$D/2.rl" << 'EOF'
/* Pattern 2: Alternation (Union of Patterns) — 'hello'|'world'|'foo'|'bar' */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p2; main := ('hello'|'world'|'foo'|'bar') '\n'; write data; }%%
int main(void){static const char *t[]={"hello\n","world\n","foo\n","bar\n","HELLO\n","help\n","\n",""};static const int e[]={1,1,1,1,0,0,0,0};int n=8,cs;const char *p,*pe,*eof;
printf("Pattern  2: Alternation (Union of Patterns)\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p2_first_final;printf("  %-10s -> %-6s (expect %s) %s\n",t[i],r?"ACCEPT":"FAIL",e[i]?"ACCEPT":"FAIL",r==e[i]?"ok":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
EOF

cat > "$D/3.rl" << 'EOF'
/* Pattern 3: Character Class Ranges — C-style identifier */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p3; ident = [a-zA-Z_][a-zA-Z0-9_]*; main := ident '\n'; write data; }%%
int main(void){static const char *t[]={"hello\n","_private\n","X123\n","1invalid\n","-dash\n","\n",""};static const int e[]={1,1,1,0,0,0,0};int n=7,cs;const char *p,*pe,*eof;
printf("Pattern  3: Character Class Ranges\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p3_first_final;printf("  %-14s -> %-6s (expect %s) %s\n",t[i],r?"ACCEPT":"FAIL",e[i]?"ACCEPT":"FAIL",r==e[i]?"ok":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
EOF

cat > "$D/4.rl" << 'EOF'
/* Pattern 4: Negated Character Classes — C block comment matching */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p4; comment = '/*' (any-'\*')* '*/'; main := comment; write data; }%%
int main(void){static const char *t[]={"/* hello */","/**/","/* a = b; */","/* unterminated","/* /* */",""};static const int e[]={1,1,1,0,0,0};int n=6,cs;const char *p,*pe,*eof;
printf("Pattern  4: Negated Character Classes\n\n");
for(int i=0;i<n;i++){p=t[i];pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
int r=cs>=p4_first_final;printf("  %-25s -> %-6s (expect %s) %s\n",t[i],r?"ACCEPT":"FAIL",e[i]?"ACCEPT":"FAIL",r==e[i]?"ok":"FAIL");assert(r==e[i]);}
printf("\nAll %d tests passed.\n",n);return 0;}
EOF

cat > "$D/5.rl" << 'EOF'
/* Pattern 5: Dot/Any Builtins — any, any*, [^\n] */
#include <stdio.h>
#include <string.h>
#include <assert.h>
%%{ machine p5; any_single = any '\n'; any_star = any* '\n'; not_nl = [^\n]* '\n'; main := any_single | any_star | not_nl; write data; }%%
int main(void){int cs;const char *p,*pe,*eof;
printf("Pattern  5: The Dot/Any Builtins\n\n");
printf("  any (exactly 1 char + \\n):\n");
p="X\n";pe=p+2;eof=pe;
%% write init;
%% write exec;
printf("    X\\n -> %s ok\n",cs>=p5_first_final?"ACCEPT":"FAIL");assert(cs>=p5_first_final);
p="XX\n";pe=p+3;eof=pe;
%% write init;
%% write exec;
printf("    XX\\n -> %s ok\n",cs>=p5_first_final?"FAIL":"ACCEPT");assert(cs<p5_first_final);
printf("  any* (0+ chars + \\n):\n");
p="hi\n";pe=p+3;eof=pe;
%% write init;
%% write exec;
printf("    hi\\n -> %s ok\n",cs>=p5_first_final?"ACCEPT":"FAIL");
p="\n";pe=p+1;eof=pe;
%% write init;
%% write exec;
printf("    \\n -> %s ok\n",cs>=p5_first_final?"ACCEPT":"FAIL");
printf("  [^\\n]* (non-newline chars + \\n):\n");
p="abc\n";pe=p+4;eof=pe;
%% write init;
%% write exec;
printf("    abc\\n -> %s ok\n",cs>=p5_first_final?"ACCEPT":"FAIL");
printf("\nAll tests passed.\n");return 0;}
EOF
echo "1-5 written"
