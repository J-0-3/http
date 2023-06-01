#include "trim_whitespace.h"
#include <string.h>

// trim whitespace from string. modifies original string and returns a pointer into it like strtok.
char* trim_whitespace(char* string) {
    char* start_ptr = string;
    int length = strlen(string);
    if (length == 0) {
        return string;
    }
    while(*start_ptr == ' ' && (start_ptr - string) < length) {
        start_ptr++;
    }
    while(string[length - 1] == ' ' && length > 0) {
        length--;
    }
    if (length < strlen(string)) {
        string[length] = '\0';
    }
    return start_ptr;
}
