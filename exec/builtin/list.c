//
// Created by arseniy on 16.06.18.
//

#include "functions.h"

#include "utils/log.h"

BUILTIN_FUNC(cons)
{
    if (argc != 2)
    {
        log("cons: expected two arguments");
        exit(1);
    }
    Expr car = exec_eval(exec, callContext, args[0]);
    Expr cdr = exec_eval(exec, callContext, args[1]);
    Expr pair = make_pair(exec, car, cdr);
    if (pair.type == VT_NONE)
    {
        log("cons: make_pair failed");
        exit(1);
    }
    return pair;
}

BUILTIN_FUNC(head)
{
    if (argc != 1)
    {
        log("head: wrong arguments count");
        exit(1);
    }
    Expr arg = exec_eval(exec, callContext, args[0]);
    if (arg.type != VT_PAIR)
    {
        log("head: argument not a pair");
        exit(1);
    }
    return exec->cars[arg.val_pair];
}

BUILTIN_FUNC(tail)
{
    if (argc != 1)
    {
        log("tail: wrong arguments count");
        exit(1);
    }
    Expr arg = exec_eval(exec, callContext, args[0]);
    if (arg.type != VT_PAIR)
    {
        log("tail: argument not a pair");
        exit(1);
    }
    return exec->cdrs[arg.val_pair];
}

Expr set_pair_impl(pExecutor exec,
                   pContext context,
                   Expr *args,
                   int argc,
                   char *caller,
                   void (*set_func)(pExecutor, Expr, Expr))
{
    if (argc != 2)
    {
        logf("%s: wrong arguments syntax", caller);
        exit(1);
    }
    Expr pair = exec_eval(exec, context, args[0]);
    if (pair.type != VT_PAIR)
    {
        logf("%s: first argument not a pair", caller);
        exit(1);
    }
    // Evaluate value
    Expr val = exec_eval(exec, context, args[1]);

    set_func(exec, pair, val);
    return val;
}

BUILTIN_FUNC(set_head_builtin)
{
    return set_pair_impl(exec, callContext, args, argc, "set-head", set_head);
}

BUILTIN_FUNC(set_tail_builtin)
{
    return set_pair_impl(exec, callContext, args, argc, "set-tail", set_tail);
}
