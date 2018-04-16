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

    pMap macros = create_map(sizeof(Expr));
    if (macros == NULL)
    {
        log("create_context: create_map failed");
        free_map(map);
        return NULL;
    }

    pContext res = malloc(sizeof(Context));
    if (res == NULL)
    {
        perror("create_context: malloc failed");
        free_map(map);
        free_map(macros);
        return NULL;
    }

    res->links = 1;
    res->base = NULL;
    res->bindings = map;
    res->macros = macros;
    res->gc_index = 0;
    return res;
}

void free_context(pContext context)
{
    if (context == NULL) return;
    context_unlink(context->base);
    free_map(context->bindings);
    free_map(context->macros);
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

int context_set_macro(pContext context, size_t key, Expr value)
{
    return map_set(context->macros, key, &value);
}
int context_get_macro(pContext context, size_t key, Expr *value)
{
    while (context != NULL)
    {
        int res = map_get(context->macros, key, value);
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

pContextStack create_context_stack()
{
    pContextStack res = malloc(sizeof(ContextStack));
    if (res == NULL)
    {
        perror("create_context_stack: malloc failed");
        return NULL;
    }
    res->head = NULL;
    return res;
}

void free_context_stack(pContextStack stack)
{
    while (stack->head != NULL)
        context_stack_pop(stack);
    free(stack);
}

pContextStackFrame create_context_stack_frame()
{
    pContextStackFrame res = malloc(sizeof(ContextStackFrame));
    if (res == NULL)
    {
        perror("create_context_stack_frame: malloc failed");
        return NULL;
    }
    res->next = NULL;
    return res;
}

void free_context_stack_frame(pContextStackFrame frame)
{
    free(frame);
}

int context_stack_push(pContextStack stack, pContext context)
{
    pContextStackFrame frame = create_context_stack_frame();
    if (frame == NULL)
    {
        log("context_stack_push: create_context_stack_frame failed");
        return 0;
    }
    context_link(context);
    frame->context = context;
    frame->next = stack->head;

    stack->head = frame;
    return 1;
}

int context_stack_pop(pContextStack stack)
{
    if (stack->head == NULL)
    {
        log("context_stack_pop: stack is empty");
        return 0;
    }
    pContextStackFrame frame = stack->head;
    stack->head = frame->next;

    context_unlink(frame->context);
    free_context_stack_frame(frame);
    return 1;
}
