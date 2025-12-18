#include "lox.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "token.h"
#include "parser.h"
#include "interpreter.h"
#include "stmt.h"

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
    Statements statements = parser_parse(&parser);

    if (parser.had_error)
    {
        fprintf(stderr, "Unexpected expression\n");
    }

    Interpreter interpreter = {
        .statements = statements,
        .environment_ptr = NULL,
    };
    intepreter_init(&interpreter);
    intepreter_interpret(&interpreter);

    // for (size_t i = 0; i < scanner.tokens_count; ++i)
    // {
    //     Token token = scanner.tokens[i];
    //     token_free(&token);
    // }

    // expr_free(expr);
    // intepreter_free(&literal);
    free(c);
}