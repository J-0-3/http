#include "trim_whitespace.h"
#include <string.h>
#include <assert.h>

int main(int argc, char** argv) {
    char to_trim[256] = " Preceding whitespace";
    char* trimmed = trim_whitespace(to_trim);
    assert(strcmp(trimmed, "Preceding whitespace") == 0);

    strcpy(to_trim, "Trailing whitespace  ");
    trimmed = trim_whitespace(to_trim);
    assert(strcmp(trimmed, "Trailing whitespace") == 0);

    strcpy(to_trim, "  Surrounding whitespace     ");
    trimmed = trim_whitespace(to_trim);
    assert(strcmp(trimmed, "Surrounding whitespace") == 0);

    strcpy(to_trim, "No whitespace");
    trimmed = trim_whitespace(to_trim);
    assert(strcmp(trimmed, "No whitespace") == 0);

    strcpy(to_trim, "       ");
    trimmed = trim_whitespace(to_trim);
    assert(strlen(trimmed) == 0);

    strcpy(to_trim, "");
    trimmed = trim_whitespace(to_trim);
    assert(strlen(trimmed) == 0);
}