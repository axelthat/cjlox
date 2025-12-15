#include "parser.h"
#include "expr.h"
#include <stdio.h>

static bool parser_match(Parser *parser, enum TokenType *token_types, size_t token_types_count);
static bool parser_is_at_end(Parser *parser);
static void parser_advance(Parser *parser);
static void parser_consume(Parser *parser, enum TokenType token_type, const char *message);
static Token *parser_previous(Parser *parser);
static Token *parser_peek(Parser *parser);
static Expr parser_expression(Parser *parser);
static Expr parser_assignment(Parser *parser);
static Expr parser_or(Parser *parser);
static Expr parser_and(Parser *parser);
static Expr parser_equality(Parser *parser);
static Expr parser_comparison(Parser *parser);
static Expr parser_term(Parser *parser);
static Expr parser_factor(Parser *parser);
static Expr parser_unary(Parser *parser);
static Expr parser_primary(Parser *parser);

void parser_init(Parser *parser)
{
    parser->current = 0;
}

Expr parser_parse(Parser *parser)
{
    return parser_expression(parser);
}

static bool parser_is_at_end(Parser *parser)
{
    return parser_peek(parser)->type == TOKEN_TYPE_EOF;
}

static bool parser_match(Parser *parser, enum TokenType *token_types, size_t token_types_count)
{
    for (size_t i = 0; i < token_types_count; ++i)
    {
        if (parser_peek(parser)->type == token_types[i])
        {
            parser_advance(parser);
            return true;
        }
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
    if (parser_match(parser, (enum TokenType[1]){token_type}, 1))
    {
        parser_advance(parser);
        return;
    }

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

static Expr parser_expression(Parser *parser)
{
    return parser_assignment(parser);
}

static Expr parser_assignment(Parser *parser)
{
    return parser_or(parser);
}

static Expr parser_or(Parser *parser)
{
    Expr expr = parser_and(parser);
    if (parser->had_error)
    {
        return expr;
    }

    while (parser_match(parser, (enum TokenType[1]){TOKEN_TYPE_OR}, 1))
    {
        Token *operator = parser_previous(parser);

        Expr right_expr = parser_and(parser);
        if (parser->had_error)
        {
            return right_expr;
        }
        Expr *right_expr_ptr = malloc(sizeof(right_expr));
        *right_expr_ptr = right_expr;

        Expr *left_expr_ptr = malloc(sizeof(expr));
        *left_expr_ptr = expr;

        expr = (Expr){
            .type = EXPR_TYPE_LOGICAL,
            .as.logical = {
                .left = left_expr_ptr,
                .operator = operator,
                .right = right_expr_ptr,
            },
        };
    }

    return expr;
}

static Expr parser_and(Parser *parser)
{
    Expr expr = parser_equality(parser);
    if (parser->had_error)
    {
        return expr;
    }

    while (parser_match(parser, (enum TokenType[1]){TOKEN_TYPE_AND}, 1))
    {
        Token *operator = parser_previous(parser);

        Expr right_expr = parser_equality(parser);
        if (parser->had_error)
        {
            return right_expr;
        }
        Expr *right_expr_ptr = malloc(sizeof(right_expr));
        *right_expr_ptr = right_expr;

        Expr *left_expr_ptr = malloc(sizeof(expr));
        *left_expr_ptr = expr;

        expr = (Expr){
            .type = EXPR_TYPE_LOGICAL,
            .as.logical = {
                .left = left_expr_ptr,
                .operator = operator,
                .right = right_expr_ptr,
            },
        };
    }

    return expr;
}

static Expr parser_equality(Parser *parser)
{
    Expr expr = parser_comparison(parser);
    if (parser->had_error)
    {
        return expr;
    }

    while (parser_match(parser, (enum TokenType[2]){TOKEN_TYPE_BANG_EQUAL, TOKEN_TYPE_EQUAL_EQUAL}, 2))
    {
        Token *operator = parser_previous(parser);

        Expr right_expr = parser_comparison(parser);
        if (parser->had_error)
        {
            return right_expr;
        }
        Expr *right_expr_ptr = malloc(sizeof(right_expr));
        *right_expr_ptr = right_expr;

        Expr *left_expr_ptr = malloc(sizeof(expr));
        *left_expr_ptr = expr;

        expr = (Expr){
            .type = EXPR_TYPE_BINARY,
            .as.binary = {
                .left = left_expr_ptr,
                .operator = operator,
                .right = right_expr_ptr,
            },
        };
    }

    return expr;
}

static Expr parser_comparison(Parser *parser)
{
    Expr expr = parser_term(parser);
    if (parser->had_error)
    {
        return expr;
    }

    while (parser_match(parser, (enum TokenType[4]){TOKEN_TYPE_GREATER, TOKEN_TYPE_GREATER_EQUAL, TOKEN_TYPE_LESS, TOKEN_TYPE_LESS_EQUAL}, 4))
    {
        Token *operator = parser_previous(parser);

        Expr right_expr = parser_term(parser);
        if (parser->had_error)
        {
            return right_expr;
        }
        Expr *right_expr_ptr = malloc(sizeof(right_expr));
        *right_expr_ptr = right_expr;

        Expr *left_expr_ptr = malloc(sizeof(expr));
        *left_expr_ptr = expr;

        expr = (Expr){
            .type = EXPR_TYPE_BINARY,
            .as.binary = {
                .left = left_expr_ptr,
                .operator = operator,
                .right = right_expr_ptr,
            },
        };
    }

    return expr;
}

static Expr parser_term(Parser *parser)
{
    Expr expr = parser_factor(parser);
    if (parser->had_error)
    {
        return expr;
    }

    while (parser_match(parser, (enum TokenType[2]){TOKEN_TYPE_PLUS, TOKEN_TYPE_MINUS}, 2))
    {
        Token *operator = parser_previous(parser);

        Expr right_expr = parser_factor(parser);
        if (parser->had_error)
        {
            return right_expr;
        }
        Expr *right_expr_ptr = malloc(sizeof(right_expr));
        *right_expr_ptr = right_expr;

        Expr *left_expr_ptr = malloc(sizeof(expr));
        *left_expr_ptr = expr;

        expr = (Expr){
            .type = EXPR_TYPE_BINARY,
            .as.binary = {
                .left = left_expr_ptr,
                .operator = operator,
                .right = right_expr_ptr,
            },
        };
    }

    return expr;
}

static Expr parser_factor(Parser *parser)
{
    Expr expr = parser_unary(parser);
    if (parser->had_error)
    {
        return expr;
    }

    while (parser_match(parser, (enum TokenType[2]){TOKEN_TYPE_SLASH, TOKEN_TYPE_STAR}, 2))
    {
        Token *operator = parser_previous(parser);

        Expr right_expr = parser_unary(parser);
        if (parser->had_error)
        {
            return right_expr;
        }
        Expr *right_expr_ptr = malloc(sizeof(right_expr));
        *right_expr_ptr = right_expr;

        Expr *left_expr_ptr = malloc(sizeof(expr));
        *left_expr_ptr = expr;

        expr = (Expr){
            .type = EXPR_TYPE_BINARY,
            .as.binary = {
                .left = left_expr_ptr,
                .operator = operator,
                .right = right_expr_ptr,
            },
        };
    }

    return expr;
}

static Expr parser_unary(Parser *parser)
{
    if (parser_match(parser, (enum TokenType[2]){TOKEN_TYPE_BANG, TOKEN_TYPE_MINUS}, 2))
    {
        Token *operator = parser_previous(parser);

        Expr expr = parser_unary(parser);
        if (parser->had_error)
        {
            return expr;
        }
        Expr *expr_ptr = malloc(sizeof(expr));
        *expr_ptr = expr;

        return (Expr){
            .type = EXPR_TYPE_UNARY,
            .as.unary = {
                .token = operator,
                .expr = expr_ptr,
            },
        };
    }

    return parser_primary(parser);
}

static Expr parser_primary(Parser *parser)
{
    if (parser_match(parser, (enum TokenType[1]){TOKEN_TYPE_FALSE}, 1))
    {
        return (Expr){
            .type = EXPR_TYPE_LITERAL,
            .as.literal = {
                .literal_type = LITERAL_BOOL,
                .literal.b_value = false,
            },
        };
    }

    if (parser_match(parser, (enum TokenType[1]){TOKEN_TYPE_TRUE}, 1))
    {
        return (Expr){
            .type = EXPR_TYPE_LITERAL,
            .as.literal = {
                .literal_type = LITERAL_BOOL,
                .literal.b_value = true,
            },
        };
    }

    if (parser_match(parser, (enum TokenType[1]){TOKEN_TYPE_NIL}, 1))
    {
        return (Expr){
            .type = EXPR_TYPE_LITERAL,
            .as.literal = {
                .literal_type = LITERAL_STRING,
                .literal.s_value = NULL,
            },
        };
    }

    if (parser_match(parser, (enum TokenType[1]){TOKEN_TYPE_STRING}, 1))
    {
        return (Expr){
            .type = EXPR_TYPE_LITERAL,
            .as.literal = {
                .literal_type = LITERAL_STRING,
                .literal = parser_previous(parser)->literal,
            },
        };
    }

    if (parser_match(parser, (enum TokenType[1]){TOKEN_TYPE_NUMBER}, 1))
    {
        return (Expr){
            .type = EXPR_TYPE_LITERAL,
            .as.literal = {
                .literal_type = LITERAL_NUMBER,
                .literal = parser_previous(parser)->literal,
            },
        };
    }

    if (parser_match(parser, (enum TokenType[1]){TOKEN_TYPE_IDENTIFIER}, 1))
    {
        return (Expr){
            .type = EXPR_TYPE_VARIABLE,
            .as.variable = {
                .name = parser_previous(parser),
            },
        };
    }

    if (parser_match(parser, (enum TokenType[1]){TOKEN_TYPE_LEFT_PAREN}, 1))
    {
        Expr expr = parser_expression(parser);
        if (parser->had_error)
        {
            return expr;
        }
        Expr *expr_ptr = malloc(sizeof(expr));
        *expr_ptr = expr;

        parser_consume(parser, TOKEN_TYPE_RIGHT_PAREN, "Expect ')' after expression.");
        return (Expr){
            .type = EXPR_TYPE_GROUPING,
            .as.grouping = {
                .expr = expr_ptr,
            },
        };
    }

    parser->had_error = true;
    return EXPR_ERROR;
}
