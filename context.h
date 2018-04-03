#ifndef CONTEXT_H_INCLUDED
#define CONTEXT_H_INCLUDED

#include "map.h"

typedef struct Context Context, *pContext;

// exec.h uses type `Context` so it must be included here
#include "exec.h"

struct Context
{
    int links;
    pContext base;
    pMap bindings;
};

pContext create_context(void);
void free_context(pContext context);

pContext context_inherit(pContext base);
int context_bind(pContext context, size_t key, Expr expr);
int context_get(pContext context, size_t key, Expr *expr);

void context_link(pContext context);
void context_unlink(pContext context);

#endif // CONTEXT_H_INCLUDED
