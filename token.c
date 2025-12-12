#include "token.h"
#include <stdlib.h>
#include <stdio.h>

void token_free(Token *token)
{
    if (token->lexeme != NULL)
    {
        free(token->lexeme);
    }

    if (token->literal_type == LITERAL_STRING && token->literal.s_value != NULL)
    {
        free(token->literal.s_value);
    }
}

void token_to_string(Token *token)
{
    char *text = "";
    char str_buffer[32];

    switch (token->literal_type)
    {
    case LITERAL_STRING:
        text = token->literal.s_value;
        break;
    case LITERAL_NUMBER:
        snprintf(str_buffer, 32, "%.4f", token->literal.i_value);
        text = str_buffer;
        break;
    case LITERAL_BOOL:
        text = token->literal.b_value ? "true" : "false";
        break;
    default:
        break;
    }

    fprintf(stdout, "%s %s %s\n", token_type_get_name(&token->token_type), token->lexeme, text);
}