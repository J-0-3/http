#include "sendall.h"
#include <sys/socket.h>

int sendall(int sock_fd, const char* buf, unsigned int bytes) {
    unsigned int to_send = bytes;
    while (to_send > 0) {
        int sent = send(sock_fd, buf, to_send, MSG_NOSIGNAL);
        if (sent < 0) {
            return -1;
        }
        to_send -= sent;
    }
    return 0;
}