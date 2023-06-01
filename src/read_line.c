#include "read_line.h"
#include <unistd.h>
#include <string.h>
#include <malloc.h>

#define EOL "\r\n"

int safe_realloc(char** buf, size_t s) {
    char* new_buf = realloc(*buf, s);
    if (new_buf == NULL) {
        return -1;
    }
    *buf = new_buf;
    return 0;
}

int read_line(int fd, char** buf) {
    *buf = malloc(2);
    memset(*buf, 0, 2);
    int index = 1;
    if (read(fd, *buf, 1) <= 0) {
        free(*buf);
        return -2;
    }
    do {
        int n_read = read(fd, *buf + index, 1);
        if (n_read < 0) {
            free(*buf);
            return -2;
        }
        if (safe_realloc(buf, index + 2) < 0) {
            free(*buf);
            return -2;
        }
        (*buf)[index + 1] = '\x00';
        if (n_read == 0) {
            return -1;
        }
    } while (strncmp(*buf + index++ - 1, EOL, 2) != 0);
    if (safe_realloc(buf, index - 1) < 0) {
        free(*buf);
        return -2;
    }
    (*buf)[index - 2] = '\x00';
    return 0;
}