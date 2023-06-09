#include "server.h"
#include "http_communication.h"
#include "http.h"
#include "files.h"
#include "paths.h"
#include "int_to_str.h"
#include "sanitise.h"
#include <stdio.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <malloc.h>
#include <pthread.h>
#include <math.h>

static const server_configuration* CONFIG;

int init_socket(unsigned short int port, const char* addr) {
    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    int reuseaddr = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)) < 0) {
        return -1;
    }
    struct sockaddr_in listen_addr = { 0 };
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, addr, &(listen_addr.sin_addr)) < 1) {
        close(sock_fd);
        return -1;
    };
    if (bind(sock_fd, (struct sockaddr*)&listen_addr, sizeof(listen_addr)) < 0) {
        close(sock_fd);
        return -1;
    }
    if (listen(sock_fd, 16) < 0) {
        close(sock_fd);
        return -1;
    }
    return sock_fd;
}

int accept_connection(int sock_fd, struct sockaddr_in* conn_addr) {
    socklen_t conn_addr_len = sizeof(struct sockaddr_in);
    int conn_fd = accept(sock_fd, (struct sockaddr*)conn_addr, &conn_addr_len);
    return conn_fd;
}

int load_page(const char* page, char** content_buf) {
    int path_length = strlen(page) + strlen(CONFIG->file_root) + 2;
    char path[path_length]; // +2 for terminator and / in between
    if (build_path(path, path_length, 2, CONFIG->file_root, page) != 0) {
        return -1;
    }
    char sanitised[path_length];
    if (sanitise_path(path, sanitised, path_length) != 0) {
        return -1;
    }
    long file_size = get_filesize(sanitised);
    if (file_size < 0) {
        return -2;
    }
    *content_buf = malloc(file_size);
    if (content_buf == NULL) {
        return -1;
    }
    if (read_file(sanitised, *content_buf, file_size) < 0) {
        free(*content_buf);
        return -1;
    }
    return file_size;
}

const char* get_route_file(const char* route) {
    size_t val_size;
    return search_tree_lookup(CONFIG->routes, route, &val_size);
}

http_res* build_error(http_status status_code, const char* status_message) {
    http_res* res = http_res_new_empty();    
    res->status_code = status_code;
    res->ver = HTTP_1_1;
    http_res_set_status_message(status_message, res);
    search_tree_add("Content-Type", "text/html", 10, res->headers);
    search_tree_add("Connection", "close", 6, res->headers);

    size_t name_length = strlen(CONFIG->error_directory) + 10;
    char err_page_name[name_length];     
    char status_code_as_str[4];
    if (http_status_code_as_str(status_code, status_code_as_str, 4) < 0) {
        http_res_free(res);
        return NULL;
    }
    if (build_path(err_page_name, name_length - 5, 2, CONFIG->error_directory, status_code_as_str) < 0) {
        http_res_free(res);
        return NULL;
    }
    strcat(err_page_name, ".html");

    char* err_page_contents;
    long contents_size;
    if ((contents_size = load_page(err_page_name, &err_page_contents)) < 0) {
        if (contents_size == -2) {
            err_page_contents = malloc(strlen(status_message) + 72);
            if (err_page_contents == NULL) {
                http_res_free(res);
                return NULL;
            }
            sprintf(err_page_contents, "<html><body><h1 style='color:red'><b>ERROR %03d</b>: %s</h1></body></html>", status_code % 1000, status_message);
            contents_size = strlen(err_page_contents); // no null terminator in http content
        } else {
            http_res_free(res);
            return NULL;
        }
    } 
    if (http_res_set_content(err_page_contents, contents_size, res) < 0) {
        http_res_free(res);
        free(err_page_contents);
        return NULL;
    }

    char content_length_header[32];
    if (int_to_str(contents_size, content_length_header, 32) < 0) {
        http_res_free(res);
        free(err_page_contents);
        return NULL;
    }
    if (search_tree_add("Content-Length", content_length_header, strlen(content_length_header) + 1, res->headers) != 0) {
        http_res_free(res);
        free(err_page_contents);
        return NULL;
    }
    free(err_page_contents);
    return res;
}

int send_error(int sock_fd, http_status status_code, const char* status_message) {
    http_res* res;
    if ((res = build_error(status_code, status_message)) == NULL) {
        return -1;
    }
    if (send_response(sock_fd, res) < 0) {
        http_res_free(res);
        return -1;
    }
    http_res_free(res);
    return 0;
}

void* handle_connection(void* fd_vp) {
    // remember that fd_vp is heap allocated and must be freed by handle_connection!!!
    int conn_fd = *(int*) fd_vp;
    int err = 0;
    http_req* req = recv_request(conn_fd, &err);
    if (req == NULL) {
        close(conn_fd);
        free(fd_vp);
        return NULL;
    }
    if (err == HTTP_ERR_INTERNAL) {
        send_error(conn_fd, INTERNAL_SERVER_ERROR, "Internal Server Error");
        goto end;
    } else if (err == HTTP_ERR_REQUEST_MALFORMED) {
        send_error(conn_fd, BAD_REQUEST, "Bad Request");
        goto end;
    } else if (err != 0) {
        goto end;
    }

    const char* filename = get_route_file(req->resource);
    if (filename == NULL) {
        if (CONFIG->allow_unrouted) {
            filename = req->resource;
        }
        else {
            send_error(conn_fd, NOT_FOUND, "Not Found");
            goto end;
        }
    }

    char* page_contents;
    int content_length;
    if ((content_length = load_page(filename, &page_contents)) < 0) {
        if (content_length == -2) {
            send_error(conn_fd, NOT_FOUND, "Not Found");
            goto end;
        } else {
            goto end;
        }
    }

    char content_length_header[32];
    if (int_to_str(content_length, content_length_header, 32) < 0) {
        free(page_contents);
        goto end;
    }

    http_res* res = http_res_new_empty();
    res->status_code = OK;
    res->ver = req->ver;
    http_res_set_status_message("OK", res);
    http_res_set_content(page_contents, content_length, res);
    search_tree_add("Connection", "close", 6, res->headers);
    char mimetype[64];
    http_get_mimetype_from_ext(filename, mimetype, 64);
    search_tree_add("Content-Type", mimetype, strlen(mimetype) + 1, res->headers); // change this add mimetypes for images etc
    search_tree_add("Content-Length", content_length_header, strlen(content_length_header) + 1, res->headers);

    send_response(conn_fd, res);

    http_res_free(res);
    free(page_contents);
end:
    http_req_free(req);
    close(conn_fd);
    free(fd_vp);
    return NULL;
}

int run_server(server_configuration* server_config) {
    CONFIG = server_config;
    int accept_sock_fd = init_socket(CONFIG->port, CONFIG->addr);
    struct timeval socket_timeout = { .tv_sec = CONFIG->request_timeout, .tv_usec = 0 };
    while (1) {
        struct sockaddr_in conn_addr = { 0 };
        int* conn_fd = malloc(sizeof(int));
        *conn_fd = accept_connection(accept_sock_fd, &conn_addr);
        setsockopt(*conn_fd, SOL_SOCKET, SO_RCVTIMEO, &socket_timeout, sizeof(socket_timeout));
        pthread_t thread_handle;
        pthread_create(&thread_handle, NULL, handle_connection, conn_fd);
        pthread_detach(thread_handle);
    }
}
