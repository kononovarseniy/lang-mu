#ifndef LANG_MU_EXEC_PREDICATES_H_INCLUDED
#define LANG_MU_EXEC_PREDICATES_H_INCLUDED

#include <stdbool.h>

#include "exec.h"

_Bool is_none(Expr expr);
_Bool is_equal(Expr a, Expr b);
_Bool is_true(pExecutor exec, Expr expr);

_Bool is_atom(Expr expr);
_Bool is_pair(Expr expr);
_Bool is_char(Expr expr);
_Bool is_int(Expr expr);
_Bool is_real(Expr expr);
_Bool is_string(Expr expr);
_Bool is_function(Expr expr);
_Bool is_macro(Expr expr);
_Bool is_list(pExecutor exec, Expr expr);

#endif // LANG_MU_EXEC_PREDICATES_H_INCLUDED
