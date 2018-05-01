#ifndef EXEC_PREDICATES_H_INCLUDED
#define EXEC_PREDICATES_H_INCLUDED

#include "exec.h"

int is_none(Expr expr);
int is_equal(Expr a, Expr b);
int is_true(pExecutor exec, Expr expr);
int is_macro(Expr expr);

int is_atom(Expr expr);
int is_pair(Expr expr);
int is_int(Expr expr);
int is_real(Expr expr);
int is_char(Expr expr);
int is_string(Expr expr);

#endif // EXEC_PREDICATES_H_INCLUDED
