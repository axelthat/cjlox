#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "expr.h"
#include "stmt.h"
#include "environment.h"
#include "token.h"

typedef struct
{
    Statements statements;
    Environment *environment_ptr;
} Interpreter;

typedef enum
{
    INTERPRETER_RESULT_TYPE_NEXT,
    INTERPRETER_RESULT_TYPE_RETURN,
    INTERPRETER_RESULT_TYPE_NONE,
} InterpreterResultType;

typedef struct
{
    InterpreterResultType type;
    Literal value;
} InterpreterResult;

void intepreter_init(Interpreter *interpreter);
void intepreter_interpret(Interpreter *interpreter);
InterpreterResult interpreter_execute_block(Statements *statements, Environment *block_environment);
void intepreter_free(Literal *literal);

#endif