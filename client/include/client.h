#include "http.h"
#include <sys/socket.h>

/// @brief Resolve a hostname to an IPv4 address
/// @param host the alphanumeric hostname to lookup
/// @param addr a pointer to a sockaddr struct to store the result in
/// @retval 0 OK
/// @retval -1 Error
int resolve_host(const char* host, struct sockaddr* addr);
