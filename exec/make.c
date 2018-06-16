#include "make.h"

#include <string.h>

#include "utils/log.h"

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

Expr make_int(pExecutor exec, pLongInt value)
{
    pLongInt copy = longint_copy(value);
    if (copy == NULL)
    {
        log("make_int: longint_copy failed");
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
    pLongInt num = longint_zero();
    if (num == NULL)
    {
        log("make_int_zero: longint_zero failed");
        exit(1);
    }
    Expr res = make_int(exec, num);
    free_longint(num);
    return res;
}

Expr make_int_one(pExecutor exec)
{
    pLongInt num = longint_one();
    if (num == NULL)
    {
        log("make_int_zero: longint_one failed");
        exit(1);
    }
    Expr res = make_int(exec, num);
    free_longint(num);
    return res;
}

Expr make_int_negative_one(pExecutor exec)
{
    pLongInt num = longint_negative_one();
    if (num == NULL)
    {
        log("make_int_zero: longint_negative_one failed");
        exit(1);
    }
    Expr res = make_int(exec, num);
    free_longint(num);
    return res;
}

Expr make_int_from_long(pExecutor exec, long value)
{
    pLongInt num = longint_from_long(value);
    if (num == NULL)
    {
        log("make_int_from_long: longnum_from_int failed");
        exit(1);
    }
    Expr res = make_int(exec, num);
    free_longint(num);
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

Expr make_string_without_clone(pExecutor exec, pString value)
{
    Expr res;
    res.type = VT_STRING_VAL;
    res.val_str = value;

    Expr ptr = gc_register(exec->heap, res);
    if (is_none(ptr))
    {
        log("make_string_without_clone: gc_register failed");
        return make_none();
    }
    return ptr;
}

Expr make_string(pExecutor exec, pString value)
{
    pString str = string_clone(value);
    if (str == NULL)
    {
        perror("make_string: string_clone failed");
        return make_none();
    }
    Expr res = make_string_without_clone(exec, str);
    if (is_none(res))
    {
        free_string(str);
        log("make_string: make_string_without_clone failed");
        return make_none();
    }
    return res;
}

Expr make_string_from_array(pExecutor exec, char *value, size_t len)
{
    pString str = string_from_array(value, len);
    if (str == NULL)
    {
        perror("make_string_from_array: string_from_array failed");
        return make_none();
    }
    Expr res = make_string_without_clone(exec, str);
    if (is_none(res))
    {
        free_string(str);
        log("make_string_from_array: make_string_without_clone failed");
        return make_none();
    }
    return res;
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
