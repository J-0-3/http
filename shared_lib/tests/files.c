#include "files.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

int main(int argc, char** argv) {
    srand(time(NULL));
    int test_file_size = rand() % 65535;
    char test_file_data[test_file_size];
    FILE* urandom = fopen("/dev/urandom", "r");
    fread(test_file_data, 1, test_file_size, urandom);
    fclose(urandom);
    FILE* file = fopen("/tmp/testfile", "w");
    fwrite(test_file_data, 1, test_file_size, file);
    fclose(file);
    assert(get_filesize("/tmp/testfile") == test_file_size);

    char file_contents[test_file_size];
    assert(read_file("/tmp/testfile", file_contents, test_file_size) == test_file_size);
    assert(memcmp(test_file_data, file_contents, test_file_size) == 0);

    remove("/tmp/testfile");
}
