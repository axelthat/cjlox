#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdbool.h>
#include "token.h"

#define MAX_TOKENS 256

typedef struct
{
    Token tokens[MAX_TOKENS];
    size_t tokens_count;
    const char *source;
    size_t length;
    size_t start;
    size_t current;
    size_t line;
} Scanner;

void scanner_init(Scanner *scanner);
void scanner_read_tokens(Scanner *scanner);

#endif