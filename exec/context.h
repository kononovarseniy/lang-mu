#ifndef LANG_MU_EXEC_CONTEXT_H_INCLUDED
#define LANG_MU_EXEC_CONTEXT_H_INCLUDED

#include "exec.h"
#include "utils/map.h"

pContext create_context(void);
void free_context(pContext context);

pContext context_inherit(pContext base);

int context_bind(pContext context, size_t key, Expr value);
int context_set(pContext context, size_t key, Expr value);
int context_get(pContext context, size_t key, Expr *value);

int context_bind_macro(pContext context, size_t key, Expr value);
int context_set_macro(pContext context, size_t key, Expr value);
int context_get_macro(pContext context, size_t key, Expr *value);

void context_link(pContext context);
void context_unlink(pContext context);

pContextStack create_context_stack();
void free_context_stack(pContextStack stack);

int context_stack_push(pContextStack stack, pContext context);
int context_stack_pop(pContextStack stack);

#endif // LANG_MU_EXEC_CONTEXT_H_INCLUDED
