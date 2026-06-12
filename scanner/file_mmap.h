#ifndef FILE_MMAP_H
#define FILE_MMAP_H

#include <stddef.h>

/**
 * Creates a file if it doesn't exist, or memory-maps and appends to it if it does.
 * 
 * @param filename The path to the file to operate on
 * @param data The data to write (new file) or append (existing file)
 * @param data_len The length of the data in bytes
 * @return 0 on success, -1 on error (check errno for details)
 */
int file_create_or_append(const char *filename, const char *data, size_t data_len);

/**
 * Convenience wrapper for string data.
 * Appends the string (including null terminator) to existing file,
 * or creates file with the string content.
 * 
 * @param filename The path to the file
 * @param str The string to write/append
 * @return 0 on success, -1 on error
 */
int file_write_string(const char *filename, const char *str);

#endif /* FILE_MMAP_H */