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

Expr lambda_impl(pExecutor exec, pContext callContext, Expr *args, int argc, enum FunctionType type)
{
    /*
        (lambda (ar gu me nts) body)
    */
    if (argc < 2 || !is_list(exec, args[0]))
    {
        logf("%s: wrong arguments", type == FT_USER ? "lambda" : "macro");
        exit(1);
    }
    int f_argc;
    Expr *f_args = get_list(exec, args[0], &f_argc);
    pFunction func = create_lambda(exec, callContext, f_args, f_argc, args + 1, argc - 1, type);
    if (func == NULL)
    {
        logf("%s: create_lambda failed", type == FT_USER ? "lambda" : "macro");
        exit(1);
    }

    Expr res;
    res.type = VT_FUNC;
    res.val_func = func;

    return res;
}

BUILTIN_FUNC(lambda)
{
    return lambda_impl(exec, callContext, args, argc, FT_USER);
}

BUILTIN_FUNC(cond)
{
    for (int i = 0; i < argc; i++)
    {
        int len;
        Expr *list = get_list(exec, args[i], &len);

        if (len == 0)
        {
            log("cond: invalid arguments");
            free(list);
            exit(1);
        }

        Expr cond_expr = exec_eval(exec, callContext, list[0]);
        if (is_true(exec, cond_expr))
        {
            Expr res = exec_eval_array(exec, callContext, list + 1, len - 1);
            free(list);
            return res;
        }

        free(list);
    }
    return exec->nil;
}

BUILTIN_FUNC(gensym)
{
    if (argc != 0)
    {
        log("gensym: too many arguments");
        exit(1);
    }
    Expr counter = make_atom(exec, "*gensym-counter*");
    if (counter.type == VT_NONE)
    {
        log("gensym make_atom failed");
        exit(1);
    }
    Expr value;
    if (context_get(defContext, counter.val_atom, &value) == MAP_FAILED)
    {
        value.type = VT_INT;
        value.val_int = 1;
    }

    for (int n = value.val_int;;n++)
    {
        char name[7];
        sprintf(name, "#:%d", n);
        size_t found = find_atom(exec, name);
        if (found == EXPR_ERROR)
        {
            log("gensym: find_atom failed");
            exit(1);
        }
        if (found == EXPR_NOT_FOUND)
        {
            Expr res = make_atom(exec, name);
            if (res.type == VT_NONE)
            {
                log("gensym: make_atom failed");
                exit(1);
            }
            value.val_int = n + 1;
            if (context_bind(defContext, counter.val_atom, value) == MAP_FAILED)
            {
                log("gensym: context_bind failed");
                exit(1);
            }
            return res;
        }
    }
}


Expr backquote_impl(pExecutor exec, pContext context, Expr expr)
{
    if (is_equal(expr, exec->nil)) return exec->nil;
    if (expr.type == VT_ATOM)
    {
        if (!is_equal(expr, exec->comma) &&
            !is_equal(expr, exec->comma_atsign))
        {
            return expr;
        }
        else
        {
            if (is_equal(expr, exec->comma))
            {
                log("backquote: unexpected `,`");
                exit(1);
            }
            if (is_equal(expr, exec->comma_atsign))
            {
                log("backquote: unexpected `,@`");
                exit(1);
            }
        }
    }
    if (expr.type != VT_PAIR) return expr;
    Expr head = get_head(exec, expr);
    if (is_equal(head, exec->comma))
    {
        Expr tail = get_tail(exec, expr);
        Expr next = get_head(exec, tail);
        if (is_none(next))
        {
            log("backquote: unexpected end of list");
            exit(1);
        }

        Expr evaluated = exec_eval(exec, context, next);
        Expr rest = backquote_impl(exec, context, get_tail(exec, tail));
        return make_pair(exec, evaluated, rest);
    }
    else if (is_equal(head, exec->comma_atsign))
    {
        Expr tail = get_tail(exec, expr);
        Expr next = get_head(exec, tail);
        if (is_none(next))
        {
            log("backquote: unexpected end of list");
            exit(1);
        }

        Expr evaluated = exec_eval(exec, context, next);
        Expr rest = backquote_impl(exec, context, get_tail(exec, tail));

        if (evaluated.type != VT_PAIR)
        {
            log("backquote: not a list after `,@`");
            exit(1);
        }

        int len;
        Expr *items = get_list(exec, evaluated, &len);
        for (int i = len - 1; i >= 0; i--)
        {
            rest = make_pair(exec, items[i], rest);
            if (is_none(rest))
            {
                log("backuote: make_pair failed");
                exit(1);
            }
        }

        return rest;
    }
    else
        return make_pair(exec,
                         backquote_impl(exec, context, head),
                         backquote_impl(exec, context, get_tail(exec, expr)));
}

BUILTIN_FUNC(backquote)
{
    if (argc > 1)
    {
        log("backquote: too many arguments");
        exit(1);
    }
    if (argc < 1)
    {
        log("backquote: too few arguments");
        exit(1);
    }
    return backquote_impl(exec, callContext, args[0]);
}

BUILTIN_FUNC(macro)
{
    return lambda_impl(exec, callContext, args, argc, FT_MACRO);
}

BUILTIN_FUNC(setmacro)
{
    /*
        (set a 1)
    */
    if (argc != 2)
    {
        log("setmacro: wrong arguments syntax");
        exit(1);
    }
    Expr atom = args[0];
    if (atom.type != VT_ATOM)
    {
        log("setmacro: wrong arguments syntax");
        exit(1);
    }
    Expr val = exec_eval(exec, callContext, args[1]);
    if (context_set_macro(callContext, atom.val_atom, val) == MAP_FAILED)
    {
        log("setmacro: context_set_macro failed");
        exit(1);
    }

    return val;
}
