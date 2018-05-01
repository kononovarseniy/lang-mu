#ifndef EXEC_MATH_H_INCLUDED
#define EXEC_MATH_H_INCLUDED

#include <stdbool.h>

#include "exec.h"
#include "stdlib/longnum_stub.h"

long exec_int_to_long(pExecutor exec, Expr num);
double exec_int_to_double(pExecutor exec, Expr num);

Expr exec_int_add(pExecutor exec, Expr a, Expr b);
Expr exec_int_sub(pExecutor exec, Expr a, Expr b);
Expr exec_int_inverse(pExecutor exec, Expr a);
Expr exec_int_inc(pExecutor exec, Expr a);

Expr exec_int_product(pExecutor exec, Expr a, Expr b);
Expr exec_int_div(pExecutor exec, Expr a, Expr b);
Expr exec_int_rem(pExecutor exec, Expr a, Expr b);
Expr exec_int_division_inverse(pExecutor exec, Expr a);
Expr exec_int_gcd(pExecutor exec, Expr a, Expr b);

Expr exec_convert_to_real(pExecutor exec, Expr expr);
Expr exec_convert_to_int(pExecutor exec, Expr expr);
Expr exec_convert_to_char(pExecutor exec, Expr expr);

_Bool exec_to_most_generic_type(pExecutor exec, Expr *a, Expr *b);

int exec_compare(pExecutor exec, Expr a, Expr b);
Expr exec_sum(pExecutor exec, Expr a, Expr b);
Expr exec_difference(pExecutor exec, Expr a, Expr b);
Expr exec_inverse(pExecutor exec, Expr a);
Expr exec_product(pExecutor exec, Expr a, Expr b);
Expr exec_quotient(pExecutor exec, Expr a, Expr b);
Expr exec_division_inverse(pExecutor exec, Expr a);
Expr exec_remainder(pExecutor exec, Expr a, Expr b);

#endif // EXEC_MATH_H_INCLUDED
