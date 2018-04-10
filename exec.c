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
    switch (a.type)
    {
    case VT_ATOM:
        return a.val_atom == b.val_atom;
    case VT_CHAR:
        return a.val_char == b.val_char;
    case VT_INT:
        return a.val_int == b.val_int;
    case VT_STRING:
        return strcmp(a.val_str, b.val_str) == 0;
    case VT_FUNC:
    case VT_PAIR:
    case VT_NONE:
    default:
        log("is_equal: unable to compare");
        exit(1);
    }
}

pExecutor create_executor(void)
{
    char **atoms = malloc(MAX_ATOMS * sizeof(char*));
    if (atoms == NULL)
    {
        perror("create_executor: `atoms` allocation failed");
        return NULL;
    }
    enum GCFlags *atomsFlags = malloc(MAX_ATOMS * sizeof(enum GCFlags));
    if (atomsFlags == NULL)
    {
        perror("create_executor: `atomsFlags` allocation failed");
        free(atoms);
        return NULL;
    }

    Expr *cars = malloc(MAX_PAIRS * sizeof(Expr));
    if (cars == NULL)
    {
        perror("create_executor: `cars` allocation failed");
        free(atoms);
        free(atomsFlags);
        return NULL;
    }

    Expr *cdrs = malloc(MAX_PAIRS * sizeof(Expr));
    if (cdrs == NULL)
    {
        perror("create_executor: `cdrs` allocation failed");
        free(atoms);
        free(atomsFlags);
        free(cars);
        return NULL;
    }
    enum GCFlags *pairsFlags = malloc(MAX_PAIRS * sizeof(enum GCFlags));
    if (pairsFlags == NULL)
    {
        perror("create_executor: `pairsFlags` allocation failed");
        free(atoms);
        free(atomsFlags);
        free(cars);
        free(cdrs);
        return NULL;
    }

    pExecutor res = malloc(sizeof(Executor));
    if (res == NULL)
    {
        perror("create_executor: memory allocation failed");
        free(atoms);
        free(atomsFlags);
        free(cars);
        free(cdrs);
        free(pairsFlags);
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
    return res;
}
void free_executor(pExecutor exec)
{
    for (int i = 0; i < exec->atomsCount; i++)
        free((exec->atoms[i]));
    free(exec->atoms);
    free(exec->atomsFlags);
    free(exec->cars);
    free(exec->cdrs);
    free(exec->pairsFlags);
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
    exec->comma = register_atom(exec, context, "#:comma", 0);
    exec->comma_atsign = register_atom(exec, context, "#:comma_atsign", 0);

    // Register built-in function
    register_function(exec, context, "set", set);
    register_function(exec, context, "lambda", lambda);
    register_function(exec, context, "cond", cond);
    register_function(exec, context, "print", print);
    register_function(exec, context, "prints", prints);
    register_function(exec, context, "quote", quote);
    register_function(exec, context, "cons", cons);
    register_function(exec, context, "head", head);
    register_function(exec, context, "tail", tail);

    register_function(exec, context, "gensym", gensym);
    register_function(exec, context, "backquote", backquote);
    register_function(exec, context, "macro", macro);
    register_function(exec, context, "setmacro", setmacro);
    register_function(exec, context, "getmacro", getmacro);
    register_function(exec, context, "macroexpand", macroexpand);

    Expr plus_func = register_function(exec, context, "plus", plus);
    context_bind(context, add_atom(exec, "+"), plus_func);
}

void exec_cleanup(pExecutor exec)
{
    // TODO: implement exec_cleanup
}

Expr load_atom(pExecutor exec, pSTree item)
{
    return make_atom(exec, item->name);
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
    return make_pair(exec, item, tail);
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
    if (func->type != FT_MACRO)
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
    // Execute
    Expr res = exec_eval_all(exec, execContext, user->body);
    context_unlink(execContext);
    // Execute return
    if (func->type == FT_MACRO && !expand)
        res = exec_eval(exec, callContext, res);

    return res;
}

Expr exec_function(pExecutor exec, pContext callContext, pFunction func, Expr *args, int argc)
{
    if (func->type == FT_BUILTIN)
        return exec_builtin_function(exec, func, callContext, args, argc);
    else if (func->type == FT_USER || func->type == FT_MACRO)
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
            return macro;
        }
    }
    Expr func = exec_eval(exec, context, list_head);
    if (func.type != VT_FUNC || func.val_func->type == FT_MACRO)
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
    if (macro->type != FT_MACRO)
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

int is_true(pExecutor exec, Expr expr)
{
    if ((expr.type == VT_ATOM && expr.val_atom == exec->nil.val_atom) ||
        (expr.type == VT_INT && expr.val_int == 0) ||
        (expr.type == VT_NONE))
        return 0;
    return 1;
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
        rest = expr_none();

    func->args = req;
    func->argc = req_len;
    func->opt = opt;
    func->def = def;
    func->optc = opt_len;
    func->rest = rest;

    return 1;
}

pFunction create_lambda(pExecutor exec, pContext defContext, Expr *args, int argc, Expr *body, int len, enum FunctionType type)
{
    Expr bodyExpr = make_list(exec, body, len);
    if (bodyExpr.type == VT_NONE)
    {
        log("create_lambda: make_list failed");
        return NULL;
    }

    pUserFunction user = create_user_function();
    if (user == NULL)
    {
        log("create_lambda: create_user_function failed");
        return NULL;
    }
    if (!parse_arguments(exec, user, args, argc))
    {
        log("create_lambda: parse_arguments failed");
        free(user);
        return NULL;
    }
    user->body = bodyExpr;

    pFunction func = create_function();
    if (func == NULL)
    {
        log("create_lambda: create_function failed");
        free_user_function(user); // this will also free argsCopy
        return NULL;
    }
    func->type = type;
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
        case FT_MACRO:
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
    free(func->opt);
    free(func->def);
    free(func);
}

