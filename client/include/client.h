#include "http.h"
#include <sys/socket.h>
#include <arpa/inet.h>

typedef struct url {
    char* resource;
    char* hostname;
    char* protocol;
} url_t;

void free_url_t(url_t* url);

/// @brief Resolve a hostname to an IPv4 address
/// @param host the alphanumeric hostname to lookup
/// @param addr a pointer to a sockaddr struct to store the result in
/// @retval 0 OK
/// @retval -1 Error
int resolve_host(const char* host, struct sockaddr* addr);

/// @brief Parse a header string in the form "key: value"
/// @param header_str the header string
/// @param header_tree a search_tree to add the key-value pair to
/// @retval 0 OK
/// @retval -1 Error
int parse_header_string(const char* header_str, search_tree header_tree);

/// @brief Parse a URL in the form protocol://hostname/resource
/// @param url the url to parse
/// @return a url struct
/// @retval NULL Error
url_t* parse_url(const char* url);

/// @brief Parse a proxy address in the form host:port
/// @param proxy the proxy address
/// @param addr a sockaddr_in struct which will contain the resolved IP and port number
/// @retval 0 OK
/// @retval -1 Error
int parse_proxy_address(const char* proxy, struct sockaddr_in* addr);
