#ifndef EXEC_H_INCLUDED
#define EXEC_H_INCLUDED

#include <stdlib.h>
#include <limits.h>
#include "exec_types.h"
#include "exec_eval.h"
#include "exec_make.h"
#include "exec_math.h"
#include "exec_predicates.h"
#include "exec_builtin.h"
#include "gc.h"

#define EXPR_ERROR INT_MAX
#define EXPR_NOT_FOUND (INT_MAX - 1)
#define MAX_ATOMS 102400
#define MAX_PAIRS 102400

pExecutor create_executor(void);
void free_executor(pExecutor exec);

pFunction create_function();
void free_function(pFunction func);

pUserFunction create_user_function();
void free_user_function(pUserFunction func);

Expr register_atom(pExecutor exec, pContext context, char *name, int bind);
Expr register_function(pExecutor exec, pContext context, char *name, pBuiltinFunction func);

void exec_init(pExecutor exec);
void exec_set_code(pExecutor exec, Expr code);
Expr exec_execute(pExecutor exec);

size_t find_atom(pExecutor exec, char *name);
size_t add_atom(pExecutor exec, char *name);
void del_atom(pExecutor exec, size_t atom);
size_t add_pair(pExecutor exec);
void del_pair(pExecutor exec, size_t pair);

Expr dereference(Expr ptr);
Expr get_head(pExecutor exec, Expr pair);
Expr get_tail(pExecutor exec, Expr pair);
void set_head(pExecutor exec, Expr pair, Expr value);
void set_tail(pExecutor exec, Expr pair, Expr value);
int get_len(pExecutor exec, Expr expr);
Expr *get_items(pExecutor exec, Expr expr, int cnt);
Expr *get_list(pExecutor exec, Expr expr, int *len);

#endif // EXEC_H_INCLUDED
