/*
 * cluster_find.c — OpenRefine-style clustering for sidecar data.
 * See tools/cluster_find.c for full implementation.
 * This is a placeholder that demonstrates the API.
 */
#include <stdio.h>
int main(int argc, char *argv[]) {
    fprintf(stderr, "cluster_find: use the full implementation in tools/cluster_find.c\n");
    fprintf(stderr, "Usage: %s sidecar.json [type_filter] [--levenshtein N]\n",
            argc > 0 ? argv[0] : "cluster_find");
    return 1;
}
