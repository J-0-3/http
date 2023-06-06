#pragma once
#include "search_tree.h"
#include "config.h"
#include <netinet/in.h>
#include <stdbool.h>

int run_server(server_configuration* config);
void stop_server();
bool server_running();