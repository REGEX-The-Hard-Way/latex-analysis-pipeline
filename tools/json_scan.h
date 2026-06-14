/* json_scan.h — Ragel JSON scanner API */
#ifndef JSON_SCAN_H
#define JSON_SCAN_H
#include <stddef.h>

typedef void (*json_field_cb)(const char *key, int klen,
                               const char *val, int vlen,
                               int val_type, void *user);

int json_scan(const char *data, size_t len, json_field_cb cb, void *user);
int json_scan_file(const char *filename, json_field_cb cb, void *user);

#endif
