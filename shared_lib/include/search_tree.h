#pragma once
#include <stddef.h>
#define SEARCH_TREE_DUPLICATE_ERR 1
#define SEARCH_TREE_NOT_FOUND_ERR 2

typedef struct search_tree_node search_tree_node;
struct search_tree_node {
    search_tree_node* left;
    search_tree_node* right;
    char* key;
    void* val;
    size_t val_size;
};
typedef search_tree_node** search_tree;

search_tree search_tree_new();
void search_tree_free(search_tree node);
int search_tree_add(const char* key, const void* val, size_t val_size, search_tree tree);
const void* search_tree_lookup(search_tree tree, const char* key, size_t* val_size);
search_tree search_tree_copy(search_tree);
void search_tree_foreach(search_tree tree, void(*callback)(const char*, const void*, size_t, void*), void* arg);