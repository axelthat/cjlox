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
    EXPR_TYPE_ASSIGN,
    EXPR_TYPE_ERROR
} ExprType;

typedef struct
{
    Literal literal;
} ExprLiteral;

typedef struct
{
    Token *name;
} ExprVariable;

typedef struct
{
    Token *name;
    Expr *value;
} ExprAssign;

typedef struct
{
    Expr *expr;
} ExprGrouping;

typedef struct
{
    Token *operator;
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
    union
    {
        ExprLiteral literal;
        ExprVariable variable;
        ExprGrouping grouping;
        ExprUnary unary;
        ExprBinary binary;
        ExprLogical logical;
        ExprAssign assign;
        ExprError error;
    } as;
};

void expr_free(Expr *expr);
void expr_print_string(Expr *expr);

#endif