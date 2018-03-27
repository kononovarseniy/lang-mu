#include "context.h"

#include <stdlib.h>
#include <stdio.h>

pContext create_context(void)
{
    pMap map = create_map();
    if (map == NULL)
    {
        printf("create_context: create_map failed");
        return NULL;
    }
    pContext res = malloc(sizeof(Context));
    if (res == NULL)
    {
        perror("create_context: malloc failed");
        return NULL;
    }
    res->base = NULL;
    res->bindings = map;

    return res;
}

void free_context(pContext context)
{
    free(context->bindings);
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
    return context;
}

int context_bind(pContext context, int key, void *value)
{
    return map_set(context->bindings, key, value);
}

int context_get(pContext context, int key, void **value)
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
