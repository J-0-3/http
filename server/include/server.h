#pragma once
#include "search_tree.h"
#include <netinet/in.h>
#include <stdbool.h>

#define HTML_404_DEFAULT "<html><body><h1 style='color:red'>ERROR 404: NOT FOUND</h1></body></html>"
#define HTML_500_DEFAULT "<html><body><h1 style='color:red'>ERROR 500: INTERNAL SERVER ERROR</h1></body></html>"

typedef struct server_configuration {
    char* addr;
    unsigned short int port;
    search_tree routes;
    char* file_root;
    char* error_directory;
    char* log_path;
    int request_timeout;
    char** file_blacklist;
} server_configuration;

int run_server(server_configuration* config);
void stop_server();
bool server_running();