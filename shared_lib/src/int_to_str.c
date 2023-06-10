#include "int_to_str.h"
#include <math.h>
#include <stdio.h>

int int_to_str (int i, char* buf, unsigned int buf_len) {
    int required_length = (int) floor(log10(i)) + 2;
    if (i < 0) {
        required_length += 1;
    }
    if (buf_len < required_length) {
        return -1;
    }
    sprintf(buf, "%d", i);
    return 0;
}
