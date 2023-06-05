#include "http_communication.h"
#include "http.h"
#include "server.h"
#include "search_tree.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char** argv) {
    server_configuration config = { 0 };
    config.addr = "0.0.0.0";
    config.port = 8080;
    config.request_timeout = 30;
    config.error_directory = "error";
    config.routes = search_tree_new();
    config.file_root = "/var/www/html";
    if (run_server(&config) != 0) {
        printf("Error occurred running the server\n.");
        search_tree_free(config.routes);
        return -1;
    }
}