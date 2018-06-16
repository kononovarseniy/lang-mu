//
// Created by arseniy on 16.06.18.
//

#include "functions.h"

#include "utils/log.h"

/*
    Logical functions
*/

BUILTIN_FUNC(eq)
{
    if (argc < 2)
    {
        log("eq: too few arguments");
        exit(1);
    }
    if (argc > 2)
    {
        log("eq: too many arguments");
        exit(1);
    }
    Expr a = exec_eval(exec, callContext, args[0]);
    Expr b = exec_eval(exec, callContext, args[1]);
    if (is_equal(a, b))
        return exec->t;
    else
        return exec->nil;
}

BUILTIN_FUNC(and)
{
    for (int i = 0; i < argc; i++)
    {
        Expr val = exec_eval(exec, callContext, args[i]);
        if (!is_true(exec, val))
            return exec->nil;
    }
    return exec->t;
}

BUILTIN_FUNC(or)
{
    for (int i = 0; i < argc; i++)
    {
        Expr val = exec_eval(exec, callContext, args[i]);
        if (is_true(exec, val))
            return exec->t;
    }
    return exec->nil;
}

BUILTIN_FUNC(not)
{
    if (argc < 1)
    {
        log("not: too few arguments");
        exit(1);
    }
    if (argc > 1)
    {
        log("not: too many arguments");
        exit(1);
    }

    Expr val = exec_eval(exec, callContext, args[0]);
    if (is_true(exec, val))
        return exec->nil;
    else
        return exec->t;
}

BUILTIN_FUNC(xor)
{
    int cnt = 0;
    for (int i = 0; i < argc; i++)
    {
        Expr val = exec_eval(exec, callContext, args[i]);
        if (is_true(exec, val))
            cnt++;
    }
    if (cnt == 1)
        return exec->t;
    else
        return exec->nil;

}

/*
    Comparison functions
*/

Expr compare_impl(pExecutor exec, pContext context, Expr *args, int argc, int order, int strict)
{
    int success = 1;

    Expr prev = make_none();
    for (int i = 0; i < argc; i++)
    {
        Expr value = exec_eval(exec, context, args[i]);
        if (i > 0)
        {
            int cmpres = exec_compare(exec, prev, value);
            if (!(cmpres == order || (!strict && cmpres == 0)))
            {
                success = 0;
                break;
            }
        }
        prev = value;
    }
    if (success)
        return exec->t;
    else
        return exec->nil;
}

BUILTIN_FUNC(less_builtin)
{
    return compare_impl(exec, callContext, args, argc, -1, 1);
}

BUILTIN_FUNC(less_or_equals_builtin)
{
    return compare_impl(exec, callContext, args, argc, -1, 0);
}

BUILTIN_FUNC(num_equals_builtin)
{
    return compare_impl(exec, callContext, args, argc, 0, 0);
}

BUILTIN_FUNC(more_or_equals_builtin)
{
    return compare_impl(exec, callContext, args, argc, 1, 0);
}

BUILTIN_FUNC(more_builtin)
{
    return compare_impl(exec, callContext, args, argc, 1, 1);
}

/*
    Arithmetical functions
*/

BUILTIN_FUNC(sum_builtin)
{
    if (argc == 0)
        return make_int_zero(exec);

    Expr *values = exec_eval_each(exec, callContext, args, argc);

    Expr num = values[0];
    for (int i = 1; i < argc; i++)
        num = exec_sum(exec, num, values[i]);

    free(values);
    return num;
}

BUILTIN_FUNC(difference_builtin)
{
    if (argc == 0)
    {
        log("difference: too few arguments");
        exit(1);
    }
    Expr *values = exec_eval_each(exec, callContext, args, argc);

    Expr first = values[0];
    if (argc == 1)
    {
        free(values);
        return exec_inverse(exec, first);
    }

    Expr num = first;
    for (int i = 1; i < argc; i++)
        num = exec_difference(exec, num, values[i]);

    free(values);
    return num;
}

BUILTIN_FUNC(product_builtin)
{
    if (argc == 0)
        return make_int_one(exec);

    Expr *values = exec_eval_each(exec, callContext, args, argc);

    Expr num = values[0];
    for (int i = 1; i < argc; i++)
        num = exec_product(exec, num, values[i]);

    free(values);
    return num;
}

BUILTIN_FUNC(quotient_builtin)
{
    if (argc == 0)
    {
        log("quotient: too few arguments");
        exit(1);
    }
    Expr *values = exec_eval_each(exec, callContext, args, argc);

    Expr first = values[0];
    if (argc == 1)
    {
        free(values);
        return exec_division_inverse(exec, first);
    }

    // Calculate product of all arguments except first
    Expr divisor = values[1];
    for (int i = 2; i < argc; i++)
        divisor = exec_product(exec, divisor, values[i]);

    free(values);

    return exec_quotient(exec, first, divisor);
}

BUILTIN_FUNC(remainder_builtin)
{
    if (argc < 2)
    {
        log("remainder: too few arguments");
        exit(1);
    }
    if (argc > 2)
    {
        log("remainder: too many arguments");
        exit(1);
    }
    Expr *values = exec_eval_each(exec, callContext, args, argc);

    Expr first = values[0];
    Expr second = values[1];
    free(values);

    return exec_remainder(exec, first, second);
}
