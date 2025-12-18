#ifndef TOKEN_H
#define TOKEN_H

#include "token_type.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct StmtFunction StmtFunction;

typedef enum
{
    LITERAL_STRING,
    LITERAL_NUMBER,
    LITERAL_BOOL,
    LITERAL_FUNCTION,
    LITERAL_NONE
} LiteralType;

typedef struct
{
    struct StmtFunction *stmt;
    void *f;
} LiteralFunction;

typedef struct
{
    LiteralType type;
    union
    {
        char *s;
        double i;
        bool b;
        LiteralFunction f;
    } value;
    bool is_owned;
} Literal;

typedef struct
{
    size_t count;
    Literal *value;
} Literals;

typedef struct
{
    enum TokenType type;
    char *lexeme;
    Literal literal;
} Token;

typedef struct
{
    size_t count;
    Token **value;
} Tokens;

void token_free(Token *token);
void token_print_string(Token *token);

#endif