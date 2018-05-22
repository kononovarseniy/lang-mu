#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include "log.h"

extern int yylineno;

extern pSTree parsing_result;

int yyparse();

pSTree parse_file(char *name)
{
    FILE *f;
    if ((f = freopen(name, "r", stdin)) == NULL)
    {
        printf("%s\n", name);
        perror("Unable to open file");
        exit(1);
    }

    yylineno = 1;
    yyparse();

    fclose(f);
    return parsing_result;
}
