#include "token.h"
#include <stdlib.h>
#include <stdio.h>

void token_free(Token *token)
{
    if (token->lexeme != NULL)
    {
        free(token->lexeme);
    }

    if (token->literal.type == LITERAL_STRING && token->literal.value.s != NULL)
    {
        free(token->literal.value.s);
    }
}

void token_print_string(Token *token)
{
    char *text = "";
    char str_buffer[32];

    switch (token->literal.type)
    {
    case LITERAL_STRING:
        text = token->literal.value.s;
        break;
    case LITERAL_NUMBER:
        snprintf(str_buffer, 32, "%.4f", token->literal.value.i);
        text = str_buffer;
        break;
    case LITERAL_BOOL:
        text = token->literal.value.b ? "true" : "false";
        break;
    default:
        break;
    }

    fprintf(stdout, "%s %s %s\n", token_type_get_name(&token->type), token->lexeme, text);
}