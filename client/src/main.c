#include "client.h"
#include "http_communication.h"
#include "array_utils.h"
#include "http.h"
#include "search_tree.h"
#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define VERBOSE_OPT_VAL -1

typedef struct program_args { 
    const char* url;
    const char* method;
    search_tree headers;
    const char* data;
    unsigned short port;
    int verbose;
    const char* version;
    const char* proxy;
 } program_args;

static program_args args = { 
    .url = NULL,
    .method = "GET",
    .headers = NULL,
    .data = NULL,
    .proxy = NULL,
    .verbose = 0,
    .port = 0,
    .version = "HTTP/1.1"
 };

error_t parse_args(int key, char* arg, struct argp_state *state) {
    switch (key) {
        case 'u':
            args.url = arg;
            break;
        case 'm':
            args.method = arg;
            break;
        case 'd':
            args.data = arg;   
            break;
        case 'h':
            if (args.headers == NULL) {
                args.headers = search_tree_new();
            }
            if (parse_header_string(arg, args.headers) != 0) {
                fprintf(stderr, "Failed to parse header %s\n", arg);
                exit(1);
            }
            break;
        case VERBOSE_OPT_VAL:
            args.verbose = 1;
            break;
        case 'p':
            args.port = atoi(arg);
            break;
        case 'v':
            args.version = arg;
        case 'x':
            args.proxy = arg;
    }
    return 0;
}

int main (int argc, char** argv) {
    struct argp_option options[] = {
        {"url", 'u', "url", 0, "The URL to request"},
        {"method", 'm', "method", 0, "The HTTP method to perform (GET,POST,PUT,HEAD,TRACE,OPTIONS,CONNECT,DELETE) (default GET)"},
        {"data", 'd', "data", 0, "The data to include in the HTTP request"},
        {"header", 'h', "header", 0, "Additional HTTP header to include (in form \"key: value\""},
        {"verbose", VERBOSE_OPT_VAL, 0, 0, "Produce verbose output"},
        {"port", 'p', "port", 0, "Custom port to connect on"},
        {"version", 'v', "version", 0, "The HTTP protocol version to use (HTTP/1.0, HTTP/1.1) (default HTTP/1.1)"},
        {"proxy", 'x', "proxy", 0, "HTTP proxy to use (in form \"server:port\")"},
        { 0 }
    };
    struct argp parser = {options, parse_args, 0, 0};
    argp_parse(&parser, argc, argv, 0, 0, NULL);
    if (args.url == NULL) {
        perror("Must specify --url (-u)\n");
        return -1;
    } 
    if (all_str((char*[]){"GET", "POST", "PUT", "HEAD", "TRACE", "OPTIONS", "CONNECT", "DELETE"}, 8, strcmp, args.method)) {
        perror("Invalid HTTP method.\n");
        return -1;
    }
    url_t* url = parse_url(args.url);
    if (url == NULL) {
        perror("URL Malformed.\n");
        return -1;
    }
    if (all_str((char*[]){"http", "https"}, 2, strcmp, url->protocol)) {
        perror("Invalid protocol.\n");
        free_url_t(url);
        return -1;
    }
    if (all_str((char*[]){"HTTP/1.1", "HTTP/1.0"}, 2, strcmp, args.version)) {
        perror("Invalid HTTP version.\n");
        free_url_t(url);
        return -1;
    }
    
    http_req* request = http_req_new_empty();
    if (args.proxy != NULL) {
        http_req_set_resource(args.url, request);
    } else {
        http_req_set_resource(url->resource, request);
    }
    char meth_as_str[16];
    for (http_method method = GET; method < HTTP_METHOD_COUNT; method++) {
        http_meth_enum_as_str(method, meth_as_str, 16);
        if (strcmp(meth_as_str, args.method) == 0) {
            request->meth = method;
        }
    }
    char ver_as_str[16];
    for (http_version ver = HTTP_1_0; ver < HTTP_VERSION_COUNT; ver++) {
        http_version_enum_as_str(ver, ver_as_str, 16);
        if (strcmp(ver_as_str, args.version) == 0) {
            request->ver = ver;
        }
    }
    struct sockaddr_in addr = { 0 };
    if (args.verbose) {
        printf("Resolving hostname...\n");
    }
    if (!args.proxy) {
        if (resolve_host(url->hostname, (struct sockaddr*)&addr) != 0) {
            perror("Could not resolve domain name\n");
            free_url_t(url);
            http_req_free(request);
            return -1;
        }
    } else {
        if (parse_proxy_address(args.proxy, &addr) != 0) {
            perror("Could not resolve proxy address\n");
            free_url_t(url);
            http_req_free(request);
            return -1;
        }
    }
    if (args.verbose) {
        printf("Got IP address: %s.\n", inet_ntoa(addr.sin_addr));
    }
    if (args.headers != NULL) {
        search_tree_merge(request->headers, args.headers);
    }
    if (args.data != NULL) {
        if (http_req_set_content(args.data, strlen(args.data), request, 1) != 0) {
            perror("Error setting content (maybe you have set the Content-Length header separately?).\n");
            free_url_t(url);
            http_req_free(request);
            return -1;
        }
    }
    if (!args.proxy) {
        if (args.port == 0) {
            if (strcmp(url->protocol, "http") == 0) { 
                addr.sin_port = htons(80);
            } else {
                addr.sin_port = htons(443);
            }
        } else {
            addr.sin_port = htons(args.port);
        }
    }
    free_url_t(url);
    addr.sin_family = AF_INET;
    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (args.verbose) {
        printf("Connecting to host...\n");
    }
    if (connect(sock_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) != 0) {
        perror("Failed to connect to host.\n");
        http_req_free(request);
        exit(1);
    }
    if (args.verbose) {
        printf("Sending request...\n");
    }
    if (send_request(sock_fd, request) != 0) {
        perror("Failed to send request.\n");
        http_req_free(request);
        exit(1);
    }
    http_req_free(request);
    int err;
    if (args.verbose) {
        printf("Receiving response...\n");
    }
    http_res* response = recv_response(sock_fd, &err);
    if (response == NULL) {
        if (err == HTTP_ERR_INTERNAL) {
            perror("Error while receiving request.\n");
            exit(1);
        } else if (err == HTTP_ERR_RESPONSE_MALFORMED) {
            perror("Response from server malformed.\n");
            exit(1);
        } else {
            perror("Network error receiving response from server.\n");
            exit(1);
        }
    }
    http_res_print(response);
    http_res_free(response);
}
