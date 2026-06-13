/* graph_scan.h — Ragel mmap JSON direct query */
#ifndef GRAPH_SCAN_H
#define GRAPH_SCAN_H
#include "cypher_parser.h"

int cypher_scan_sidecar(const char *filename, const char *label,
                         const char *contains, int limit,
                         cypher_result_t *result);
#endif
