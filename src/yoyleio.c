#include <stdio.h>
#include <stdlib.h>

#include "inc/yoyleio.h"
#include "inc/yoyleerr.h"

char* read_file(const char* filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fatal("Failed to open file");
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        fatal("fseek failed");
    }

    long filesize = ftell(file);
    if (filesize < 0) {
        fclose(file);
        fatal("ftell failed");
    }

    rewind(file);

    char *buffer = malloc(filesize + 1);
    if (!buffer) {
        fclose(file);
        fatal("Memory allocation failed");
    }

    size_t read_bytes = fread(buffer, 1, filesize, file);
    if (read_bytes != filesize) {
        free(buffer);
        fclose(file);
        fatal("Failed to read entire file");
    }

    buffer[filesize] = '\0';

    fclose(file);

    return buffer;
}
