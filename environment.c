#include "environment.h"
#include <string.h>

Literal *environment_get(Environment *environment, char *key)
{
    for (size_t i = 0; i < environment->entries.count; ++i)
    {
        if (strcmp(environment->entries.value[i].key, key) == 0)
        {
            return &environment->entries.value[i].value;
        }
    }

    return NULL;
}

void environment_define(Environment *environment, char *key, Literal value)
{
    environment->entries.count++;
    environment->entries.value[environment->entries.count - 1].key = key;
    environment->entries.value[environment->entries.count - 1].value = value;
}

void environment_assign(Environment *environment, char *key, Literal value)
{
    Literal *entry = environment_get(environment, key);
    if (entry != NULL)
    {
        *entry = value;
    }
}