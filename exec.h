#ifndef EXEC_H_INCLUDED
#define EXEC_H_INCLUDED

#include <stdlib.h>
#include <limits.h>
#include "stree.h"
#include "exec_types.h"
#include "gc.h"

#define EXPR_ERROR INT_MAX
#define EXPR_NOT_FOUND (INT_MAX - 1)
#define MAX_ATOMS 102400
#define MAX_PAIRS 102400

Expr expr_none();
int is_none(Expr expr);
int is_equal(Expr a, Expr b);

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
Expr exec_macroexpand(pExecutor exec, pContext context, pFunction macro, Expr *args, int len);

size_t find_atom(pExecutor exec, char *name);
size_t add_atom(pExecutor exec, char *name);
void del_atom(pExecutor exec, size_t atom);
size_t add_pair(pExecutor exec);
void del_pair(pExecutor exec, size_t pair);

Expr dereference(Expr ptr);
Expr get_head(pExecutor exec, Expr pair);
Expr get_tail(pExecutor exec, Expr pair);
int get_len(pExecutor exec, Expr expr);
Expr *get_items(pExecutor exec, Expr expr, int cnt);
Expr *get_list(pExecutor exec, Expr expr, int *len);

Expr make_atom(pExecutor exec, char *name);
Expr make_pair(pExecutor exec, Expr car, Expr cdr);
Expr make_list(pExecutor exec, Expr *arr, int len);

int is_true(pExecutor exec, Expr expr);

pFunction create_lambda(pExecutor exec, pContext defContext, Expr *args, int argc, Expr *body, int len, enum FunctionType type);

pFunction create_function();
void free_function(pFunction func);

pUserFunction create_user_function();
void free_user_function(pUserFunction func);

#endif // EXEC_H_INCLUDED
