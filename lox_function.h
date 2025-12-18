#ifndef LOX_FUNCTION_H
#define LOX_FUNCTION_H

#include "token.h"
#include "stmt.h"
#include "environment.h"

typedef Literal (*LoxCallableFn)(Environment *environment_ptr, StmtFunction *stmt, Literals arguments);

Literal lox_function_call(Environment *environment_ptr, StmtFunction *stmt, Literals arguments);

#endif