#ifndef CONTEXT_H_INCLUDED
#define CONTEXT_H_INCLUDED

#include "map.h"
#include "exec.h"

typedef struct Expr Expr; // Defined in exec.h
typedef struct Context Context, *pContext;

struct Context
{
    int links;
    pContext base;
    pMap bindings;
    pMap macros;
};

pContext create_context(void);
void free_context(pContext context);

pContext context_inherit(pContext base);
int context_bind(pContext context, size_t key, Expr value);
int context_get(pContext context, size_t key, Expr *value);

int context_set_macro(pContext context, size_t key, Expr value);
int context_get_macro(pContext context, size_t key, Expr *value);

void context_link(pContext context);
void context_unlink(pContext context);

#endif // CONTEXT_H_INCLUDED
