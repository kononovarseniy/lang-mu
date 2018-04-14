#include "gc.h"

#include "log.h"

#define BLOCK_SIZE 1024

int gc_adjust_size(pHeap heap, size_t size)
{
    int cur_blocks = heap->size % BLOCK_SIZE;
    int req_blocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (cur_blocks == req_blocks) return 1;
    int new_size = req_blocks * BLOCK_SIZE;

    pGCPointer array = realloc(heap->values, new_size * sizeof(GCPointer));
    if (array == NULL && req_blocks != 0)
    {
        perror("gc_adjust_size: realloc failed");
        return 0;
    }
    heap->values = array;
    heap->size = new_size;
    return 1;
}

int is_valid_ptr(pHeap heap, pGCPointer ptr)
{
    if (ptr < heap->values || ptr >= heap->values + heap->size)
        return 0;
    return ptr->flags & GC_USED;
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
            gc_free(heap, heap->values[i].value);
        }
    }
    free(heap->values);
    free(heap);
}

Expr gc_register(pHeap heap, Expr value)
{
    int success = 0;
    size_t ind;
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
            return expr_none();
        }
    }

    pGCPointer ptr = heap->values + ind;
    ptr->flags = GC_USED;
    ptr->value = value;
    Expr res;
    res.type = value.type | VT_POINTER;
    res.val_ptr = ptr;
    return res;
}

void gc_free(pHeap heap, Expr expr_ptr)
{
    if (!(expr_ptr.type & VT_POINTER))
        return;
    pGCPointer ptr = expr_ptr.val_ptr;
    if (!is_valid_ptr(heap, ptr))
        return;
    Expr value = ptr->value;
    ptr->flags = GC_NONE;
    // free value
    switch (value.type)
    {
    case VT_FUNC_VAL:
        free_function(value.val_func);
        break;
    case VT_STRING_VAL:
        free(value.val_str);
        break;
    default:
        break;
    }
}
