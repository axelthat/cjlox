#ifndef TOKEN_H
#define TOKEN_H

#include "token_type.h"
#include <stdbool.h>

typedef enum
{
    LITERAL_STRING,
    LITERAL_NUMBER,
    LITERAL_BOOL,
    LITERAL_NONE
} LiteralType;

typedef struct
{
    LiteralType type;
    union
    {
        char *s;
        double i;
        bool b;
    } value;
    bool is_owned;
} Literal;

typedef struct
{
    enum TokenType type;
    char *lexeme;
    Literal literal;
} Token;

void token_free(Token *token);
void token_print_string(Token *token);

#endif