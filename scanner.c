#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include <string.h>
#include "util.h"

static void scanner_read_token(Scanner *scanner);
static char scanner_peek(Scanner *scanner);
static char scanner_peek_next(Scanner *scanner);
static char scanner_advance(Scanner *scanner);
static bool scanner_is_at_end(Scanner *scanner);
// static bool scanner_is_alpha(char c);
static bool scanner_is_digit(char c);
static void scanner_string(Scanner *scanner);
static void scanner_number(Scanner *scanner);

void scanner_init(Scanner *scanner)
{
    scanner->length = strlen(scanner->source);
    scanner->start = 0;
    scanner->current = 0;
}

void scanner_read_tokens(Scanner *scanner)
{
    while (!scanner_is_at_end(scanner))
    {
        scanner->start = scanner->current;
        scanner_read_token(scanner);
    }
}

static void scanner_read_token(Scanner *scanner)
{
    char c = scanner_advance(scanner);

    if (c == '"')
    {
        scanner_string(scanner);
    }
    else if (scanner_is_digit(c))
    {
        scanner_number(scanner);
    }
    else if (c == '\n')
    {
        printf("New line\n");
    }
    else
    {
        printf("Invalid token\n");
    }
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

static char scanner_advance(Scanner *scanner)
{
    return scanner->source[scanner->current++];
}

static bool scanner_is_at_end(Scanner *scanner)
{
    return scanner->current >= scanner->length;
}

// static bool scanner_is_alpha(char c)
// {
//     return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
// }

static bool scanner_is_digit(char c)
{
    return (c >= '0' && c <= '9');
}

static void scanner_string(Scanner *scanner)
{
    while (scanner_peek(scanner) != '"')
    {
        scanner_advance(scanner);
    }

    scanner_advance(scanner);

    char *value = substring(scanner->source, scanner->start, scanner->current);

    printf("String: %s\n", value);

    free(value);
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

    char *value = substring(scanner->source, scanner->start, scanner->current);

    printf("Number: %s\n", value);

    free(value);
}