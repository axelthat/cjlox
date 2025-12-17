#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "expr.h"
#include "stmt.h"

typedef struct
{
    Statements statements;
} Interpreter;

void intepreter_interpret(Interpreter *interpreter);
void intepreter_free(Literal *literal);

#endif