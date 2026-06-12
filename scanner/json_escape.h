/*
 * json_escape.h — Simple JSON string escaping utility.
 *
 * json_escape(dst, dst_size, src, src_len):
 *   Writes a JSON-escaped version of `src` (length `src_len`) into `dst`
 *   (size `dst_size`).  Escapes: " \ / \b \f \n \r \t and control chars.
 *   Returns the number of bytes written (excluding NUL terminator), or -1
 *   if the buffer was too small.
 */
#ifndef JSON_ESCAPE_H
#define JSON_ESCAPE_H

#include <stddef.h>

int json_escape(char *dst, size_t dst_size, const char *src, size_t src_len);

#endif
