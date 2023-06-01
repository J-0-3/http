#include "read_line.h"
#include "search_tree.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

#define ERROR(err) fprintf(stderr, "ERROR: %s\n", err); return -1;

int main(int argc, char** argv) {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int reuseaddr = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) < 0) {
        close(sock_fd);
        ERROR("Failed setting socket option");
    }
    if (bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock_fd);
        ERROR("Failed to bind");
    };
    if (listen(sock_fd, 1) < 0) {
        close(sock_fd);
        ERROR("Failed to listen");
    }
    struct sockaddr conn_addr = { 0 };
    socklen_t conn_addr_len = 0;
    int conn_fd = accept(sock_fd, &conn_addr, &conn_addr_len);
    printf("Incoming request\n");
    unsigned short complete = 0; 
    while (!complete) {
        char* line_buf = NULL;
        if (read_line(conn_fd, &line_buf) < 0) {
            close(conn_fd);
            ERROR("Error reading from socket");
        }
        if (strlen(line_buf) == 0) {
            printf("Request complete.\n");
            close(conn_fd);
            close(sock_fd);
            complete = 1;
        } else {
            printf("%s\n", line_buf);
        }
        free(line_buf);
    }
    return 0;
}