#ifndef STMT_H
#define STMT_H

#include <stdlib.h>
#include "expr.h"

typedef struct Stmt Stmt;

typedef enum
{
    STMT_TYPE_BLOCK,
    STMT_TYPE_EXPRESSION,
    STMT_TYPE_FUNCTION,
    STMT_TYPE_IF,
    STMT_TYPE_PRINT,
    STMT_TYPE_RETURN,
    STMT_TYPE_VAR,
    STMT_TYPE_WHILE,
} StmtType;

typedef struct
{
    size_t count;
    Stmt **value;
} Statements;

typedef struct
{
    Expr *condition;
    Stmt *then_branch;
    Stmt *else_branch;
} StmtIf;

typedef struct
{
    Expr *value;
} StmtPrint;

typedef struct
{
    Token *keyword;
    Expr *value;
} StmtReturn;

typedef struct
{
    Expr *condition;
    Stmt *body;
} StmtWhile;

typedef struct
{
    Statements statements;
} StmtBlock;

typedef struct
{
    Expr *expr;
} StmtExpr;

typedef struct
{
    Token *name;
    Expr *initializer;
} StmtVar;

struct StmtFunction
{
    Token *name;
    Tokens params;
    Statements body;
};

struct Stmt
{
    StmtType type;
    union
    {
        StmtIf iff;
        StmtPrint print;
        StmtReturn returnn;
        StmtWhile whilee;
        StmtBlock block;
        StmtExpr expr;
        StmtVar var;
        StmtFunction function;
    } as;
};

#endif