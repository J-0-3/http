#pragma once
#include <stddef.h>

typedef enum file_type {
    PNG,
    JPG,
    TXT,
    HTML,
    JS,
    CSS,
    SVG,
    MPEG4,
    DATA
} file_type;

long get_filesize(const char* filename);
long read_file(const char* filename, char* buf, size_t buf_size);
file_type get_filetype(const char* filename);
int build_path(char* buf, size_t buf_size, unsigned int num_parts, ...);