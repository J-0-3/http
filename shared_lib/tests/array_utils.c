#include "array_utils.h"
#include <assert.h>
#include <string.h>

static int greater_than(const void* val, const void* cmp) {
    return *(int*)val > *(int*)cmp;
}

int main(int argc, char** argv) {
    int vals[16] = {5, 3, 7, 1, 9, 8, 1, 4, 3, 6, 3, 6, 2, 2, 1, 8};
    int v = 5;
    assert(any(vals, sizeof(int), 16, greater_than, &v));
    assert(!all(vals, sizeof(int), 16, greater_than, &v));
    v = 10;
    assert(!any(vals, sizeof(int), 16, greater_than, &v));
    assert(!all(vals, sizeof(int), 16, greater_than, &v));
    v = 0;
    assert(any(vals, sizeof(int), 16, greater_than, &v));
    assert(all(vals, sizeof(int), 16, greater_than, &v));

    char* names[5] = {"Dave", "John", "Emily", "Lucy", "Tyrone"};
    assert(!all_str(names, 5, strcmp, "John")); // strcmp returns 0 on match
    assert(all_str(names, 5, strcmp, "Clara"));
}
