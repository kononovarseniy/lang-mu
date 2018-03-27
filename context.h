#ifndef CONTEXT_H_INCLUDED
#define CONTEXT_H_INCLUDED

#include "map.h"

typedef struct Context Context, *pContext;
struct Context
{
    pContext base;
    pMap bindings;
};

pContext create_context(void);
void free_context(pContext context);

pContext context_inherit(pContext base);
int context_bind(pContext context, int key, void *value);
int context_get(pContext context, int key, void **value);

#endif // CONTEXT_H_INCLUDED
