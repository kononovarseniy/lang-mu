#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include "log.h"

extern int yylineno;

extern pSTree parsing_result;

extern FILE *yyin;

int yyparse();

pSTree parse_file(char *name)
{
    if ((yyin = fopen(name, "r")) == NULL)
    {
        printf("%s\n", name);
        perror("Unable to open file");
        exit(1);
    }

    yylineno = 1;
    yyparse();

    fclose(yyin);
    return parsing_result;
}
