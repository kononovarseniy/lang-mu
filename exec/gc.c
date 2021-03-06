#include "gc.h"

#include "utils/log.h"

#define BLOCK_SIZE 1024

int gc_adjust_size(pHeap heap, size_t size)
{
    size_t old_size = heap->size;
    size_t cur_blocks = old_size % BLOCK_SIZE;
    size_t req_blocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (cur_blocks == req_blocks) return 1;
    size_t new_size = req_blocks * BLOCK_SIZE;

    pPointerInfo array = realloc(heap->values, new_size * sizeof(PointerInfo));
    if (array == NULL && new_size != 0)
    {
        perror("gc_adjust_size: realloc failed");
        return 0;
    }
    for (size_t i = old_size; i < new_size; i++)
        array[i].flags = GC_NONE;

    heap->values = array;
    heap->size = new_size;
    return 1;
}

Expr gc_register(pHeap heap, Expr value)
{
    // Find place in pointer table
    int success = 0;
    size_t ind = 0;
    for (size_t i = 0; i < heap->size; i++)
    {
        if (!(heap->values[i].flags & GC_USED))
        {
            ind = i;
            success = 1;
        }
    }
    if (!success)
    {
        ind = heap->size;
        if (!gc_adjust_size(heap, heap->size + 1))
        {
            log("gc_register: gc_adjust_size failed");
            return make_none();
        }
    }
    // Create new pointer
    pPointer ptr = malloc(sizeof(Pointer));
    if (ptr == NULL)
    {
        log("gc_register: memory allocation failed");
        return make_none();
    }
    ptr->id = ind;
    ptr->value = value;

    heap->values[ind].ptr = ptr;
    heap->values[ind].flags = GC_USED;

    Expr res;
    res.type = value.type | VT_POINTER;
    res.val_ptr = ptr;
    return res;
}

void gc_free(pPointerInfo info)
{
    if (!(info->flags & GC_USED))
        return;
    Expr value = info->ptr->value;
    // Free pointer
    info->flags = GC_NONE;
    free(info->ptr);
    // Free value
    switch (value.type)
    {
    case VT_FUNC_VAL:
        free_function(value.val_func);
        break;
    case VT_STRING_VAL:
        free_string(value.val_str);
        break;
    case VT_INT_VAL:
        free_longint(value.val_int);
    default:
        break;
    }
}

pHeap create_heap()
{
    pHeap res = malloc(sizeof(Heap));
    if (res == NULL)
    {
        perror("create_heap: malloc failed");
        return NULL;
    }
    res->size = 0;
    res->values = NULL;
    return res;
}

void free_heap(pHeap heap)
{
    for (size_t i = 0; i < heap->size; i++)
    {
        if (heap->values[i].flags & GC_USED)
        {
            gc_free(heap->values + i);
        }
    }
    free(heap->values);
    free(heap);
}

void gc_mark_atom(pExecutor exec, size_t atom);
void gc_mark_pair(pExecutor exec, size_t pair);
void gc_mark_pointer(pExecutor exec, pPointer ptr);
void gc_scan_user_function(pExecutor exec, pUserFunction func);
void gc_mark_function(pExecutor exec, pFunction func);
void gc_mark_expression(pExecutor exec, Expr expr);
void gc_mark_expression_array(pExecutor exec, Expr *arr, int len);
void gc_scan_map(pExecutor exec, pMap map);
void gc_scan_context(pExecutor exec, pContext context);

void gc_mark_atom(pExecutor exec, size_t atom)
{
    exec->atomsFlags[atom] |= GC_REFERENCED;
}

void gc_mark_pair(pExecutor exec, size_t pair)
{
    if (exec->pairsFlags[pair] & GC_REFERENCED)
        return;
    exec->pairsFlags[pair] |= GC_REFERENCED;

    gc_mark_expression(exec, exec->cars[pair]);
    gc_mark_expression(exec, exec->cdrs[pair]);
}

void gc_mark_pointer(pExecutor exec, pPointer ptr)
{
    pPointerInfo info = exec->heap->values + ptr->id;
    if (info->flags & GC_REFERENCED)
        return;
    info->flags |= GC_REFERENCED;

    gc_mark_expression(exec, ptr->value);
}

void gc_scan_user_function(pExecutor exec, pUserFunction func)
{
    gc_mark_expression_array(exec, func->args, func->argc);
    gc_mark_expression_array(exec, func->opt, func->optc);
    gc_mark_expression_array(exec, func->def, func->optc);
    gc_mark_expression(exec, func->rest);
    gc_mark_expression(exec, func->body);
}

void gc_mark_function(pExecutor exec, pFunction func)
{
    if (func->gc_index == exec->gc_index)
        return;
    func->gc_index = exec->gc_index;

    gc_scan_context(exec, func->context);
    if (func->type == FT_USER ||
        func->type == FT_USER_MACRO)
        gc_scan_user_function(exec, func->user);
}

void gc_mark_expression(pExecutor exec, Expr expr)
{
    if (expr.type == VT_ATOM)
        gc_mark_atom(exec, expr.val_atom);
    else if (expr.type == VT_PAIR)
        gc_mark_pair(exec, expr.val_pair);
    else if (expr.type & VT_POINTER)
        gc_mark_pointer(exec, expr.val_ptr);
    else if (expr.type == VT_FUNC_VAL)
        gc_mark_function(exec, expr.val_func);
}

void gc_mark_expression_array(pExecutor exec, Expr *arr, int len)
{
    for (int i = 0; i < len; i++)
        gc_mark_expression(exec, arr[i]);
}

void gc_scan_map(pExecutor exec, pMap map)
{
    if (map->count == 0)
        return;

    size_t *keys = map_get_keys(map);
    if (keys == NULL)
    {
        log("gc_scan_map: map_get_keys failed");
        exit(1);
    }
    Expr *values = map_get_values(map);
    if (values == NULL)
    {
        log("gc_scan_map: map_get_values failed");
        exit(1);
    }

    for (int i = 0; i < map->count; i++)
    {
        gc_mark_atom(exec, keys[i]);
        gc_mark_expression(exec, values[i]);
    }

    free(keys);
    free(values);
}

void gc_scan_context(pExecutor exec, pContext context)
{
    if (context->gc_index == exec->gc_index)
        return;
    context->gc_index = exec->gc_index;

    gc_scan_map(exec, context->bindings);
    gc_scan_map(exec, context->macros);
    if (context->base != NULL)
        gc_scan_context(exec, context->base);
}

void gc_scan_context_stack(pExecutor exec, pContextStack stack)
{
    pContextStackFrame curr = stack->head;
    while (curr != NULL)
    {
        gc_scan_context(exec, curr->context);
        curr = curr->next;
    }
}

int gc_del_atoms(pExecutor exec)
{
    int deleted = 0;
    size_t cnt = exec->atomsCount;
    for (size_t i = 0; i < cnt; i++)
    {
        enum GCFlags flags = exec->atomsFlags[i];
        if (flags & GC_USED)
        {
            if (flags & GC_REFERENCED)
                exec->atomsFlags[i] &= ~GC_REFERENCED;
            else
            {
                del_atom(exec, i);
                deleted++;
            }
        }
    }
    return deleted;
}

int gc_del_pairs(pExecutor exec)
{
    int deleted = 0;
    size_t cnt = exec->pairsCount;
    for (size_t i = 0; i < cnt; i++)
    {
        enum GCFlags flags = exec->pairsFlags[i];
        if (flags & GC_USED)
        {
            if (flags & GC_REFERENCED)
                exec->pairsFlags[i] &= ~GC_REFERENCED;
            else
            {
                del_pair(exec, i);
                deleted++;
            }
        }
    }
    return deleted;
}

int gc_del_pointers(pExecutor exec)
{
    int deleted = 0;
    pHeap heap = exec->heap;
    size_t cnt = heap->size;
    for (size_t i = 0; i < cnt; i++)
    {
        pPointerInfo info = heap->values + i;
        if (info->flags & GC_USED)
        {
            if (info->flags & GC_REFERENCED)
                info->flags &= ~GC_REFERENCED;
            else
            {
                gc_free(info);
                deleted++;
            }
        }
    }
    return deleted;
}

void gc_collectv(pExecutor exec, int *atoms, int *pairs, int *pointers)
{
    exec->gc_index++;
    gc_scan_context_stack(exec, exec->stack);
    gc_mark_expression(exec, exec->code);

    int atoms_count = gc_del_atoms(exec);
    int pairs_count = gc_del_pairs(exec);
    int pointers_count = gc_del_pointers(exec);
    if (atoms != NULL) *atoms = atoms_count;
    if (pairs != NULL) *pairs = pairs_count;
    if (pointers != NULL) *pointers = pointers_count;
}

void gc_collect(pExecutor exec)
{
    return gc_collectv(exec, NULL, NULL, NULL);
}
