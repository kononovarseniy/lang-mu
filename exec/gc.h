#ifndef LANG_MU_EXEC_GC_H_INCLUDED
#define LANG_MU_EXEC_GC_H_INCLUDED

#include "exec.h"

pHeap create_heap();
void free_heap(pHeap heap);

Expr gc_register(pHeap heap, Expr value);

void gc_collectv(pExecutor exec, int *atoms, int *pairs, int *pointers);
void gc_collect(pExecutor exec);

#endif // LANG_MU_EXEC_GC_H_INCLUDED
