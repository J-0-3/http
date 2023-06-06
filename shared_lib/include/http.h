#pragma once
#include "search_tree.h"

typedef enum http_version {
    HTTP_1_0,
    HTTP_1_1
} http_version;

typedef enum http_method {
    GET,
    POST,
    PUT,
    CONNECT,
    OPTIONS,
    HEAD,
    DELETE,
    TRACE,
    PATCH
} http_method;

typedef enum http_status {
    CONTINUE = 100,
    SWITCHING_PROTOCOLS = 101,
    PROCESSING = 102,
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NO_CONTENT = 204,
    RESET_CONTENT = 205,
    PARTIAL_CONTENT = 206,
    MULTI_STATUS = 207,
    MULTIPLE_CHOICES = 300,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    SEE_OTHER = 303,
    NOT_MODIFIED = 304,
    TEMPORARY_REDIRECT = 307,
    PERMANENT_REDIRECT = 308,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    NOT_ACCEPTABLE = 406,
    PROXY_AUTHENTICATION_REQUIRED = 407,
    REQUEST_TIMEOUT = 408,
    CONFLICT = 409,
    GONE = 410,
    LENGTH_REQUIRED = 411,
    PRECONDITION_FAILED = 412,
    CONTENT_TOO_LARGE = 413,
    URI_TOO_LONG = 414,
    UNSUPPORTED_MEDIA_TYPE = 415,
    RANGE_NOT_SATISFIABLE = 416,
    EXPECTATION_FAILED = 417,
    IM_A_TEAPOT = 418,
    MISDIRECTED_REDIRECT = 421,
    UPGRADE_REQUIRED = 426,
    PRECONDITION_REQUIRED = 428,
    TOO_MANY_REQUESTS = 429,
    REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
    UNAVAILABLE_FOR_LEGAL_REASONS = 451,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,
    GATEWAY_TIMEOUT = 504,
    HTTP_VERSION_NOT_SUPPORTED = 505,
    VARIANT_ALSO_NEGOTIATES = 506,
    NOT_EXTENDED = 510,
    NETWORK_AUTHENTICATION_REQUIRED = 511
} http_status;

typedef struct http_req {
    http_method meth;
    char* resource;
    http_version ver;
    search_tree headers;
    size_t content_length;
    char* content;
} http_req;

typedef struct http_res {
    http_version ver;
    http_status status_code;
    char* status_message;
    search_tree headers;
    size_t content_length;
    char* content;
} http_res;

http_req* http_req_new(http_method meth, const char* resource, http_version ver, search_tree headers, size_t content_length, const char* content);
http_req* http_req_new_empty();
http_res* http_res_new(http_status status_code, const char* status_message, http_version ver, search_tree headers, size_t content_length, const char* content);
http_res* http_res_new_empty();
void http_req_free(http_req* req);
void http_res_free(http_res* res);
int http_parse_req_status_line(const char* status_line, http_req* req);
int http_parse_req_header_line(const char* header_line, http_req* req);
int http_req_set_content(const char* content, size_t content_length, http_req* req);
int http_parse_res_status_line(const char* status_line, http_res* res);
int http_parse_res_header_line(const char* header_line, http_res* res);
int http_res_set_content(const char* content, size_t content_length, http_res* res);
int http_res_set_status_message(const char* status_message, http_res* res);
int http_req_print(http_req* req);
int http_res_print(http_res* res);
int http_meth_enum_as_str(http_method method, char* out, unsigned int out_len);
int http_version_enum_as_str(http_version version, char* out, unsigned int out_len);
int http_status_code_as_str(http_status state, char* out, unsigned int out_len);
int http_get_mimetype_from_ext(const char* filename, char* out, unsigned int out_len);