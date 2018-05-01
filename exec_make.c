#include "exec_make.h"

#include <string.h>

#include "log.h"

Expr make_none()
{
    Expr res;
    res.type = VT_NONE;
    return res;
}

Expr make_bool(pExecutor exec, _Bool value)
{
    if (value)
        return exec->t;
    else
        return exec->nil;
}

Expr make_atom(pExecutor exec, char *name)
{
    size_t atom = add_atom(exec, name);
    if (atom == EXPR_ERROR)
    {
        log("make_atom: add_atom failed");
        return make_none();
    }

    Expr res;
    res.type = VT_ATOM;
    res.val_atom = atom;
    return res;
}

Expr make_int(pExecutor exec, pLongNum value)
{
    pLongNum copy = longnum_copy(value);
    if (copy == NULL)
    {
        log("make_int: longnum_copy failed");
        return make_none();
    }
    Expr res;
    res.type = VT_INT_VAL;
    res.val_int = copy;

    Expr ptr = gc_register(exec->heap, res);
    if (is_none(ptr))
    {
        log("make_int: gc_register failed");
        free(copy);
        return make_none();
    }
    return ptr;
}

Expr make_int_zero(pExecutor exec)
{
    pLongNum num = longnum_zero();
    if (num == NULL)
    {
        log("make_int_zero: longnum_zero failed");
        exit(1);
    }
    Expr res = make_int(exec, num);
    free_longnum(num);
    return res;
}

Expr make_int_one(pExecutor exec)
{
    pLongNum num = longnum_one();
    if (num == NULL)
    {
        log("make_int_zero: longnum_one failed");
        exit(1);
    }
    Expr res = make_int(exec, num);
    free_longnum(num);
    return res;
}

Expr make_int_negative_one(pExecutor exec)
{
    pLongNum num = longnum_negative_one();
    if (num == NULL)
    {
        log("make_int_zero: longnum_negative_one failed");
        exit(1);
    }
    Expr res = make_int(exec, num);
    free_longnum(num);
    return res;
}

Expr make_int_from_long(pExecutor exec, long value)
{
    pLongNum num = longnum_from_int(value);
    if (num == NULL)
    {
        log("make_int_from_long: longnum_from_int failed");
        exit(1);
    }
    Expr res = make_int(exec, num);
    free_longnum(num);
    if (is_none(res))
    {
        log("make_int_from_long: make_int failed");
        return make_none();
    }
    return res;
}

Expr make_real(pExecutor exec, double value)
{
    Expr res;
    res.type = VT_REAL;
    res.val_real = value;
    return res;
}

Expr make_char(pExecutor exec, char value)
{
    Expr res;
    res.type = VT_CHAR;
    res.val_char = value;
    return res;
}

Expr make_string(pExecutor exec, char *value)
{
    char *copy = strdup(value);
    if (copy == NULL)
    {
        perror("make_string: strdup failed");
        return make_none();
    }

    Expr res;
    res.type = VT_STRING_VAL;
    res.val_str = copy;

    Expr ptr = gc_register(exec->heap, res);
    if (is_none(ptr))
    {
        log("make_string: gc_register failed");
        free(copy);
        return make_none();
    }
    return ptr;
}

Expr make_function(pExecutor exec, pFunction value)
{
    Expr res;
    res.type = VT_FUNC_VAL;
    res.val_func = value;

    Expr ptr = gc_register(exec->heap, res);
    if (is_none(ptr))
    {
        log("make_function: gc_register failed");
        return make_none();
    }
    return ptr;
}

Expr make_builtin_function(pExecutor exec, pBuiltinFunction func, pContext context)
{
    pFunction funcval = create_function();
    if (funcval == NULL)
    {
        log("make_builtin_function: create_function failed");
        return make_none();
    }
    funcval->type = FT_BUILTIN;
    funcval->context = context;
    funcval->builtin = func;
    context_link(context);

    Expr res = make_function(exec, funcval);
    if (is_none(res))
    {
        log("make_builtin_function: make_function failed");
        return make_none();
    }
    return res;
}

Expr make_user_function(pExecutor exec, pUserFunction func, pContext context, enum FunctionType type)
{
    pFunction funcval = create_function();
    if (funcval == NULL)
    {
        log("make_user_function: create_function failed");
        return make_none();
    }
    funcval->type = type;
    funcval->context = context;
    funcval->user = func;
    context_link(context);

    Expr res = make_function(exec, funcval);
    if (is_none(res))
    {
        log("make_user_function: make_function failed");
        return make_none();
    }
    return res;
}

Expr make_pair(pExecutor exec, Expr car, Expr cdr)
{
    size_t pair = add_pair(exec);
    if (pair == EXPR_ERROR)
    {
        log("make_list: add_pair failed");
        return make_none();
    }
    exec->cars[pair] = car;
    exec->cdrs[pair] = cdr;

    Expr res;
    res.type = VT_PAIR;
    res.val_pair = pair;
    return res;
}

Expr make_list(pExecutor exec, Expr *arr, int len)
{
    Expr list = exec->nil;
    for (int i = len - 1; i >= 0; i--)
    {
        Expr tail = make_pair(exec, arr[i], list);
        if (tail.type == VT_NONE)
        {
            log("make_list: make_pair failed");
            return make_none();
        }
        list = tail;
    }
    return list;
}
