//
// Created by arseniy on 16.06.18.
//

#ifndef LANG_MU_EXEC_CONSTRUCTIONS_H_INCLUDED
#define LANG_MU_EXEC_CONSTRUCTIONS_H_INCLUDED

#include "exec.h"

BUILTIN_FUNC(cond);

BUILTIN_FUNC(let);

BUILTIN_FUNC(block);

/*
    Function creation
*/
BUILTIN_FUNC(lambda);

BUILTIN_FUNC(macro);

/*
    Setters
*/
BUILTIN_FUNC(def);

BUILTIN_FUNC(set);

BUILTIN_FUNC(defm);

BUILTIN_FUNC(setm);

BUILTIN_FUNC(getm);

/*
    Quote/Backquote
*/
BUILTIN_FUNC(quote);

BUILTIN_FUNC(backquote);

#endif // LANG_MU_EXEC_CONSTRUCTIONS_H_INCLUDED
