#include <stdlib.h>
#include <stdio.h>

#include "parser.h"
#include "exec.h"

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

void print_indent(int i)
{
    while (i-- > 0)
    {
        printf("    ");
    }
}

void print_code_tree_impl(pSTree tree, int indent)
{
    print_indent(indent); printf("(\n");
    while (tree != NULL)
    {
        switch (tree->type)
        {
        case NODE_LIST:
            print_code_tree_impl(tree->child, indent + 1);
            break;
        case NODE_INT:
            print_indent(indent + 1);
            printf("%ld\n", tree->int_val);
            break;
        case NODE_NAME:
            print_indent(indent + 1);
            printf("%s\n", tree->name);
            break;
        case NODE_STR:
            print_indent(indent + 1);
            printf("\"%s\"\n", tree->str_val);
            break;
        case NODE_CHAR:
            print_indent(indent + 1);
            printf("\'%c\'\n", tree->char_val);
            break;
        default:
            print_indent(indent + 1);
            printf("`Unknown node type`\n");
            break;
        }
        tree = tree->next;
    }
    print_indent(indent); printf(")\n");

}

void print_code_tree(pSTree tree)
{
    print_code_tree_impl(tree, 0);
}

Expr execute_program(pSTree code_tree)
{
    pExecutor exec = create_executor();
    pContext context = create_context();
    exec_init(exec, context);

    Expr expr = exec_load_tree(exec, parsing_result);
    Expr result = exec_eval_all(exec, context, expr);

    exec_cleanup(exec);
    free_executor(exec);
    free_context(context);

    return result;
}

void print_expression(Expr value)
{

}

int main(int argc, char **argv)
{
    pSTree code_tree = parse_file("tests/hello-world.mu");

    print_code_tree(code_tree);
    Expr result = execute_program(code_tree);

    print_expression(result);

    return 0;
}
