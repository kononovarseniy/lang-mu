#include "exec.h"
#include "exec_builtin.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "strutils.h"
#include "log.h"

Expr expr_none()
{
    Expr res;
    res.type = VT_NONE;
    return res;
}
int is_none(Expr expr)
{
    return expr.type == VT_NONE;
}
int is_equal(Expr a, Expr b)
{
    if (a.type != b.type) return 0;
    if (a.type & VT_POINTER)
    {
        a = dereference(a);
        b = dereference(b);
    }
    switch (a.type)
    {
    case VT_ATOM:
        return a.val_atom == b.val_atom;
    case VT_CHAR:
        return a.val_char == b.val_char;
    case VT_INT:
        return a.val_int == b.val_int;
    case VT_STRING_VAL:
        return strcmp(a.val_str, b.val_str) == 0;
    case VT_PAIR:
    case VT_NONE:
    default:
        log("is_equal: unable to compare");
        exit(1);
    }
}
int is_true(pExecutor exec, Expr expr)
{
    if ((expr.type == VT_ATOM && expr.val_atom == exec->nil.val_atom) ||
        (expr.type == VT_INT && expr.val_int == 0) ||
        (expr.type == VT_NONE))
        return 0;
    return 1;
}

pExecutor create_executor(void)
{
    char **atoms                = malloc(MAX_ATOMS * sizeof(char*));
    enum GCFlags *atomsFlags    = malloc(MAX_ATOMS * sizeof(enum GCFlags));
    Expr *cars                  = malloc(MAX_PAIRS * sizeof(Expr));
    Expr *cdrs                  = malloc(MAX_PAIRS * sizeof(Expr));
    enum GCFlags *pairsFlags    = malloc(MAX_PAIRS * sizeof(enum GCFlags));

    pHeap heap          = create_heap();
    pContextStack stack = create_context_stack();

    pExecutor res = malloc(sizeof(Executor));

    int success = 1;
    if (atoms == NULL)
    {
        perror("create_executor: `atoms` allocation failed");
        success = 0;
    }
    if (atomsFlags == NULL)
    {
        perror("create_executor: `atomsFlags` allocation failed");
        success = 0;
    }
    if (cars == NULL)
    {
        perror("create_executor: `cars` allocation failed");
        success = 0;
    }
    if (cdrs == NULL)
    {
        perror("create_executor: `cdrs` allocation failed");
        success = 0;
    }
    if (pairsFlags == NULL)
    {
        perror("create_executor: `pairsFlags` allocation failed");
        success = 0;
    }
    if (heap == NULL)
    {
        log("create_executor: create_heap failed");
        success = 0;
    }
    if (stack == NULL)
    {
        log("create_executor: create_context_stack failed");
        success = 0;
    }
    if (res == NULL)
    {
        perror("create_executor: memory allocation failed");
        success = 0;
    }

    if (!success)
    {
        free(atoms);
        free(atomsFlags);
        free(cars);
        free(cdrs);
        free(pairsFlags);
        free_heap(heap);
        free_context_stack(stack);
        free(res);
        return NULL;
    }

    for (size_t i = 0; i < MAX_ATOMS; i++)
        atomsFlags[i] = GC_NONE;
    for (size_t i = 0; i < MAX_PAIRS; i++)
        pairsFlags[i] = GC_NONE;

    res->atomsCount = MAX_ATOMS;
    res->pairsCount = MAX_PAIRS;
    res->atoms = atoms;
    res->atomsFlags = atomsFlags;
    res->cars = cars;
    res->cdrs = cdrs;
    res->pairsFlags = pairsFlags;
    res->heap = heap;
    res->stack = stack;
    res->global = NULL;

    res->code = expr_none();
    res->gc_index = 0;

    return res;
}
void free_executor(pExecutor exec)
{
    context_unlink(exec->global);

    for (int i = 0; i < exec->atomsCount; i++)
        free((exec->atoms[i]));
    free(exec->atoms);
    free(exec->atomsFlags);
    free(exec->cars);
    free(exec->cdrs);
    free(exec->pairsFlags);
    free_heap(exec->heap);
    free_context_stack(exec->stack);
    free(exec);
}

Expr register_atom(pExecutor exec, pContext context, char *name, int bind)
{
    Expr res = make_atom(exec, name);
    if (is_none(res))
    {
        log("register_atom: make_atom failed");
        exit(1);
    }

    if (bind && context_bind(context, res.val_atom, res) == MAP_FAILED)
    {
        log("register_atom: context_bind failed");
        exit(1);
    }
    return res;
}

Expr register_function(pExecutor exec, pContext context, char *name, pBuiltinFunction func)
{
    size_t atom = add_atom(exec, name);

    Expr res = make_builtin_function(exec, func, context);
    if (is_none(res))
    {
        log("register_function: make_builtin_function failed");
        exit(1);
    }

    if (context_bind(context, atom, res) == MAP_FAILED)
    {
        log("register_function: context_bind failed");
        exit(1);
    }

    return res;
}

void exec_init(pExecutor exec)
{
    // Create global
    pContext global = create_context();
    if (global == NULL)
    {
        log("exec_init: create_context failed");
        exit(1);
    }
    context_stack_push(exec->stack, global);
    exec->global = global;


    // Create minimal set of atoms
    exec->nil = register_atom(exec, global, "nil", 1);
    exec->t = register_atom(exec, global, "t", 1);
    exec->quote = register_atom(exec, global, "quote", 0);
    exec->comma = register_atom(exec, global, "#:comma", 0);
    exec->comma_atsign = register_atom(exec, global, "#:comma_atsign", 0);

    // Set code to empty list
    exec->code = exec->nil;

    // Register built-in function
    register_function(exec, global, "set", set);
    register_function(exec, global, "lambda", lambda);
    register_function(exec, global, "cond", cond);
    register_function(exec, global, "print", print);
    register_function(exec, global, "prints", prints);
    register_function(exec, global, "quote", quote);
    register_function(exec, global, "cons", cons);
    register_function(exec, global, "head", head);
    register_function(exec, global, "tail", tail);

    register_function(exec, global, "gensym", gensym);
    register_function(exec, global, "backquote", backquote);
    register_function(exec, global, "macro", macro);
    register_function(exec, global, "setmacro", setmacro);
    register_function(exec, global, "getmacro", getmacro);
    register_function(exec, global, "macroexpand", macroexpand);

    Expr plus_func = register_function(exec, global, "plus", plus);
    context_bind(global, add_atom(exec, "+"), plus_func);
}

void exec_set_code(pExecutor exec, Expr code)
{
    exec->code = code;
}

Expr exec_execute(pExecutor exec)
{
    return exec_eval_all(exec, exec->global, exec->code);
}

Expr exec_builtin_function(pExecutor exec, pFunction func, pContext callContext, Expr *args, int argc)
{
    Expr res = func->builtin(exec, func->context, callContext, args, argc);
    return res;
}

int check_args_count(pUserFunction func, int argc)
{
    if (argc < func->argc) // Too few
        return 0;
    if (func->rest.type != VT_NONE) // Rest present
        return 1;
    if (argc <= func->argc + func->optc) // Not too many
        return 1;
    return 0;
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
    if (!check_args_count(user, argc))
    {
        log("exec_user_function: wrong number of arguments");
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
        return expr_none();
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
    Expr res;
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
Expr exec_macroexpand(pExecutor exec, pContext context, pFunction macro, Expr *args, int len)
{
    if (macro->type != FT_USER_MACRO)
        return expr_none();
    else
        return exec_user_function(exec, macro, context, args, len, 1);
}

char *normalize_name(char *name)
{
    char *copy = strdup(name);
    if (copy == NULL)
    {
        perror("add_atom: strdup failed");
        return NULL;
    }
    strtolower(copy);
    return copy;
}

#define FIND_PLACE_SUCCESS 0
#define FIND_PLACE_EXISTS 1
#define FIND_PLACE_ERROR 2
int find_place_for_atom(pExecutor exec, char *name, size_t *place)
{
    size_t free_pos = EXPR_ERROR;
    for (size_t i = 0; i < exec->atomsCount; i++)
    {
        if (exec->atomsFlags[i] & GC_USED)
        {
            if (strcmp(name, exec->atoms[i]) == 0)
            {
                *place = i;
                return FIND_PLACE_EXISTS;
            }
        }
        else if (free_pos == EXPR_ERROR)
        {
            free_pos = i;
        }
    }
    if (free_pos == EXPR_ERROR)
        return FIND_PLACE_ERROR;
    *place = free_pos;
    return FIND_PLACE_SUCCESS;
}

int find_place_for_pair(pExecutor exec, size_t *place)
{
    for (size_t i = 0; i < exec->atomsCount; i++)
    {
        if (!(exec->pairsFlags[i] & GC_USED))
        {
            *place = i;
            return FIND_PLACE_SUCCESS;
        }
    }
    *place = EXPR_ERROR;
    return FIND_PLACE_ERROR;
}

size_t find_atom(pExecutor exec, char *name)
{
    // Create name copy
    char *copy = normalize_name(name);
    size_t found;
    int res = find_place_for_atom(exec, copy, &found);
    free(copy);
    if (res != FIND_PLACE_EXISTS)
        return EXPR_NOT_FOUND;
    return found;
}

size_t add_atom(pExecutor exec, char *name)
{
    // Create name copy
    char *copy = normalize_name(name);
    size_t found;
    int res = find_place_for_atom(exec, copy, &found);
    if (res == FIND_PLACE_ERROR)
    {
        // not enough place
        // TODO: call GC_collect. And try again
        log("add_atom: out of memory");
        free(copy);
        return EXPR_ERROR;
    }
    if (res == FIND_PLACE_EXISTS)
    {
        free(copy);
        return found;
    }

    exec->atomsFlags[found] = GC_USED;
    exec->atoms[found] = copy;
    return found;
}
void del_atom(pExecutor exec, size_t atom)
{
    exec->atomsFlags[atom] = GC_NONE;
    free(exec->atoms[atom]);
}

size_t add_pair(pExecutor exec)
{
    size_t place;
    int res = find_place_for_pair(exec, &place);
    if (res == FIND_PLACE_ERROR)
    {
        log("add_pair: out of memory");
        return EXPR_ERROR;
    }
    exec->pairsFlags[place] = GC_USED;
    return place;
}
void del_pair(pExecutor exec, size_t pair)
{
    exec->pairsFlags[pair] = GC_NONE;
}

Expr dereference(Expr ptr)
{
    if (!(ptr.type & VT_POINTER))
        return expr_none();
    return ptr.val_ptr->value;
}
Expr get_head(pExecutor exec, Expr pair)
{
    if (pair.type != VT_PAIR) return expr_none();
    return exec->cars[pair.val_pair];
}
Expr get_tail(pExecutor exec, Expr pair)
{
    if (pair.type != VT_PAIR) return expr_none();
    return exec->cdrs[pair.val_pair];
}

int get_len(pExecutor exec, Expr expr)
{
    int len = 0;
    Expr current = expr;
    while (current.type == VT_PAIR)
    {
        len++;
        current = exec->cdrs[current.val_pair];
    }
    return len;
}

Expr *get_items(pExecutor exec, Expr expr, int cnt)
{
    Expr *res = malloc(cnt * sizeof(Expr));
    if (res == NULL)
    {
        perror("get_items: malloc failed");
        return NULL;
    }
    Expr current = expr;
    int i = 0;
    while (i < cnt)
    {
        if (current.type != VT_PAIR)
        {
            log("get_items: len is out of range");
            free(res);
            return NULL;
        }
        res[i] = exec->cars[current.val_pair];
        current = exec->cdrs[current.val_pair];
        i++;
    }
    return res;
}

Expr *get_list(pExecutor exec, Expr expr, int *len)
{
    int l = get_len(exec, expr);
    Expr *arr = get_items(exec, expr, l);
    *len = l;
    return arr;
}

Expr make_atom(pExecutor exec, char *name)
{
    size_t atom = add_atom(exec, name);
    if (atom == EXPR_ERROR)
    {
        log("make_atom: add_atom failed");
        return expr_none();
    }

    Expr res;
    res.type = VT_ATOM;
    res.val_atom = atom;
    return res;
}
Expr make_int(pExecutor exec, long value)
{
    Expr res;
    res.type = VT_INT;
    res.val_int = value;
    return res;
}
Expr make_char(pExecutor exec, char value)
{
    Expr res;
    res.type = VT_CHAR;
    res.val_char = value;
    return res;
}
Expr make_string(pExecutor exec, char *value)
{
    char *copy = strdup(value);
    if (copy == NULL)
    {
        perror("make_string: strdup failed");
        return expr_none();
    }

    Expr res;
    res.type = VT_STRING_VAL;
    res.val_str = copy;

    Expr ptr = gc_register(exec->heap, res);
    if (is_none(ptr))
    {
        log("make_string: gc_register failed");
        free(copy);
        return expr_none();
    }
    return ptr;
}
Expr make_function(pExecutor exec, pFunction value)
{
    Expr res;
    res.type = VT_FUNC_VAL;
    res.val_func = value;

    Expr ptr = gc_register(exec->heap, res);
    if (is_none(ptr))
    {
        log("make_function failed");
        return expr_none();
    }
    return ptr;
}
Expr make_builtin_function(pExecutor exec, pBuiltinFunction func, pContext context)
{
    pFunction funcval = create_function();
    if (funcval == NULL)
    {
        log("make_builtin_function: create_function failed");
        return expr_none();
    }
    funcval->type = FT_BUILTIN;
    funcval->context = context;
    funcval->builtin = func;
    context_link(context);

    Expr res = make_function(exec, funcval);
    if (is_none(res))
    {
        log("make_builtin_function: make_function failed");
        return expr_none();
    }
    return res;
}
Expr make_user_function(pExecutor exec, pUserFunction func, pContext context, enum FunctionType type)
{
    pFunction funcval = create_function();
    if (funcval == NULL)
    {
        log("make_user_function: create_function failed");
        return expr_none();
    }
    funcval->type = type;
    funcval->context = context;
    funcval->user = func;
    context_link(context);

    Expr res = make_function(exec, funcval);
    if (is_none(res))
    {
        log("make_user_function: make_function failed");
        return expr_none();
    }
    return res;
}

Expr make_pair(pExecutor exec, Expr car, Expr cdr)
{
    size_t pair = add_pair(exec);
    if (pair == EXPR_ERROR)
    {
        log("make_list: add_pair failed");
        return expr_none();
    }
    exec->cars[pair] = car;
    exec->cdrs[pair] = cdr;

    Expr res;
    res.type = VT_PAIR;
    res.val_pair = pair;
    return res;
}

Expr make_list(pExecutor exec, Expr *arr, int len)
{
    Expr list = exec->nil;
    for (int i = len - 1; i >= 0; i--)
    {
        Expr tail = make_pair(exec, arr[i], list);
        if (tail.type == VT_NONE)
        {
            log("make_list: make_pair failed");
            return expr_none();
        }
        list = tail;
    }
    return list;
}

// struct Function functions

pFunction create_function()
{
    pFunction res = malloc(sizeof(Function));
    if (res == NULL)
    {
        perror("create_function: malloc failed");
        return NULL;
    }
    res->type = FT_NONE;
    res->context = NULL;
    res->gc_index = 0;
    return res;
}

void free_function(pFunction func)
{
    switch (func->type)
    {
        case FT_BUILTIN:
            break;
        case FT_USER_MACRO:
        case FT_USER:
            free_user_function(func->user);
            break;
        default:
            log("free_function: unknown function type. Unable to free");
            break;
    }
    context_unlink(func->context);
    free(func);
}

pUserFunction create_user_function()
{
    pUserFunction res = malloc(sizeof(UserFunction));
    if (res == NULL)
    {
        log("create_user_function: malloc failed");
        return NULL;
    }
    return res;
}

void free_user_function(pUserFunction func)
{
    free(func->args);
    free(func->opt);
    free(func->def);
    free(func);
}
