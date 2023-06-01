#include "http.h"
#include "search_tree.h"
#include "trim_whitespace.h"
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

http_req* http_req_new(http_method meth, const char* resource, http_version ver, search_tree headers, size_t content_length, const char* content) {
    http_req* req = malloc(sizeof(http_req));
    memset(req, 0, sizeof(http_req));
    req->meth = meth;
    req->ver = ver;
    req->resource = malloc((strlen(resource) + 1) * sizeof(char));
    memset(req->resource, 0, (strlen(resource) + 1) * sizeof(char));
    strcpy(req->resource, resource);
    req->headers = search_tree_copy(headers);
    req->content_length = content_length;
    if (content != NULL) {
        req->content = malloc(content_length);
        memcpy(req->content, content, content_length);
    }
    return req;
}

http_req* http_req_new_empty() {
    http_req* req = malloc(sizeof(http_req));
    memset(req, 0, sizeof(http_req));
    req->headers = search_tree_new();
    return req;
}

void http_req_free(http_req* req) {
    free(req->resource);
    free(req->content);
    search_tree_free(req->headers);
    free(req);
}

http_res* http_res_new(http_status status_code, const char* status_message, http_version ver, search_tree headers, size_t content_length, const char* content) {
    http_res* res = malloc(sizeof(http_res));
    memset(res, 0, sizeof(http_res));
    res->status_code = status_code;
    res->ver = ver;
    res->content_length = content_length;
    res->status_message = malloc((strlen(status_message) + 1) * sizeof(char));
    memset(res->status_message, 0, (strlen(status_message) + 1) * sizeof(char));
    strcpy(res->status_message, status_message);
    res->headers = search_tree_copy(headers);
    res->content = malloc(content_length);
    if(content != NULL) {
        memcpy(res->content, content, content_length);
    }
    return res;
}

http_res* http_res_new_empty() {
    http_res* res = malloc(sizeof(http_res));
    memset(res, 0, sizeof(http_res));
    res->headers = search_tree_new();
    return res;
}

void http_res_free(http_res* res) {
    free(res->status_message);
    free(res->content);
    search_tree_free(res->headers);
    free(res);
}

/** Parse the status line of an HTTP request, filling the struct req with the parsed information.
    Return -1 if an error occurs when processing, -2 if the status line is malformed*/
int http_parse_req_status_line(const char* status_line, http_req* req) {
    char* status_line_copy = malloc(strlen(status_line) + 1);
    if (status_line_copy == NULL) {
        return -1;
    }
    memset(status_line_copy, 0, strlen(status_line) + 1);
    strcpy(status_line_copy, status_line);
    const char* method = strtok(status_line_copy, " ");
    if (method == NULL) {
        free(status_line_copy);
        return -2;
    }
    if (strcmp(method, "GET") == 0) {
        req->meth = GET;
    } else if (strcmp(method, "POST") == 0) {
        req->meth = POST;
    } else if (strcmp(method, "PUT") == 0) {
        req->meth = PUT;
    } else if (strcmp(method, "CONNECT") == 0) {
        req->meth = CONNECT;
    } else if (strcmp(method, "OPTIONS") == 0) {
        req->meth = OPTIONS;
    } else if (strcmp(method, "HEAD") == 0) {
        req->meth = HEAD;
    } else if (strcmp(method, "DELETE") == 0) {
        req->meth = DELETE;
    } else if (strcmp(method, "TRACE") == 0) {
        req->meth = TRACE;
    } else if (strcmp(method, "PATCH") == 0) {
        req->meth = PATCH;
    } else {
        free(status_line_copy);
        return -2;
    }
    const char* resource = strtok(NULL, " ");
    if (resource == NULL) {
        free(status_line_copy);
        return -2;
    }
    req->resource = malloc(sizeof(char) * (strlen(resource) + 1));
    if (req->resource == NULL) {
        free(status_line_copy);
        return -1;
    }
    memset(req->resource, 0, strlen(resource) + 1);
    strcpy(req->resource, resource);
    const char* version = strtok(NULL, " ");
    if (version == NULL) {
        free(status_line_copy);
        free(req->resource);
        req->resource = NULL;
        return -2;
    }
    if (strcmp(version, "HTTP/1.1") == 0) {
        req->ver = HTTP_1_1;
    } else if (strcmp(version, "HTTP/1.0") == 0) {
        req->ver = HTTP_1_0;
    } else {
        free(status_line_copy);
        free(req->resource);
        req->resource = NULL;
        return -2;
    }
    // be pedantic
    if (strtok(NULL, " ") != NULL) {
        free(status_line_copy);
        free(req->resource);
        req->resource = NULL;
        return -2;
    }
    free(status_line_copy);
    return 0;
}

int http_parse_req_header_line(const char* header_line, http_req* req) {
    char* header_line_copy = malloc(strlen(header_line) + 1);
    if (header_line_copy == NULL) {
        return -1;
    }
    strcpy(header_line_copy, header_line);
    const char* header_name = strtok(header_line_copy, ":");
    if (header_name == NULL) {
        free(header_line_copy);
        return -2;
    }
    char* header_value = strtok(NULL, "");
    if (header_value == NULL) {
        if (strlen(header_line_copy) == strlen(header_line)) { 
        // if ':' not in header line (strtok removes it from the copy)
            free(header_line_copy);
            return -2;
        }
        header_value = "";
    }
    char* header_value_trimmed = trim_whitespace(header_value);
    int err;
    if ((err = search_tree_add(header_name, header_value_trimmed, strlen(header_value_trimmed) + 1, req->headers)) != 0) {
        if (err == SEARCH_TREE_DUPLICATE_ERR) {
            free(header_line_copy);
            return -2;
        }
        return -1;
    }
    free(header_line_copy);
    return 0;
}

int http_req_set_content(const char* content, size_t content_length, http_req* req) {
    req->content_length = content_length;
    req->content = malloc(content_length);
    if (req->content == NULL) {
        return -1;
    }
    memcpy(req->content, content, content_length);
    return 0;
}

int http_parse_res_status_line(const char* status_line, http_res* res) {
    char* status_line_copy = malloc(strlen(status_line) + 1);
    if (status_line_copy == NULL) {
        return -1;
    }
    strcpy(status_line_copy, status_line);
    char* version = strtok(status_line_copy, " ");
    if (version == NULL) {
        free(status_line_copy);
        return -2;
    }
    if (strcmp(version, "HTTP/1.1") == 0) {
        res->ver = HTTP_1_1;
    } else if (strcmp(version, "HTTP/1.0") == 0) {
        res->ver = HTTP_1_0;
    } else {
        free(status_line_copy);
        return -2;
    }
    char* status_code_s = strtok(NULL, " ");
    if (status_code_s == NULL) {
        free(status_line_copy);
        return -2;
    }
    int status_code = atoi(status_code_s); 
    if (status_code == 0) {
        free(status_line_copy);
        return -2;
    }
    res->status_code = status_code; // might not actually be a member of enum but dw about it :)
    char* status_message = strtok(NULL, ""); // classic strtok
    if (status_message == NULL) {
        free(status_line_copy);
        return -2;
    }
    char* status_message_trimmed = trim_whitespace(status_message);
    if (strlen(status_message_trimmed) == 0) {
        free(status_line_copy);
        return -2;
    }
    res->status_message = malloc(strlen(status_message_trimmed) + 1);
    if (res->status_message == NULL) {
        free(status_line_copy);
        return -1;
    }
    strcpy(res->status_message, status_message_trimmed);
    free(status_line_copy);
    return 0;
}

int http_parse_res_header_line(const char* header_line, http_res* res) {
    char* header_line_copy = malloc(strlen(header_line) + 1);
    strcpy(header_line_copy, header_line);
    char* header_name = strtok(header_line_copy, ":");
    if (header_name == NULL) {
        free(header_line_copy);
        return -2;
    }
    char* header_value = strtok(NULL, "");
    if (header_value == NULL) {
        if (strlen(header_line_copy) == strlen(header_line)) {
        // if no ':' in header_line (strtok trick)
            free(header_line_copy);
            return -2;
        }
        header_value = "";
    }
    char* header_value_trimmed = trim_whitespace(header_value);
    int err;
    if ((err = search_tree_add(header_name, header_value_trimmed, strlen(header_value_trimmed) + 1, res->headers)) != 0) {
        if (err == SEARCH_TREE_DUPLICATE_ERR) {
            free(header_line_copy);
            return -2;
        }
        return -1;
    }
    free(header_line_copy);
    return 0;
}

int http_res_set_content(const char* content, size_t content_length, http_res* res) {
    res->content_length = content_length;
    res->content = malloc(content_length);
    if (res->content == NULL) {
        return -1;
    }
    memcpy(res->content, content, content_length);
    return 0;
}