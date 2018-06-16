#include "eval.h"

#include "utils/log.h"

Expr exec_builtin_function(pExecutor exec, pFunction func, pContext callContext, Expr *args, int argc)
{
    Expr res = func->builtin(exec, func->context, callContext, args, argc);
    return res;
}

// -1 too few
//  0 accepted
// +1 too many
int check_args_count(pUserFunction func, int argc)
{
    // Less than number of required arguments
    if (argc < func->argc)
        return -1;
    // Rest is present so any big number is accepted
    if (func->rest.type != VT_NONE)
        return 0;
    // Less than number of required and optional arguments
    if (argc <= func->argc + func->optc) // Not too many
        return 0;
    // Too many
    return 1;
}

int bind_req_and_opt_args(pExecutor exec, pContext execContext, pUserFunction func, Expr *args, int argc)
{
    int fullc = func->argc + func->optc;
    for (int i = 0; i < fullc; i++)
    {
        Expr var;
        if (i < func->argc)
            var = func->args[i];
        else
            var = func->opt[i - func->argc];

        Expr value;
        if (i < argc)
            value = args[i];
        else
            value = func->def[i - func->argc];

        if (context_bind(execContext, var.val_atom, value) == MAP_FAILED)
        {
            log("bind_req_and_opt_args: context_bind failed");
            return 0;
        }
    }
    return 1;
}

int bind_rest(pExecutor exec, pContext execContext, pUserFunction func, Expr *args, int argc)
{
    int fullc = func->argc + func->optc;
    if (func->rest.type != VT_NONE) // Rest present
    {
        Expr rest_value = exec->nil;
        int restc = argc - fullc;
        if (restc > 0)
            rest_value = make_list(exec, args + fullc, restc);

        if (context_bind(execContext, func->rest.val_atom, rest_value) == MAP_FAILED)
        {
            log("bind_rest: context_bind failed");
            return 0;
        }
    }
    return 1;
}

int user_function_bind_args(pExecutor exec, pContext execContext, pContext callContext, pUserFunction func, Expr *args, int argc)
{
    if (!bind_req_and_opt_args(exec, execContext, func, args, argc))
    {
        return 0;
    }
    if (!bind_rest(exec, execContext, func, args, argc))
    {
        return 0;
    }

    return 1;
}

Expr exec_user_function(pExecutor exec, pFunction func, pContext callContext, Expr *args, int argc, int expand)
{
    pUserFunction user = func->user;
    int check_res = check_args_count(user, argc);
    if (check_res != 0)
    {
        logf("exec_user_function: too %s arguments", check_res == -1 ? "few" : "many");
        exit(1);
    }
    // Eval arguments
    Expr *values = malloc(argc * sizeof(Expr));
    if (func->type != FT_USER_MACRO)
    {
        if (values == NULL)
        {
            log("exec_user_function: malloc failed");
            exit(1);
        }
        for (int i = 0; i < argc; i++)
            values[i] = exec_eval(exec, callContext, args[i]);
    }
    else
    {
        for (int i = 0; i < argc; i++)
            values[i] = args[i];
    }
    // Bind arguments
    pContext execContext = context_inherit(func->context);
    if (!user_function_bind_args(exec, execContext, callContext, user, values, argc))
    {
        log("exec_user_function: user_function_bind_args failed");
        free(values);
        exit(1);
    }
    free(values);
    // Push context
    if (!context_stack_push(exec->stack, execContext))
    {
        log("exec_user_function: context_stack_push failed");
        exit(1);
    }
    // Execute
    Expr res = exec_eval_all(exec, execContext, user->body);
    // Pop context
    if (!context_stack_pop(exec->stack))
    {
        log("exec_user_function: context_stack_pop failed");
        exit(1);
    }
    context_unlink(execContext);
    // Execute return
    if (func->type == FT_USER_MACRO && !expand)
        res = exec_eval(exec, callContext, res);

    return res;
}

Expr exec_function(pExecutor exec, pContext callContext, pFunction func, Expr *args, int argc)
{
    if (func->type == FT_BUILTIN)
        return exec_builtin_function(exec, func, callContext, args, argc);
    else if (func->type == FT_USER || func->type == FT_USER_MACRO)
        return exec_user_function(exec, func, callContext, args, argc, 0);
    else
    {
        log("exec_function: invalid_function");
        // TODO: print debug info
        exit(1);
    }
}

Expr get_function(pExecutor exec, pContext context, Expr list_head)
{
    if (list_head.type == VT_ATOM)
    {
        Expr macro;
        if (context_get_macro(context, list_head.val_atom, &macro) == MAP_SUCCESS)
        {
            return dereference(macro);
        }
    }
    Expr func = exec_eval(exec, context, list_head);
    if (func.type & VT_POINTER)
        func = dereference(func);

    // if value is macro it is unregistered macro.
    if (func.type != VT_FUNC_VAL || func.val_func->type == FT_USER_MACRO)
    {
        return make_none();
    }
    return func;
}

Expr exec_eval(pExecutor exec, pContext context, Expr expr)
{
    if (expr.type == VT_ATOM)
    {
        Expr value;
        if (context_get(context, expr.val_atom, &value) == MAP_FAILED)
        {
            logf("eval: atom \"%s\" not binded", exec->atoms[expr.val_atom]);
            exit(1);
        }
        return value;
    }
    else if (expr.type != VT_PAIR)
    {
        return expr;
    }

    int len;
    Expr *list = get_list(exec, expr, &len);

    Expr func = get_function(exec, context, list[0]);
    if (is_none(func))
    {
        log("eval: list head not a function or registered macro");
        // TODO: print debug information
        exit(1);
    }
    Expr result = exec_function(exec, context, func.val_func, list + 1, len - 1);
    free(list);
    return result;
}

Expr exec_eval_array(pExecutor exec, pContext context, Expr *array, int len)
{
    Expr res = exec->nil;
    for (int i = 0; i < len; i++)
    {
        res = exec_eval(exec, context, array[i]);
    }
    return res;
}
Expr exec_eval_all(pExecutor exec, pContext context, Expr expr)
{
    int len;
    Expr *list = get_list(exec, expr, &len);
    Expr res = exec_eval_array(exec, context, list, len);
    free(list);
    return res;
}
Expr *exec_eval_each(pExecutor exec, pContext context, Expr *array, int len)
{
    Expr *res = malloc(len * sizeof(Expr));
    for (int i = 0; i < len; i++)
    {
        res[i] = exec_eval(exec, context, array[i]);
    }
    return res;
}
Expr exec_macroexpand(pExecutor exec, pContext context, pFunction macro, Expr *args, int len)
{
    if (macro->type != FT_USER_MACRO)
        return make_none();
    else
        return exec_user_function(exec, macro, context, args, len, 1);
}
