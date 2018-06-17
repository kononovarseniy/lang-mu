//
// Created by arseniy on 16.06.18.
//

#include "constructions.h"

#include <stdlib.h>
#include <stdio.h>
#include "utils/log.h"

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

BUILTIN_FUNC(let)
{
    if (argc < 2)
    {
        log("let: too few arguments");
        exit(1);
    }
    if (!is_pair(args[0]) && !is_equal(args[0], exec->nil))
    {
        log("let: first argument not a list");
        exit(1);
    }
    int var_cnt;
    Expr *var_list = get_list(exec, args[0], &var_cnt);

    pContext context = context_inherit(callContext);
    for (int i = 0; i < var_cnt; i++)
    {
        int len;
        Expr *list = get_list(exec, var_list[i], &len);
        if (len == 0 || len > 2 || !is_atom(list[0]))
        {
            log("let: invalid initializer");
            exit(1);
        }
        Expr name = list[0];
        Expr value;
        if (len == 1)
            value = exec->nil;
        else
            value = exec_eval(exec, context, list[1]);
        free(list);
        if (context_bind(context, name.val_atom, value) == MAP_FAILED)
        {
            log("let context_bind failed");
            exit(1);
        }
    }
    free(var_list);

    Expr res = exec_eval_array(exec, context, args + 1, argc - 1);
    context_unlink(context);

    return res;
}

BUILTIN_FUNC(block)
{
    return exec_eval_array(exec, callContext, args, argc);
}

/* Function creation */

int scan_arguments(pExecutor exec, Expr *args, int argc, int *opt_pos, int *rest_pos)
{
    Expr opt_flag = make_atom(exec, "&optional");
    Expr rest_flag = make_atom(exec, "&rest");

    int opt = -1;
    int rest = -1;

    for (int i = 0; i < argc; i++)
    {
        if (args[i].type == VT_ATOM)
        {
            if (args[i].val_atom == opt_flag.val_atom)
            {
                if (opt == -1 && rest == -1)
                    opt = i;
                else
                    return 0;
            }
            else if (args[i].val_atom == rest_flag.val_atom)
            {
                if (rest == -1)
                    rest = i;
                else
                    return 0;
            }
        }
        else if (opt != -1 && rest == -1) // optional section
        {
            int len;
            Expr *list = get_list(exec, args[i], &len);
            int good = len == 2 && list[0].type == VT_ATOM;
            free(list);
            if (!good)
                return 0;
        }
        else
            return 0;
    }
    if (rest == -1)
        rest = argc;
    if (opt == -1)
        opt = rest;

    *opt_pos = opt;
    *rest_pos = rest;
    return 1;
}

void parse_opt_arg(pExecutor exec, Expr expr, Expr *arg, Expr *def)
{
    if (expr.type == VT_ATOM)
    {
        *arg = expr;
        *def = exec->nil;
    }
    else // VT_PAIR
    {
        int len;
        Expr *list = get_list(exec, expr, &len);
        *arg = list[0];
        *def = list[1];
        free(list);
    }
}

int parse_arguments(pExecutor exec, pUserFunction func, Expr *args, int argc)
{
    int opt_pos, rest_pos;
    if (!scan_arguments(exec, args, argc, &opt_pos, &rest_pos))
        return 0;

    int req_len = opt_pos;
    int opt_len = rest_pos - opt_pos - 1;
    if (opt_len < 0) opt_len = 0;
    int rest_len = argc - rest_pos;

    Expr *req, *opt, *def, rest;

    req = malloc(req_len * sizeof(Expr));
    if (req == NULL)
    {
        log("parse_arguments: malloc failed");
        return 0;
    }
    opt = malloc(opt_len * sizeof(Expr));
    if (opt == NULL)
    {
        log("parse_arguments: malloc failed");
        free(req);
        return 0;
    }
    def = malloc(opt_len * sizeof(Expr));
    if (def == NULL)
    {
        log("parse_arguments: malloc failed");
        free(req);
        free(opt);
        return 0;
    }

    for (int i = 0; i < req_len; i++)
        req[i] = args[i];
    for (int i = 0; i < opt_len; i++)
        parse_opt_arg(exec, args[opt_pos + 1 + i], opt + i, def + i);
    if (rest_len != 0)
        rest = args[rest_pos + 1];
    else
        rest = make_none();

    func->args = req;
    func->argc = req_len;
    func->opt = opt;
    func->def = def;
    func->optc = opt_len;
    func->rest = rest;

    return 1;
}

pUserFunction build_user_function(pExecutor exec, Expr *args, int argc, Expr *body, size_t len)
{
    Expr bodyExpr = make_list(exec, body, len);
    if (is_none(bodyExpr))
    {
        log("build_user_function: make_list failed");
        return NULL;
    }

    pUserFunction res = create_user_function();
    if (res == NULL)
    {
        log("build_user_function: create_user_function failed");
        return NULL;
    }
    if (!parse_arguments(exec, res, args, argc))
    {
        log("build_user_function: parse_arguments failed");
        free(res);
        return NULL;
    }
    res->body = bodyExpr;
    return res;
}

Expr lambda_impl(pExecutor exec, pContext context, Expr *args, size_t argc, enum FunctionType type)
{
    /*
        (lambda (ar gu me nts) body)
    */
    char *caller_name = type == FT_USER ? "lambda" : "macro";
    if (argc < 1 || !is_list(exec, args[0]))
    {
        logf("%s: wrong arguments", caller_name);
        exit(1);
    }
    int f_argc;
    Expr *f_args = get_list(exec, args[0], &f_argc);

    Expr *body = args + 1;
    size_t bodyLen = argc - 1;

    pUserFunction func = build_user_function(exec, f_args, f_argc, body, bodyLen);
    free(f_args);
    if (func == NULL)
    {
        logf("%s: build_user_function failed", caller_name);
        exit(1);
    }
    Expr res = make_user_function(exec, func, context, type);
    if (is_none(res))
    {
        logf("%s: make_user_function failed", caller_name);
        free_user_function(func);
        exit(1);
    }

    return res;
}

BUILTIN_FUNC(lambda)
{
    return lambda_impl(exec, callContext, args, (size_t) argc, FT_USER);
}

BUILTIN_FUNC(macro)
{
    return lambda_impl(exec, callContext, args, (size_t) argc, FT_USER_MACRO);
}

/* Setters */

Expr set_impl(pExecutor exec,
              pContext context,
              Expr *args,
              int argc,
              int check_macro,
              char *caller,
              int (*set_func)(pContext, size_t, Expr))
{
    /*
        (def a 1)
    */
    if (argc != 2)
    {
        logf("%s: wrong arguments syntax", caller);
        exit(1);
    }
    Expr atom = args[0];
    if (atom.type != VT_ATOM)
    {
        logf("%s: wrong arguments syntax", caller);
        exit(1);
    }

    // Evaluate value
    Expr val = exec_eval(exec, context, args[1]);

    if (check_macro && !is_macro(val))
    {
        logf("%s: argument is not a macro", caller);
        exit(1);
    }

    // Set variable
    if (set_func(context, atom.val_atom, val) == MAP_FAILED)
    {
        logf("%s: set failed", caller);
        exit(1);
    }
    return val;
}

BUILTIN_FUNC(def)
{
    return set_impl(exec, callContext, args, argc, 0, "def", context_bind);
}

BUILTIN_FUNC(set)
{
    return set_impl(exec, callContext, args, argc, 0, "set", context_set);
}

BUILTIN_FUNC(defm)
{
    return set_impl(exec, callContext, args, argc, 1, "defm", context_bind_macro);
}

BUILTIN_FUNC(setm)
{
    return set_impl(exec, callContext, args, argc, 1, "setm", context_set_macro);
}

BUILTIN_FUNC(getm)
{
    if (argc != 1)
    {
        log("getm: wrong arguments syntax");
        exit(1);
    }
    Expr atom = args[0];
    if (atom.type != VT_ATOM)
    {
        log("getm: wrong arguments syntax");
        exit(1);
    }
    Expr res;
    if (context_get_macro(callContext, atom.val_atom, &res) == MAP_FAILED)
    {
        log("getm: context_get_macro failed");
        exit(1);
    }

    return res;
}

/* Quote/Backquote */

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

        if (evaluated.type != VT_PAIR &&
            !is_equal(exec->nil, evaluated))
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
        free(items);

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