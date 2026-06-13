#define _XOPEN_SOURCE 500 // Required for nftw in some systems

#include "globals.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <math.h>
#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define INITIAL_ARRAY_SIZE 1024
#define ARRAY_RESIZE_FACTOR 2

struct MatchingFiles {
  char **filepaths;
  int count;
  int capacity;
};

struct MatchingFiles matching_files;
regex_t regex;

static int g_single_file_mode = 0;

int process_path(const char *filepath, const struct stat *sb, int typeflag,
                 struct FTW *ftwbuf) {
  if (typeflag == FTW_F) {
    if (regexec(&regex, filepath, 0, NULL, 0) == 0) {
      if (matching_files.count >= matching_files.capacity) {
        matching_files.capacity *= ARRAY_RESIZE_FACTOR;
        matching_files.filepaths = realloc(
            matching_files.filepaths, matching_files.capacity * sizeof(char *));
        if (!matching_files.filepaths) {
          perror("Memory allocation error");
          exit(EXIT_FAILURE);
        }
      }
      matching_files.filepaths[matching_files.count] = realpath(filepath, NULL);
      if (!matching_files.filepaths[matching_files.count]) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
      }
      matching_files.count++;
    }
  }
  return 0;
}

void free_matching_files_array() {
  for (int i = 0; i < matching_files.count; i++) {
    free(matching_files.filepaths[i]);
  }
  free(matching_files.filepaths);
  matching_files.filepaths = NULL;
  matching_files.count = 0;
  matching_files.capacity = 0;
}

static void scan_file(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("EXIT FAILURE %s", path);
        return;
    }
    struct stat s;
    fstat(fd, &s);
    char *buff = mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (buff != (void *)-1) {
        uint32_t filepath_id = murmur3_seeded_v2(0, path, strlen(path));
        scanner((char *)buff, s.st_size, (char *)path,
                filepath_id, filepath_id, 0, 0);
        close(fd);
    }
}

static void open_output_files(void) {
    g_out_tok = fopen("sidecar.tok", "w");
    g_out_json = fopen("sidecar.json", "w");
}

static void close_output_files(void) {
    if (g_out_tok) { fclose(g_out_tok); g_out_tok = NULL; }
    if (g_out_json) { fclose(g_out_json); g_out_json = NULL; }
}

static void print_usage(const char *prog) {
    fprintf(stderr,
        "Usage:\n"
        "  %s [--json] <file.tex> <regex>     # Single file, regex match on filename\n"
        "  %s [--json] <directory> <regex>    # Directory traversal with regex\n"
        "\nOptions:\n"
        "  --json   Output JSON Lines to sidecar.json instead of custom format\n"
        "\nExamples:\n"
        "  %s paper.tex tex                    # Tokenize single file\n"
        "  %s --json paper.tex tex             # JSON output\n"
        "  %s /path/to/texfiles '.*\\\\.tex$'    # Process directory\n",
        prog, prog, prog, prog, prog);
}

int main(int argc, char *argv[]) {
    int arg_idx = 1;

    /* Parse --json flag */
    if (argc >= 2 && !strcmp(argv[1], "--json")) {
        g_json_mode = 1;
        arg_idx++;
    }

    int remaining = argc - arg_idx;

    if (remaining != 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    char *path_or_dir = argv[arg_idx];
    char *regex_pattern = argv[arg_idx + 1];

    /* Check if first argument is a regular file (single-file mode) */
    struct stat path_stat;
    if (stat(path_or_dir, &path_stat) == 0 && S_ISREG(path_stat.st_mode)) {
        /* Single file mode: use regex to match the filename (usually just "tex") */
        regex_t file_regex;
        int rc = regcomp(&file_regex, regex_pattern, REG_EXTENDED);
        if (rc) {
            char errbuf[512];
            regerror(rc, &file_regex, errbuf, sizeof(errbuf));
            fprintf(stderr, "Regex compilation error: %s\n", errbuf);
            return EXIT_FAILURE;
        }
        if (regexec(&file_regex, path_or_dir, 0, NULL, 0) == 0) {
            open_output_files();
            scan_file(path_or_dir);
            close_output_files();
        }
        regfree(&file_regex);
        return EXIT_SUCCESS;
    }

    /* Directory mode */
    int regex_comp_result = regcomp(&regex, regex_pattern, REG_EXTENDED);
    if (regex_comp_result) {
        char error_buffer[512];
        regerror(regex_comp_result, &regex, error_buffer, sizeof(error_buffer));
        fprintf(stderr, "Regex compilation error: %s\n", error_buffer);
        return EXIT_FAILURE;
    }

    matching_files.capacity = INITIAL_ARRAY_SIZE;
    matching_files.count = 0;
    matching_files.filepaths = malloc(matching_files.capacity * sizeof(char *));
    if (!matching_files.filepaths) {
        perror("Memory allocation error");
        regfree(&regex);
        return EXIT_FAILURE;
    }

    if (nftw(path_or_dir, process_path, 20, FTW_PHYS) == -1) {
        perror("nftw");
        regfree(&regex);
        free_matching_files_array();
        return EXIT_FAILURE;
    }

    open_output_files();
    for (size_t i = 0; i < matching_files.count; i++) {
        scan_file(matching_files.filepaths[i]);
    }
    close_output_files();

    free_matching_files_array();
    regfree(&regex);

    return EXIT_SUCCESS;
}
