#include "exec_builtin.h"

#include <stdlib.h>
#include <stdio.h>
#include "log.h"

#include "print.h"

BUILTIN_FUNC(set)
{
    /*
        (set a 1)
    */
    if (argc != 2)
    {
        log("set: wrong arguments syntax");
        exit(1);
    }
    Expr atom = args[0];
    if (atom.type != VT_ATOM)
    {
        log("set: wrong arguments syntax");
        exit(1);
    }
    Expr val = exec_eval(exec, callContext, args[1]);
    context_bind(callContext, atom.val_atom, val);

    return val;
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

BUILTIN_FUNC(prints)
{
    for (int i = 0; i < argc; i++)
    {
        Expr value = exec_eval(exec, callContext, args[i]);
        print_expression(stdout, exec, value, PF_SHORT_QUOTE | PF_UNESCAPE, 0);
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

int is_list(pExecutor exec, Expr expr)
{
    return
        expr.type == VT_PAIR ||
        (expr.type == VT_ATOM && expr.val_atom == exec->nil.val_atom);
}

BUILTIN_FUNC(lambda)
{
    /*
        (lambda (ar gu me nts) body)
    */
    if (argc < 2 || !is_list(exec, args[0]))
    {
        log("lambda: wrong arguments");
        exit(1);
    }
    int f_argc;
    Expr *f_args = get_list(exec, args[0], &f_argc);
    pFunction func = create_lambda(exec, callContext, f_args, f_argc, args + 1, argc - 1);
    if (func == NULL)
    {
        log("lambda: create_lambda failed");
        exit(1);
    }

    Expr res;
    res.type = VT_FUNC;
    res.val_func = func;

    return res;
}
