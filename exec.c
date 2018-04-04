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

pExecutor create_executor(void)
{
    char **atoms = malloc(MAX_ATOMS * sizeof(char*));
    if (atoms == NULL)
    {
        perror("create_executor: `atoms` allocation failed");
        return NULL;
    }

    Expr *cars = malloc(MAX_PAIRS * sizeof(Expr));
    if (cars == NULL)
    {
        perror("create_executor: `cars` allocation failed");
        free(atoms);
        return NULL;
    }

    Expr *cdrs = malloc(MAX_PAIRS * sizeof(Expr));
    if (cdrs == NULL)
    {
        perror("create_executor: `cdrs` allocation failed");
        free(atoms);
        free(cdrs);
        return NULL;
    }

    pExecutor res = malloc(sizeof(Executor));
    if (res == NULL)
    {
        perror("create_executor: memory allocation failed");
        free(atoms);
        free(cars);
        free(cdrs);
        return NULL;
    }

    res->atomsCount = 0;
    res->pairsCount = 0;
    res->atoms = atoms;
    res->cars = cars;
    res->cdrs = cdrs;
    return res;
}
void free_executor(pExecutor exec)
{
    for (int i = 0; i < exec->atomsCount; i++)
        free((exec->atoms[i]));
    free(exec->atoms);
    free(exec->cars);
    free(exec->cdrs);
    free(exec);
}

Expr register_atom(pExecutor exec, pContext context, char *name, int bind)
{
    size_t atom = add_atom(exec, name);
    if (atom == EXPR_ERROR)
    {
        log("register_atom: add_atom failed");
        exit(1);
    }

    Expr res;
    res.type = VT_ATOM;
    res.val_atom = atom;

    if (bind && context_bind(context, atom, res) == MAP_FAILED)
    {
        log("self_bind_atom: context_bind failed");
        exit(1);
    }
    return res;
}

int self_bind_atom(pContext context, Expr atom)
{
    return 1;
}

Expr register_function(pExecutor exec, pContext context, char *name, pBuiltinFunction func)
{
    size_t atom = add_atom(exec, name);

    pFunction funcval = create_function();
    if (funcval == NULL)
    {
        log("register_function: create_function failed");
        exit(1);
    }
    funcval->type = FT_BUILTIN;
    funcval->context = context;
    funcval->builtin = func;
    context_link(context);

    Expr expr;
    expr.type = VT_FUNC;
    expr.val_func = funcval;

    if (context_bind(context, atom, expr) == MAP_FAILED)
    {
        log("register_function: context_bind failed");
        exit(1);
    }

    return expr;
}

void exec_init(pExecutor exec, pContext context)
{
    // Create minimal set of atoms
    exec->nil = register_atom(exec, context, "nil", 1);
    exec->t = register_atom(exec, context, "t", 1);
    exec->quote = register_atom(exec, context, "quote", 0);

    // Register built-in function
    register_function(exec, context, "set", set);
    register_function(exec, context, "lambda", lambda);
    register_function(exec, context, "print", print);
    register_function(exec, context, "prints", prints);
    register_function(exec, context, "quote", quote);
    register_function(exec, context, "cons", cons);
    register_function(exec, context, "head", head);
    register_function(exec, context, "tail", tail);
    Expr plus_func = register_function(exec, context, "plus", plus);

    context_bind(context, add_atom(exec, "+"), plus_func);
}

void exec_cleanup(pExecutor exec)
{
    // TODO: implement exec_cleanup
}

Expr load_atom(pExecutor exec, pSTree item)
{
    Expr res;
    res.type = VT_ATOM;
    res.val_atom = add_atom(exec, item->name);
    return res;
}

Expr load_int(pExecutor exec, pSTree item)
{
    Expr res;
    res.type = VT_INT;
    res.val_int = item->int_val;
    return res;
}

Expr load_char(pExecutor exec, pSTree item)
{
    Expr res;
    res.type = VT_CHAR;
    res.val_char = item->char_val;
    return res;
}

Expr load_string(pExecutor exec, pSTree item)
{
    // TODO: register string somewhere to be able to free it
    Expr res;
    res.type = VT_STRING;
    res.val_str = item->str_val;
    return res;
}

Expr load_item(pExecutor exec, pSTree item)
{
    switch (item->type)
    {
    case NODE_LIST:
        return exec_load_tree(exec, item->child);
    case NODE_NAME:
        return load_atom(exec, item);
    case NODE_INT:
        return load_int(exec, item);
    case NODE_CHAR:
        return load_char(exec, item);
    case NODE_STR:
        return load_string(exec, item);
    default:
        log("load_item: unknown node type");
        return expr_none();
    }
}

Expr exec_load_tree(pExecutor exec, pSTree tree)
{
    if (tree == NULL) return exec->nil;

    // Load item
    Expr item = load_item(exec, tree);
    if (item.type == VT_NONE)
    {
        log("exec_load_tree: item loading failed");
        return expr_none();
    }

    // Load tail
    Expr tail = exec_load_tree(exec, tree->next);
    if (tail.type == VT_NONE)
    {
        log("exec_load_tree: tail loading failed");
        return expr_none();
    }

    // Make pair
    size_t pair = add_pair(exec);
    if (pair == EXPR_ERROR)
    {
        log("exec_load_tree: add_pair failed");
        return expr_none();
    }

    exec->cars[pair] = item;
    exec->cdrs[pair] = tail;

    Expr res;
    res.type = VT_PAIR;
    res.val_pair = pair;
    return res;
}

Expr exec_builtin_function(pExecutor exec, pFunction func, pContext callContext, Expr *args, int argc)
{
    Expr res = func->builtin(exec, func->context, callContext, args, argc);
    return res;
}

Expr exec_user_function(pExecutor exec, pFunction func, pContext callContext, Expr *args, int argc)
{
    pUserFunction user = func->user;
    if (argc != user->argc)
    {
        log("exec_function: wrong number of parameters");
        exit(1);
    }
    pContext execContext = context_inherit(func->context);
    for (int i = 0; i < argc; i++)
    {
        Expr value = exec_eval(exec, callContext, args[i]);
        int bind_res = context_bind(execContext, user->args[i].val_atom, value);
        if (bind_res == MAP_FAILED)
        {
            log("exec_user_function: context_bind failed");
            exit(1);
        }
    }
    Expr res = exec_eval_all(exec, execContext, user->body);
    context_unlink(execContext);
    return res;
}

Expr exec_function(pExecutor exec, pContext callContext, pFunction func, Expr *args, int argc)
{
    if (func->type == FT_BUILTIN)
        return exec_builtin_function(exec, func, callContext, args, argc);
    else if (func->type == FT_USER)
        return exec_user_function(exec, func, callContext, args, argc);
    else
    {
        log("exec_function: invalid_function");
        // TODO: print debug info
        exit(1);
    }
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

    Expr func = exec_eval(exec, context, list[0]);
    if (func.type != VT_FUNC)
    {
        log("eval: list head not a function");
        // TODO: print debug information
        exit(1);
    }
    Expr result = exec_function(exec, context, func.val_func, list + 1, len - 1);
    free(list);
    return result;
}

Expr exec_eval_all(pExecutor exec, pContext context, Expr expr)
{
    int len;
    Expr *list = get_list(exec, expr, &len);

    Expr res;
    for (int i = 0; i < len; i++)
    {
        res = exec_eval(exec, context, list[i]);
    }
    return res;
}

size_t add_atom(pExecutor exec, char *name)
{
    // Create name copy
    char *copy = strdup(name);
    if (copy == NULL)
    {
        perror("add_atom: strdup failed");
        return EXPR_ERROR;
    }
    strtolower(copy);
    // Search for existing atom with such name
    for (size_t i = 0; i < exec->atomsCount; i++)
    {
        if (strcmp(copy, exec->atoms[i]) == 0)
        {
            free(copy);
            return i;
        }
    }
    // Atom not exists. Create it!
    if (exec->atomsCount == MAX_ATOMS)
    {
        log("add_atom: limit exceeded");
        return EXPR_ERROR;
    }
    exec->atoms[exec->atomsCount] = copy;

    size_t res = exec->atomsCount;
    exec->atomsCount++;
    return res;
}

size_t add_pair(pExecutor exec)
{
    if (exec->pairsCount == MAX_PAIRS)
    {
        log("add_pair: limit exceeded");
        return EXPR_ERROR;
    }

    int res = exec->pairsCount;
    exec->pairsCount++;
    return res;
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

pFunction create_lambda(pExecutor exec, pContext defContext, Expr *args, int argc, Expr *body, int len)
{
    // Check arguments list
    for (int i = 0; i < 0; i++)
    {
        if (args[i].type != VT_ATOM)
        {
            log("create_lambda: all arguments must be atoms");
            return NULL;
        }
    }

    Expr bodyExpr = make_list(exec, body, len);
    if (bodyExpr.type == VT_NONE)
    {
        log("create_lambda: make_list failed");
        return NULL;
    }

    Expr *argsCopy = malloc(argc * sizeof(Expr));
    if (argsCopy == NULL)
    {
        log("create_lambda: malloc failed");
        return NULL;
    }
    memcpy(argsCopy, args, argc * sizeof(Expr));

    pUserFunction user = create_user_function();
    if (user == NULL)
    {
        log("create_lambda: create_user_function failed");
        free(argsCopy);
        return NULL;
    }
    user->args = argsCopy;
    user->argc = argc;
    user->body = bodyExpr;

    pFunction func = create_function();
    if (func == NULL)
    {
        log("create_lambda: create_function failed");
        free_user_function(user); // this will also free argsCopy
        return NULL;
    }
    func->type = FT_USER;
    func->context = defContext;
    func->user = user;
    context_link(defContext);

    return func;
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
    return res;
}

void free_function(pFunction func)
{
    switch (func->type)
    {
        case FT_BUILTIN: break;
        case FT_USER:
            free_user_function(func->user);
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
    free(func);
}

