#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdbool.h>

typedef struct
{
    const char *source;
    size_t length;
    size_t start;
    size_t current;
} Scanner;

void scanner_init(Scanner *scanner);
void scanner_read_tokens(Scanner *scanner);

#endif