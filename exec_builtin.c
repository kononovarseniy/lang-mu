#include "exec_builtin.h"

#include <stdlib.h>
#include <stdio.h>
#include "log.h"

BUILTIN_FUNC(def)
{
    /*
        (def (a 1) (c a))
    */
    Expr last;
    for (int i = 0; i < argc; i++)
    {
        int len;
        Expr *list = get_list(exec, args[i], &len);
        if (list == NULL || len != 2)
        {
            log("def: wrong arguments syntax");
            exit(1);
        }
        Expr atom = list[0];
        if (atom.type != VT_ATOM)
        {
            log("def: wrong arguments syntax");
            exit(1);
        }
        Expr val = exec_eval(exec, callContext, list[1]);
        context_bind(callContext, atom.val_atom, val);
        free(list);
        last = val;
    }
    return last;
}

BUILTIN_FUNC(print)
{
    for (int i = 0; i < argc; i++)
    {
        if (args[i].type == VT_ATOM)
        {
            printf("%s", exec->atoms[args[i].val_atom]);
        }
        else
        {
            printf("list");
        }
        if (i < argc - 1) printf(" ");
    }

    return exec->t;
}
