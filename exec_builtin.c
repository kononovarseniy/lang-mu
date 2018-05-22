#include "exec_builtin.h"

#include <stdlib.h>
#include <stdio.h>
#include "log.h"

#include "print.h"

Expr set_impl(
              pExecutor exec,
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

Expr set_pair_impl(pExecutor exec, pContext context, Expr *args, int argc, char *caller, void (*set_func)(pExecutor, Expr, Expr))
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

BUILTIN_FUNC(print)
{
    for (int i = 0; i < argc; i++)
    {
        Expr value = exec_eval(exec, callContext, args[i]);
        print_expression(stdout, exec, value, PF_SHORT_QUOTE, 0);
        if (i < argc - 1) printf(" ");
    }
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

BUILTIN_FUNC(eq)
{
    if (argc < 2)
    {
        log("eq: too few arguments");
        exit(1);
    }
    if (argc > 2)
    {
        log("eq: too many arguments");
        exit(1);
    }
    Expr a = exec_eval(exec, callContext, args[0]);
    Expr b = exec_eval(exec, callContext, args[1]);
    if (is_equal(a, b))
        return exec->t;
    else
        return exec->nil;
}
BUILTIN_FUNC(and)
{
    for (int i = 0; i < argc; i++)
    {
        Expr val = exec_eval(exec, callContext, args[i]);
        if (!is_true(exec, val))
            return exec->nil;
    }
    return exec->t;
}
BUILTIN_FUNC(or)
{
    for (int i = 0; i < argc; i++)
    {
        Expr val = exec_eval(exec, callContext, args[i]);
        if (is_true(exec, val))
            return exec->t;
    }
    return exec->nil;
}
BUILTIN_FUNC(not)
{
    if (argc < 1)
    {
        log("not: too few arguments");
        exit(1);
    }
    if (argc > 1)
    {
        log("not: too many arguments");
        exit(1);
    }

    Expr val = exec_eval(exec, callContext, args[0]);
    if (is_true(exec, val))
        return exec->nil;
    else
        return exec->t;
}
BUILTIN_FUNC(xor)
{
    int cnt = 0;
    for (int i = 0; i < argc; i++)
    {
        Expr val = exec_eval(exec, callContext, args[i]);
        if (is_true(exec, val))
            cnt++;
    }
    if (cnt == 1)
        return exec->t;
    else
        return exec->nil;

}

Expr compare_impl(pExecutor exec, pContext context, char *caller, Expr *args, int argc, int order, int strict)
{
    int success = 1;

    Expr prev;
    for (int i = 0; i < argc; i++)
    {
        Expr value = exec_eval(exec, context, args[i]);
        if (i > 0)
        {
            int cmpres = exec_compare(exec, prev, value);
            if (!(cmpres == order || (!strict && cmpres == 0)))
            {
                success = 0;
                break;
            }
        }
        prev = value;
    }
    if (success)
        return exec->t;
    else
        return exec->nil;
}

BUILTIN_FUNC(less_builtin)
{
    return compare_impl(exec, callContext, "less", args, argc, -1, 1);
}
BUILTIN_FUNC(less_or_equals_builtin)
{
    return compare_impl(exec, callContext, "less_or_equals", args, argc, -1, 0);
}
BUILTIN_FUNC(num_equals_builtin)
{
    return compare_impl(exec, callContext, "less_or_equals", args, argc, 0, 0);
}
BUILTIN_FUNC(more_or_equals_builtin)
{
    return compare_impl(exec, callContext, "more_or_equals", args, argc, 1, 0);
}
BUILTIN_FUNC(more_builtin)
{
    return compare_impl(exec, callContext, "more", args, argc, 1, 1);
}

void check_numeric_arguments(Expr *args, int argc, char *caller)
{
    for (int i = 0; i < argc; i++)
    {
        if (!is_int(args[i]))
        {
            logf("%s: integer expected", caller);
            exit(1);
        }
    }
}

Expr make_int_checked(pExecutor exec, pLongNum num, char *caller)
{
    Expr res = make_int(exec, num);
    if (is_none(res))
    {
        logf("%s: make_int failed", caller);
        exit(1);
    }
    return res;
}

BUILTIN_FUNC(sum_builtin)
{
    if (argc == 0)
        return make_int_zero(exec);

    Expr *values = exec_eval_each(exec, callContext, args, argc);

    Expr num = values[0];
    for (int i = 1; i < argc; i++)
        num = exec_sum(exec, num, values[i]);

    free(values);
    return num;
}
BUILTIN_FUNC(difference_builtin)
{
    if (argc == 0)
    {
        log("difference: too few arguments");
        exit(1);
    }
    Expr *values = exec_eval_each(exec, callContext, args, argc);

    Expr first = values[0];
    if (argc == 1)
    {
        free(values);
        return exec_inverse(exec, first);
    }

    Expr num = first;
    for (int i = 1; i < argc; i++)
        num = exec_difference(exec, num, values[i]);

    free(values);
    return num;
}
BUILTIN_FUNC(product_builtin)
{
    if (argc == 0)
        return make_int_one(exec);

    Expr *values = exec_eval_each(exec, callContext, args, argc);

    Expr num = values[0];
    for (int i = 1; i < argc; i++)
        num = exec_product(exec, num, values[i]);

    free(values);
    return num;
}
BUILTIN_FUNC(quotient_builtin)
{
    if (argc == 0)
    {
        log("quotient: too few arguments");
        exit(1);
    }
    Expr *values = exec_eval_each(exec, callContext, args, argc);

    Expr first = values[0];
    if (argc == 1)
    {
        free(values);
        return exec_division_inverse(exec, first);
    }

    // Calculate product of all arguments except first
    Expr divisor = values[1];
    for (int i = 2; i < argc; i++)
        divisor = exec_product(exec, divisor, values[i]);

    free(values);

    return exec_quotient(exec, first, divisor);
}
BUILTIN_FUNC(remainder_builtin)
{
    if (argc < 2)
    {
        log("remainder: too few arguments");
        exit(1);
    }
    if (argc > 2)
    {
        log("remainder: too many arguments");
        exit(1);
    }
    Expr *values = exec_eval_each(exec, callContext, args, argc);

    Expr first = values[0];
    Expr second = values[1];
    free(values);

    return exec_remainder(exec, first, second);
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

pUserFunction build_user_function(pExecutor exec, Expr *args, int argc, Expr *body, int len)
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

Expr lambda_impl(pExecutor exec, pContext context, Expr *args, int argc, enum FunctionType type)
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
    int bodyLen = argc - 1;

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
    return lambda_impl(exec, callContext, args, argc, FT_USER);
}

BUILTIN_FUNC(macro)
{
    return lambda_impl(exec, callContext, args, argc, FT_USER_MACRO);
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
BUILTIN_FUNC(error)
{
    fprintf(stderr, "ERROR: ");
    for (int i = 0; i < argc; i++)
    {
        Expr value = exec_eval(exec, callContext, args[i]);
        print_expression(stderr, exec, value, PF_SHORT_QUOTE | PF_UNESCAPE, 0);
        if (i < argc - 1) fprintf(stderr, " ");
    }
    exit(1);
}

BUILTIN_FUNC(block)
{
    return exec_eval_array(exec, callContext, args, argc);
}

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
    for (;;n++)
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

BUILTIN_FUNC(gc_collect_builtin)
{
    if (argc != 0)
    {
        log("gc-collect: too many arguments");
        exit(1);
    }
    int atoms, pairs, objects;
    gc_collectv(exec, &atoms, &pairs, &objects);
    Expr arr[3] =
    {
        make_int_from_long(exec, atoms),
        make_int_from_long(exec, pairs),
        make_int_from_long(exec, objects)
    };
    Expr res = make_list(exec, arr, 3);
    return res;
}

