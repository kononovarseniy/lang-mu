%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mu.parser.h"

int yyerror(char *s);

char *copystr(const char *s)
{
    char *res = malloc((strlen(yytext) + 1)*sizeof(char));
    strcpy(res, s);
    return res;
}

%}

%option noyywrap


digit [0-9]

%%

{digit}+    { yylval.int_val = atoi(yytext); return INTEGER_LITERAL; }
"+"         { yylval.op_val = copystr(yytext); return PLUS; }
"*"         { yylval.op_val = copystr(yytext); return MULT; }

[ \t]*      {}
[\n]        { yylineno++; }

.           { printf("SCANNER "); yyerror("Unexpected character"); exit(1); }

