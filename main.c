#include <stdlib.h>
#include <stdio.h>

int yyparse();

int main(int argc, char **argv)
{
    FILE *f;
    if ((f = freopen("tests/in.mu", "r", stdin)) == NULL)
    {
        perror("Unable to open file");
        exit(1);
    }

    yyparse();

    fclose(f);
    return 0;
}
