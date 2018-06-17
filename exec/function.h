//
// Created by arseniy on 17.06.18.
//

#ifndef LANG_MU_EXEC_FUNCTION_H
#define LANG_MU_EXEC_FUNCTION_H

#include "exec.h"

pFunction create_function();
void free_function(pFunction func);

pUserFunction create_user_function();
void free_user_function(pUserFunction func);

#endif // LANG_MU_EXEC_FUNCTION_H
