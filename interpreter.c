#include "interpreter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static Literal interpreter_evaluate(Expr *expr);
static bool interpreter_is_truthy(Literal literal);
static bool interpreter_is_equal(Literal left, Literal right);
static Literal interpreter_visit_literal_expr(ExprLiteral *expr);
static Literal interpreter_visit_grouping_expr(ExprGrouping *expr);
static Literal interpreter_visit_unary_expr(ExprUnary *expr);
static Literal interpreter_visit_binary_expr(ExprBinary *expr);
static Literal interpreter_visit_logical_expr(ExprLogical *expr);

Literal intepreter_interpret(Interpreter *interpreter)
{
    return interpreter_evaluate(interpreter->expr);
}

static Literal interpreter_evaluate(Expr *expr)
{
    switch (expr->type)
    {
    case EXPR_TYPE_LITERAL:
        return interpreter_visit_literal_expr(&expr->as.literal);
    case EXPR_TYPE_GROUPING:
        return interpreter_visit_grouping_expr(&expr->as.grouping);
    case EXPR_TYPE_UNARY:
        return interpreter_visit_unary_expr(&expr->as.unary);
    case EXPR_TYPE_BINARY:
        return interpreter_visit_binary_expr(&expr->as.binary);
    case EXPR_TYPE_LOGICAL:
        return interpreter_visit_logical_expr(&expr->as.logical);
    default:
        break;
    }

    return (Literal){
        .type = LITERAL_NONE,
        .value.s = NULL,
    };
}

static bool interpreter_is_truthy(Literal literal)
{
    if (literal.type == LITERAL_NONE)
    {
        return false;
    }

    if (literal.type == LITERAL_BOOL)
    {
        return literal.value.b;
    }

    return true;
}

static bool interpreter_is_equal(Literal left, Literal right)
{
    if (left.type == LITERAL_NONE && right.type == LITERAL_NONE)
    {
        return true;
    }
    if (left.type == LITERAL_NONE)
    {
        return false;
    }

    if (left.type == LITERAL_NUMBER && right.type == LITERAL_NUMBER)
    {
        return left.value.i == right.value.i;
    }

    if (left.type == LITERAL_BOOL && right.type == LITERAL_BOOL)
    {
        return left.value.b == right.value.b;
    }

    if (left.type == LITERAL_STRING && right.type == LITERAL_STRING)
    {
        return strcmp(left.value.s, right.value.s) == 0;
    }

    return false;
}

static Literal interpreter_visit_literal_expr(ExprLiteral *expr)
{
    return expr->literal;
}

static Literal interpreter_visit_grouping_expr(ExprGrouping *expr)
{
    return interpreter_evaluate(expr->expr);
}

static Literal interpreter_visit_unary_expr(ExprUnary *expr)
{
    Literal right = interpreter_evaluate(expr->expr);

    switch (expr->operator->type)
    {
    case TOKEN_TYPE_BANG:
    {
        Literal b_literal = right;
        b_literal.value.b = !interpreter_is_truthy(b_literal);
        return b_literal;
    }
    case TOKEN_TYPE_MINUS:
    {
        Literal m_literal = right;
        m_literal.value.i = -m_literal.value.i;
        return m_literal;
    }
    default:
        break;
    }

    return (Literal){
        .type = LITERAL_NONE,
        .value.s = NULL,
    };
}

static Literal interpreter_visit_binary_expr(ExprBinary *expr)
{
    Literal left = interpreter_evaluate(expr->left);
    Literal right = interpreter_evaluate(expr->right);

    switch (expr->operator->type)
    {
    case TOKEN_TYPE_GREATER:
        return (Literal){
            .type = LITERAL_BOOL,
            .value.b = left.value.i > right.value.i,
        };
    case TOKEN_TYPE_GREATER_EQUAL:
        return (Literal){
            .type = LITERAL_BOOL,
            .value.b = left.value.i >= right.value.i,
        };
        break;
    case TOKEN_TYPE_LESS:
        return (Literal){
            .type = LITERAL_BOOL,
            .value.b = left.value.i < right.value.i,
        };
        break;
    case TOKEN_TYPE_LESS_EQUAL:
        return (Literal){
            .type = LITERAL_BOOL,
            .value.b = left.value.i <= right.value.i,
        };
        break;
    case TOKEN_TYPE_MINUS:
        return (Literal){
            .type = LITERAL_NUMBER,
            .value.i = left.value.i - right.value.i,
        };
        break;
    case TOKEN_TYPE_PLUS:
        if (left.type == LITERAL_NUMBER && right.type == LITERAL_NUMBER)
        {
            return (Literal){
                .type = LITERAL_NUMBER,
                .value.i = left.value.i + right.value.i,
            };
        }

        if (left.type == LITERAL_STRING && right.type == LITERAL_STRING)
        {
            size_t len1 = strlen(left.value.s);
            size_t len2 = strlen(right.value.s);
            char *result = malloc(len1 + len2 + 1);
            memcpy(result, left.value.s, len1);
            memcpy(result + len1, right.value.s, len2 + 1);

            return (Literal){
                .type = LITERAL_STRING,
                .value.s = result,
                .is_owned = true,
            };
        }
        break;
    case TOKEN_TYPE_SLASH:
        return (Literal){
            .type = LITERAL_NUMBER,
            .value.i = left.value.i / right.value.i,
        };
        break;
    case TOKEN_TYPE_STAR:
        return (Literal){
            .type = LITERAL_NUMBER,
            .value.i = left.value.i * right.value.i,
        };
        break;
    case TOKEN_TYPE_BANG_EQUAL:
        return (Literal){
            .type = LITERAL_BOOL,
            .value.b = !interpreter_is_equal(left, right),
        };
        break;
    case TOKEN_TYPE_EQUAL_EQUAL:
        return (Literal){
            .type = LITERAL_BOOL,
            .value.b = interpreter_is_equal(left, right),
        };
        break;
    default:
        break;
    }

    return (Literal){
        .type = LITERAL_NONE,
        .value.s = NULL,
    };
}

static Literal interpreter_visit_logical_expr(ExprLogical *expr)
{
    Literal left = interpreter_evaluate(expr->left);

    if (expr->operator->type == TOKEN_TYPE_OR)
    {
        if (interpreter_is_truthy(left))
            return left;
    }
    else
    {
        if (!interpreter_is_truthy(left))
            return left;
    }

    return interpreter_evaluate(expr->right);
}

void intepreter_free(Literal *literal)
{
    if (literal->type == LITERAL_STRING && literal->is_owned)
    {
        free(literal->value.s);
    }
}