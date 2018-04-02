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

Expr register_atom(pExecutor exec, char *name)
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
    return res;
}

Expr register_function(pExecutor exec, pContext context, char *name, BuiltinFunction func)
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
    exec->nil = register_atom(exec, "nil");
    exec->t = register_atom(exec, "t");
    exec->quote = register_atom(exec, "quote");

    // Register built-in function
    register_function(exec, context, "def", def);
    register_function(exec, context, "print", print);
    register_function(exec, context, "quote", quote);
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

Expr exec_function(pExecutor exec, pContext callContext, pFunction func, Expr *args, int argc)
{
    if (func->type == FT_BUILTIN)
    {
        Expr res = func->builtin(exec, func->context, callContext, args, argc);
        return res;
    }
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
        default:
            log("free_function: unknown function type. Unable to free");
            break;
    }
    free(func);
}
