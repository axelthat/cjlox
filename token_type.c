#include "token_type.h"
#include <stdlib.h>
#include <stdio.h>

char *token_type_get_name(enum TokenType *token_type)
{
    // printf("%d\n", *token_type);

    switch (*token_type)
    {
    case TOKEN_TYPE_LEFT_PAREN:
        return "(";
    case TOKEN_TYPE_RIGHT_PAREN:
        return ")";
    case TOKEN_TYPE_LEFT_BRACE:
        return "{";
    case TOKEN_TYPE_RIGHT_BRACE:
        return "}";
    case TOKEN_TYPE_COMMA:
        return ",";
    case TOKEN_TYPE_DOT:
        return ".";
    case TOKEN_TYPE_MINUS:
        return "-";
    case TOKEN_TYPE_PLUS:
        return "+";
    case TOKEN_TYPE_SEMICOLON:
        return ";";
    case TOKEN_TYPE_SLASH:
        return "/";
    case TOKEN_TYPE_STAR:
        return "*";
    case TOKEN_TYPE_BANG:
        return "!";
    case TOKEN_TYPE_BANG_EQUAL:
        return "!=";
    case TOKEN_TYPE_EQUAL:
        return "=";
    case TOKEN_TYPE_EQUAL_EQUAL:
        return "==";
    case TOKEN_TYPE_GREATER:
        return ">";
    case TOKEN_TYPE_GREATER_EQUAL:
        return ">=";
    case TOKEN_TYPE_LESS:
        return "<";
    case TOKEN_TYPE_LESS_EQUAL:
        return "<=";
    case TOKEN_TYPE_IDENTIFIER:
        return "IDENTIFIER";
    case TOKEN_TYPE_STRING:
        return "STRING";
    case TOKEN_TYPE_NUMBER:
        return "NUMBER";
    case TOKEN_TYPE_AND:
        return "&&";
    case TOKEN_TYPE_CLASS:
        return "CLASS";
    case TOKEN_TYPE_ELSE:
        return "else";
    case TOKEN_TYPE_FALSE:
        return "false";
    case TOKEN_TYPE_FUN:
        return "FUN";
    case TOKEN_TYPE_FOR:
        return "for";
    case TOKEN_TYPE_IF:
        return "if";
    case TOKEN_TYPE_NIL:
        return "nil";
    case TOKEN_TYPE_OR:
        return "||";
    case TOKEN_TYPE_PRINT:
        return "print";
    case TOKEN_TYPE_RETURN:
        return "return";
    case TOKEN_TYPE_SUPER:
        return "SUPER";
    case TOKEN_TYPE_THIS:
        return "this";
    case TOKEN_TYPE_TRUE:
        return "true";
    case TOKEN_TYPE_VAR:
        return "var";
    case TOKEN_TYPE_WHILE:
        return "WHILE";
    case TOKEN_TYPE_EOF:
        return "EOF";
    default:
        return NULL;
    }
}