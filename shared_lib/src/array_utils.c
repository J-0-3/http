#include "array_utils.h"
#include <string.h>

/// @brief test whether a predicate is true for 1 or more values in an array
/// @param values an array of values
/// @param num_values the length of *values*
/// @param predicate a bool function which takes a pointer to the current value and the value of *arg* 
/// @param arg an optional extra argument to supply to the predicate function
/// @retval 1
/// @retval 0
bool any(const void* values, int num_values, bool(*predicate)(const void*, const void*), const void* arg) {
    for (int i = 0; i < num_values; i++) {
        if (predicate(&(values[i]), arg)) {
            return true;
        }
    }
    return false;
}

/// @brief test whether a predicate is true for 1 or more strings in an array (should be preferred over any for an array of strings)
/// @param values an array of strings
/// @param num_values the length of *values*
/// @param predicate a bool function which takes the current string and the value of <arg>
/// @param arg an optional extra argument to supply to the predicate function
/// @retval 1
/// @retval 0
bool any_str(const char** values, int num_values, bool(*predicate)(const char*, const void*), const void* arg) {
    for (int i = 0; i < num_values; i++) {
        if (predicate(values[i], arg)) {
            return true;
        }
    }
    return false;
}

/// @brief test whether a predicate is true for all values in an array
/// @param values an array of values
/// @param num_values the length of *values*
/// @param predicate a bool function which takes a pointer to the current value and the value of arg
/// @param arg an optional extra argument to supply to the predicate function
/// @retval 1
/// @retval 0
bool all(const void* values, int num_values, bool(*predicate)(const void*, const void*), const void* arg) {
    for (int i = 0; i < num_values; i++) {
        if (!predicate(&(values[i]), arg)) {
            return false;
        }
    }
    return true;
}

/// @brief test whether a predicate is true for all strings in an array (should be preferred over all for an array of string)
/// @param values an array of strings
/// @param num_values the length of *values*
/// @param predicate a bool function which takes the current string and the value of arg
/// @param arg an optional extra argument to supply to the predicate function
/// @retval 1
/// @retval 0
bool all_str(const char** values, int num_values, bool(*predicate)(const char*, const void*), const void* arg) {
    for (int i = 0; i < num_values; i++) {
        if (!predicate(values[i], arg)) {
            return false;
        }
    }
    return true;
}