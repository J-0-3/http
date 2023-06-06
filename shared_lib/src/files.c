#include "files.h"
#include <stdio.h>
#include <sys/stat.h>

long get_filesize(const char* filename) {
    struct stat info;
    if (stat(filename, &info) < 0) { // file doesn't exist
        return -1;
    }
    if (!S_ISREG(info.st_mode)) { // file is a directory
        return -1;
    }
    if (info.st_size < 0) {
        return -1;
    }
    else {
        return info.st_size;
    }
}

long read_file(const char* filename, char* buf, size_t buf_size) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }
    size_t read = fread(buf, 1, buf_size, file);
    if (read <= 0) {
        fclose(file);
        return -1;
    }
    fclose(file);
    return read;
}
