#include "exec_builtin.h"

#include <stdbool.h>

#include "log.h"

Expr predicate_impl(pExecutor exec, pContext context, char *caller, Expr *args, int argc, _Bool (*p)(Expr))
{
    if (argc > 1)
    {
        logf("%s: too many arguments", caller);
        exit(1);
    }
    if (argc < 1)
    {
        logf("%s: too few arguments", caller);
        exit(1);
    }
    Expr value = exec_eval(exec, context, args[0]);
    return make_bool(exec, p(value));
}

BUILTIN_FUNC(atom_p)
{
    return predicate_impl(exec, callContext, "atom?", args, argc, is_atom);
}
BUILTIN_FUNC(pair_p)
{
    return predicate_impl(exec, callContext, "pair?", args, argc, is_pair);
}
BUILTIN_FUNC(char_p)
{
    return predicate_impl(exec, callContext, "char?", args, argc, is_char);
}
BUILTIN_FUNC(int_p)
{
    return predicate_impl(exec, callContext, "int?", args, argc, is_int);
}
BUILTIN_FUNC(real_p)
{
    return predicate_impl(exec, callContext, "real?", args, argc, is_real);
}
BUILTIN_FUNC(string_p)
{
    return predicate_impl(exec, callContext, "string?", args, argc, is_string);
}
BUILTIN_FUNC(function_p)
{
    return predicate_impl(exec, callContext, "function?", args, argc, is_function);
}
BUILTIN_FUNC(macro_p)
{
    return predicate_impl(exec, callContext, "macro?", args, argc, is_macro);
}
