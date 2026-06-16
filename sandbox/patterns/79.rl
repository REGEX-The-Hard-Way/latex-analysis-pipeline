/* Pattern 79: RPN Calculator — digit+ push, operators pop-push, print stack */
#include <stdio.h>
#include <string.h>
#include <assert.h>

int stack[32];
int sp = 0;
const char *num_start;

void push(int v) { stack[sp++] = v; }
int pop() { return stack[--sp]; }

%%{ machine p79;

action mark     { num_start = p; }
action push_num {
    int v = 0;
    const char *c = num_start;
    while (c < p && *c >= '0' && *c <= '9') { v = v*10 + (*c-'0'); c++; }
    push(v);
    printf("push %d\n", v);
}
action add_op   { int b=pop(); int a=pop(); push(a+b); printf("add -> %d\n", a+b); }
action sub_op   { int b=pop(); int a=pop(); push(a-b); printf("sub -> %d\n", a-b); }
action mul_op   { int b=pop(); int a=pop(); push(a*b); printf("mul -> %d\n", a*b); }
action div_op   { int b=pop(); int a=pop(); push(a/b); printf("div -> %d\n", a/b); }

stuff = digit+ >mark %push_num
      | '+' @add_op
      | '-' @sub_op
      | '*' @mul_op
      | '/' @div_op
      ;

main := ( space | stuff space )*;

write data;
}%%
int main(void){int cs;const char *p,*pe,*eof;
printf("Pattern 79: RPN Calculator\n\n");
const char *expr = "3 4 + 2 * 5 /\n";
p=expr;pe=p+strlen(p);eof=pe;
%% write init;
%% write exec;
printf("Stack top = %d\n", stack[sp-1]);
printf("-> %s ok\n",cs>=p79_first_final?"ACCEPT":"FAIL");
assert(stack[sp-1]==2);
printf("\nAll tests passed.\n");return 0;}
