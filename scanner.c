#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include <string.h>
#include "util.h"
#include "token.h"

static void scanner_get_token(Scanner *scanner);
static void scanner_add_token(Scanner *scanner, enum TokenType token_type, LiteralType literal_type, Literal literal);
static char scanner_peek(Scanner *scanner);
static char scanner_peek_next(Scanner *scanner);
static char scanner_match(Scanner *scanner, char c);
static char scanner_advance(Scanner *scanner);
static bool scanner_is_at_end(Scanner *scanner);
static bool scanner_is_alpha(char c);
static bool scanner_is_digit(char c);
static bool scanner_is_alphanumeric(char c);
static enum TokenType scanner_reserved_keyword(char *keyword);
static void scanner_string(Scanner *scanner);
static void scanner_number(Scanner *scanner);
static void scanner_identifier(Scanner *scanner);

void scanner_init(Scanner *scanner)
{
    scanner->length = strlen(scanner->source);
    scanner->start = 0;
    scanner->current = 0;
    scanner->tokens_count = 0;
    scanner->line = 1;
}

void scanner_tokens(Scanner *scanner)
{
    while (!scanner_is_at_end(scanner))
    {
        scanner_get_token(scanner);
        scanner->start = scanner->current;
    }

    scanner_add_token(scanner, TOKEN_TYPE_EOF, LITERAL_NONE, (Literal){0});
}

static void scanner_get_token(Scanner *scanner)
{
    char c = scanner_advance(scanner);

    switch (c)
    {
    case '(':
        scanner_add_token(scanner, TOKEN_TYPE_LEFT_PAREN, LITERAL_NONE, (Literal){0});
        break;
    case ')':
        scanner_add_token(scanner, TOKEN_TYPE_RIGHT_PAREN, LITERAL_NONE, (Literal){0});
        break;
    case '{':
        scanner_add_token(scanner, TOKEN_TYPE_LEFT_BRACE, LITERAL_NONE, (Literal){0});
        break;
    case '}':
        scanner_add_token(scanner, TOKEN_TYPE_RIGHT_BRACE, LITERAL_NONE, (Literal){0});
        break;
    case ',':
        scanner_add_token(scanner, TOKEN_TYPE_COMMA, LITERAL_NONE, (Literal){0});
        break;
    case '.':
        scanner_add_token(scanner, TOKEN_TYPE_DOT, LITERAL_NONE, (Literal){0});
        break;
    case '-':
        scanner_add_token(scanner, TOKEN_TYPE_MINUS, LITERAL_NONE, (Literal){0});
        break;
    case '+':
        scanner_add_token(scanner, TOKEN_TYPE_PLUS, LITERAL_NONE, (Literal){0});
        break;
    case ';':
        scanner_add_token(scanner, TOKEN_TYPE_SEMICOLON, LITERAL_NONE, (Literal){0});
        break;
    case '*':
        scanner_add_token(scanner, TOKEN_TYPE_STAR, LITERAL_NONE, (Literal){0});
        break;
    case '!':
        scanner_add_token(scanner, scanner_match(scanner, '=') ? TOKEN_TYPE_BANG_EQUAL : TOKEN_TYPE_BANG, LITERAL_NONE, (Literal){0});
        break;
    case '=':
        scanner_add_token(scanner, scanner_match(scanner, '=') ? TOKEN_TYPE_EQUAL_EQUAL : TOKEN_TYPE_EQUAL, LITERAL_NONE, (Literal){0});
        break;
    case '<':
        scanner_add_token(scanner, scanner_match(scanner, '=') ? TOKEN_TYPE_LESS_EQUAL : TOKEN_TYPE_LESS, LITERAL_NONE, (Literal){0});
        break;
    case '>':
        scanner_add_token(scanner, scanner_match(scanner, '=') ? TOKEN_TYPE_GREATER_EQUAL : TOKEN_TYPE_GREATER, LITERAL_NONE, (Literal){0});
        break;
    case '/':
        scanner_add_token(scanner, TOKEN_TYPE_SLASH, LITERAL_NONE, (Literal){0});
        break;
    case '"':
        scanner_string(scanner);
        break;
    case ' ':
    case '\r':
    case '\t':
        // Ignore whitespace.
        break;

    case '\n':
        scanner->line++;
        break;
    default:
        if (scanner_is_digit(c))
        {
            scanner_number(scanner);
        }
        else if (scanner_is_alpha(c))
        {
            scanner_identifier(scanner);
        }
        else
        {
            fprintf(stdout, "Unexpected character.");
        }
        break;
    }
}

static void scanner_add_token(Scanner *scanner, enum TokenType token_type, LiteralType literal_type, Literal literal)
{
    scanner->tokens[scanner->tokens_count++] = (Token){
        .lexeme = substring(scanner->source, scanner->start, scanner->current),
        .literal = literal,
        .literal_type = literal_type,
        .type = token_type,
    };
}

static char scanner_peek(Scanner *scanner)
{
    if (scanner_is_at_end(scanner))
    {
        return '\0';
    }
    return scanner->source[scanner->current];
}

static char scanner_peek_next(Scanner *scanner)
{
    if (scanner->current + 1 >= scanner->length)
    {
        return '\0';
    }
    return scanner->source[scanner->current + 1];
}

static char scanner_match(Scanner *scanner, char c)
{
    if (scanner_is_at_end(scanner))
    {
        return false;
    }

    if (scanner->source[scanner->current] != c)
    {
        return false;
    }

    scanner->current++;
    return true;
}

static char scanner_advance(Scanner *scanner)
{
    return scanner->source[scanner->current++];
}

static bool scanner_is_at_end(Scanner *scanner)
{
    return scanner->current >= scanner->length;
}

static bool scanner_is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool scanner_is_digit(char c)
{
    return (c >= '0' && c <= '9');
}

static bool scanner_is_alphanumeric(char c)
{
    return scanner_is_alpha(c) || scanner_is_digit(c);
}

static enum TokenType scanner_reserved_keyword(char *keyword)
{
    if (strcmp(keyword, "and") == 0)
    {
        return TOKEN_TYPE_AND;
    }
    if (strcmp(keyword, "class") == 0)
    {
        return TOKEN_TYPE_CLASS;
    }
    if (strcmp(keyword, "else") == 0)
    {
        return TOKEN_TYPE_ELSE;
    }
    if (strcmp(keyword, "false") == 0)
    {
        return TOKEN_TYPE_FALSE;
    }
    if (strcmp(keyword, "for") == 0)
    {
        return TOKEN_TYPE_FOR;
    }
    if (strcmp(keyword, "fun") == 0)
    {
        return TOKEN_TYPE_FUN;
    }
    if (strcmp(keyword, "if") == 0)
    {
        return TOKEN_TYPE_IF;
    }
    if (strcmp(keyword, "nil") == 0)
    {
        return TOKEN_TYPE_NIL;
    }
    if (strcmp(keyword, "or") == 0)
    {
        return TOKEN_TYPE_OR;
    }
    if (strcmp(keyword, "print") == 0)
    {
        return TOKEN_TYPE_PRINT;
    }
    if (strcmp(keyword, "return") == 0)
    {
        return TOKEN_TYPE_RETURN;
    }
    if (strcmp(keyword, "super") == 0)
    {
        return TOKEN_TYPE_SUPER;
    }
    if (strcmp(keyword, "this") == 0)
    {
        return TOKEN_TYPE_THIS;
    }
    if (strcmp(keyword, "true") == 0)
    {
        return TOKEN_TYPE_TRUE;
    }
    if (strcmp(keyword, "var") == 0)
    {
        return TOKEN_TYPE_VAR;
    }
    if (strcmp(keyword, "while") == 0)
    {
        return TOKEN_TYPE_WHILE;
    }

    return TOKEN_TYPE_NONE;
}

static void scanner_string(Scanner *scanner)
{
    while (scanner_peek(scanner) != '"')
    {
        scanner_advance(scanner);
    }

    scanner_advance(scanner);

    char *value = substring(scanner->source, scanner->start + 1, scanner->current - 1);

    scanner_add_token(scanner, TOKEN_TYPE_STRING, LITERAL_STRING, (Literal){.s_value = value});
}

static void scanner_number(Scanner *scanner)
{
    while (scanner_is_digit(scanner_peek(scanner)))
    {
        scanner_advance(scanner);
    }

    if (scanner_peek(scanner) == '.' && scanner_is_digit(scanner_peek_next(scanner)))
    {
        scanner_advance(scanner);

        while (scanner_is_digit(scanner_peek(scanner)))
        {
            scanner_advance(scanner);
        }
    }

    char *lexeme = substring(scanner->source, scanner->start, scanner->current);
    char *endptr;
    double value = strtod(lexeme, &endptr);

    free(lexeme);

    scanner_add_token(scanner, TOKEN_TYPE_NUMBER, LITERAL_NUMBER, (Literal){.i_value = value});
}

static void scanner_identifier(Scanner *scanner)
{
    while (scanner_is_alphanumeric(scanner_peek(scanner)))
    {
        scanner_advance(scanner);
    }

    char *value = substring(scanner->source, scanner->start, scanner->current);
    enum TokenType token_type = scanner_reserved_keyword(value);
    if (token_type == TOKEN_TYPE_NONE)
    {
        token_type = TOKEN_TYPE_IDENTIFIER;
    }

    scanner_add_token(scanner, token_type, LITERAL_STRING, (Literal){.s_value = value});
}