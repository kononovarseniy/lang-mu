%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Prototype of yylex implemented in auto-generated "mu.scanner.c"
int yylex(void);

int yyerror(char *s);

%}

%union
{
    int int_val;
    char *op_val;
}

%start	input 

%token	<int_val>	INTEGER_LITERAL
%type	<int_val>	exp
%left	PLUS
%left	MULT

%%

input:  /* empty */
        | exp	{ printf("Result: %d\n", $1); }
        ;

exp:    INTEGER_LITERAL	{ $$ = $1; }
        | exp PLUS exp	{ $$ = $1 + $3; }
        | exp MULT exp	{ $$ = $1 * $3; }
        ;

%%

int yyerror(char *s)
{
    extern int yylineno;	// defined and maintained in lex.c
    extern char *yytext;	// defined and maintained in lex.c
 
    printf("ERROR: %s at symbol \"%s\" on line %d\n", s, yytext, yylineno);
    exit(1);
}

