#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "token.h"
#include <stdlib.h>

typedef struct Environment Environment;

typedef struct
{
    size_t count;
    struct
    {
        char *key;
        Literal value;
    } value[256];
} Entries;

struct Environment
{
    Entries entries;
    Environment *enclosing;
};

Literal *environment_get(Environment *environment, char *key);
void environment_define(Environment *environment, char *key, Literal value);
void environment_assign(Environment *environment, char *key, Literal value);

#endif