#include "paths.h"
#include <stdarg.h>
#include <string.h>

int build_path(char* buf, size_t buf_size, unsigned int num_parts, ...) {
    va_list parts;
    va_start(parts, num_parts);
    size_t req_size = num_parts -1 + 1;
    memset(buf, 0, buf_size);
    for (int i = 0; i < num_parts; i++) {
        const char* part = va_arg(parts, const char*);
        req_size += strlen(part);
        if (buf_size < req_size) {
            return -1;
        }
        if (i > 0) {
            strcat(buf, "/");
        }
        strcat(buf, part);
    }
    va_end(parts);
    return 0;
}

// returns pointer *into* filename
const char* get_extension(const char* filename) {
    const char* extension = filename + strlen(filename); // by default empty string (point to terminator)
    int c;
    for (c = 0; c < strlen(filename); c++) {
        if (filename[c] == '.') {
            extension = filename + c + 1;
        }
    }
    return extension;
}