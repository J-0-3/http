#pragma once
#include "http.h"

#define HTTP_ERR_REQUEST_MALFORMED 1
#define HTTP_ERR_INTERNAL 2
#define HTTP_ERR_SOCKET 3

http_req* recv_request(int sock_fd, int* err_out);
int send_response(int sock_fd, http_res* response);