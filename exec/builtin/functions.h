#ifndef EXEC_BUILTIN_H_INCLUDED
#define EXEC_BUILTIN_H_INCLUDED

#include "exec/exec.h"

/*
    Predicates
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
    List functions
*/
BUILTIN_FUNC(cons);

BUILTIN_FUNC(head);

BUILTIN_FUNC(tail);
BUILTIN_FUNC(set_head_builtin);
BUILTIN_FUNC(set_tail_builtin);

/*
    Math functions
*/
// Logical functions
BUILTIN_FUNC(eq);
BUILTIN_FUNC(and);
BUILTIN_FUNC(or);
BUILTIN_FUNC(not);
BUILTIN_FUNC(xor);
// Comparison functions
BUILTIN_FUNC(less_builtin);
BUILTIN_FUNC(less_or_equals_builtin);
BUILTIN_FUNC(num_equals_builtin);
BUILTIN_FUNC(more_or_equals_builtin);
BUILTIN_FUNC(more_builtin);
// Arithmetical functions
BUILTIN_FUNC(sum_builtin);
BUILTIN_FUNC(difference_builtin);
BUILTIN_FUNC(product_builtin);
BUILTIN_FUNC(quotient_builtin);
BUILTIN_FUNC(remainder_builtin);

/*
    String functions
*/
BUILTIN_FUNC(str_from_list);
BUILTIN_FUNC(str_to_list);
BUILTIN_FUNC(str_repeat_char);
BUILTIN_FUNC(str_repeat_str);
BUILTIN_FUNC(str_len);
BUILTIN_FUNC(str_at);
BUILTIN_FUNC(str_cmp_sub);
BUILTIN_FUNC(str_cat);
BUILTIN_FUNC(str_ind);

BUILTIN_FUNC(str_insert);
BUILTIN_FUNC(str_sub);
BUILTIN_FUNC(str_remove);

BUILTIN_FUNC(to_lower);
BUILTIN_FUNC(to_upper);

/*
    Evaluation functions
*/
BUILTIN_FUNC(gensym);
BUILTIN_FUNC(macroexpand);
BUILTIN_FUNC(eval_builtin);

/*
    Control functions
*/
BUILTIN_FUNC(error);

BUILTIN_FUNC(gc_collect_builtin);

/*
    I/O
*/
BUILTIN_FUNC(print);
BUILTIN_FUNC(prints);
BUILTIN_FUNC(scanline);

#endif // EXEC_BUILTIN_H_INCLUDED
