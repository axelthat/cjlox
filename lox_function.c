#include "lox_function.h"
#include "environment.h"
#include "interpreter.h"
#include <stdio.h>

Literal lox_function_call(Environment *environment_ptr, StmtFunction *stmt, Literals arguments)
{
    Environment environment = {
        .enclosing = environment_ptr,
        .entries = {
            .count = 0,
            .value = {},
        },
    };

    for (size_t i = 0; i < stmt->params.count; ++i)
    {
        environment_define(&environment, stmt->params.value[i]->lexeme, arguments.value[i]);
    }

    return interpreter_execute_block(&stmt->body, &environment).value;
}