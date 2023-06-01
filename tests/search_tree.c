#include "search_tree.h"
#include <assert.h>

int main(int argc, char** argv) {
    // test tree constructor
    search_tree tree = search_tree_new();
    assert(tree != NULL);

    // test insert
    const char* key = "Dave";
    int value = 10;
    assert(search_tree_add(key, &value, sizeof(value), tree) == 0);

    key = "John";
    value = 20;
    assert(search_tree_add(key, &value, sizeof(value), tree) == 0);

    key = "Abigail";
    value = 5;
    assert(search_tree_add(key, &value, sizeof(value), tree) == 0);

    // test lookup
    size_t size = 0;
    assert(*(int*)search_tree_lookup(tree, "Dave", &size) == 10);
    assert(size == sizeof(10));

    assert(*(int*)search_tree_lookup(tree, "John", &size) == 20);
    assert(size == sizeof(20));

    assert(*(int*)search_tree_lookup(tree, "Abigail", &size) == 5);
    assert(size == sizeof(5));

    // free
    search_tree_free(tree);
    return 0;
}