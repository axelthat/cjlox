#include "lox.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "token.h"

void lox_run(const char *filename)
{
    char *c = util_read_file(filename);
    Scanner scanner = {
        .source = c,
    };
    scanner_init(&scanner);
    scanner_read_tokens(&scanner);

    for (size_t i = 0; i < scanner.tokens_count; ++i)
    {
        Token token = scanner.tokens[i];
        token_to_string(&token);
        token_free(&token);
    }

    free(c);
}