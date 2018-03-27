#include "exec_internal.h"
#include "exec_builtin.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "strutils.h"

pExecutor create_executor(void)
{
    pExecutor res = malloc(sizeof(Executor));
    if (res == NULL)
    {
        perror("create_executor: malloc failed");
        return NULL;
    }

    res->atoms = malloc(MAX_ATOMS * sizeof(char*));
    if (res->atoms == NULL)
    {
        perror("create_executor: malloc failed");
        free(res);
        return NULL;
    }

    res->pairs = malloc(MAX_PAIRS * sizeof(Pair));
    if (res->pairs == NULL)
    {
        perror("create_executor: malloc failed");
        free(res);
        return NULL;
    }

    res->atomsCount = 0;
    res->pairsCount = 0;
    return res;
}
void free_executor(pExecutor exec)
{
    for (int i = 0; i < exec->atomsCount; i++)
        free((exec->atoms[i]));
    free(exec->atoms);
    free(exec->pairs);
    free(exec);
}

void register_function(pExecutor exec, pContext context, char *name, BuiltinFunction func)
{
    int atom = add_atom(exec, name);

    pFunction funcval = create_function();
    if (funcval == NULL)
    {
        printf("register_function: create_function failed");
        exit(1);
    }
    funcval->context = context;
    funcval->type = FT_BUILTIN;
    funcval->builtin = func;

    pTypedValue val = create_typed_value();
    if (val == NULL)
    {
        printf("register_function: create_typed_value failed");
        exit(1);
    }
    val->type = VT_FUNC;
    val->func = funcval;

    if (context_bind(context, atom, val) == MAP_FAILED)
    {
        printf("register_function: context_bind failed");
        exit(1);
    }
    typed_value_link(val);
}

void exec_init(pExecutor exec, pContext context)
{
    exec->nil = add_atom(exec, "nil");
    exec->t = add_atom(exec, "t");

    register_function(exec, context, "def", def);
    register_function(exec, context, "print", print);
}

int add_or_get_atom(pExecutor exec, char *name)
{
    name = strdup(name);
    strtolower(name);
    for (int i = 0; i < exec->atomsCount; i++)
    {
        if (strcmp(name, exec->atoms[i]) == 0)
        {
            free(name);
            return i;
        }
    }
    int res = add_atom(exec, name);
    free(name);
    return res;
}

int exec_load_tree(pExecutor exec, pSTree tree)
{
    if (tree == NULL) return exec->nil;

    // Load item
    int item = EXPR_ERROR;
    switch (tree->type)
    {
    case NODE_LIST:
        item = exec_load_tree(exec, tree->child);
        break;
    case NODE_NAME:
        item = add_or_get_atom(exec, tree->name);
        break;
    default:
        printf("exec_load_tree: unknown node type");
        break;
    }
    if (item == EXPR_ERROR)
    {
        printf("exec_load_tree: subexpression loading failed");
        return EXPR_ERROR;
    }

    // Load tail
    int tail = exec_load_tree(exec, tree->next);
    if (tail == EXPR_ERROR)
    {
        printf("exec_load_tree: tail loading failed");
        return EXPR_ERROR;
    }

    // Make pair
    int pair = add_pair(exec);
    if (pair == EXPR_ERROR)
    {
        printf("exec_load_tree: add_pair failed");
        return EXPR_ERROR;
    }

    pPair p = exec->pairs + pair_ind(pair);
    p->left = item;
    p->right = tail;

    return pair;
}

pTypedValue exec_function(pExecutor exec, pContext callContext, pFunction func, int args)
{
    if (func->type == FT_BUILTIN)
    {
        int argc;
        int *args_arr = get_list(exec, args, &argc);
        if (args_arr == NULL)
        {
            printf("exec_function: get_list failed");
            exit(1);
        }
        pTypedValue res = func->builtin(exec, func->context, callContext, args_arr, argc);
        free(args_arr);
        return res;
    }
    else
    {
        printf("exec_function: invalid_function");
        // TODO: print debug info
        exit(1);
    }
}

pTypedValue exec_eval(pExecutor exec, int expr, pContext context)
{
    // (foo 1 2 3 (+ 4 5))
    pPair name_args = get_pair(exec, expr);
    if (name_args == NULL) // if expr is atom
    {
        pTypedValue val;
        if (context_get(context, expr, (void **)&val) == MAP_FAILED)
        {
            printf("exec_eval: atom \"%s\" not binded", exec->atoms[expr]);
            exit(1);
        }
        typed_value_link(val);
        return val;
    }
    int func = name_args->left;
    int args = name_args->right;

    pTypedValue head_val = exec_eval(exec, func, context);
    if (head_val->type != VT_FUNC)
    {
        printf("exec_eval: list head not a function");
        // TODO: print debug information
        exit(1);
    }

    pTypedValue res = exec_function(exec, context, head_val->func, args);

    return res;
}
