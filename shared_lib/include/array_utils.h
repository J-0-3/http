#pragma once
#include <stddef.h>

int any(const void* values, size_t val_size, int num_values, int(*predicate)(const void*, const void*), const void* arg);
int any_str(char** values, int num_values, int(*predicate)(const char*, const char*), const char* arg);
int all(const void* values, size_t val_size, int num_values, int(*predicate)(const void*, const void*), const void* arg);
int all_str(char** values, int num_values, int(*predicate)(const char*, const char*), const char* arg);
