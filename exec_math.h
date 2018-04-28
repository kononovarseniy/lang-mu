#ifndef EXEC_MATH_H_INCLUDED
#define EXEC_MATH_H_INCLUDED

#include "exec.h"
#include "stdlib/longnum_stub.h"

Expr exec_long_one(pExecutor exec);
Expr exec_long_zero(pExecutor exec);
Expr exec_long_negative_one(pExecutor exec);

Expr exec_long_from_int(pExecutor exec, long val);
Expr exec_long_from_double(pExecutor exec, double val);
long exec_long_to_int(pExecutor exec, Expr num);
double exec_long_to_double(pExecutor exec, Expr num);

Expr exec_long_add(pExecutor exec, Expr a, Expr b);
Expr exec_long_sub(pExecutor exec, Expr a, Expr b);
Expr exec_long_inc(pExecutor exec, Expr a);

Expr exec_long_product(pExecutor exec, Expr a, Expr b);
Expr exec_long_div(pExecutor exec, Expr a, Expr b);
Expr exec_long_rem(pExecutor exec, Expr a, Expr b);
Expr exec_long_gcd(pExecutor exec, Expr a, Expr b);

int exec_long_compare(Expr a, Expr b);
int exec_long_equals(Expr a, Expr b);

#endif // EXEC_MATH_H_INCLUDED
