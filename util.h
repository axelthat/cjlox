#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

char *util_read_file(const char *filename);
char *substring(const char *source, size_t start_index, size_t end_index);
int imin(int a, int b);
int imax(int a, int b);

#endif