#include "http_communication.h"
#include "http.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_header(const char* header_name, const void* header_value, size_t val_size, void*) {
    printf("%s: %s\n", header_name, (const char*)header_value);
}

int main(int argc, char** argv) {
    const int PORT = htons(8000);
    printf("Server will listen on port %d\n", ntohs(PORT));
    int server_sock = socket(PF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in server_addr;
    server_addr.sin_port = PORT;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = 0;
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        perror("Failed to bind\n");
        close(server_sock);
        return 0;
    }
    printf("Bound\n");
    if (listen(server_sock, 10) != 0) {
        perror("Failed to listen\n");
        close(server_sock);
        return 0;
    }
    printf("Listening for incoming connections...\n");
    struct sockaddr conn_addr;
    socklen_t conn_addr_size = sizeof(conn_addr);
    for (int i = 0; i < 10; i++) {
	    int conn_fd = accept(server_sock, &conn_addr, &conn_addr_size);
	    if (conn_fd < 0) {
	        perror("Error accepting connection\n");
	        close(server_sock);
	        return 0;
	    }
	    printf("Connection established\nAwaiting HTTP Request...\n");
        int err;
	    http_req* http_request = recv_request(conn_fd, &err);
	    if (http_request == NULL) {
            if (err == HTTP_ERR_SOCKET) {
				printf("Error occurred reading from socket or client closed the connection prematurely.\n");
			} else if (err == HTTP_ERR_INTERNAL) {
				printf("Error occurred internally while processing HTTP request\n");
			} else if (err == HTTP_ERR_REQUEST_MALFORMED) {
				printf("HTTP request malformed.\n");
			}
	        close(conn_fd);
			continue;
	    }
	    printf("HTTP Request Received:\n");
	    if (http_request->ver == HTTP_1_0) {
	        printf("Version: HTTP/1.0\n");
	    } else if (http_request->ver == HTTP_1_1) {
	        printf("Version: HTTP/1.1\n");
	    }
	    switch (http_request->meth)
	    {
	    case GET:
	        printf("Method: GET\n");
	        break;
	    case POST:
	        printf("Method: POST\n");
	        break;
	    case PUT:
	        printf("Method: PUT\n");
	        break;
	    case CONNECT:
	        printf("Method: CONNECT\n");
	        break;
	    case OPTIONS:
	        printf("Method: OPTIONS\n");
	        break;
	    case HEAD:
	        printf("Method: HEAD\n");
	        break;
	    case DELETE:
	        printf("Method: DELETE\n");
	        break;
	    case TRACE:
	        printf("Method: TRACE\n");
	        break;
        case PATCH:
            printf("Method: PATCH\n");
            break;
        }
        printf("Resource: %s\n", http_request->resource);
        search_tree_foreach(http_request->headers, print_header, NULL);
		if (http_request->content_length > 0) {
			printf("Body: ");
			for (int c = 0; c < http_request->content_length; c++) {
				printf("%c", http_request->content[c]);
			}
			printf("\n");
		}
        http_req_free(http_request);
        close(conn_fd);
		printf("\n");
    }
    close(server_sock);
    return 0;
}