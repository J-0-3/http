#include "client.h"
#include "search_tree.h"
#include <assert.h>
#include <string.h>
#include <malloc.h>

int main(int argc, char** argv) {
    search_tree headers = search_tree_new();
    assert(parse_header_string("HeaderKey: HeaderVal", headers) == 0);
    size_t val_size;
    const char* header_val;
    header_val = search_tree_lookup(headers, "HeaderKey", &val_size);
    assert(header_val != NULL);
    assert(strcmp(header_val, "HeaderVal") == 0);
    url_t* url;
    url = parse_url("http://website.com/page/file.html");
    assert(url != NULL);
    assert(strcmp(url->protocol, "http") == 0);
    assert(strcmp(url->hostname, "website.com") == 0);
    assert(strcmp(url->resource, "/page/file.html") == 0);
    free(url->hostname);
    free(url->protocol);
    free(url->resource);
    free(url);
    url = parse_url("https://google.com");
    assert(url != NULL);
    assert(strcmp(url->protocol, "https") == 0);
    assert(strcmp(url->hostname, "google.com") == 0);
    assert(strcmp(url->resource, "/") == 0);
    free(url->hostname);
    free(url->protocol);
    free(url->resource);
    free(url);
}
