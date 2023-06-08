#include "client.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stddef.h>
#include <string.h>

int resolve_host(const char* host, struct sockaddr* addr) {
    struct addrinfo* result;
    struct addrinfo hints = { 0 };
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, NULL, &hints, &result) != 0) {
        return -1;
    }
    memcpy(addr, result->ai_addr, sizeof(struct sockaddr));
    freeaddrinfo(result);
    return 0;
}