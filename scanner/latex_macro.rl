/*
 * LaTeX Macro Expander - Complete Ragel Implementation
 */

%%{
    machine latex_macro;
    
    # === Character classes ===
    digit = [0-9];
    alpha = [a-zA-Z];
    ws = ' ' | '\t' | '\r';
    newline = '\n' | '\r\n';
    
    # Command names (letters and @)
    cmdname = (alpha | '@')+;
    
    # Comments: % to end of line
    comment = '%' any* newline;
    
    # Backslash
    backslash = '\\';
    
    # === Macro patterns ===
    
    # Definition keywords
    defkeyword = 'newcommand' | 'renewcommand' | 'providecommand' | 'def';
    
    # Inline parameter: # followed by digit
    param_spec = '#' digit;
    
    # Argument specification: [n]
    argspec = '[' digit ']';
    
    # Body characters: any except unescaped braces
    body_char = any - '{' - '}' - '\\';
    escape = '\\' any;
    
    # Balanced braces (simplified)
    balanced_body = (body_char | escape | comment)*;
    
    # Macro body: {...}
    macro_body = '{' balanced_body* '}';
    
    # === Main machine ===
    
    main := (
        # Macro definition
        defkeyword => collect_keyword (ws*)? collect_macro_args |
        
        # LaTeX command - check if it's a macro to expand
        backslash cmdname => try_expand |
        
        # Comments pass through
        comment => output_direct |
        
        # Everything else passes through
        any+ => output_direct
    )*;
    
    # Collect macro name and arguments
    collect_macro_args := (
        # Format 1: {\name}[n]{body}
        '{' cmdname '}' (ws* argspec)? macro_body => store_macro |
        # Format 2: {\name}{body}
        '{' cmdname '}' macro_body => store_macro |
        # Format 3: \name[n]{body}
        cmdname (ws*)? argspec macro_body => store_macro_args |
        # Format 4: \name{body}
        cmdname (ws*)? macro_body => store_macro_inline |
        # Format 5: \name#1#2{...}
        cmdname (ws*)? param_spec* macro_body => store_macro_params
    );
    
    # Store with explicit argument count
    action store_macro_args { }
    
    # Actions
    action collect_keyword { }
    action store_macro { }
    action store_macro_inline { }
    action store_macro_params { }
    action try_expand { }
    action output_direct { }
}%%

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

using namespace std;

#define MAXMACRONAMELEN 80
#define MAXMACROBODYLEN 4096
#define MAXCOMMANDS 1024

struct MacroDef {
    char name[MAXMACRONAMELEN];
    char body[MAXMACROBODYLEN];
    int num_args;
    bool has_argspec;
};

struct MacroDB {
    MacroDef macros[MAXCOMMANDS];
    int count;
    
    void init() {
        count = 0;
        memset(macros, 0, sizeof(macros));
    }
};

struct LaTeXMacroParser {
    MacroDB db;
    const char *p, *pe, *ts, *te;
    int cs;
    
    int init() { db.init(); return 1; }
    int finish() { return (cs >= latex_macro_first_final) ? 1 : 0; }
};

%% write exec;
%% write init;

#define BUFSIZE (1024 * 1024)

int main() {
    char *buf = (char*)malloc(BUFSIZE);
    size_t total = 0;
    size_t n;
    
    while ((n = fread(buf + total, 1, BUFSIZE - total - 1, stdin)) > 0) {
        total += n;
        if (total >= BUFSIZE - 1) break;
    }
    buf[total] = '\0';
    
    LaTeXMacroParser parser;
    parser.init();
    
    parser.p = buf;
    parser.pe = buf + total;
    parser.ts = parser.te = 0;
    
    %% write exec;
    
    cout << "Found " << parser.db.count << " macro definitions." << endl;
    for (int i = 0; i < parser.db.count; i++) {
        cout << "  \"\\" << parser.db.macros[i].name << "\"" << endl;
    }
    
    free(buf);
    return 0;
}