#ifndef GC_H_INCLUDED
#define GC_H_INCLUDED

#include "exec.h"

pHeap create_heap();
void free_heap(pHeap heap);

Expr gc_register(pHeap heap, Expr value);
void gc_free(pHeap heap, Expr ptr);

#endif // GC_H_INCLUDED
