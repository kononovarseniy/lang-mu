#ifndef EXEC_H_INCLUDED
#define EXEC_H_INCLUDED

#include <stdlib.h>
#include <limits.h>
#include "stree.h"

#define EXPR_ERROR INT_MAX
#define EXPR_NOT_FOUND (INT_MAX - 1)
#define MAX_ATOMS 102400
#define MAX_PAIRS 102400

enum VarType;
enum FunctionType;
typedef struct Expr Expr;
typedef struct Function Function, *pFunction;
typedef struct UserFunction UserFunction, *pUserFunction;
typedef struct Executor Executor, *pExecutor;

// context.h uses type `Expr` so it must be included here
#include "context.h"

#define BUILTIN_FUNC(NAME) Expr NAME(pExecutor exec, pContext defContext, pContext callContext, Expr *args, int argc)
typedef BUILTIN_FUNC((*pBuiltinFunction));

enum ValueType
{
    VT_NONE,
    VT_ATOM,
    VT_PAIR,
    VT_FUNC,
    VT_INT,
    VT_CHAR,
    VT_STRING
};

struct Expr
{
    enum ValueType type;
    union
    {
        size_t val_pair;
        size_t val_atom;
        pFunction val_func;
        long val_int;
        char val_char;
        char *val_str;

    };
};

Expr expr_none();
int is_none(Expr expr);
int is_equal(Expr a, Expr b);

enum FunctionType
{
    FT_NONE,
    FT_BUILTIN,
    FT_USER
};

struct UserFunction
{
    Expr *args;
    int argc;
    Expr *opt;
    Expr *def;
    int optc;
    Expr rest;
    Expr body;
};

struct Function
{
    enum FunctionType type;
    pContext context;
    union
    {
        pBuiltinFunction builtin;
        pUserFunction user;
    };
};

struct Executor
{
    size_t atomsCount;
    size_t pairsCount;
    char **atoms;
    Expr *cars;
    Expr *cdrs;

    Expr nil;
    Expr t;
    Expr quote;
    Expr comma;
    Expr comma_atsign;
};


pExecutor create_executor(void);
void free_executor(pExecutor exec);

Expr register_atom(pExecutor exec, pContext context, char *name, int bind);
Expr register_function(pExecutor exec, pContext context, char *name, pBuiltinFunction func);

void exec_init(pExecutor exec, pContext context);
void exec_cleanup(pExecutor exec);

Expr exec_load_tree(pExecutor exec, pSTree tree);
Expr exec_eval_array(pExecutor exec, pContext context, Expr *array, int len);
Expr exec_eval_all(pExecutor exec, pContext context, Expr expr);
Expr exec_eval(pExecutor exec, pContext context, Expr expr);

size_t find_atom(pExecutor exec, char *name);
size_t add_atom(pExecutor exec, char *name);
size_t add_pair(pExecutor exec);

Expr get_head(pExecutor exec, Expr pair);
Expr get_tail(pExecutor exec, Expr pair);
int get_len(pExecutor exec, Expr expr);
Expr *get_items(pExecutor exec, Expr expr, int cnt);
Expr *get_list(pExecutor exec, Expr expr, int *len);
Expr make_atom(pExecutor exec, char *name);
Expr make_pair(pExecutor exec, Expr car, Expr cdr);
Expr make_list(pExecutor exec, Expr *arr, int len);

int is_true(pExecutor exec, Expr expr);

pFunction create_lambda(pExecutor exec, pContext defContext, Expr *args, int argc, Expr *body, int len);

pFunction create_function();
void free_function(pFunction func);

pUserFunction create_user_function();
void free_user_function(pUserFunction func);

#endif // EXEC_H_INCLUDED
