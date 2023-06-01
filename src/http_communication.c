#include "http_communication.h"
#include "read_line.h"
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>

http_req* recv_request(int sock_fd, int* err_out) {
    *err_out = 0;
    char* status_line = NULL;
    int err;
    if ((err = read_line(sock_fd, &status_line)) != 0) {
        *err_out = HTTP_ERR_SOCKET;
        return NULL;
    }
    http_req* req = http_req_new_empty();
    if ((err = http_parse_req_status_line(status_line, req)) != 0) {
        if (err == -1) {
            *err_out = HTTP_ERR_INTERNAL;
        } else {
            *err_out = HTTP_ERR_REQUEST_MALFORMED;
        }
        http_req_free(req);
        return NULL;
    }
    free(status_line);
    char* header_line = NULL;
    while ((err = read_line(sock_fd, &header_line)) == 0) {
        if (strcmp(header_line, "") == 0) {
            free(header_line);
            break;
        }
        err = http_parse_req_header_line(header_line, req);
        if (err != 0) {
            if (err == -1) {
                *err_out = HTTP_ERR_INTERNAL;
            } else {
                *err_out = HTTP_ERR_REQUEST_MALFORMED;
            }
            free(header_line);
            http_req_free(req);
            return NULL;
        }
        free(header_line);
    }
    if (err != 0) {
        *err_out = HTTP_ERR_SOCKET;
        http_req_free(req);
        return NULL;
    }
    size_t val_size;
    const char* content_length_header;
    if ((content_length_header = search_tree_lookup(req->headers, "Content-Length", &val_size)) != NULL) {
        int content_length = atoi(content_length_header);
        char* buf = malloc(content_length);
        if (content_length > 0) {
            int received = recv(sock_fd, buf, content_length, 0);
            if (received == -1) {
                free(buf);
                http_req_free(req);
                *err_out = HTTP_ERR_SOCKET;
                return NULL;
            }
            if (http_req_set_content(buf, received, req) != 0) {
                free(buf);
                http_req_free(req);
                *err_out = HTTP_ERR_INTERNAL;
                return NULL;
            }
            free(buf);
        }
    }
    return req;
}