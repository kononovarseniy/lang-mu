#ifndef LANG_MU_EXEC_MAKE_H_INCLUDED
#define LANG_MU_EXEC_MAKE_H_INCLUDED

#include <stdbool.h>

#include "exec.h"

Expr make_none();
Expr make_bool(pExecutor exec, _Bool value);
Expr make_atom(pExecutor exec, char *name);

Expr make_int(pExecutor exec, pLongInt value);
Expr make_int_zero(pExecutor exec);
Expr make_int_one(pExecutor exec);
Expr make_int_negative_one(pExecutor exec);
Expr make_int_from_long(pExecutor exec, long value);
Expr make_real(pExecutor exec, double value);
Expr make_char(pExecutor exec, char value);
Expr make_string(pExecutor exec, pString value);
Expr make_string_from_array(pExecutor exec, char *value, size_t len);
Expr make_function(pExecutor exec, pFunction value);
Expr make_builtin_function(pExecutor exec, pBuiltinFunction func, pContext context);
Expr make_user_function(pExecutor exec, pUserFunction func, pContext context, enum FunctionType type);
Expr make_pair(pExecutor exec, Expr car, Expr cdr);
Expr make_list(pExecutor exec, Expr *arr, size_t len);

#endif // LANG_MU_EXEC_MAKE_H_INCLUDED
