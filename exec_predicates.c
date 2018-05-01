#include "exec_predicates.h"

#include <string.h>

#include "log.h"

int is_none(Expr expr)
{
    return expr.type == VT_NONE;
}

int is_equal(Expr a, Expr b)
{
    if (a.type != b.type) return 0;
    switch (a.type)
    {
    case VT_ATOM:
        return a.val_atom == b.val_atom;
    case VT_CHAR:
        return a.val_char == b.val_char;
    case VT_INT_PTR:
        return longnum_compare(
            dereference(a).val_int,
            dereference(b).val_int) == 0;
    case VT_REAL:
        return a.val_real == b.val_real;
    case VT_STRING_PTR:
        return strcmp(
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

int is_true(pExecutor exec, Expr expr)
{
    if ((expr.type == VT_ATOM && expr.val_atom == exec->nil.val_atom) ||
        (expr.type == VT_NONE))
        return 0;
    return 1;
}

int is_macro(Expr expr)
{
    return
        expr.type == VT_FUNC_PTR &&
        dereference(expr).val_func->type == FT_USER_MACRO;
}

int is_atom(Expr expr)
{
    return expr.type == VT_ATOM;
}

int is_pair(Expr expr)
{
    return expr.type == VT_PAIR;
}

int is_int(Expr expr)
{
    return expr.type == VT_INT_PTR;
}

int is_real(Expr expr)
{
    return expr.type == VT_REAL;
}

int is_char(Expr expr)
{
    return expr.type == VT_CHAR;
}

int is_string(Expr expr)
{
    return expr.type == VT_STRING_PTR;
}
