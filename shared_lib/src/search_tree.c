#include "search_tree.h"
#include <stdbool.h>
#include <malloc.h>
#include <string.h>

search_tree search_tree_new() {
    search_tree_node** root_ptr = malloc(sizeof(search_tree_node*));
    if (root_ptr == NULL) {
        return NULL;
    }
    *root_ptr = NULL;
    return root_ptr;
};

search_tree_node* search_tree_node_new(const char* key, const void* val, size_t val_size) {
    search_tree_node* node = ( search_tree_node* ) malloc(sizeof(search_tree_node));
    if (node == NULL) {
        return NULL;
    }
    node->left = NULL;
    node->right = NULL;
    node->val_size = val_size;
    node->key = ( char* ) malloc((strlen(key) + 1) * sizeof(char));
    strcpy(node->key, key);
    node->val = malloc(val_size);
    memcpy(node->val, val, val_size);
    return node;
}

void search_tree_copy_subtree(search_tree_node* subtree, search_tree tree) {
    search_tree_add(subtree->key, subtree->val, subtree->val_size, tree);
    if (subtree->right != NULL) {
        search_tree_copy_subtree(subtree->right, tree);
    }
    if (subtree->left != NULL) {
        search_tree_copy_subtree(subtree->left, tree);
    }
}

search_tree search_tree_copy(search_tree tree) {
    search_tree new = search_tree_new();
    if(tree != NULL) {
        search_tree_copy_subtree(*tree, new);
    }
    return new;
}

void search_tree_free_subtree(search_tree_node* subtree) {
    if (subtree->left != NULL) {
        search_tree_free_subtree(subtree->left);
        free(subtree->left);
        subtree->left = NULL; // for debugging
    }
    if (subtree->right != NULL) {
        search_tree_free_subtree(subtree->right);
        free(subtree->right);
        subtree->right = NULL; // for debugging
    }
    free(subtree->val);
    free(subtree->key);
}

void search_tree_free(search_tree tree) {
    search_tree_node* root = *tree;
    if (root != NULL) {
        search_tree_free_subtree(root);
        free(root);
        *tree = NULL;
    }
    free(tree);
}

int search_tree_add(const char* key, const void* val, size_t val_size, search_tree tree) {
    search_tree_node* root = *tree;
    if (root == NULL) {
        *tree = search_tree_node_new(key, val, val_size);
        if (*tree == NULL) {
            return -1;
        }
        return 0;
    }
    int cmp = strcoll(key, root->key);
    if (cmp == 0) {
        return SEARCH_TREE_DUPLICATE_ERR;
    } else if (cmp > 0) {
        if (root->right == NULL) {
            root->right = search_tree_node_new(key, val, val_size);
            if (root->right == NULL) {
                return -1;
            }
            return 0;
        }
        return search_tree_add(key, val, val_size, &(root->right));
    } else {
        if (root->left == NULL) {
            root->left = search_tree_node_new(key, val, val_size);
            if (root->left == NULL) {
                return -1;
            }
            return 0;
        } 
        return search_tree_add(key, val, val_size, &(root->left));
    }
}

const void* search_tree_lookup(search_tree tree, const char* key, size_t* val_size) {
    search_tree_node* root = *tree;
    if (root == NULL) {
        return NULL;
    }
    int cmp = strcmp(key, root->key);
    if (cmp == 0) {
        *val_size = root->val_size;
        return root->val;
    } else if (cmp > 0) {
        if (root->right == NULL) {
            return NULL;
        }
        return search_tree_lookup(&(root->right), key, val_size);
    } else {
        if (root->left == NULL) {
            return NULL;
        }
        return search_tree_lookup(&(root->left), key, val_size);
    }
}

void search_tree_foreach(search_tree tree, void(*callback)(const char*, const void*, size_t, void*), void* arg) {
    search_tree_node* root = *tree;
    if (root == NULL) {
        return;
    }
    callback(root->key, root->val, root->val_size, arg);
    search_tree_foreach(&(root->left), callback, arg);
    search_tree_foreach(&(root->right), callback, arg);
}