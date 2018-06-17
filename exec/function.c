//
// Created by arseniy on 17.06.18.
//

#include <stdlib.h>

#include "function.h"

#include "utils/log.h"

pFunction create_function()
{
    pFunction res = malloc(sizeof(Function));
    if (res == NULL)
    {
        perror("create_function: malloc failed");
        return NULL;
    }
    res->type = FT_NONE;
    res->context = NULL;
    res->gc_index = 0;
    return res;
}

void free_function(pFunction func)
{
    switch (func->type)
    {
        case FT_BUILTIN:
            break;
        case FT_USER_MACRO:
        case FT_USER:
            free_user_function(func->user);
            break;
        default:
            log("free_function: unknown function type. Unable to free");
            break;
    }
    context_unlink(func->context);
    free(func);
}

pUserFunction create_user_function()
{
    pUserFunction res = malloc(sizeof(UserFunction));
    if (res == NULL)
    {
        log("create_user_function: malloc failed");
        return NULL;
    }
    return res;
}

void free_user_function(pUserFunction func)
{
    free(func->args);
    free(func->opt);
    free(func->def);
    free(func);
}
