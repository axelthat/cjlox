#include "parser.h"
#include "expr.h"
#include <stdio.h>

static bool parser_match(Parser *parser, enum TokenType token_type);
static bool parser_is_at_end(Parser *parser);
static void parser_advance(Parser *parser);
static void parser_consume(Parser *parser, enum TokenType token_type, const char *message);
static Token *parser_previous(Parser *parser);
static Token *parser_peek(Parser *parser);
static Expr *parser_expression(Parser *parser);
static Expr *parser_assignment(Parser *parser);
static Expr *parser_or(Parser *parser);
static Expr *parser_and(Parser *parser);
static Expr *parser_equality(Parser *parser);
static Expr *parser_comparison(Parser *parser);
static Expr *parser_term(Parser *parser);
static Expr *parser_factor(Parser *parser);
static Expr *parser_unary(Parser *parser);
static Expr *parser_primary(Parser *parser);

void parser_init(Parser *parser)
{
    parser->current = 0;
}

Expr *parser_parse(Parser *parser)
{
    return parser_expression(parser);
}

static bool parser_is_at_end(Parser *parser)
{
    return parser_peek(parser)->type == TOKEN_TYPE_EOF;
}

static bool parser_match(Parser *parser, enum TokenType token_type)
{
    if (parser_peek(parser)->type == token_type)
    {
        parser_advance(parser);
        return true;
    }

    return false;
}

static void parser_advance(Parser *parser)
{
    if (parser_is_at_end(parser))
    {
        return;
    }
    parser->current++;
}

static void parser_consume(Parser *parser, enum TokenType token_type, const char *message)
{
    if (parser_peek(parser)->type == token_type)
    {
        parser_advance(parser);
        return;
    }

    parser->had_error = true;
    fprintf(stderr, "%s", message);
}

static Token *parser_previous(Parser *parser)
{
    return &parser->tokens[parser->current - 1];
}

static Token *parser_peek(Parser *parser)
{
    return &parser->tokens[parser->current];
}

static Expr *parser_expression(Parser *parser)
{
    return parser_assignment(parser);
}

static Expr *parser_assignment(Parser *parser)
{
    return parser_or(parser);
}

static Expr *parser_or(Parser *parser)
{
    Expr *expr = parser_and(parser);
    if (parser->had_error)
    {
        return expr;
    }

    while (parser_match(parser, TOKEN_TYPE_OR))
    {
        Token *operator = parser_previous(parser);

        Expr *right_expr = parser_and(parser);
        if (parser->had_error)
        {
            return right_expr;
        }
        Expr *left_expr = expr;

        expr = malloc(sizeof(Expr));
        *expr = (Expr){
            .type = EXPR_TYPE_LOGICAL,
            .as.logical = {
                .left = left_expr,
                .operator = operator,
                .right = right_expr,
            },
        };
    }

    return expr;
}

static Expr *parser_and(Parser *parser)
{
    Expr *expr = parser_equality(parser);
    if (parser->had_error)
    {
        return expr;
    }

    while (parser_match(parser, TOKEN_TYPE_AND))
    {
        Token *operator = parser_previous(parser);

        Expr *right_expr = parser_equality(parser);
        if (parser->had_error)
        {
            return right_expr;
        }

        Expr *left_expr = expr;

        expr = malloc(sizeof(Expr));
        *expr = (Expr){
            .type = EXPR_TYPE_LOGICAL,
            .as.logical = {
                .left = left_expr,
                .operator = operator,
                .right = right_expr,
            },
        };
    }

    return expr;
}

static Expr *parser_equality(Parser *parser)
{
    Expr *expr = parser_comparison(parser);
    if (parser->had_error)
    {
        return expr;
    }

    while (parser_match(parser, TOKEN_TYPE_BANG_EQUAL) || parser_match(parser, TOKEN_TYPE_EQUAL_EQUAL))
    {
        Token *operator = parser_previous(parser);

        Expr *right_expr = parser_comparison(parser);
        if (parser->had_error)
        {
            return right_expr;
        }
        Expr *left_expr = expr;

        expr = malloc(sizeof(Expr));
        *expr = (Expr){
            .type = EXPR_TYPE_BINARY,
            .as.binary = {
                .left = left_expr,
                .operator = operator,
                .right = right_expr,
            },
        };
    }

    return expr;
}

static Expr *parser_comparison(Parser *parser)
{
    Expr *expr = parser_term(parser);
    if (parser->had_error)
    {
        return expr;
    }

    while (parser_match(parser, TOKEN_TYPE_GREATER) || parser_match(parser, TOKEN_TYPE_GREATER_EQUAL) || parser_match(parser, TOKEN_TYPE_LESS) || parser_match(parser, TOKEN_TYPE_LESS_EQUAL))
    {
        Token *operator = parser_previous(parser);

        Expr *right_expr = parser_term(parser);
        if (parser->had_error)
        {
            return right_expr;
        }
        Expr *left_expr = expr;

        expr = malloc(sizeof(Expr));
        *expr = (Expr){
            .type = EXPR_TYPE_BINARY,
            .as.binary = {
                .left = left_expr,
                .operator = operator,
                .right = right_expr,
            },
        };
    }

    return expr;
}

static Expr *parser_term(Parser *parser)
{
    Expr *expr = parser_factor(parser);
    if (parser->had_error)
    {
        return expr;
    }

    while (parser_match(parser, TOKEN_TYPE_PLUS) || parser_match(parser, TOKEN_TYPE_MINUS))
    {
        Token *operator = parser_previous(parser);

        Expr *right_expr = parser_factor(parser);
        if (parser->had_error)
        {
            return right_expr;
        }
        Expr *left_expr = expr;

        expr = malloc(sizeof(Expr));
        *expr = (Expr){
            .type = EXPR_TYPE_BINARY,
            .as.binary = {
                .left = left_expr,
                .operator = operator,
                .right = right_expr,
            },
        };
    }

    return expr;
}

static Expr *parser_factor(Parser *parser)
{
    Expr *expr = parser_unary(parser);
    if (parser->had_error)
    {
        return expr;
    }

    while (parser_match(parser, TOKEN_TYPE_SLASH) || parser_match(parser, TOKEN_TYPE_STAR))
    {
        Token *operator = parser_previous(parser);

        Expr *right_expr = parser_unary(parser);
        if (parser->had_error)
        {
            return right_expr;
        }
        Expr *left_expr = expr;

        expr = malloc(sizeof(Expr));
        *expr = (Expr){
            .type = EXPR_TYPE_BINARY,
            .as.binary = {
                .left = left_expr,
                .operator = operator,
                .right = right_expr,
            },
        };
    }

    return expr;
}

static Expr *parser_unary(Parser *parser)
{
    if (parser_match(parser, TOKEN_TYPE_BANG) || parser_match(parser, TOKEN_TYPE_MINUS))
    {
        Token *operator = parser_previous(parser);

        Expr *expr = parser_unary(parser);
        if (parser->had_error)
        {
            return expr;
        }

        Expr *result = malloc(sizeof(Expr));
        *result = (Expr){
            .type = EXPR_TYPE_UNARY,
            .as.unary = {
                .token = operator,
                .expr = expr,
            },
        };

        return result;
    }

    return parser_primary(parser);
}

static Expr *parser_primary(Parser *parser)
{
    Expr *expr = NULL;

    if (parser_match(parser, TOKEN_TYPE_FALSE))
    {
        expr = malloc(sizeof(Expr));
        *expr = (Expr){
            .type = EXPR_TYPE_LITERAL,
            .as.literal = {
                .literal_type = LITERAL_BOOL,
                .literal.b_value = false,
            },
        };
    }
    else if (parser_match(parser, TOKEN_TYPE_TRUE))
    {
        expr = malloc(sizeof(Expr));
        *expr = (Expr){
            .type = EXPR_TYPE_LITERAL,
            .as.literal = {
                .literal_type = LITERAL_BOOL,
                .literal.b_value = true,
            },
        };
    }
    else if (parser_match(parser, TOKEN_TYPE_NIL))
    {
        expr = malloc(sizeof(Expr));
        *expr = (Expr){
            .type = EXPR_TYPE_LITERAL,
            .as.literal = {
                .literal_type = LITERAL_STRING,
                .literal.s_value = NULL,
            },
        };
    }
    else if (parser_match(parser, TOKEN_TYPE_STRING))
    {
        expr = malloc(sizeof(Expr));
        *expr = (Expr){
            .type = EXPR_TYPE_LITERAL,
            .as.literal = {
                .literal_type = LITERAL_STRING,
                .literal = parser_previous(parser)->literal,
            },
        };
    }
    else if (parser_match(parser, TOKEN_TYPE_NUMBER))
    {
        expr = malloc(sizeof(Expr));
        *expr = (Expr){
            .type = EXPR_TYPE_LITERAL,
            .as.literal = {
                .literal_type = LITERAL_NUMBER,
                .literal = parser_previous(parser)->literal,
            },
        };
    }
    else if (parser_match(parser, TOKEN_TYPE_IDENTIFIER))
    {
        expr = malloc(sizeof(Expr));
        *expr = (Expr){
            .type = EXPR_TYPE_VARIABLE,
            .as.variable = {
                .name = parser_previous(parser),
            },
        };
    }
    else if (parser_match(parser, TOKEN_TYPE_LEFT_PAREN))
    {
        Expr *expr_inner = parser_expression(parser);
        if (parser->had_error)
        {
            return expr_inner;
        }

        parser_consume(parser, TOKEN_TYPE_RIGHT_PAREN, "Expect ')' after expression.");
        if (parser->had_error)
        {
            return expr_inner;
        }

        expr = malloc(sizeof(Expr));
        *expr = (Expr){
            .type = EXPR_TYPE_GROUPING,
            .as.grouping = {
                .expr = expr_inner,
            },
        };
    }

    parser->had_error = expr == NULL;
    return expr;
}
