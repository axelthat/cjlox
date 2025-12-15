#ifndef EXPR_H
#define EXPR_H

#include "token.h"

typedef struct Expr Expr;

typedef enum
{
    EXPR_TYPE_LITERAL,
    EXPR_TYPE_VARIABLE,
    EXPR_TYPE_GROUPING,
    EXPR_TYPE_UNARY,
    EXPR_TYPE_BINARY,
    EXPR_TYPE_LOGICAL,
    EXPR_TYPE_ERROR
} ExprType;

typedef struct
{
    LiteralType literal_type;
    Literal literal;
} ExprLiteral;

typedef struct
{
    Token *name;
} ExprVariable;

typedef struct
{
    Expr *expr;
} ExprGrouping;

typedef struct
{
    Token *token;
    Expr *expr;
} ExprUnary;

typedef struct
{
    Expr *left;
    Token *operator;
    Expr *right;
} ExprBinary;

typedef struct
{
    Expr *left;
    Token *operator;
    Expr *right;
} ExprLogical;

typedef struct
{
    char value;
} ExprError;

struct Expr
{
    ExprType type;
    union expr
    {
        ExprLiteral literal;
        ExprVariable variable;
        ExprGrouping grouping;
        ExprUnary unary;
        ExprBinary binary;
        ExprLogical logical;
        ExprError error;
    } as;
};

#define EXPR_ERROR ((Expr){.type = EXPR_TYPE_ERROR, .as.error = {0}})

void expr_free(Expr *expr);
void expr_print_string(Expr *expr);

#endif