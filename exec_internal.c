#include "exec_internal.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "strutils.h"

pTypedValue create_typed_value()
{
    pTypedValue res = malloc(sizeof(TypedValue));
    if (res == NULL)
    {
        perror("create_typed_value: malloc failed");
        return NULL;
    }
    res->links = 0;
    res->type = VT_NONE;

    return res;
}

void free_typed_value(pTypedValue val)
{
    switch (val->type)
    {
    case VT_FUNC:
        free_function(val->func);
        break;
    default:
        printf("free_typed_value: unknown type. Unable to free");
        break;
    }
    free(val);
}

void typed_value_link(pTypedValue val)
{
    val->links++;
}

void typed_value_unlink(pTypedValue val)
{
    if (--val->links == 0)
        free_typed_value(val);
}

pFunction create_function()
{
    pFunction res = malloc(sizeof(Function));
    if (res == NULL)
    {
        perror("create_function: malloc failed");
        return NULL;
    }
    res->context = NULL;
    res->type = FT_NONE;
    return res;
}

void free_function(pFunction func)
{
    switch (func->type)
    {
        case FT_BUILTIN: break;
        default:
            printf("free_function: unknown function type. Unable to free");
            break;
    }
    free(func);
}

int is_pair(int expr)
{
    return (expr & PAIR_MASK) != 0;
}

int is_atom(int expr)
{
    return (expr & PAIR_MASK) == 0;
}

int atom_ind(int expr)
{
    if (expr & PAIR_MASK) return EXPR_ERROR;
    return expr;
}

int pair_ind(int expr)
{
    if (expr & PAIR_MASK) return expr & ~PAIR_MASK;
    return EXPR_ERROR;
}

int add_atom(pExecutor exec, char *name)
{
    if (exec->atomsCount == MAX_ATOMS)
    {
        printf("add_atom: limit exceeded");
        return EXPR_ERROR;
    }
    char *copy = strdup(name);
    if (copy == NULL)
    {
        perror("add_atom: strdup failed");
        return EXPR_ERROR;
    }
    strtolower(copy);
    exec->atoms[exec->atomsCount] = copy;
    int res = exec->atomsCount;
    exec->atomsCount++;
    return res;
}

int add_pair(pExecutor exec)
{
    if (exec->pairsCount == MAX_PAIRS)
    {
        printf("add_pair: limit exceeded");
        return EXPR_ERROR;
    }
    int res = exec->pairsCount;
    exec->pairsCount++;
    return res | PAIR_MASK;
}

pPair get_pair(pExecutor exec, int expr)
{
    int ind;
    if ((ind = pair_ind(expr)) == EXPR_ERROR)
    {
        return NULL;
    }
    return exec->pairs + ind;
}

int get_len(pExecutor exec, int expr)
{
    int len = 0;
    pPair p;
    while ((p = get_pair(exec, expr)) != NULL)
    {
        len++;
        expr = p->right;
    }
    return len;
}

int *get_items(pExecutor exec, int expr, int cnt)
{
    int *res = malloc(cnt*sizeof(int));
    if (res == NULL)
    {
        perror("get_list: malloc failed");
        return NULL;
    }
    pPair p;
    int i = 0;
    while (i < cnt)
    {
        if ((p = get_pair(exec, expr)) == NULL)
        {
            printf("get_list: len is out of range");
            free(res);
            return NULL;
        }
        res[i] = p->left;
        expr = p->right;
        i++;
    }
    return res;
}

int *get_list(pExecutor exec, int expr, int *len)
{
    int l = get_len(exec, expr);
    int *arr = get_items(exec, expr, l);
    *len = l;
    return arr;
}
