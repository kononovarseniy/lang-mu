#include "exec_builtin.h"

#include <stdlib.h>
#include <stdio.h>
#include "log.h"

#include "print.h"

BUILTIN_FUNC(def)
{
    /*
        (def (a 1) (c a))
    */
    Expr last;
    for (int i = 0; i < argc; i++)
    {
        int len;
        Expr *list = get_list(exec, args[i], &len);
        if (list == NULL || len != 2)
        {
            log("def: wrong arguments syntax");
            exit(1);
        }
        Expr atom = list[0];
        if (atom.type != VT_ATOM)
        {
            log("def: wrong arguments syntax");
            exit(1);
        }
        Expr val = exec_eval(exec, callContext, list[1]);
        context_bind(callContext, atom.val_atom, val);
        free(list);
        last = val;
    }
    return last;
}

BUILTIN_FUNC(print)
{
    for (int i = 0; i < argc; i++)
    {
        Expr value = exec_eval(exec, callContext, args[i]);
        print_expression(stdout, exec, value, PF_SHORT_QUOTE, 0);
        if (i < argc - 1) printf(" ");
    }
    printf("\n");
    return exec->t;
}

BUILTIN_FUNC(quote)
{
    if (argc > 1)
    {
        log("quote: too many arguments");
        exit(1);
    }
    if (argc < 1)
    {
        log("quote: too few arguments");
        exit(1);
    }
    return args[0];
}

BUILTIN_FUNC(plus)
{
    long res = 0;
    for (int i = 0; i < argc; i++)
    {
        Expr value = exec_eval(exec, callContext, args[i]);
        if (value.type != VT_INT)
        {
            log("plus: integer expected");
            exit(1);
        }
        res += value.val_int;
    }
    Expr expr;
    expr.type = VT_INT;
    expr.val_int = res;
    return expr;
}
