#include "file_mmap.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

int file_create_or_append(const char *filename, const char *data, size_t data_len) {
    int fd;
    int is_new_file = 0;

    /* Try to open existing file */
    fd = open(filename, O_RDWR, 0644);
    
    if (fd == -1) {
        /* File doesn't exist, create it */
        fd = open(filename, O_RDWR | O_CREAT, 0644);
        if (fd == -1) {
            return -1;
        }
        is_new_file = 1;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        close(fd);
        return -1;
    }

    off_t file_size = sb.st_size;

    /* For new/empty files, just write directly */
    if (is_new_file || file_size == 0) {
        if (write(fd, data, data_len) != (ssize_t)data_len) {
            close(fd);
            return -1;
        }
        close(fd);
        return 0;
    }

    /* Existing file: mmap, append, then unmap */
    if (ftruncate(fd, file_size + data_len) == -1) {
        close(fd);
        return -1;
    }

    char *mapped = mmap(NULL, file_size + data_len, 
                        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        close(fd);
        return -1;
    }

    /* Append data at the end */
    for (size_t i = 0; i < data_len; i++) {
        mapped[file_size + i] = data[i];
    }

    if (munmap(mapped, file_size + data_len) == -1) {
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int file_write_string(const char *filename, const char *str) {
    size_t len = 0;
    if (str != NULL) {
        len = strlen(str);
        /* Do NOT include null terminator - just the string content */
    }
    return file_create_or_append(filename, str, len);
}