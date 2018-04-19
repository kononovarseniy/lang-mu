#ifndef EXEC_BUILTIN_H_INCLUDED
#define EXEC_BUILTIN_H_INCLUDED

#include "exec.h"

BUILTIN_FUNC(def);
BUILTIN_FUNC(set);

BUILTIN_FUNC(defm);
BUILTIN_FUNC(setm);
BUILTIN_FUNC(getm);

BUILTIN_FUNC(set_head_builtin);
BUILTIN_FUNC(set_tail_builtin);

BUILTIN_FUNC(print);
BUILTIN_FUNC(prints);
BUILTIN_FUNC(quote);
BUILTIN_FUNC(backquote);

BUILTIN_FUNC(eq);
BUILTIN_FUNC(and);
BUILTIN_FUNC(or);
BUILTIN_FUNC(not);
BUILTIN_FUNC(xor);

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
