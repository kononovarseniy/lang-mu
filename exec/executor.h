//
// Created by arseniy on 17.06.18.
//

#ifndef LANG_MU_EXEC_EXECUTOR_H_INCLUDED
#define LANG_MU_EXEC_EXECUTOR_H_INCLUDED

#include <stdlib.h>

#include "exec.h"

#define MAX_ATOMS 102400
#define MAX_PAIRS 102400

pExecutor create_executor(void);
void free_executor(pExecutor exec);

Expr register_atom(pExecutor exec, pContext context, char *name, int bind);
Expr register_function(pExecutor exec, pContext context, char *name, pBuiltinFunction func);

void init_executor(pExecutor exec);
void add_code(pExecutor exec, Expr code);
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

#endif // LANG_MU_EXEC_EXECUTOR_H_INCLUDED
