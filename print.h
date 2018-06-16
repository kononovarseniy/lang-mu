#ifndef PRINT_H_INCLUDED
#define PRINT_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include "exec/exec.h"

enum PrintingFlags
{
    PF_PRETTY = 1 << 0,
    PF_UNESCAPE = 1 << 1,
    PF_SHORT_QUOTE = 1 << 2,
    PF_FORCE_QUOTE = 1 << 3,
    PF_DEFAULT = PF_PRETTY | PF_SHORT_QUOTE
};

void print_expression(FILE *f, pExecutor exec, Expr expr, enum PrintingFlags flsgs, int indent);

#endif // PRINT_H_INCLUDED
