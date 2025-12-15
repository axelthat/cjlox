#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "expr.h"
#include <stdlib.h>

typedef struct
{
    Token *tokens;
    size_t current;
    bool had_error;
} Parser;

void parser_init(Parser *parser);
Expr *parser_parse(Parser *parser);

#endif