#ifndef EXEC_BUILTIN_H_INCLUDED
#define EXEC_BUILTIN_H_INCLUDED

#include "exec.h"

BUILTIN_FUNC(set);
BUILTIN_FUNC(setmacro);
BUILTIN_FUNC(getmacro);

BUILTIN_FUNC(print);
BUILTIN_FUNC(prints);
BUILTIN_FUNC(quote);
BUILTIN_FUNC(backquote);

BUILTIN_FUNC(plus);

BUILTIN_FUNC(cons);
BUILTIN_FUNC(head);
BUILTIN_FUNC(tail);

BUILTIN_FUNC(lambda);
BUILTIN_FUNC(macro);

BUILTIN_FUNC(cond);

BUILTIN_FUNC(gensym);
BUILTIN_FUNC(macroexpand);
BUILTIN_FUNC(gc_collect_builtin);

#endif // EXEC_BUILTIN_H_INCLUDED
