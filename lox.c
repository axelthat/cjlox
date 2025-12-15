#include "lox.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "token.h"
#include "parser.h"

void lox_run(const char *filename)
{
    char *c = util_read_file(filename);
    Scanner scanner = {
        .source = c,
    };
    scanner_init(&scanner);
    scanner_tokens(&scanner);

    Parser parser = {
        .tokens = scanner.tokens,
    };
    parser_init(&parser);
    Expr expr = parser_parse(&parser);

    // printf("%s", expr.type == EXPR_TYPE_UNARY ? "true" : "false");

    if (parser.had_error)
    {
        fprintf(stdout, "Unexpected expression\n");
    }
    else
    {
        expr_print_string(&expr);
    }

    for (size_t i = 0; i < scanner.tokens_count; ++i)
    {
        Token token = scanner.tokens[i];
        // token_print_string(&token);
        token_free(&token);
    }

    expr_free(&expr);
    free(c);
}