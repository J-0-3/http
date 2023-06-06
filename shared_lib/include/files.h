#pragma once
#include <stddef.h>

long get_filesize(const char* filename);
long read_file(const char* filename, char* buf, size_t buf_size);