#include "interpreter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "environment.h"

static void interpreter_execute(Stmt *stmt);
static Literal interpreter_evaluate(Expr *expr);
static bool interpreter_is_truthy(Literal literal);
static bool interpreter_is_equal(Literal left, Literal right);
static void interpreter_visit_block_stmt(StmtBlock *stmt);
static void interpreter_execute_block(Statements *statements, Environment *block_environment);
static void interpreter_visit_expression_stmt(StmtExpr *stmt);
static void interpreter_visit_if_stmt(StmtIf *stmt);
static void interpreter_visit_print_stmt(StmtPrint *stmt);
static void interpreter_visit_while_stmt(StmtWhile *stmt);
static void interpreter_visit_var_stmt(StmtVar *stmt);
static Literal interpreter_visit_literal_expr(ExprLiteral *expr);
static Literal interpreter_visit_assign_expr(ExprAssign *expr);
static Literal interpreter_visit_var_expr(ExprVariable *expr);
static Literal interpreter_visit_grouping_expr(ExprGrouping *expr);
static Literal interpreter_visit_unary_expr(ExprUnary *expr);
static Literal interpreter_visit_binary_expr(ExprBinary *expr);
static Literal interpreter_visit_logical_expr(ExprLogical *expr);

static Environment environment = (Environment){
    .enclosing = NULL,
    .entries = {
        .count = 0,
        .value = {},
    },
};
static Environment *environment_ptr = &environment;

void intepreter_interpret(Interpreter *interpreter)
{
    for (size_t i = 0; i < interpreter->statements.count; ++i)
    {
        Stmt *stmt = interpreter->statements.value[i];
        interpreter_execute(stmt);
    }
}

static void interpreter_execute(Stmt *stmt)
{
    switch (stmt->type)
    {
    case STMT_TYPE_BLOCK:
        interpreter_visit_block_stmt(&stmt->as.block);
        break;
    case STMT_TYPE_EXPRESSION:
        interpreter_visit_expression_stmt(&stmt->as.expr);
        break;
    case STMT_TYPE_IF:
        interpreter_visit_if_stmt(&stmt->as.iff);
        break;
    case STMT_TYPE_PRINT:
        interpreter_visit_print_stmt(&stmt->as.print);
        break;
    case STMT_TYPE_WHILE:
        interpreter_visit_while_stmt(&stmt->as.whilee);
        break;
    case STMT_TYPE_VAR:
        interpreter_visit_var_stmt(&stmt->as.var);
        break;
    default:
        break;
    }
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
    case EXPR_TYPE_ASSIGN:
        return interpreter_visit_assign_expr(&expr->as.assign);
    case EXPR_TYPE_VARIABLE:
        return interpreter_visit_var_expr(&expr->as.variable);
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

static void interpreter_visit_block_stmt(StmtBlock *stmt)
{
    Environment block_environment = {
        .enclosing = &environment,
        .entries = {
            .count = 0,
            .value = {},
        },
    };
    interpreter_execute_block(&stmt->statements, &block_environment);
}

static void interpreter_execute_block(Statements *statements, Environment *block_environment)
{
    Environment *previous = environment_ptr;
    environment_ptr = block_environment;
    for (size_t i = 0; i < statements->count; ++i)
    {
        interpreter_execute(statements->value[i]);
    }
    environment_ptr = previous;
}

static void interpreter_visit_expression_stmt(StmtExpr *stmt)
{
    interpreter_evaluate(stmt->expr);
}

static void interpreter_visit_if_stmt(StmtIf *stmt)
{
    if (interpreter_is_truthy(interpreter_evaluate(stmt->condition)))
    {
        interpreter_execute(stmt->then_branch);
    }
    else if (stmt->else_branch != NULL)
    {
        interpreter_execute(stmt->else_branch);
    }
}

static void interpreter_visit_print_stmt(StmtPrint *stmt)
{
    Literal literal = interpreter_evaluate(stmt->value);
    switch (literal.type)
    {
    case LITERAL_STRING:
        fprintf(stdout, "%s\n", literal.value.s);
        break;
    case LITERAL_NUMBER:
        fprintf(stdout, "%f\n", literal.value.i);
        break;
    case LITERAL_BOOL:
        fprintf(stdout, "%s\n", literal.value.b ? "true" : "false");
        break;
    default:
        break;
    }
}

static void interpreter_visit_while_stmt(StmtWhile *stmt)
{
    while (interpreter_is_truthy(interpreter_evaluate(stmt->condition)))
    {
        interpreter_execute(stmt->body);
    }
}

static void interpreter_visit_var_stmt(StmtVar *stmt)
{
    if (stmt->initializer != NULL)
    {
        Literal value = interpreter_evaluate(stmt->initializer);
        environment_define(environment_ptr, stmt->name->lexeme, value);
    }
}

static Literal interpreter_visit_literal_expr(ExprLiteral *expr)
{
    return expr->literal;
}

static Literal interpreter_visit_assign_expr(ExprAssign *expr)
{
    Literal value = interpreter_evaluate(expr->value);
    environment_assign(environment_ptr, expr->name->lexeme, value);
    return value;
}

static Literal interpreter_visit_var_expr(ExprVariable *expr)
{
    return *environment_get(environment_ptr, expr->name->lexeme);
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