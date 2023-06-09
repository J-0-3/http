#include "client.h"
#include "array_utils.h"
#include "http.h"
#include <argp.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

typedef struct program_args { 
    const char* url;
    const char* method;
    const char* headers;
    const char* data;
 } program_args;

static program_args args = { 
    .url = NULL,
    .method = "GET",
    .headers = NULL,
    .data = NULL
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
            args.method = arg;   
            break;
        case 'h':
            args.headers = arg;
            break;
    }
    return 0;
}

int main (int argc, char** argv) {
    struct argp_option options[] = {
        {"url", 'u', "url", 0, "The URL to request"},
        {"method", 'm', "method", 0, "The HTTP method to perform (GET,POST,PUT,HEAD,TRACE,OPTIONS,CONNECT,DELETE)"},
        {"data", 'd', "data", 0, "The data to include in the HTTP request"},
        {"headers", 'h', "headers", 0, "Additional HTTP headers to include (in form \"key=value key=value key=value\""},
        { 0 }
    };
    struct argp parser = {options, parse_args, 0, 0};
    argp_parse(&parser, argc, argv, 0, 0, NULL);
    if (args.url == NULL) {
        perror("Must specify --url (-u)\n");
        return 1;
    } 
    if (all_str((char*[]){"GET", "POST", "PUT", "HEAD", "TRACE", "OPTIONS", "CONNECT", "DELETE"}, 8, strcmp, args.method)) {
        perror("Invalid HTTP method.\n");
        return 1;
    }
    http_req* request = http_req_new_empty();
    char meth_as_str[16];
    for (http_method method = GET; method < HTTP_METHOD_COUNT; method++) {
        http_meth_enum_as_str(method, meth_as_str, 16);
        if (strcmp(meth_as_str, args.method) == 0) {
            request->meth = method;
        }
    }
}
