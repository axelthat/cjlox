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

typedef union
{
    char *s_value;
    double i_value;
    bool b_value;
} Literal;

typedef struct
{
    enum TokenType type;
    char *lexeme;
    LiteralType literal_type;
    Literal literal;
} Token;

void token_free(Token *token);
void token_print_string(Token *token);

#endif