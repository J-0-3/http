#include "config.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    FILE* test_config_file = fopen("/tmp/http_server_config_test", "w");
    fwrite("ROOT /var/www/html\nERROR_DIR error\nADDR 127.0.0.1\nPORT 8080\nROUTE / index.html\nALLOW_UNROUTED_FILE_ACCESS 1", 1, 107, test_config_file);
    fclose(test_config_file);
    server_configuration* config = server_configuration_new();
    assert(config != NULL);
    assert(load_config("/tmp/http_server_config_test", config) == 0);
    assert(config->port == 8080);
    assert(strcmp(config->addr, "127.0.0.1") == 0);
    assert(strcmp(config->file_root, "/var/www/html") == 0);
    assert(strcmp(config->error_directory, "error") == 0);
    size_t val_size;
    const char* root_file = search_tree_lookup(config->routes, "/", &val_size);
    assert(root_file != NULL);
    assert(strcmp(root_file, "index.html") == 0);
    assert(config->allow_unrouted == 1);
    server_configuration_free(config);
    remove("/tmp/http_server_config_test");
}