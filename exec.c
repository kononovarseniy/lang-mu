#include "exec.h"

#include <stdlib.h>
#include <string.h>

#include "strutils.h"
#include "log.h"

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

    res->code = make_none();
    res->gc_index = 0;

    return res;
}
void free_executor(pExecutor exec)
{
    context_unlink(exec->global);

    for (int i = 0; i < exec->atomsCount; i++)
        if (exec->atomsFlags[i] & GC_USED)
            free(exec->atoms[i]);
    free(exec->atoms);
    free(exec->atomsFlags);
    free(exec->cars);
    free(exec->cdrs);
    free(exec->pairsFlags);
    free_heap(exec->heap);
    free_context_stack(exec->stack);
    free(exec);
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

Expr register_alias(pExecutor exec, pContext context, char *name, char *alias)
{
    Expr key = make_atom(exec, name);
    Expr alias_key = make_atom(exec, alias);
    if (is_none(key) || is_none(alias_key))
    {
        log("register_alias: make_atom failed");
        exit(1);
    }
    Expr value = exec_eval(exec, context, key);
    if (context_bind(context, alias_key.val_atom, value) == MAP_FAILED)
    {
        log("register_alias: context_bind failed");
        exit(1);
    }
    return value;
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
    register_function(exec, global, "def", def);
    register_function(exec, global, "set", set);

    register_function(exec, global, "defm", defm);
    register_function(exec, global, "setm", setm);
    register_function(exec, global, "getm", getm);

    register_function(exec, global, "set-head", set_head_builtin);
    register_function(exec, global, "set-tail", set_tail_builtin);

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
    register_function(exec, global, "macroexpand", macroexpand);
    register_function(exec, global, "gc-collect", gc_collect_builtin);

    register_function(exec, global, "eq", eq);
    register_function(exec, global, "and", and);
    register_function(exec, global, "or", or);
    register_function(exec, global, "not", not);
    register_function(exec, global, "xor", xor);

    register_function(exec, global, "+", sum_builtin);
    register_function(exec, global, "-", difference_builtin);
    register_function(exec, global, "*", product_builtin);
    register_function(exec, global, "/", quotient_builtin);
    register_function(exec, global, "%", remainder_builtin);

    register_function(exec, global, "<", less_builtin);
    register_function(exec, global, "<=", less_or_equals_builtin);
    register_function(exec, global, "==", num_equals_builtin);
    register_function(exec, global, ">=", more_or_equals_builtin);
    register_function(exec, global, ">", more_builtin);
}

void exec_set_code(pExecutor exec, Expr code)
{
    exec->code = code;
}

Expr exec_execute(pExecutor exec)
{
    return exec_eval_all(exec, exec->global, exec->code);
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
        // free unused memory and try again
        gc_collect(exec);
        res = find_place_for_atom(exec, copy, &found);
        if (res == FIND_PLACE_ERROR)
        {
            log("add_atom: out of memory");
            free(copy);
            return EXPR_ERROR;
        }
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
        // free unused memory and try again
        gc_collect(exec);
        res = find_place_for_pair(exec, &place);
        if (res == FIND_PLACE_ERROR)
        {
            log("add_pair: out of memory");
            return EXPR_ERROR;
        }
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
        return make_none();
    return ptr.val_ptr->value;
}
Expr get_head(pExecutor exec, Expr pair)
{
    if (pair.type != VT_PAIR) return make_none();
    return exec->cars[pair.val_pair];
}
Expr get_tail(pExecutor exec, Expr pair)
{
    if (pair.type != VT_PAIR) return make_none();
    return exec->cdrs[pair.val_pair];
}
void set_head(pExecutor exec, Expr pair, Expr value)
{
    if (pair.type != VT_PAIR) return;
    exec->cars[pair.val_pair] = value;
}
void set_tail(pExecutor exec, Expr pair, Expr value)
{
    if (pair.type != VT_PAIR) return;
    exec->cdrs[pair.val_pair] = value;
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
