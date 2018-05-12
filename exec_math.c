#include "exec_math.h"
#include "log.h"

// Helpers
Expr exec_int_binary_operation(pExecutor exec, Expr a, Expr b, pLongNum (*op)(pLongNum, pLongNum), char *caller)
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
        return make_none();
    }
    return res;
}

long exec_int_to_long(pExecutor exec, Expr num)
{
    if (!is_int(num))
    {
        log("exec_int_to_long: argument is not integer");
        exit(1);
    }
    return longnum_to_long(dereference(num).val_int);
}
double exec_int_to_double(pExecutor exec, Expr num)
{
    if (!is_int(num))
    {
        log("exec_int_to_double: argument is not integer");
        exit(1);
    }
    return longnum_to_double(dereference(num).val_int);
}

Expr exec_int_add(pExecutor exec, Expr a, Expr b)
{
    return exec_int_binary_operation(exec, a, b, longnum_add, "exec_int_add");
}
Expr exec_int_sub(pExecutor exec, Expr a, Expr b)
{
    return exec_int_binary_operation(exec, a, b, longnum_sub, "exec_int_sub");
}
Expr exec_int_inverse(pExecutor exec, Expr a)
{
    if (!is_int(a))
    {
        log("exec_int_inverse: argument is not integer");
        exit(1);
    }
    pLongNum num = longnum_inverse(dereference(a).val_int);
    if (num == NULL)
    {
        log("exec_int_inverse: longnum_inverse failed");
        exit(1);
    }
    Expr res = make_int(exec, num);
    free_longnum(num);
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
    if (one == NULL)
    {
        log("exec_int_inc: longnum_one failed");
        exit(1);
    }
    pLongNum num = longnum_add(dereference(a).val_int, one);
    if (num == NULL)
    {
        log("exec_int_inc: longnum_add failed");
        exit(1);
    }
    free_longnum(one);
    Expr res = make_int(exec, num);
    if (is_none(res))
    {
        log("exec_int_inc: make_int failed");
        return make_none();
    }
    free_longnum(num);
    return res;
}


Expr exec_int_product(pExecutor exec, Expr a, Expr b)
{
    return exec_int_binary_operation(exec, a, b, longnum_product, "exec_int_product");
}
Expr exec_int_div(pExecutor exec, Expr a, Expr b)
{
    return exec_int_binary_operation(exec, a, b, longnum_div, "exec_int_div");
}
Expr exec_int_rem(pExecutor exec, Expr a, Expr b)
{
    return exec_int_binary_operation(exec, a, b, longnum_rem, "exec_int_rem");
}
Expr exec_int_division_inverse(pExecutor exec, Expr a)
{
    if (!is_int(a))
    {
        log("exec_int_division_inverse: argument is not integer");
        exit(1);
    }
    pLongNum one = longnum_one();
    if (one == NULL)
    {
        log("exec_int_division_inverse: longnum_one failed");
        exit(1);
    }
    pLongNum num = longnum_div(one, dereference(a).val_int);
    if (num == NULL)
    {
        log("exec_int_division_inverse: longnum_div failed");
        exit(1);
    }
    free_longnum(one);
    Expr res = make_int(exec, num);
    if (is_none(res))
    {
        log("exec_int_division_inverse: make_int failed");
        exit(1);
    }
    free_longnum(num);
    return res;
}
Expr exec_int_gcd(pExecutor exec, Expr a, Expr b)
{
    return exec_int_binary_operation(exec, a, b, longnum_greatest_common_divisor, "exec_int_gcd");
}

Expr exec_convert_to_real(pExecutor exec, Expr expr)
{
    if (is_real(expr))
        return expr;
    else if (is_int(expr))
        return make_real(exec, longnum_to_double(dereference(expr).val_int));
    else if (is_char(expr))
        return make_real(exec, (double)expr.val_char);
    else
        return make_none();
}
Expr exec_convert_to_int(pExecutor exec, Expr expr)
{
    if (is_real(expr))
    {
        pLongNum num = longnum_from_double(expr.val_real);
        Expr res = make_int(exec, num);
        free_longnum(num);
        return res;
    }
    else if (is_int(expr))
        return expr;
    else if (is_char(expr))
    {
        pLongNum num = longnum_from_long(expr.val_char);
        Expr res = make_int(exec, num);
        free_longnum(num);
        return res;
    }
    else
        return make_none();
}
Expr exec_convert_to_char(pExecutor exec, Expr expr)
{
    if (is_real(expr))
        return make_char(exec, (char)expr.val_real);
    else if (is_int(expr))
        return make_char(exec, (char)longnum_to_long(dereference(expr).val_int));
    else if (is_char(expr))
        return expr;
    else
        return make_none();
}

_Bool exec_to_most_generic_type(pExecutor exec, Expr *a, Expr *b)
{
    if (is_real(*a) || is_real(*b))
    {
        *a = exec_convert_to_real(exec, *a);
        *b = exec_convert_to_real(exec, *b);
    }
    else if (is_int(*a) || is_int(*b))
    {
        *a = exec_convert_to_int(exec, *a);
        *b = exec_convert_to_int(exec, *b);
    }
    else if (is_char(*a) || is_char(*b))
    {
        *a = exec_convert_to_char(exec, *a);
        *b = exec_convert_to_char(exec, *b);
    }
    else
        return false;
    return !is_none(*a) && !is_none(*b);
}

int exec_compare_char(Expr a, Expr b)
{
    int diff = (int)a.val_char - b.val_char;
    if (diff > 0)
        return 1;
    else if (diff < 0)
        return -1;
    return 0;
}
int exec_compare_int(Expr a, Expr b)
{
    return longnum_compare(dereference(a).val_int, dereference(b).val_int);
}
int exec_compare_real(Expr a, Expr b)
{
    double diff = a.val_real - b.val_real;
    if (diff > 0)
        return 1;
    else if (diff < 0)
        return -1;
    return 0;
}

int exec_compare(pExecutor exec, Expr a, Expr b)
{
    if (!exec_to_most_generic_type(exec, &a, &b))
    {
        log("exec_compare: unable to find generic type");
        exit(1);
    }
    switch (a.type)
    {
    case VT_CHAR:
        return exec_compare_char(a, b);
    case VT_INT_PTR:
        return exec_compare_int(a, b);
    case VT_REAL:
        return exec_compare_real(a, b);
    default:
        log("exec_compare: unknown type");
        exit(1);
    }
}
Expr exec_sum(pExecutor exec, Expr a, Expr b)
{
    if (!exec_to_most_generic_type(exec, &a, &b))
    {
        log("exec_sum: unable to find generic type");
        exit(1);
    }
    switch (a.type)
    {
    case VT_CHAR:
        return make_char(exec, a.val_char + b.val_char);
    case VT_INT_PTR:
        return exec_int_add(exec, a, b);
    case VT_REAL:
        return make_real(exec, a.val_real + b.val_real);
    default:
        log("exec_sum: unknown type");
        exit(1);
    }
}
Expr exec_difference(pExecutor exec, Expr a, Expr b)
{
    if (!exec_to_most_generic_type(exec, &a, &b))
    {
        log("exec_difference: unable to find generic type");
        exit(1);
    }
    switch (a.type)
    {
    case VT_CHAR:
        return make_char(exec, a.val_char - b.val_char);
    case VT_INT_PTR:
        return exec_int_sub(exec, a, b);
    case VT_REAL:
        return make_real(exec, a.val_real - b.val_real);
    default:
        log("exec_difference: unknown type");
        exit(1);
    }
}
Expr exec_inverse(pExecutor exec, Expr a)
{
    switch (a.type)
    {
    case VT_CHAR:
        return make_char(exec, -a.val_char);
    case VT_INT_PTR:
        return exec_int_inverse(exec, a);
    case VT_REAL:
        return make_real(exec, -a.val_real);
    default:
        log("exec_inverse: unknown type");
        exit(1);
    }
}
Expr exec_product(pExecutor exec, Expr a, Expr b)
{
    if (!exec_to_most_generic_type(exec, &a, &b))
    {
        log("exec_product: unable to find generic type");
        exit(1);
    }
    switch (a.type)
    {
    case VT_CHAR:
        return make_char(exec, a.val_char * b.val_char);
    case VT_INT_PTR:
        return exec_int_product(exec, a, b);
    case VT_REAL:
        return make_real(exec, a.val_real * b.val_real);
    default:
        log("exec_product: unknown type");
        exit(1);
    }
}
Expr exec_quotient(pExecutor exec, Expr a, Expr b)
{
    // TODO: handle division by zero
    if (!exec_to_most_generic_type(exec, &a, &b))
    {
        log("exec_quotient: unable to find generic type");
        exit(1);
    }
    switch (a.type)
    {
    case VT_CHAR:
        return make_char(exec, a.val_char / b.val_char);
    case VT_INT_PTR:
        return exec_int_div(exec, a, b);
    case VT_REAL:
        return make_real(exec, a.val_real / b.val_real);
    default:
        log("exec_quotient: unknown type");
        exit(1);
    }
}
Expr exec_division_inverse(pExecutor exec, Expr a)
{
    // TODO: handle division by zero
    switch (a.type)
    {
    case VT_CHAR:
        return make_char(exec, 1/a.val_char);
    case VT_INT_PTR:
        return exec_int_division_inverse(exec, a);
    case VT_REAL:
        return make_real(exec, 1/a.val_real);
    default:
        log("exec_division_inverse: unknown type");
        exit(1);
    }
}
Expr exec_remainder(pExecutor exec, Expr a, Expr b)
{
    // TODO: handle division by zero
    if (!exec_to_most_generic_type(exec, &a, &b))
    {
        log("exec_remainder: unable to find generic type");
        exit(1);
    }
    switch (a.type)
    {
    case VT_CHAR:
        return make_char(exec, a.val_char % b.val_char);
    case VT_INT_PTR:
        return exec_int_rem(exec, a, b);
    case VT_REAL:
        // TODO: floating point remainder
    default:
        log("exec_reminder: unknown type");
        exit(1);
    }
}
