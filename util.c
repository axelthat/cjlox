#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *util_read_file(const char *filename)
{
    FILE *file = fopen(filename, "r");

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);

    char *buffer = malloc(length + 1);

    size_t bytes = fread(buffer, sizeof(*buffer), length, file);
    buffer[bytes] = '\0';

    fclose(file);

    return buffer;
}

char *substring(const char *source, size_t start_index, size_t end_index)
{
    size_t length = end_index - start_index;

    char *buffer = malloc(length + 1);
    strncpy(buffer, &source[start_index], length);
    buffer[length] = '\0';

    return buffer;
}

int imin(int a, int b)
{
    return a < b ? a : b;
}

int imax(int a, int b)
{
    return a > b ? a : b;
}