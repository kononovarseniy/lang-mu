#ifndef EXEC_INTERNAL_H_INCLUDED
#define EXEC_INTERNAL_H_INCLUDED

#include "exec.h"

#include <limits.h>

#define PAIR_MASK (INT_MIN)
#define EXPR_ERROR INT_MAX

pTypedValue create_typed_value();
void free_typed_value(pTypedValue val);
void typed_value_link(pTypedValue val);
void typed_value_unlink(pTypedValue val);

pFunction create_function();
void free_function(pFunction func);

int is_pair(int expr);
int is_atom(int expr);

int atom_ind(int expr);
int pair_ind(int expr);

int add_atom(pExecutor exec, char *name);
int add_pair(pExecutor exec);
pPair get_pair(pExecutor exec, int expr);
int get_len(pExecutor exec, int expr);
int *get_items(pExecutor exec, int expr, int cnt);
int *get_list(pExecutor exec, int expr, int *len);

#endif // EXEC_INTERNAL_H_INCLUDED
