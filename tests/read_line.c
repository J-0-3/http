#include "read_line.h"
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <malloc.h>

int main(int argc, char** argv) {
    int fd = open("/tmp/read_line_test_file", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    write(fd, "LINE 1\r\n", 8);
    write(fd, "LINE 2\r\n", 8);
    write(fd, "LONG LINE 3\r\n", 13);
    write(fd, "\r\n", 2);
    write(fd, "UNTERMINATED", 12);
    close(fd);
    fd = open("/tmp/read_line_test_file", O_RDONLY);
    char* line_buf;
    assert(read_line(fd, &line_buf) == 0);
    assert(strcmp("LINE 1", line_buf) == 0);
    free(line_buf);
    assert(read_line(fd, &line_buf) == 0);
    assert(strcmp("LINE 2", line_buf) == 0);
    free(line_buf);
    assert(read_line(fd, &line_buf) == 0);
    assert(strcmp("LONG LINE 3", line_buf) == 0);
    free(line_buf);
    assert(read_line(fd, &line_buf) == 0);
    assert(strcmp("", line_buf) == 0);
    free(line_buf);
    assert(read_line(fd, &line_buf) == -1);
    assert(strcmp("UNTERMINATED", line_buf) == 0);
    free(line_buf);
    close(fd);
    remove("/tmp/read_line_test_file");
    return 0;
}