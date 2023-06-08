#include "client.h"
#include <argp.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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
    if (args.url != NULL) {
        struct sockaddr_in addr = { 0 };
        if (resolve_host(args.url, (struct sockaddr*)&addr) != 0) {
            perror("Coudl not resolve host\n");
        } else {
            printf("%s\n", inet_ntoa(addr.sin_addr));
        }
    }
}