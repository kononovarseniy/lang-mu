#ifndef LOAD_H_INCLUDED
#define LOAD_H_INCLUDED

#include "exec/exec.h"
#include "parsing/stree.h"

Expr load_parsed_tree(pExecutor exec, pSTree tree);

#endif // LOAD_H_INCLUDED
