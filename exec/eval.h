#ifndef LANG_MU_EXEC_EVAL_H_INCLUDED
#define LANG_MU_EXEC_EVAL_H_INCLUDED

#include "exec.h"

#define EVAL_ARG(IND) exec_eval(exec, callContext, args[IND])

Expr exec_eval(pExecutor exec, pContext context, Expr expr);
Expr exec_eval_array(pExecutor exec, pContext context, Expr *array, int len);
Expr exec_eval_all(pExecutor exec, pContext context, Expr expr);
Expr *exec_eval_each(pExecutor exec, pContext context, Expr *array, int len);
Expr exec_macroexpand(pExecutor exec, pContext context, pFunction macro, Expr *args, int len);

#endif // LANG_MU_EXEC_EVAL_H_INCLUDED
