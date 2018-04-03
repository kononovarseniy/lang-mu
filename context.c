#include "context.h"

#include <stdlib.h>
#include <stdio.h>
#include "log.h"

pContext create_context(void)
{
    pMap map = create_map(sizeof(Expr));
    if (map == NULL)
    {
        log("create_context: create_map failed");
        return NULL;
    }

    pContext res = malloc(sizeof(Context));
    if (res == NULL)
    {
        perror("create_context: malloc failed");
        return NULL;
    }

    res->links = 1;
    res->base = NULL;
    res->bindings = map;
    return res;
}

void free_context(pContext context)
{
    if (context == NULL) return;
    context_unlink(context->base);
    free_map(context->bindings);
    free(context);
}

pContext context_inherit(pContext base)
{
    pContext context = create_context();
    if (context == NULL)
    {
        printf("context_inherit: create_context failed");
        return NULL;
    }
    context->base = base;
    context_link(base);
    return context;
}

int context_bind(pContext context, size_t key, Expr value)
{
    return map_set(context->bindings, key, &value);
}

int context_get(pContext context, size_t key, Expr *value)
{
    while (context != NULL)
    {
        int res = map_get(context->bindings, key, value);
        if (res == MAP_SUCCESS)
            return MAP_SUCCESS;
        context = context->base;
    }
    return MAP_FAILED;
}

void context_link(pContext context)
{
    if (context == NULL) return;
    context->links++;
}

void context_unlink(pContext context)
{
    if (context == NULL) return;
    context->links--;
    if (context->links <= 0)
    {
        free_context(context);
    }
}
