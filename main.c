#include <stdlib.h>
#include <stdio.h>

#include "parser.h"
#include "exec.h"
#include "exec_internal.h"//TODO: remove

void pindent(int i)
{
    while (i-- > 0)
    {
        printf("    ");
    }
}

void print_tree(pSTree tree, int indent)
{
    pindent(indent); printf("(\n");
    while (tree != NULL)
    {
        switch (tree->type)
        {
        case NODE_LIST:
            print_tree(tree->child, indent + 1);
            break;
        case NODE_INT:
            pindent(indent + 1);
            printf("%ld\n", tree->int_val);
            break;
        case NODE_NAME:
            pindent(indent + 1);
            printf("%s\n", tree->name);
            break;
        case NODE_STR:
            pindent(indent + 1);
            printf("\"%s\"\n", tree->str_val);
            break;
        case NODE_CHAR:
            pindent(indent + 1);
            printf("\'%c\'\n", tree->char_val);
            break;
        default:
            pindent(indent + 1);
            printf("`Unknown node type`\n");
            break;
        }
        tree = tree->next;
    }
    pindent(indent); printf(")\n");
}

int main(int argc, char **argv)
{
    FILE *f;
    if ((f = freopen("tests/hello-world.mu", "r", stdin)) == NULL)
    {
        perror("Unable to open file");
        exit(1);
    }

    yyparse();

    fclose(f);

    print_tree(parsing_result, 0);

    pExecutor exec = create_executor();
    pContext context = create_context();
    exec_init(exec, context);

    int expr = exec_load_tree(exec, parsing_result);
    int len;
    int *list = get_list(exec, expr, &len);
    exec_eval(exec, list[0], context);
    list = get_list(exec, list[0], &len);

    return 0;
}
