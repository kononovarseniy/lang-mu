#include <stdlib.h>
#include <stdio.h>

#include "parser.h"

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
        if (tree->type == NODE_LIST)
            print_tree(tree->child, indent + 1);
        else if (tree->type == NODE_NAME)
        {
            pindent(indent + 1); printf("%s\n", tree->name);
        }
        tree = tree->next;
    }
    pindent(indent); printf(")\n");
}

int main(int argc, char **argv)
{
    FILE *f;
    if ((f = freopen("tests/in.mu", "r", stdin)) == NULL)
    {
        perror("Unable to open file");
        exit(1);
    }

    yyparse();
    print_tree(parsing_result, 0);

    fclose(f);
    return 0;
}
