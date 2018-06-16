//
// Created by arseniy on 16.06.18.
//

#include "functions.h"

#include "utils/log.h"

BUILTIN_FUNC(gensym)
{
    if (argc != 0)
    {
        log("gensym: too many arguments");
        exit(1);
    }
    Expr counter = make_atom(exec, "*gensym-counter*");
    if (is_none(counter))
    {
        log("gensym: make_atom failed");
        exit(1);
    }
    Expr value;
    if (context_get(defContext, counter.val_atom, &value) == MAP_FAILED || !is_int(value))
    {
        value = make_int_one(exec);
        if (is_none(value))
        {
            log("gensym: exec_int_one failed");
            exit(1);
        }
    }
    long n = exec_int_to_long(exec, value);
    for (;; n++)
    {
        char name[7];
        sprintf(name, "#:%ld", n);
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
            value = make_int_from_long(exec, n);
            if (context_set(defContext, counter.val_atom, value) == MAP_FAILED)
            {
                log("gensym: context_bind failed");
                exit(1);
            }
            return res;
        }
    }
}

BUILTIN_FUNC(macroexpand)
{
    if (argc != 2)
    {
        log("macroexpand: wrong arguments count");
        exit(1);
    }
    Expr name = exec_eval(exec, callContext, args[0]);
    if (name.type != VT_ATOM)
    {
        log("macroexpand: wrong argument type");
        exit(1);
    }
    Expr macro_args = exec_eval(exec, callContext, args[1]);
    if (macro_args.type != VT_PAIR)
    {
        log("macroexpand: wrong argument type");
        exit(1);
    }
    Expr expanded;
    Expr success;
    Expr macro;
    if (context_get_macro(callContext, name.val_atom, &macro) == MAP_FAILED)
    {
        expanded = exec->nil;
        success = exec->nil;
    }
    else
    {
        macro = dereference(macro);
        int len;
        Expr *args_array = get_list(exec, macro_args, &len);
        Expr expand_res = exec_macroexpand(exec, callContext, macro.val_func, args_array, len);
        if (is_none(expand_res))
        {
            expanded = exec->nil;
            success = exec->nil;
        }
        else
        {
            expanded = expand_res;
            success = exec->t;
        }
        free(args_array);
    }
    return make_pair(exec, expanded, make_pair(exec, success, exec->nil));
}

BUILTIN_FUNC(eval_builtin)
{
    if (argc > 1)
    {
        log("eval: too many arguments");
        exit(1);
    }
    if (argc < 1)
    {
        log("eval: too few arguments");
        exit(1);
    }
    // Get expression to evaluate
    Expr expr = exec_eval(exec, callContext, args[0]);
    return exec_eval(exec, callContext, expr);
}
