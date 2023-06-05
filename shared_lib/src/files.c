#include "files.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdarg.h>

long get_filesize(const char* filename) {
    struct stat info;
    if (stat(filename, &info) < 0) { // file doesn't exist
        return -1;
    }
    if (!S_ISREG(info.st_mode)) { // file is a directory
        return -1;
    }
    if (info.st_size < 0) {
        return -1;
    }
    else {
        return info.st_size;
    }
}

long read_file(const char* filename, char* buf, size_t buf_size) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }
    size_t read = fread(buf, 1, buf_size, file);
    if (read <= 0) {
        fclose(file);
        return -1;
    }
    fclose(file);
    return read;
}

file_type get_filetype(const char* filename) {
    char filename_copy[strlen(filename) + 1];
    strcpy(filename_copy, filename);
    const char* pre_period = NULL;
    const char* extension = NULL;
    for(pre_period = strtok(filename_copy, "."); pre_period != NULL; pre_period = strtok(NULL, ".")) {
        extension = pre_period;
    }
    if (strcmp(extension, "png") == 0) {
        return PNG;
    } else if (strcmp(extension, "jpg") == 0) {
        return JPG;
    } else if (strcmp(extension, "txt") == 0) {
        return TXT;
    } else if (strcmp(extension, "html") == 0) {
        return HTML;
    } else if (strcmp(extension, "js") == 0) {
        return JS;
    } else if (strcmp(extension, "css") == 0) {
        return CSS;
    } else if (strcmp(extension, "svg") == 0) {
        return SVG;
    } else if (strcmp(extension, "mp4") == 0) {
        return MPEG4;
    } else {
        return DATA;
    }
}

int build_path(char* buf, size_t buf_size, unsigned int num_parts, ...) {
    va_list parts;
    va_start(parts, num_parts);
    size_t req_size = num_parts -1 + 1;
    memset(buf, 0, buf_size);
    for (int i = 0; i < num_parts; i++) {
        const char* part = va_arg(parts, const char*);
        req_size += strlen(part);
        if (buf_size < req_size) {
            return -1;
        }
        if (i > 0) {
            strcat(buf, "/");
        }
        strcat(buf, part);
    }
    va_end(parts);
    return 0;
}