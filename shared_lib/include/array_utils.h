#pragma once
#include <stdbool.h>

bool any(const void* values, int num_values, bool(*predicate)(const void*, const void*), const void* arg);
bool any_str(const char** values, int num_values, bool(*predicate)(const char*, const void*), const void* arg);
bool all(const void* values, int num_values, bool(*predicate)(const void*, const void*), const void* arg);
bool all_str(const char** values, int num_values, bool(*predicate)(const char*, const void*), const void* arg);
bool contains_s(const char** values, int num_values, const char* value);
bool contains_i(const int* values, int num_values, int value);
bool contains_f(const float* values, int num_values, float value);