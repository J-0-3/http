#pragma once
#include <stddef.h>

int build_path(char* buf, size_t buf_size, unsigned int num_parts, ...);
const char* get_extension(const char* filename);