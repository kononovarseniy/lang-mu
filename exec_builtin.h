#ifndef EXEC_BUILTIN_H_INCLUDED
#define EXEC_BUILTIN_H_INCLUDED

#include "exec.h"

/*
    Built-in predicates
*/
BUILTIN_FUNC(atom_p);
BUILTIN_FUNC(pair_p);
BUILTIN_FUNC(char_p);
BUILTIN_FUNC(int_p);
BUILTIN_FUNC(real_p);
BUILTIN_FUNC(string_p);
BUILTIN_FUNC(function_p);
BUILTIN_FUNC(macro_p);

/*
    Setters
*/
BUILTIN_FUNC(def);
BUILTIN_FUNC(set);

BUILTIN_FUNC(defm);
BUILTIN_FUNC(setm);
BUILTIN_FUNC(getm);

BUILTIN_FUNC(set_head_builtin);
BUILTIN_FUNC(set_tail_builtin);

/*
    Logical functions
*/
BUILTIN_FUNC(eq);
BUILTIN_FUNC(and);
BUILTIN_FUNC(or);
BUILTIN_FUNC(not);
BUILTIN_FUNC(xor);

/*
    Comparison functions
*/
BUILTIN_FUNC(less_builtin);
BUILTIN_FUNC(less_or_equals_builtin);
BUILTIN_FUNC(num_equals_builtin);
BUILTIN_FUNC(more_or_equals_builtin);
BUILTIN_FUNC(more_builtin);

/*
    Arithmetical functions
*/
BUILTIN_FUNC(sum_builtin);
BUILTIN_FUNC(difference_builtin);
BUILTIN_FUNC(product_builtin);
BUILTIN_FUNC(quotient_builtin);
BUILTIN_FUNC(remainder_builtin);

/*
    List functions
*/
BUILTIN_FUNC(cons);
BUILTIN_FUNC(head);
BUILTIN_FUNC(tail);
BUILTIN_FUNC(quote);
BUILTIN_FUNC(backquote);

/*
    Function creation
*/
BUILTIN_FUNC(lambda);
BUILTIN_FUNC(macro);

/*
    Macro support
*/
BUILTIN_FUNC(gensym);
BUILTIN_FUNC(macroexpand);

BUILTIN_FUNC(cond);

BUILTIN_FUNC(print);
BUILTIN_FUNC(prints);
BUILTIN_FUNC(gc_collect_builtin);

#endif // EXEC_BUILTIN_H_INCLUDED
