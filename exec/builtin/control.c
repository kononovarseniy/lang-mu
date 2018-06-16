//
// Created by arseniy on 16.06.18.
//

#include "functions.h"

#include "utils/log.h"
#include "print.h"

BUILTIN_FUNC(error)
{
    fprintf(stderr, "ERROR: ");
    for (int i = 0; i < argc; i++)
    {
        Expr value = exec_eval(exec, callContext, args[i]);
        print_expression(stderr, exec, value, PF_SHORT_QUOTE | PF_UNESCAPE, 0);
        if (i < argc - 1) fprintf(stderr, " ");
    }
    fprintf(stderr, "\n");
    exit(1);
}

BUILTIN_FUNC(gc_collect_builtin)
{
    if (argc != 0)
    {
        log("gc-collect: too many arguments");
        exit(1);
    }
    int atoms, pairs, objects;
    gc_collectv(exec, &atoms, &pairs, &objects);
    Expr arr[3] =
            {
                    make_int_from_long(exec, atoms),
                    make_int_from_long(exec, pairs),
                    make_int_from_long(exec, objects)
            };
    Expr res = make_list(exec, arr, 3);
    return res;
}

