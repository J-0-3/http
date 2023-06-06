#include "http_communication.h"
#include "http.h"
#include "server.h"
#include "search_tree.h"
#include "config.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char** argv) {
    const char* CONFIG_PATH = "/etc/http_server/http_server.conf";
    if (argc > 1) {
        CONFIG_PATH = argv[1];
    }
    server_configuration* config = server_configuration_new();
    int err = load_config(CONFIG_PATH, config);
    if (err != 0) {
        if (err < 0) {
            perror("Could not read config file.\n");
            server_configuration_free(config);
            exit(1);
        }
        fprintf(stderr, "Error in config file on line %d\n", err);
        server_configuration_free(config);
        exit(1);
    }
    printf("Serving files from %s on %s:%d...\n", config->file_root, config->addr, config->port);
    if (run_server(config) != 0) {
        perror("Failed to start server.\n");
        server_configuration_free(config);
    }
}