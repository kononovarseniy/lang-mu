#include "exec_math.h"
#include "log.h"

// Helpers
Expr exec_long_binary_operation(pExecutor exec, Expr a, Expr b, pLongNum (*op)(pLongNum, pLongNum), char *caller)
{
    if (!is_int(a) || !is_int(b))
    {
        logf("%s: argument is not integer", caller);
        exit(1);
    }
    pLongNum num = op(dereference(a).val_int, dereference(b).val_int);
    Expr res = make_int(exec, num);
    free_longnum(num);
    if (is_none(res))
    {
        logf("%s: make_int failed", caller);
        return expr_none();
    }
    return res;
}


Expr exec_long_one(pExecutor exec)
{
    pLongNum val = longnum_one();
    Expr res = make_int(exec, val);
    free_longnum(val);
    if (is_none(res))
    {
        log("exec_int_one: make_int failed");
        return expr_none();
    }
    return res;
}
Expr exec_long_zero(pExecutor exec)
{
    pLongNum val = longnum_zero();
    Expr res = make_int(exec, val);
    free_longnum(val);
    if (is_none(res))
    {
        log("exec_int_zero: make_int failed");
        return expr_none();
    }
    return res;
}
Expr exec_long_negative_one(pExecutor exec)
{
    pLongNum val = longnum_negative_one();
    Expr res = make_int(exec, val);
    free_longnum(val);
    if (is_none(res))
    {
        log("exec_int_negative_one: make_int failed");
        return expr_none();
    }
    return res;
}


Expr exec_long_from_int(pExecutor exec, long val)
{
    pLongNum num = longnum_from_int(val);
    Expr res = make_int(exec, num);
    free_longnum(num);
    if (is_none(res))
    {
        log("exec_long_from_int: make_int failed");
        return expr_none();
    }
    return res;
}
Expr exec_long_from_double(pExecutor exec, double val)
{
    log("exec_long_from_double: not implemented");
    return expr_none();
}
long exec_long_to_int(pExecutor exec, pLongNum num)
{
    log("exec_long_to_int: not implemented");
    return 0;
}
double exec_long_to_double(pExecutor exec, pLongNum num)
{
    log("exec_long_to_double: not implemented");
    return 0;
}


Expr exec_long_add(pExecutor exec, Expr a, Expr b)
{
    return exec_long_binary_operation(exec, a, b, longnum_add, "exec_long_add");
}
Expr exec_long_sub(pExecutor exec, Expr a, Expr b)
{
    return exec_long_binary_operation(exec, a, b, longnum_sub, "exec_long_sub");
}
Expr exec_long_inc(pExecutor exec, Expr a)
{
    if (!is_int(a))
    {
        log("exec_int_inc: argument is not integer");
        exit(1);
    }
    pLongNum one = longnum_one();
    pLongNum num = longnum_add(dereference(a).val_int, one);
    Expr res = make_int(exec, num);
    free_longnum(one);
    free_longnum(num);
    if (is_none(res))
    {
        log("exec_int_inc: make_int failed");
        return expr_none();
    }
    return res;
}


Expr exec_long_product(pExecutor exec, Expr a, Expr b)
{
    return exec_long_binary_operation(exec, a, b, longnum_product, "exec_long_product");
}
Expr exec_long_div(pExecutor exec, Expr a, Expr b)
{
    return exec_long_binary_operation(exec, a, b, longnum_div, "exec_long_div");
}
Expr exec_long_rem(pExecutor exec, Expr a, Expr b)
{
    return exec_long_binary_operation(exec, a, b, longnum_rem, "exec_long_rem");
}
Expr exec_long_gcd(pExecutor exec, Expr a, Expr b)
{
    return exec_long_binary_operation(exec, a, b, longnum_greatest_common_divisor, "exec_long_gcd");
}


int exec_long_compare(Expr a, Expr b)
{
    if (!is_int(a) || !is_int(b))
    {
        log("exec_long_comapre: argument is not integer");
        exit(1);
    }
    return longnum_compare(dereference(a).val_int, dereference(b).val_int);
}
int exec_long_equals(Expr a, Expr b)
{
    return exec_long_equals(a, b);
}
