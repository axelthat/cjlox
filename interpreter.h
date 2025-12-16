#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "expr.h"

typedef struct
{
    Expr *expr;
} Interpreter;

Literal intepreter_interpret(Interpreter *interpreter);
void intepreter_free(Literal *literal);

#endif