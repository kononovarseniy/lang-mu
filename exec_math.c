#include "exec_math.h"
#include "log.h"

Expr exec_int_one(pExecutor exec)
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
Expr exec_int_zero(pExecutor exec)
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
Expr exec_int_nagative_one(pExecutor exec)
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
Expr exec_long_from_int(pExecutor exec, long n)
{
    pLongNum val = longnum_from_int(n);
    Expr res = make_int(exec, val);
    free_longnum(val);
    if (is_none(res))
    {
        log("exec_long_from_int: make_int failed");
        return expr_none();
    }
    return res;
}

Expr exec_int_add(pExecutor exec, Expr a, Expr b)
{
    if (!is_int(a) || !is_int(b))
    {
        log("exec_int_add: argument is not integer");
        exit(1);
    }
    pLongNum sum = longnum_add(dereference(a).val_int, dereference(b).val_int);
    Expr res = make_int(exec, sum);
    free_longnum(sum);
    if (is_none(res))
    {
        log("exec_int_add: make_int failed");
        return expr_none();
    }
    return res;
}
Expr exec_int_inc(pExecutor exec, Expr a)
{
    if (!is_int(a))
    {
        log("exec_int_inc: argument is not integer");
        exit(1);
    }
    pLongNum one = longnum_one();
    pLongNum sum = longnum_add(dereference(a).val_int, one);
    Expr res = make_int(exec, sum);
    free_longnum(one);
    free_longnum(sum);
    if (is_none(res))
    {
        log("exec_int_inc: make_int failed");
        return expr_none();
    }
    return res;
}
