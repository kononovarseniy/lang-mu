#ifndef GC_H_INCLUDED
#define GC_H_INCLUDED

#include "exec.h"

pHeap create_heap();
void free_heap(pHeap heap);

Expr gc_register(pHeap heap, Expr value);
void gc_free(pHeap heap, pGCPointer ptr);

void gc_collectv(pExecutor exec, int *atoms, int *pairs, int *pointers);
void gc_collect(pExecutor exec);

#endif // GC_H_INCLUDED
