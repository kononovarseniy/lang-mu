#ifndef EXEC_MATH_H_INCLUDED
#define EXEC_MATH_H_INCLUDED

#include "exec.h"
#include "stdlib/longnum.h"

Expr exec_int_one(pExecutor exec);
Expr exec_int_zero(pExecutor exec);
Expr exec_int_nagative_one(pExecutor exec);
Expr exec_long_from_int(pExecutor exec, long n);

Expr exec_int_add(pExecutor exec, Expr a, Expr b);
Expr exec_int_inc(pExecutor exec, Expr a);

#endif // EXEC_MATH_H_INCLUDED
