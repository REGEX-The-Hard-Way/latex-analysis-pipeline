#!/usr/bin/env python3
"""Apply Phase 2+3 changes to scanner sources. Simple line-based approach."""

# === latex.rl: add citation patterns ===
with open('scanner/latex.rl', 'r') as f:
    lines = f.readlines()

# Find the line with "cite = " and expand from there
new_cite_lines = []
in_cite_block = False
for i, line in enumerate(lines):
    if line.startswith('cite = ') and 'braces|brackets braces| braces braces' in line:
        in_cite_block = True
        new_cite_lines.append(line)  # keep original cite line
        new_cite_lines.append('\n')
        new_cite_lines.append('/* natbib citation variants */\n')
        new_cite_lines.append("citep      = '\\\\citep' (braces|brackets braces| braces braces) ;\n")
        new_cite_lines.append("citet      = '\\\\citet' (braces|brackets braces| braces braces) ;\n")
        new_cite_lines.append("citeauthor = '\\\\citeauthor' (braces|brackets braces) ;\n")
        new_cite_lines.append("citeyear   = '\\\\citeyear' (braces|brackets braces) ;\n")
        new_cite_lines.append("citealp    = '\\\\citealp' (braces|brackets braces| braces braces) ;\n")
        new_cite_lines.append('\n')
        new_cite_lines.append('/* biblatex citation variants */\n')
        new_cite_lines.append("autocite  = '\\\\autocite' (braces|brackets braces| braces braces) ;\n")
        new_cite_lines.append("textcite  = '\\\\textcite' (braces|brackets braces| braces braces) ;\n")
        new_cite_lines.append("parencite = '\\\\parencite' (braces|brackets braces| braces braces) ;\n")
        new_cite_lines.append("footcite  = '\\\\footcite' (braces|brackets braces) ;\n")
        new_cite_lines.append('\n')
        new_cite_lines.append('/* \\nocite and \\ref */\n')
        new_cite_lines.append("nocite = '\\\\nocite' braces ;\n")
        continue
    if in_cite_block and line.startswith('ref = '):
        new_cite_lines.append("ref = '\\\\ref' braces;\n")
        new_cite_lines.append('\n')
        new_cite_lines.append('/* \\bibitem with optional argument */\n')
        new_cite_lines.append("bibitem_opt = 'bibitem' brackets braces ;\n")
        continue
    if in_cite_block and line.startswith('bibitem_start = '):
        new_cite_lines.append("bibitem_start = 'bibitem' ;\n")
        continue
    if in_cite_block and line.startswith('bibitem_body = '):
        new_cite_lines.append("bibitem_body = any+ - bibitem_start;\n")
        continue
    if in_cite_block and line.startswith('bibitem = '):
        new_cite_lines.append("bibitem = (bibitem_opt | bibitem_start braces) (bibitem_body)* :>> '\\n\\\\' ;\n")
        in_cite_block = False
        continue
    if not in_cite_block:
        new_cite_lines.append(line)

# Add math sub-structure before inline_math
final_lines = []
math_added = False
for line in new_cite_lines:
    if not math_added and 'inline_math = ' in line and '"$"' in line:
        final_lines.append('/* Math sub-structure */\n')
        final_lines.append("subscript   = '_' (braces | alpha | digit) ;\n")
        final_lines.append("superscript = '^' (braces | alpha | digit) ;\n")
        final_lines.append("left_right  = '\\\\left' any* '\\\\right' any ;\n")
        final_lines.append("math_text = ('\\\\text' | '\\\\mathrm' | '\\\\mathbf' | '\\\\mathit' | '\\\\mathsf' | '\\\\mathtt') braces ;\n")
        final_lines.append('\n')
        math_added = True
    final_lines.append(line)

with open('scanner/latex.rl', 'w') as f:
    f.writelines(final_lines)
print('latex.rl: updated')

# === scanner.rl: add JSON includes, macros, and dispatch ===
with open('scanner/scanner.rl', 'r') as f:
    slines = f.readlines()

result = []
json_header_done = False
emit_macro_done = False
cites_added = False
math_added2 = False

for i, line in enumerate(slines):
    # Add json_escape include and g_json_mode after file_mmap.h include
    if not json_header_done and line.startswith('#include "file_mmap.h"'):
        result.append(line)
        result.append('#include "json_escape.h"\n')
        json_header_done = True
        continue
    if not json_header_done and 'uint32_t seed0 = 0;' in line:
        result.append(line)
        result.append('int g_json_mode = 0;  /* 0=custom, 1=JSON Lines */\n')
        json_header_done = True  # mark done here too
        continue

    # Add JSON-aware emit macros (skip old EMIT_BLOCK, replace with JSON version)
    if not emit_macro_done and '#define EMIT_BLOCK(t,prefix_len,suffix_len)' in line and line.strip().endswith('\\'):
        # Skip old multi-line EMIT_BLOCK macro
        while i < len(slines) and (slines[i].strip().endswith('\\') or '  }' not in slines[i]):
            i += 1
        i += 1  # skip the closing line
        # Insert JSON-aware macros
        result.append('\n')
        result.append('/* JSON-aware emit: writes sidecar.json or sidecar.tok */\n')
        result.append('#define EMIT_JSON_ORIG(t,prefix_len,suffix_len) do { \\\n')
        result.append('  uint32_t token_id = murmur3_seeded_v2(seed0, &in[ts - in], te - ts); \\\n')
        result.append('  int tok_len = (int)(te - ts); \\\n')
        result.append('  int off = prefix_len + (int)(ts - in); \\\n')
        result.append('  if (g_json_mode) { \\\n')
        result.append('    char escaped[tok_len*6 + 100]; \\\n')
        result.append('    int esc_len = json_escape(escaped, sizeof(escaped), &in[ts - in], tok_len); \\\n')
        result.append('    if (esc_len < 0) esc_len = 0; \\\n')
        result.append('    char emit_buf[esc_len + 512]; \\\n')
        result.append('    int emit_len = snprintf(emit_buf, sizeof(emit_buf), \\\n')
        result.append('      "{\\"filepath\\":\\"%s\\",\\"filepath_id\\":%lu,\\"token_id\\":%lu," \\\n')
        result.append('      "\\"parent_id\\":%lu,\\"offset\\":%d,\\"length\\":%d,\\"type\\":\\"%s\\"," \\\n')
        result.append('      "\\"text\\":\\"%.*s\\"}\\n", \\\n')
        result.append('      filename, (unsigned long)filepath_id, (unsigned long)token_id, \\\n')
        result.append('      (unsigned long)parent_id, off, tok_len, t, esc_len, escaped); \\\n')
        result.append('    file_create_or_append("sidecar.json", emit_buf, emit_len); \\\n')
        result.append('  } else { \\\n')
        result.append('    char emit_buf[tok_len+250]; \\\n')
        result.append('    memset(emit_buf,0,sizeof(emit_buf)); \\\n')
        result.append('    int emit_len = snprintf(emit_buf, sizeof(emit_buf), \\\n')
        result.append('      "{<filepath:%s>,filepath_id:%lu,token_id:%lu,parent_id:%lu,offset:%d," \\\n')
        result.append('      "length:%d,type:%s,<tok:%.*s>}\\n", \\\n')
        result.append('      filename, (unsigned long)filepath_id, (unsigned long)token_id, \\\n')
        result.append('      (unsigned long)parent_id, off, tok_len, t, tok_len, &in[ts - in]); \\\n')
        result.append('    file_create_or_append("sidecar.tok", emit_buf, emit_len); \\\n')
        result.append('  } \\\n')
        result.append('  printf("\\n\\n  %u  \\n\\n", token_id); \\\n')
        result.append('  if((int)(te-(prefix_len+suffix_len)-ts)>0){ \\\n')
        result.append('    scanner(&in[ts + prefix_len - in], te - (prefix_len + suffix_len) - ts, \\\n')
        result.append('            filename, filepath_id, token_id, prefix_len, suffix_len); \\\n')
        result.append('  } \\\n')
        result.append('} while(0)\n')
        result.append('\n')
        result.append('#define EMIT_BLOCK(t,prefix_len,suffix_len) EMIT_JSON_ORIG(t,prefix_len,suffix_len)\n')
        result.append('/* Leaf token: no interior scanning */\n')
        result.append('#define EMIT(t) EMIT_JSON_ORIG(t,0,0)\n')
        emit_macro_done = True
        continue

    # Add citation dispatch entries
    if not cites_added and 'cite               => { EMIT("cite"); };' in line:
        result.append(line)
        result.append('  citep              => { EMIT("citep"); };\n')
        result.append('  citet              => { EMIT("citet"); };\n')
        result.append('  citeauthor         => { EMIT("citeauthor"); };\n')
        result.append('  citeyear           => { EMIT("citeyear"); };\n')
        result.append('  citealp            => { EMIT("citealp"); };\n')
        result.append('  autocite           => { EMIT("autocite"); };\n')
        result.append('  textcite           => { EMIT("textcite"); };\n')
        result.append('  parencite          => { EMIT("parencite"); };\n')
        result.append('  footcite           => { EMIT("footcite"); };\n')
        result.append('  nocite             => { EMIT("nocite"); };\n')
        cites_added = True
        continue

    # Add math dispatch entries
    if not math_added2 and 'inline_math        => { EMIT("math"); };' in line:
        result.append(line)
        result.append('  subscript          => { EMIT("subscript"); };\n')
        result.append('  superscript        => { EMIT("superscript"); };\n')
        result.append('  left_right         => { EMIT("left_right"); };\n')
        result.append('  math_text          => { EMIT("math_text"); };\n')
        math_added2 = True
        continue

    result.append(line)

with open('scanner/scanner.rl', 'w') as f:
    f.writelines(result)
print('scanner.rl: updated')
print('Done.')
