#include "client.h"
#include "search_tree.h"
#include "trim_whitespace.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>

void free_url_t(url_t *url) {
    free(url->hostname);
    free(url->protocol);
    free(url->resource);
    free(url);
}

int resolve_host(const char* host, struct sockaddr* addr) {
    struct addrinfo* result;
    struct addrinfo hints = { 0 };
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, NULL, &hints, &result) != 0) {
        return -1;
    }
    memcpy(addr, result->ai_addr, sizeof(struct sockaddr));
    freeaddrinfo(result);
    return 0;
}

int parse_header_string(const char* header_str, search_tree header_tree) {
    char header_str_copy[strlen(header_str) + 1];
    strcpy(header_str_copy, header_str);
    const char* key = strtok(header_str_copy, ":");
    if (key == NULL) {
        return -1;
    }
    char* val = strtok(NULL, "");
    if (val == NULL) {
        return -1;
    }
    const char* trimmed_val = trim_whitespace(val);
    if (search_tree_add(key, trimmed_val, strlen(trimmed_val) + 1, header_tree) != 0) {
        return -1;
    }
    return 0;
}

url_t* parse_url(const char* url) {
    const char* end_of_protocol = strstr(url, "://");
    if (end_of_protocol == NULL) {
        return NULL;
    }
    url_t* url_struct = malloc(sizeof(url_t));
    if (url_struct == NULL) {
        return NULL;
    }
    url_struct->protocol = malloc(end_of_protocol - url + 1);
    if (url_struct->protocol == NULL) {
        free(url_struct);
        return NULL;
    }
    memcpy(url_struct->protocol, url, end_of_protocol - url);
    url_struct->protocol[end_of_protocol - url] = '\0';

    char url_copy[strlen(end_of_protocol + 3) + 1];
    strcpy(url_copy, end_of_protocol + 3);
    char* hostname = strtok(url_copy, "/");
    if (hostname == NULL) {
        free(url_struct->protocol);
        free(url_struct);
        return NULL;
    }
    char* path = strtok(NULL, "");
    if (path == NULL) {
        path = "";
    }
    url_struct->hostname = malloc(strlen(hostname) + 1);
    if (url_struct->hostname == NULL) {
        free(url_struct->protocol);
        free(url_struct);
        return NULL;
    }
    strcpy(url_struct->hostname, hostname);
    url_struct->resource = malloc(strlen(path) + 2);
    if (url_struct->resource == NULL) {
        free(url_struct->resource);
        free(url_struct->hostname);
        free(url_struct);
        return NULL;
    }
    strcpy(url_struct->resource, "/");
    strcat(url_struct->resource, path);
    return url_struct;
}
