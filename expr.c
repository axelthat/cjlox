#include "expr.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

static Expr *expr_get_expr(Expr *expr)
{
    if (expr == NULL)
    {
        return NULL;
    }

    switch (expr->type)
    {
    case EXPR_TYPE_GROUPING:
        return expr->as.grouping.expr;
    case EXPR_TYPE_UNARY:
        return expr->as.unary.expr;
    default:
        return NULL;
    }
}

void expr_free(Expr *expr)
{
    switch (expr->type)
    {
    case EXPR_TYPE_GROUPING:
    case EXPR_TYPE_UNARY:
    {
        Expr *expr_ptr = expr_get_expr(expr);
        if (expr_ptr != NULL)
        {
            expr_free(expr_ptr);
            free(expr_ptr);
        }
        break;
    }
    default:
        break;
    }
}

void expr_print_string(Expr *expr)
{
    switch (expr->type)
    {
    case EXPR_TYPE_LITERAL:
    {
        switch (expr->as.literal.literal_type)
        {
        case LITERAL_STRING:
            fprintf(stdout, "%s", expr->as.literal.literal.s_value);
            break;
        case LITERAL_NUMBER:
            fprintf(stdout, "%f", expr->as.literal.literal.i_value);
            break;
        case LITERAL_BOOL:
            fprintf(stdout, "%s", expr->as.literal.literal.b_value ? "true" : "false");
            break;
        default:
            break;
        }
        break;
    }
    case EXPR_TYPE_GROUPING:
        expr_print_string(expr->as.grouping.expr);
        break;
    case EXPR_TYPE_UNARY:
        fprintf(stdout, "%s", token_type_get_name(&expr->as.unary.token->type));
        expr_print_string(expr->as.unary.expr);
        break;
    case EXPR_TYPE_BINARY:
        fprintf(stdout, "(");
        fprintf(stdout, "%s ", token_type_get_name(&expr->as.binary.operator->type));
        expr_print_string(expr->as.binary.left);
        fprintf(stdout, " ");
        expr_print_string(expr->as.binary.right);
        fprintf(stdout, ")");
        break;
    case EXPR_TYPE_LOGICAL:
        fprintf(stdout, "(");
        fprintf(stdout, "%s ", token_type_get_name(&expr->as.logical.operator->type));
        expr_print_string(expr->as.logical.left);
        fprintf(stdout, " ");
        expr_print_string(expr->as.logical.right);
        fprintf(stdout, ")");
        break;
    default:
        break;
    }
}