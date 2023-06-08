#include "array_utils.h"
#include <assert.h>
#include <string.h>

static bool greater_than(const int* val, const int* cmp) {
    return *val > *cmp;
}

int main(int argc, char** argv) {
    int vals[16] = {5, 3, 7, 1, 9, 8, 1, 4, 3, 6, 3, 6, 2, 2, 1, 8};
    int v = 5;
    assert(any(vals, 16, greater_than, &v));
    assert(!all(vals, 16, greater_than, &v));
    v = 10;
    assert(!any(vals, 16, greater_than, &v));
    assert(!all(vals, 16, greater_than, &v));
    v = 0;
    assert(any(vals, 16, greater_than, &v));
    assert(all(vals, 16, greater_than, &v));

    char* names[5] = {"Dave", "John", "Emily", "Lucy", "Tyrone"};
    assert(!all_str(names, 5, strcmp, "John")); // strcmp returns 0 on match
    assert(all_str(names, 5, strcmp, "Clara"));
}