#include "config.h"
#include "files.h"
#include "read_line.h"
#include <string.h>
#include <stdlib.h>

server_configuration* server_configuration_new() {
    server_configuration* config = malloc(sizeof(server_configuration));
    memset(config, 0, sizeof(server_configuration));
    config->routes = search_tree_new();
    return config;
}

void server_configuration_set_addr(server_configuration* config, const char* addr) {
    config->addr = malloc(strlen(addr) + 1);
    strcpy(config->addr, addr);
}

void server_configuration_set_file_root(server_configuration* config, const char* root) {
    config->file_root = malloc(strlen(root) + 1);
    strcpy(config->file_root, root);

}

void server_configuration_set_error_directory(server_configuration* config, const char* dir) {
    config->error_directory = malloc(strlen(dir) + 1);
    strcpy(config->error_directory, dir);
}

void server_configuration_free(server_configuration* config) {
    search_tree_free(config->routes);
    free(config->addr);
    free(config->error_directory);
    free(config->file_root);
    free(config);
}

int load_config(const char* filename, server_configuration* config) {
    long filesize = get_filesize(filename);
    if (filesize < 0) {
        return -1;
    }
    char* config_file = malloc(filesize + 1);
    if (config_file == NULL) {
        return -1;
    }
    if (read_file(filename, config_file, filesize) < 0) {
        free(config_file);
        return -1;
    }
    config_file[filesize] = '\0';

    char* strtok_file_saveptr = NULL;
    int line_num = 1;
    char* line = strtok_r(config_file, "\n", &strtok_file_saveptr);
    while (line != NULL) {
        const char* setting = strtok(line, " ");
        const char* value = strtok(NULL, " ");
        if (value == NULL) {
            free(config_file);
            return line_num;
        }
        if (strcmp(setting, "PORT") == 0) {
            int port = atoi(value);
            if (port < 1 || port > 65535) {
                free(config_file);
                return line_num;
            }
            config->port = port;
        } else if (strcmp(setting, "ADDR") == 0) {
            server_configuration_set_addr(config, value);
        } else if (strcmp(setting, "ERROR_DIR") == 0) {
            server_configuration_set_error_directory(config, value);
        } else if (strcmp(setting, "ROUTE") == 0) {
            const char* route = value;
            const char* file = strtok(NULL, " ");
            if (file == NULL) {
                free(config_file);
                return line_num;
            }
            if (search_tree_add(route, file, strlen(file) + 1, config->routes) != 0) {
                free(config_file);
                return line_num;
            }
        } else if (strcmp(setting, "ALLOW_UNROUTED_FILE_ACCESS") == 0)  {
            config->allow_unrouted = atoi(value);
        } else if (strcmp(setting, "ROOT") == 0) {
            server_configuration_set_file_root(config, value);
        } else if (strcmp(setting, "//") != 0) {
            free(config_file);
            return line_num; 
        }
        line_num++;
        line = strtok_r(NULL, "\n", &strtok_file_saveptr);
    }
    free(config_file);
    return 0;
}