#include "sanitise.h"
#include <regex.h>
#include <string.h>

int sanitise_path(const char* path, char* out, size_t out_len) {
    if (out_len <= strlen(path)) {
        return -1;
    }
    strcpy(out, path);
    regex_t re_comp;
    if (regcomp(&re_comp, "\\.+/", REG_EXTENDED) != 0) {
        return -1;
    }
    int done = 0;
    int moved_amt = 0;
    const char* cur_match_pos = path;
    while (!done) {
        regmatch_t match[1];
        if (regexec(&re_comp, cur_match_pos, 1, match, 0) != 0) {
            done = 1;
        } else {
            regmatch_t m = match[0];
            int offset = cur_match_pos - path + m.rm_so - moved_amt;
            int length = m.rm_eo - m.rm_so;
            memset(out + offset, 0, length);
            memmove(out + offset, out + offset + length, strlen(path) + 1 - (offset + length));
            moved_amt += m.rm_eo - m.rm_so;
            cur_match_pos += m.rm_eo;
        }
    }
    regfree(&re_comp);
    return 0;
}