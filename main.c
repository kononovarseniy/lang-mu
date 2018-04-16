#include <stdlib.h>
#include <stdio.h>

#include "parser.h"
#include "exec.h"
#include "load.h"
#include "print.h"
#include "log.h"

pSTree parse_file(char *name)
{
    FILE *f;
    if ((f = freopen("tests/hello-world.mu", "r", stdin)) == NULL)
    {
        perror("Unable to open file");
        exit(1);
    }

    yyparse();

    fclose(f);
    return parsing_result;
}

Expr execute_program(pSTree code_tree)
{
    pExecutor exec = create_executor();
    exec_init(exec);

    Expr code = load_parsed_tree(exec, parsing_result);
    exec_set_code(exec, code);

    printf("\n====================\n");
    printf("Code loaded:");
    printf("\n====================\n");
    print_expression(stdout, exec, code, PF_DEFAULT, 0);

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
    pSTree code_tree = parse_file("tests/hello-world.mu");

    //print_code_tree(code_tree);
    execute_program(code_tree);

    return 0;
}
