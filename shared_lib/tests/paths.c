#include "paths.h"
#include <assert.h>
#include <string.h>

int main (int argc, char** argv) {
    char path[128];
    assert(build_path(path, 128, 3, "/etc", "cron.d", "hourly") == 0);
    assert(strcmp(path, "/etc/cron.d/hourly") == 0);
    assert(build_path(path, 128, 3, "~", "desktop", "icons") == 0);
    assert(strcmp(path, "~/desktop/icons") == 0);
    assert(build_path(path, 10, 3, "/home", "user", "desktop") == -1);
    return 0;
}