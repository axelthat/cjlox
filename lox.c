#include "lox.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "token.h"
#include "parser.h"
#include "interpreter.h"

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
    Expr *expr = parser_parse(&parser);

    if (parser.had_error)
    {
        fprintf(stderr, "Unexpected expression\n");
    }

    Interpreter interpreter = {
        .expr = expr,
    };
    Literal literal = intepreter_interpret(&interpreter);
    if (literal.type == LITERAL_NONE)
    {
        fprintf(stderr, "Unable to interpret\n");
    }
    else
    {
        switch (literal.type)
        {
        case LITERAL_STRING:
            fprintf(stdout, "%s\n", literal.value.s);
            break;
        case LITERAL_NUMBER:
            fprintf(stdout, "%f\n", literal.value.i);
            break;
        case LITERAL_BOOL:
            fprintf(stdout, "%s\n", literal.value.b ? "true" : "false");
            break;
        default:
            break;
        }
    }

    for (size_t i = 0; i < scanner.tokens_count; ++i)
    {
        Token token = scanner.tokens[i];
        token_free(&token);
    }

    expr_free(expr);
    intepreter_free(&literal);
    free(c);
}