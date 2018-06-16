#include "predicates.h"

#include <string.h>

#include "utils/log.h"

_Bool is_none(Expr expr)
{
    return expr.type == VT_NONE;
}

_Bool is_equal(Expr a, Expr b)
{
    if (a.type != b.type) return 0;
    switch (a.type)
    {
    case VT_ATOM:
        return a.val_atom == b.val_atom;
    case VT_CHAR:
        return a.val_char == b.val_char;
    case VT_INT_PTR:
        return longint_compare(
                dereference(a).val_int,
                dereference(b).val_int) == 0;
    case VT_REAL:
        return a.val_real == b.val_real;
    case VT_STRING_PTR:
        return string_compare(
            dereference(a).val_str,
            dereference(b).val_str) == 0;
    case VT_PAIR:
    case VT_INT_VAL:
    case VT_STRING_VAL:
    case VT_FUNC_PTR:
    case VT_FUNC_VAL:
        return 0;
    case VT_NONE:
    default:
        log("is_equal: unable to compare");
        exit(1);
    }
}

_Bool is_true(pExecutor exec, Expr expr)
{
    if ((expr.type == VT_ATOM && expr.val_atom == exec->nil.val_atom) ||
        (expr.type == VT_NONE))
        return 0;
    return 1;
}

_Bool is_atom(Expr expr)
{
    return expr.type == VT_ATOM;
}

_Bool is_pair(Expr expr)
{
    return expr.type == VT_PAIR;
}

_Bool is_char(Expr expr)
{
    return expr.type == VT_CHAR;
}

_Bool is_int(Expr expr)
{
    return expr.type == VT_INT_PTR;
}

_Bool is_real(Expr expr)
{
    return expr.type == VT_REAL;
}

_Bool is_string(Expr expr)
{
    return expr.type == VT_STRING_PTR;
}

_Bool is_function(Expr expr)
{
    return expr.type == VT_FUNC_PTR;
}

_Bool is_macro(Expr expr)
{
    return
        expr.type == VT_FUNC_PTR &&
        dereference(expr).val_func->type == FT_USER_MACRO;
}

_Bool is_list(pExecutor exec, Expr expr)
{
    return
        expr.type == VT_PAIR ||
        (expr.type == VT_ATOM && expr.val_atom == exec->nil.val_atom);
}
