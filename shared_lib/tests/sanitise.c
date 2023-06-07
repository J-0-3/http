#include "sanitise.h"
#include <assert.h>
#include <string.h>

int main(int argc, char** argv) {
    const char* path = "/var/www/html/../../../etc/passwd";
    char sanitised_buffer[64];
    assert(sanitise_path(path, sanitised_buffer, 64) == 0);
    assert(strcmp(sanitised_buffer, "/var/www/html/etc/passwd") == 0);
    path = "/var/www/html/././secret.txt";
    assert(sanitise_path(path, sanitised_buffer, 64) == 0);
    assert(strcmp(sanitised_buffer, "/var/www/html/secret.txt") == 0);
    path = "/var/....//etc/passwd";
    assert(sanitise_path(path, sanitised_buffer, 64) == 0);
    assert(strcmp(sanitised_buffer, "/var//etc/passwd") == 0);
    path = "/../";
    assert(sanitise_path(path, sanitised_buffer, 64) == 0);
    assert(strcmp(sanitised_buffer, "/") == 0);
    path = "../";
    assert(sanitise_path(path, sanitised_buffer, 64) == 0);
    assert(strcmp(sanitised_buffer, "") == 0);
}