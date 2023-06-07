#pragma once
#include "search_tree.h"

typedef struct server_configuration {
    char* addr;
    unsigned short int port;
    search_tree routes;
    char* file_root;
    char* error_directory;
    int request_timeout;
    int allow_unrouted;
} server_configuration;

server_configuration* server_configuration_new();
void server_configuration_free(server_configuration* config);
int load_config(const char* config_path, server_configuration* config);