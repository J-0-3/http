#include "http.h"
#include <assert.h>
#include <string.h>

int main(int argc, char** argv) {
    http_req* sample_request = http_req_new(GET, "/index.html", HTTP_1_1, NULL, 0, NULL);
    assert(sample_request != NULL);
    assert(sample_request->meth == GET);
    assert(sample_request->ver == HTTP_1_1);
    assert(strcmp(sample_request->resource, "/index.html") == 0);
    http_req_free(sample_request);

    http_res* sample_response = http_res_new(OK, "OK", HTTP_1_0, NULL, 0, NULL);
    assert(sample_response != NULL);
    assert(sample_response->status_code == OK);
    assert(strcmp(sample_response->status_message, "OK") == 0);
    assert(sample_response->ver == HTTP_1_0);
    http_res_free(sample_response);

    http_req* test_req = http_req_new_empty();
    assert(test_req != NULL);
    http_req_free(test_req);

    test_req = http_req_new_empty();
    assert(http_parse_req_status_line("POST /login.php HTTP/1.1", test_req) == 0);
    assert(test_req->meth == POST);
    assert(strcmp(test_req->resource, "/login.php") == 0);
    assert(test_req->ver == HTTP_1_1);

    assert(http_parse_req_header_line("Content-Type: application/x-www-form-urlencoded", test_req) == 0);
    size_t val_size = 0;
    assert(strcmp(search_tree_lookup(test_req->headers, "Content-Type", &val_size), "application/x-www-form-urlencoded") == 0);
    assert(val_size == 34);

    assert(http_parse_req_header_line("Content-Length:      23     ", test_req) == 0);
    assert(strcmp(search_tree_lookup(test_req->headers, "Content-Length", &val_size), "23") == 0);

    assert(http_parse_req_header_line("Content-Encoding:           ", test_req) == 0);
    assert(strcmp(search_tree_lookup(test_req->headers, "Content-Encoding", &val_size), "") == 0);

    assert(http_parse_req_header_line("Accept:", test_req) == 0);
    assert(strcmp(search_tree_lookup(test_req->headers, "Accept", &val_size), "") == 0);

    assert(http_parse_req_header_line("User-Agent: Mozilla: Windows", test_req) == 0);
    assert(strcmp(search_tree_lookup(test_req->headers, "User-Agent", &val_size), "Mozilla: Windows") == 0);

    assert(http_parse_req_header_line("INVALID HEADER LINE", test_req) == -2);
    assert(http_parse_req_header_line("", test_req) == -2);
    assert(http_parse_req_header_line(":", test_req) == -2);

    assert(http_req_set_content("name=user&password=1234", 24, test_req) == 0);
    assert(strcmp(test_req->content, "name=user&password=1234") == 0);

    http_req_free(test_req);
    test_req = http_req_new_empty();
    assert(http_parse_req_status_line("TEST /login.php HTTP/1.1", test_req) == -2);
    assert(http_parse_req_status_line("GET  HTTP/1.1", test_req) == -2);
    assert(http_parse_req_status_line("GET /index.html", test_req) == -2);
    assert(http_parse_req_status_line("GET /image.jpg FTP/1", test_req) == -2);
    assert(http_parse_req_status_line("", test_req) == -2);
    assert(http_parse_req_status_line("GET /index.html HTTP/1.1 test", test_req) == -2);
    http_req_free(test_req);

    http_res* test_res = http_res_new_empty();
    assert(test_res != NULL);

    assert(http_parse_res_status_line("HTTP/1.1 200 OK", test_res) == 0);
    assert(test_res->ver == HTTP_1_1);
    assert(test_res->status_code == OK);
    assert(strcmp(test_res->status_message, "OK") == 0);

    http_res_free(test_res);
    test_res = http_res_new_empty();

    assert(http_parse_res_status_line("HTTP/1.0 404 Not Found", test_res) == 0);
    assert(test_res->ver == HTTP_1_0);
    assert(test_res->status_code == NOT_FOUND);
    assert(strcmp(test_res->status_message, "Not Found") == 0);

    http_res_free(test_res);
    test_res = http_res_new_empty();
    
    assert(http_parse_res_status_line("HTTP/1.1 403", test_res) == -2);
    assert(http_parse_res_status_line("ABC 200 OK", test_res) == -2);
    assert(http_parse_res_status_line("HTTP/1.1 ABC Forbidden", test_res) == -2);
    assert(http_parse_res_status_line("", test_res) == -2);
    assert(http_parse_res_status_line("HTTP/1.1 300 ", test_res) == -2);

    assert(http_parse_res_status_line("HTTP/1.0 101   Switching Protocols   ", test_res) == 0);
    assert(test_res->ver == HTTP_1_0);
    assert(test_res->status_code == SWITCHING_PROTOCOLS);
    assert(strcmp(test_res->status_message, "Switching Protocols") == 0);

    assert(http_parse_res_header_line("Content-Type: text/html", test_res) == 0);
    assert(strcmp(search_tree_lookup(test_res->headers, "Content-Type", &val_size), "text/html") == 0);
    assert(val_size == 10);

    assert(http_parse_res_header_line("Content-Length:        43           ", test_res) == 0);
    assert(strcmp(search_tree_lookup(test_res->headers, "Content-Length", &val_size), "43") == 0);
    assert(val_size == 3);

    assert(http_parse_res_header_line("Content-Encoding:", test_res) == 0);
    assert(strcmp(search_tree_lookup(test_res->headers, "Content-Encoding", &val_size), "") == 0);

    assert(http_parse_res_header_line("User-Agent: Mozilla: Windows", test_res) == 0);
    assert(strcmp(search_tree_lookup(test_res->headers, "User-Agent", &val_size), "Mozilla: Windows") == 0);

    assert(http_parse_res_header_line("Host", test_res) == -2);
    assert(http_parse_res_header_line("", test_res) == -2);
    assert(http_parse_res_header_line("     ", test_res) == -2);
    assert(http_parse_res_header_line(":", test_res) == -2);

    assert(http_res_set_content("<html><head>Hello World</head></html>", 38, test_res) == 0);
    assert(strcmp(test_res->content, "<html><head>Hello World</head></html>") == 0);

    http_res_free(test_res);
    return 0;
}
