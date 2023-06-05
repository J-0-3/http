#include "http_communication.h"
#include "read_line.h"
#include "sendall.h"
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
        free(status_line);
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

struct send_header_line_args {
    int err;
    const int sock_fd;
};

void send_header_line(const char* header_name, const void* header_val, size_t header_size, void* vptr_args) {
    struct send_header_line_args* args = (struct send_header_line_args*) vptr_args; 
    if (sendall(args->sock_fd, header_name, strlen(header_name)) < 0) {
        args->err = -1;
        return;
    }
    if (sendall(args->sock_fd, ": ", 2) < 0) {
        args->err = -1;
        return;
    }
    if (sendall(args->sock_fd, header_val, header_size - 1) < 0) {
        args->err = -1;
        return;
    }
    if (sendall(args->sock_fd, "\r\n", 2) < 0) {
        args->err = -1;
    }
}

int send_response(int sock_fd, http_res* res) {
    char version[16];
    if (http_version_enum_as_str(res->ver, version, 15) < 0) {
        return -1;
    }
    if (sendall(sock_fd, version, strlen(version)) < 0) {
        return HTTP_ERR_SOCKET;
    };
    char status[4];
    snprintf(status, 4, "%03d", res->status_code % 1000);
    if (send(sock_fd, " ", 1, MSG_NOSIGNAL) < 1) {
        return HTTP_ERR_SOCKET;
    }
    if (sendall(sock_fd, status, 3) < 0) {
        return HTTP_ERR_SOCKET;
    }
    if (send(sock_fd, " ", 1, MSG_NOSIGNAL) < 1) {
        return HTTP_ERR_SOCKET;
    }
    if (sendall(sock_fd, res->status_message, strlen(res->status_message)) < 0) {
        return HTTP_ERR_SOCKET;
    }
    if (send(sock_fd, "\r\n", 2, MSG_NOSIGNAL) < 2) {
        return HTTP_ERR_SOCKET;
    }
    struct send_header_line_args args = {.err = 0, .sock_fd = sock_fd};
    search_tree_foreach(res->headers, send_header_line, &args);
    if (args.err != 0) {
        perror("Failed to send headers.\n");
        return HTTP_ERR_SOCKET;
    }
    if (sendall(sock_fd, "\r\n", 2) < 0) {
        perror("Failed to send final crlf.\n");
        return HTTP_ERR_SOCKET;
    }
    if (res->content_length > 0) {
        printf("Sending content.\n");
        if (sendall(sock_fd, res->content, res->content_length) < 0) {
            printf("Failed.\n");
            return HTTP_ERR_SOCKET;
        }
    }
    return 0;
}