#include "parser.h"
#include "expr.h"
#include <stdio.h>

static bool parser_match(Parser *parser, enum TokenType token_type);
static bool parser_is_at_end(Parser *parser);
static Token *parser_advance(Parser *parser);
static Token *parser_consume(Parser *parser, enum TokenType token_type, const char *message);
static Token *parser_previous(Parser *parser);
static Token *parser_peek(Parser *parser);
static Stmt *parser_var_declaration(Parser *parser);
static Stmt *parser_declaration(Parser *parser);
static Stmt *parser_statement(Parser *parser);
static Stmt *parser_if_statement(Parser *parser);
static Stmt *parser_print_statement(Parser *parser);
static Stmt *parser_return_statement(Parser *parser);
static Stmt *parser_while_statement(Parser *parser);
static Statements parser_block(Parser *parser);
static Stmt *parser_expression_statement(Parser *parser);
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

Statements parser_parse(Parser *parser)
{
    Stmt **stmt = malloc(256 * sizeof(Stmt *));
    size_t i = 0;
    while (!parser_is_at_end(parser))
    {
        stmt[i++] = parser_declaration(parser);
    }

    return (Statements){
        .count = i,
        .value = stmt,
    };
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

static Token *parser_advance(Parser *parser)
{
    if (!parser_is_at_end(parser))
    {
        parser->current++;
    }
    return parser_previous(parser);
}

static Token *parser_consume(Parser *parser, enum TokenType token_type, const char *message)
{
    if (parser_peek(parser)->type == token_type)
    {
        return parser_advance(parser);
    }

    parser->had_error = true;
    fprintf(stderr, "%s", message);

    return NULL;
}

static Token *parser_previous(Parser *parser)
{
    return &parser->tokens[parser->current - 1];
}

static Token *parser_peek(Parser *parser)
{
    return &parser->tokens[parser->current];
}

static Stmt *parser_var_declaration(Parser *parser)
{
    Token *name = parser_consume(parser, TOKEN_TYPE_IDENTIFIER, "Expect variable name.");

    Expr *initializer = NULL;
    if (parser_match(parser, TOKEN_TYPE_EQUAL))
    {
        initializer = parser_expression(parser);
    }

    parser_consume(parser, TOKEN_TYPE_SEMICOLON, "Expect ';' after variable declaration.");

    Stmt *stmt = malloc(sizeof(Stmt));
    *stmt = (Stmt){
        .type = STMT_TYPE_VAR,
        .as.var = {.initializer = initializer, .name = name},
    };
    return stmt;
}

static Stmt *parser_declaration(Parser *parser)
{
    if (parser_match(parser, TOKEN_TYPE_VAR))
    {
        return parser_var_declaration(parser);
    }
    return parser_statement(parser);
}

static Stmt *parser_statement(Parser *parser)
{
    if (parser_match(parser, TOKEN_TYPE_IF))
    {
        return parser_if_statement(parser);
    }
    else if (parser_match(parser, TOKEN_TYPE_PRINT))
    {
        return parser_print_statement(parser);
    }
    else if (parser_match(parser, TOKEN_TYPE_RETURN))
    {
        return parser_return_statement(parser);
    }
    else if (parser_match(parser, TOKEN_TYPE_WHILE))
    {
        return parser_while_statement(parser);
    }
    else if (parser_match(parser, TOKEN_TYPE_LEFT_BRACE))
    {
        Stmt *stmt = malloc(sizeof(Stmt));
        *stmt = (Stmt){
            .type = STMT_TYPE_BLOCK,
            .as.block = {
                .statements = parser_block(parser),
            },
        };
        return stmt;
    }

    return parser_expression_statement(parser);
}

static Stmt *parser_print_statement(Parser *parser)
{
    Expr *value = parser_expression(parser);
    parser_consume(parser, TOKEN_TYPE_SEMICOLON, "Expect ';' after value.");

    Stmt *stmt = malloc(sizeof(Stmt));
    *stmt = (Stmt){
        .type = STMT_TYPE_PRINT,
        .as.print = {.value = value},
    };
    return stmt;
}

static Stmt *parser_return_statement(Parser *parser)
{
    Token *keyword = parser_previous(parser);
    Expr *value = NULL;
    if (parser_peek(parser)->type != TOKEN_TYPE_SEMICOLON)
    {
        value = parser_expression(parser);
    }

    parser_consume(parser, TOKEN_TYPE_SEMICOLON, "Expect ';' after return value.");

    Stmt *stmt = malloc(sizeof(Stmt));
    *stmt = (Stmt){
        .type = STMT_TYPE_RETURN,
        .as.returnn = {.keyword = keyword, .value = value},
    };
    return stmt;
}

static Stmt *parser_if_statement(Parser *parser)
{
    parser_consume(parser, TOKEN_TYPE_LEFT_PAREN, "Expect '(' after 'if'");
    Expr *condition = parser_expression(parser);
    parser_consume(parser, TOKEN_TYPE_RIGHT_PAREN, "Expect ')' after 'if'");

    Stmt *then_branch = parser_statement(parser);
    Stmt *else_branch = NULL;
    if (parser_match(parser, TOKEN_TYPE_ELSE))
    {
        else_branch = parser_statement(parser);
    }

    Stmt *stmt = malloc(sizeof(Stmt));
    *stmt = (Stmt){
        .type = STMT_TYPE_IF,
        .as.iff = {
            .condition = condition,
            .then_branch = then_branch,
            .else_branch = else_branch,
        },
    };
    return stmt;
}

static Stmt *parser_while_statement(Parser *parser)
{
    parser_consume(parser, TOKEN_TYPE_LEFT_PAREN, "Expect '(' after 'while'.");
    Expr *condition = parser_expression(parser);
    parser_consume(parser, TOKEN_TYPE_RIGHT_PAREN, "Expect ')' after 'while'.");
    Stmt *body = parser_statement(parser);

    Stmt *stmt = malloc(sizeof(Stmt));
    *stmt = (Stmt){
        .type = STMT_TYPE_WHILE,
        .as.whilee = {
            .condition = condition,
            .body = body,
        },
    };
    return stmt;
}

static Statements parser_block(Parser *parser)
{
    Stmt **statements = malloc(256 * sizeof(Stmt *));
    size_t i = 0;
    while (parser_peek(parser)->type != TOKEN_TYPE_RIGHT_BRACE)
    {
        statements[i++] = parser_declaration(parser);
    }

    parser_consume(parser, TOKEN_TYPE_RIGHT_BRACE, "Expect '}' after block.");
    return (Statements){
        .count = i,
        .value = statements,
    };
}

static Stmt *parser_expression_statement(Parser *parser)
{
    Expr *expr = parser_expression(parser);
    parser_consume(parser, TOKEN_TYPE_SEMICOLON, "Expect ';' after expression.");

    Stmt *stmt = malloc(sizeof(Stmt));
    *stmt = (Stmt){
        .type = STMT_TYPE_EXPRESSION,
        .as.expr = {.expr = expr},
    };
    return stmt;
}

static Expr *parser_expression(Parser *parser)
{
    return parser_assignment(parser);
}

static Expr *parser_assignment(Parser *parser)
{
    Expr *expr = parser_or(parser);
    if (parser->had_error)
    {
        return expr;
    }

    if (parser_match(parser, TOKEN_TYPE_EQUAL))
    {
        Expr *value = parser_assignment(parser);

        if (expr->type == EXPR_TYPE_VARIABLE)
        {
            Token *name = expr->as.variable.name;
            Expr *v_expr = malloc(sizeof(Expr));
            *v_expr = (Expr){
                .type = EXPR_TYPE_ASSIGN,
                .as.assign = {
                    .name = name,
                    .value = value,
                },
            };
            return v_expr;
        }
    }

    return expr;
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

        if (parser->had_error)
        {
            return expr;
        }
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

        if (parser->had_error)
        {
            return expr;
        }
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

        if (parser->had_error)
        {
            return expr;
        }
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

        if (parser->had_error)
        {
            return expr;
        }
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

        if (parser->had_error)
        {
            return expr;
        }
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

        if (parser->had_error)
        {
            return expr;
        }
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
                .operator = operator,
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
                .literal = {
                    .type = LITERAL_BOOL,
                    .value.b = false,
                },
            },
        };
    }
    else if (parser_match(parser, TOKEN_TYPE_TRUE))
    {
        expr = malloc(sizeof(Expr));
        *expr = (Expr){
            .type = EXPR_TYPE_LITERAL,
            .as.literal = {
                .literal = {
                    .type = LITERAL_BOOL,
                    .value.b = true,
                },
            },
        };
    }
    else if (parser_match(parser, TOKEN_TYPE_NIL))
    {
        expr = malloc(sizeof(Expr));
        *expr = (Expr){
            .type = EXPR_TYPE_LITERAL,
            .as.literal = {
                .literal = {
                    .type = LITERAL_STRING,
                    .value.s = NULL,
                    .is_owned = false,
                },
            },
        };
    }
    else if (parser_match(parser, TOKEN_TYPE_STRING))
    {
        expr = malloc(sizeof(Expr));
        *expr = (Expr){
            .type = EXPR_TYPE_LITERAL,
            .as.literal = {
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
