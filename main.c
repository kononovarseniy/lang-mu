#include <stdlib.h>
#include <stdio.h>

#include "parser.h"
#include "exec/exec.h"
#include "load.h"
#include "print.h"
#include "utils/log.h"

#include "types/longint.h"

Expr load_library(pExecutor exec, char *path)
{
    pSTree code_tree = parse_file(path);
    Expr code = load_parsed_tree(exec, code_tree);
    free_stree(code_tree);

    add_code(exec, code);
    Expr res = exec_execute(exec);
    return res;
}

Expr execute_program(pSTree code_tree)
{
    pExecutor exec = create_executor();
    init_executor(exec);

    load_library(exec, "stdlib/stdlib.mu");
    /*
    printf("\n====================\n");
    printf("Stdlib loaded:");
    printf("\n====================\n");
    */

    Expr code = load_parsed_tree(exec, code_tree);
    add_code(exec, code);

    /*
    printf("\n====================\n");
    printf("Code loaded:");
    printf("\n====================\n");
    print_expression(stdout, exec, code, PF_DEFAULT, 0);
    */

    printf("\n====================\n");
    printf("Program output:");
    printf("\n====================\n");
    Expr result = exec_execute(exec);

    printf("\n====================\n");
    printf("Program returned:");
    printf("\n====================\n");
    print_expression(stdout, exec, result, PF_DEFAULT, 0);

    free_executor(exec);

    return result;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        log("USAGE: lang-mu source.file");
        return 1;
    }
    pSTree code_tree = parse_file(argv[1]);
    execute_program(code_tree);
    free_stree(code_tree);

    return 0;
}
