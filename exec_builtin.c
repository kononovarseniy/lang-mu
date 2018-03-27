#include "exec_builtin.h"

#include <stdlib.h>
#include <stdio.h>

BUILTIN_FUNC(def)
{
    /*
        (def (a 1) (c a))
    */
    pTypedValue last;
    for (int i = 0; i < argc; i++)
    {
        int len;
        int *list = get_list(exec, args[i], &len);
        if (list == NULL || len != 2)
        {
            printf("def: wrong arguments syntax");
            exit(1);
        }
        int atom = list[0];
        if (!is_atom(atom))
        {
            printf("def: wrong arguments syntax");
            exit(1);
        }
        pTypedValue val = exec_eval(exec, list[1], callContext);
        context_bind(callContext, atom, val);
        typed_value_link(val);
        free(list);
        last = val;
    }
    typed_value_link(last);
    return last;
}

BUILTIN_FUNC(print)
{
    for (int i = 0; i < argc; i++)
    {
        if (is_atom(args[i]))
        {
            int ind = atom_ind(args[i]);
            printf("%s", exec->atoms[ind]);
        }
        else
        {
            printf("list");
        }
        if (i < argc - 1) printf(" ");
    }
    pTypedValue val = create_typed_value();
    if (val == NULL)
    {
        printf("print: create_typed_value failed");
        exit(1);
    }
    val->type = VT_EXPR;
    val->expr = exec->t;
    typed_value_link(val);

    return val;
}
