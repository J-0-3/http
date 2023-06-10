#include "array_utils.h"
#include <string.h>

/// @brief test whether a predicate is 1 for 1 or more values in an array
/// @param values an array of values
/// @param num_values the length of *values*
/// @param predicate a int function which takes a pointer to the current value and the value of *arg* 
/// @param arg an optional extra argument to supply to the predicate function
/// @retval 1
/// @retval 0
int any(const void* values, size_t val_size, int num_values, int(*predicate)(const void*, const void*), const void* arg) {
    for (int i = 0; i < num_values; i++) {
        if (predicate(values + val_size * i, arg)) {
            return 1;
        }
    }
    return 0;
}

/// @brief test whether a predicate is 1 for 1 or more strings in an array (should be preferred over any for an array of strings)
/// @param values an array of strings
/// @param num_values the length of *values*
/// @param predicate a int function which takes the current string and the value of <arg>
/// @param arg an optional extra argument to supply to the predicate function
/// @retval 1
/// @retval 0
int any_str(char** values, int num_values, int(*predicate)(const char*, const char*), const char* arg) {
    for (int i = 0; i < num_values; i++) {
        if (predicate(values[i], arg)) {
            return 1;
        }
    }
    return 0;
}

/// @brief test whether a predicate is 1 for all values in an array
/// @param values an array of values
/// @param num_values the length of *values*
/// @param predicate a int function which takes a pointer to the current value and the value of arg
/// @param arg an optional extra argument to supply to the predicate function
/// @retval 1
/// @retval 0
int all(const void* values, size_t val_size, int num_values, int(*predicate)(const void*, const void*), const void* arg) {
    for (int i = 0; i < num_values; i++) {
        if (!predicate(values + val_size * i, arg)) {
            return 0;
        }
    }
    return 1;
}

/// @brief test whether a predicate is 1 for all strings in an array (should be preferred over all for an array of string)
/// @param values an array of strings
/// @param num_values the length of *values*
/// @param predicate a int function which takes the current string and the value of arg
/// @param arg an optional extra argument to supply to the predicate function
/// @retval 1
/// @retval 0
int all_str(char** values, int num_values, int(*predicate)(const char*, const char*), const char* arg) {
    for (int i = 0; i < num_values; i++) {
        if (!predicate(values[i], arg)) {
            return 0;
        }
    }
    return 1;
}
